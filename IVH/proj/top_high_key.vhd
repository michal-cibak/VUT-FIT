-- IVH Project: Patnáctka
-- Author: xcibak00 - Michal Cibák

library IEEE;
use IEEE.std_logic_1164.all;
-- use IEEE.std_logic_arith.all;
use IEEE.std_logic_unsigned."conv_integer";
use IEEE.numeric_std."mod";
use work.game_pack.all;
use work.vga_controller_cfg.all;

architecture Main of tlv_pc_ifc is

    component keyboard_controller_high
        generic (READ_INTERVAL : integer := 2 ** 14 - 1);
    port (
        -- Clock and reset signals
        CLK        : in std_logic;
        RST        : in std_logic;

        -- Pressed keys
        DATA_OUT   : out std_logic_vector(15 downto 0);
      
        -- Keyboard signals 
        KB_KIN     : out std_logic_vector(3 downto 0);
        KB_KOUT    : in  std_logic_vector(3 downto 0)
    );
    end component;

    component cell
        generic (MASK : mask_t);
    port (
        CLK         : in  std_logic;
        RESET       : in  std_logic;

        INIT_STATE  : in  std_logic_vector(3 downto 0);
        KEYS        : in  std_logic_vector(3 downto 0);
        NEIG_TOP    : in  std_logic_vector(3 downto 0);
        NEIG_LEFT   : in  std_logic_vector(3 downto 0);
        NEIG_RIGHT  : in  std_logic_vector(3 downto 0);
        NEIG_BOTTOM : in  std_logic_vector(3 downto 0);

        STATE       : out std_logic_vector(3 downto 0)
    );
    end component;

    component symbol_rom
    port (
        ADDRESS : in  std_logic_vector(3 downto 0);
        COLUMN  : in  std_logic_vector(2 downto 0);
        ROW     : in  std_logic_vector(2 downto 0);

        DATA    : out  std_logic
    );
    end component;

    component vga_controller is
        generic (REQ_DELAY : natural := 1); -- Delay for requested data
    port (
        -- Clock, reset and enable signals
        CLK    : in std_logic;
        RST    : in std_logic;
        ENABLE : in std_logic;

        -- Resolution and refresh rate (use setmode() function)
        MODE : in std_logic_vector(60 downto 0);

        -- Input
        DATA_RED   : in  std_logic_vector(2 downto 0);
        DATA_GREEN : in  std_logic_vector(2 downto 0);
        DATA_BLUE  : in  std_logic_vector(2 downto 0);
        
        -- Request
        ADDR_COLUMN : out std_logic_vector(11 downto 0);
        ADDR_ROW    : out std_logic_vector(11 downto 0);

        -- Output
        VGA_RED   : out std_logic_vector(2 downto 0);
        VGA_GREEN : out std_logic_vector(2 downto 0);
        VGA_BLUE  : out std_logic_vector(2 downto 0);
        VGA_HSYNC : out std_logic;
        VGA_VSYNC : out std_logic;
      
        -- H/V Status
        STATUS_H : out vga_hfsm_state;
        STATUS_V : out vga_vfsm_state     
    );
    end component;

    signal keyboard: std_logic_vector(15 downto 0) := (others => '0');
    signal arrow_keys: std_logic_vector(3 downto 0) := "0000";

    signal init_state: std_logic_vector(0 to 63) := "0001001000110100010101100111100010011010101111001101111011110000";
    signal game_state: std_logic_vector(0 to 63) := (others => '0');

    signal sel: std_logic_vector(3 downto 0);

    signal address: std_logic_vector(3 downto 0);
    signal data: std_logic;

    signal vga_mode: std_logic_vector(60 downto 0);
    signal column: std_logic_vector(11 downto 0);
    signal row: std_logic_vector(11 downto 0);
    signal rgb: std_logic_vector(8 downto 0);
        alias red:   std_logic_vector(2 downto 0) is rgb(8 downto 6);
        alias green: std_logic_vector(2 downto 0) is rgb(5 downto 3);
        alias blue:  std_logic_vector(2 downto 0) is rgb(2 downto 0);

begin

    instance_keyboard : keyboard_controller_high
        port map (CLK, RESET, keyboard, KIN, KOUT);

    arrow_keys <= keyboard(8) & keyboard(6) & keyboard(4) & keyboard(2);
    matrix_rows : for y in 0 to 3 generate
        matrix_cols : for x in 0 to 3 generate
            instance_cell : cell
                generic map (getmask(x, y, 4, 4))
                port map (
                    CLK   => CLK,
                    RESET => RESET,
                    
                    INIT_STATE  => init_state(state_position(x, y) to state_position(x, y) + 3),
                    KEYS        => arrow_keys,
                    NEIG_TOP    => game_state(state_position(x, (y - 1) mod 4) to state_position(x, (y - 1) mod 4) + 3),
                    NEIG_LEFT   => game_state(state_position((x - 1) mod 4, y) to state_position((x - 1) mod 4, y) + 3),
                    NEIG_RIGHT  => game_state(state_position((x + 1) mod 4, y) to state_position((x + 1) mod 4, y) + 3),
                    NEIG_BOTTOM => game_state(state_position(x, (y + 1) mod 4) to state_position(x, (y + 1) mod 4) + 3),
                    
                    STATE => game_state(state_position(x, y) to state_position(x, y) + 3)
                );
        end generate ; -- matrix_cols
    end generate ; -- matrix_rows

    sel <= row(7 downto 6) & column(7 downto 6);
    address <= game_state(conv_integer(sel) * 4 to conv_integer(sel) * 4 + 3);

    instance_rom : symbol_rom
        port map (address, column(5 downto 3), row(5 downto 3), data);

    setmode(r640x480x60, vga_mode); -- 640x480 pixels, 60 fps
    instance_vga : vga_controller
        port map (
            CLK    => CLK, 
            RST    => RESET,
            ENABLE => '1',

            MODE => vga_mode,

            ADDR_COLUMN => column,
            ADDR_ROW    => row,

            DATA_RED   => red,
            DATA_GREEN => green,
            DATA_BLUE  => blue,

            VGA_RED   => RED_V,
            VGA_BLUE  => BLUE_V,
            VGA_GREEN => GREEN_V,
            VGA_HSYNC => HSYNC_V,
            VGA_VSYNC => VSYNC_V
        );

    draw : process (CLK)
    begin
        if rising_edge(CLK) then
            red   <= "000";
            green <= "000";
            blue  <= "111";
            if column < "000100000000" and row < "000100000000" then -- CHANGE so = is used instead
                if data = '1' then
                    red   <= "000";
                    green <= "111";
                    blue  <= "000";
                else
                    rgb <= (others => '1');
                end if ;
            end if ;
        end if;
    end process draw;

end Main;
