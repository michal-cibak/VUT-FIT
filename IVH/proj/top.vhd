-- IVH Project: Patnáctka
-- Author: xcibak00 - Michal Cibák

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use IEEE.std_logic_unsigned.all;
use IEEE.numeric_std."mod";
use work.game_pack.all;
use work.vga_controller_cfg.all;

architecture Main of tlv_pc_ifc is

    component keyboard_controller
        generic (READ_INTERVAL : integer := 2 ** 14 - 1);
    port (
        -- Clock and reset signals
        CLK      : in std_logic;
        RST      : in std_logic;

        -- Pressed keys and their activity (0/1 -> none/change)
        DATA_OUT : out std_logic_vector(15 downto 0);
        DATA_VLD : out std_logic;
      
        -- Keyboard signals 
        KB_KIN   : out std_logic_vector(3 downto 0);
        KB_KOUT  : in  std_logic_vector(3 downto 0)
    );
    end component;

    component cell
        generic (MASK : mask_t);
    port (
        -- Clock and reset signals
        CLK         : in  std_logic;
        RESET       : in  std_logic;

        -- Initial state of the cell, directional keys, states of neighboring cells
        INIT_STATE  : in  std_logic_vector(3 downto 0);
        KEYS        : in  std_logic_vector(3 downto 0);
        NEIG_TOP    : in  std_logic_vector(3 downto 0);
        NEIG_LEFT   : in  std_logic_vector(3 downto 0);
        NEIG_RIGHT  : in  std_logic_vector(3 downto 0);
        NEIG_BOTTOM : in  std_logic_vector(3 downto 0);

        -- Actual state of the cell
        STATE       : out std_logic_vector(3 downto 0)
    );
    end component;

    component bcd_counter
    port (
        -- Clock, reset and count enable signals
        CLK      :  in std_logic;
        RESET    :  in std_logic;
        CE       :  in std_logic;

        -- Actual number and overflow signal (1 CLK period)
        NUMBER   : out std_logic_vector(3 downto 0);
        OVERFLOW : out std_logic
    );
    end component;

    component symbol_rom
    port (
        -- Address (corresponding to value) of desired cell and column and row of specific pixel in it
        ADDRESS : in  std_logic_vector(3 downto 0);
        COLUMN  : in  std_logic_vector(2 downto 0);
        ROW     : in  std_logic_vector(2 downto 0);

        -- State of addressed pixel (1/0 -> ON/OFF)
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

        -- Unchanging resolution and refresh rate (use setmode() function)
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

    signal keyboard_raw: std_logic_vector(15 downto 0) := (others => '0');
    signal keyboard: std_logic_vector(15 downto 0) := (others => '0');
    type t_state_keyboard is (s_wfpress, s_wfrelease);
    -- type t_state_game is (s_smth1, s_smth2);
    signal pstate, nstate: t_state_keyboard;
    
    signal init_state: std_logic_vector(0 to 63);
    signal init_state_A: std_logic_vector(0 to 63) := "1111111011011100101110101001100001110110010101000000000100100011";
    signal init_state_B: std_logic_vector(0 to 63) := "0001100011000100001100100110111001011011111110100111100111010000";
    signal init_state_C: std_logic_vector(0 to 63) := "1111100011101010001101010100100101111101110001101011000100100000";
    signal init_state_D: std_logic_vector(0 to 63) := "0001001000110100010101100111100010011010101111001101111011110000";
    signal game_state: std_logic_vector(0 to 63) := (others => '0');
    signal game_reset: std_logic := '0';
    signal arrow_keys: std_logic_vector(3 downto 0) := "0000";
    signal game_arrow_keys: std_logic_vector(3 downto 0);

    signal ovfs: std_logic_vector(3 downto 0);
        alias add_move: std_logic is ovfs(3);
        alias overflow: std_logic is ovfs(0);
    signal moves: std_logic_vector(11 downto 0);

    signal col_rom: std_logic_vector(2 downto 0);
    signal row_rom: std_logic_vector(2 downto 0);
    signal game_column: std_logic_vector(11 downto 0);
    signal game_row: std_logic_vector(11 downto 0);
    signal address_sel: std_logic_vector(3 downto 0);
    signal address: std_logic_vector(3 downto 0);
    signal data: std_logic;

    signal vga_mode: std_logic_vector(60 downto 0);
    signal column: std_logic_vector(11 downto 0);
    signal row: std_logic_vector(11 downto 0);
    signal is_inside_x: std_logic := '0';
    signal is_inside_y: std_logic := '0';
    signal is_inside_cnt_x: std_logic := '0';
    signal is_inside_cnt_y: std_logic := '0';
    signal overflowed: std_logic := '0';
    signal rgb: std_logic_vector(8 downto 0);
        alias red:   std_logic_vector(2 downto 0) is rgb(8 downto 6);
        alias green: std_logic_vector(2 downto 0) is rgb(5 downto 3);
        alias blue:  std_logic_vector(2 downto 0) is rgb(2 downto 0);

begin

    --------------
    -- KEYBOARD --
    --------------
    instance_keyboard : keyboard_controller
        port map (CLK, RESET, keyboard_raw, open, KIN, KOUT);

    keyboard_fsm_sync : process (RESET, CLK)
    begin
        if (RESET = '1') then
            pstate <= s_wfpress;
        elsif rising_edge(CLK) then
            pstate <= nstate;
        end if;
    end process;

    keyboard_fsm_output_nextstate : process (pstate, keyboard_raw)
    begin
        keyboard <= (others => '0');
        case pstate is
            when s_wfpress =>
                nstate <= s_wfpress;
                if keyboard_raw /= "0000000000000000" then
                    keyboard <= keyboard_raw;
                    nstate <= s_wfrelease;
                end if;
            when s_wfrelease =>
                nstate <= s_wfrelease;
                if keyboard_raw = "0000000000000000" then
                    nstate <= s_wfpress;
                end if;
            when others =>
                nstate <= s_wfpress;
        end case ;
    end process;

    -----------------
    -- CELL MATRIX --
    -----------------
    matrix_rows : for y in 0 to 3 generate
        matrix_cols : for x in 0 to 3 generate
            instance_cell : cell
                generic map (getmask(x, y, 4, 4))
                port map (
                    CLK   => CLK,
                    RESET => game_reset,
                    
                    INIT_STATE  => init_state(state_position(x, y) to state_position(x, y) + 3),
                    KEYS        => game_arrow_keys,
                    NEIG_TOP    => game_state(state_position(x, (y - 1) mod 4) to state_position(x, (y - 1) mod 4) + 3),
                    NEIG_LEFT   => game_state(state_position((x - 1) mod 4, y) to state_position((x - 1) mod 4, y) + 3),
                    NEIG_RIGHT  => game_state(state_position((x + 1) mod 4, y) to state_position((x + 1) mod 4, y) + 3),
                    NEIG_BOTTOM => game_state(state_position(x, (y + 1) mod 4) to state_position(x, (y + 1) mod 4) + 3),
                    
                    STATE => game_state(state_position(x, y) to state_position(x, y) + 3)
                );
        end generate ; -- matrix_cols
    end generate ; -- matrix_rows

    init_state <=
        init_state_A when keyboard(12) = '1' else
        init_state_B when keyboard(13) = '1' else
        init_state_C when keyboard(14) = '1' else
        init_state_D;
    game_reset <= '1' when RESET = '1' or keyboard(15 downto 12) /= "0000" else '0';
    arrow_keys <= keyboard(6) & keyboard(9) & keyboard(1) & keyboard(4);
    game_arrow_keys <= "0000" when game_state = "0001001000110100010101100111100010011010101111001101111011110000" else arrow_keys;

    ------------------
    -- BCD COUNTERS --
    ------------------
    bcd_array : for i in 0 to 2 generate
        instance_bcd : bcd_counter
            port map (CLK, game_reset, ovfs(i + 1), moves(i * 4 + 3 downto i * 4), ovfs(i));
    end generate;

    add_move <= '1' when game_arrow_keys /= "0000" else '0';

    ----------------
    -- SYMBOL ROM --
    ----------------
    instance_rom : symbol_rom
        port map (address, col_rom, row_rom, data);

    game_column <= column - 192;
    game_row <= row - 112;
    col_rom <= game_column(4 downto 2) when is_inside_cnt_y = '1' else game_column(5 downto 3);
    row_rom <= game_row(4 downto 2) when is_inside_cnt_y = '1' else game_row(5 downto 3);
    address_sel <= "00" & game_column(6 downto 5) when is_inside_cnt_y = '1' else game_row(7 downto 6) & game_column(7 downto 6);
    address <= moves(conv_integer(address_sel) * 4 + 3 downto conv_integer(address_sel) * 4) when is_inside_cnt_y = '1' else game_state(conv_integer(address_sel) * 4 to conv_integer(address_sel) * 4 + 3);

    -----------------
    -- VGA DRAWING --
    -----------------
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
            
            -- columns 192 to 448 are inside the game space, the signal is set one period earlier
            if column = "000010111111" then
                is_inside_x <= '1';
                is_inside_cnt_x <= '1';
            elsif column = "000110111111" then 
                is_inside_x <= '0';
            end if;
            -- column 288 is outside the move counter space
            if column = 287 then
                is_inside_cnt_x <= '0';
            end if;
            -- rows 112 to 368 are inside the game space, the signal is set one period earlier
            if row = "000001101111" then
                is_inside_y <= '1';
            elsif row = "000101101111" then 
                is_inside_y <= '0';
                is_inside_cnt_y <= '1';
            end if;
            -- row 400 is outside the move counter space
            if row = 399 then
                is_inside_cnt_y <= '0';
            end if;

            if overflow = '1' then
                overflowed <= '1';
            end if;
            if game_reset = '1' then
                overflowed <= '0';
            end if;

            -- both logic of and reaction to the is_inside signals is synchronous, thus the change will take effect in the next period
            if is_inside_x = '1' and is_inside_y = '1' then
                if data = '1' then
                    red   <= "000";
                    green <= "111";
                    blue  <= "000";
                else
                    rgb <= (others => '1');
                end if;
            end if;

            -- move counter
            if is_inside_cnt_x = '1' and is_inside_cnt_y = '1' and overflowed = '0' then
                if data = '1' then
                    rgb <= (others => '1');
                else
                    rgb <= (others => '0');
                end if;
            end if;
        end if;
    end process draw;

end Main;
