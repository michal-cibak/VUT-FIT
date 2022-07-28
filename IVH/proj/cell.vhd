-- IVH Project: Patnáctka
-- Author: xcibak00 - Michal Cibák

library IEEE;
use IEEE.std_logic_1164.all;
use work.game_pack.all;


entity cell is
    generic (
        MASK : mask_t := (others => '1')
    );

    port (
        CLK         : in  std_logic;
        RESET       : in  std_logic; -- synchronous

        INIT_STATE  : in  std_logic_vector(3 downto 0);
        KEYS        : in  std_logic_vector(3 downto 0);
        NEIG_TOP    : in  std_logic_vector(3 downto 0);
        NEIG_LEFT   : in  std_logic_vector(3 downto 0);
        NEIG_RIGHT  : in  std_logic_vector(3 downto 0);
        NEIG_BOTTOM : in  std_logic_vector(3 downto 0);

        STATE       : out std_logic_vector(3 downto 0)
    );
end cell;


architecture Behavioral of cell is
    constant IDX_TOP    : NATURAL := 0; -- index of bit in signal KEYS corresponding to UP key
    constant IDX_LEFT   : NATURAL := 1; -- index of bit in signal KEYS corresponding to LEFT key
    constant IDX_RIGHT  : NATURAL := 2; -- index of bit in signal KEYS corresponding to RIGHT key
    constant IDX_BOTTOM : NATURAL := 3; -- index of bit in signal KEYS corresponding to DOWN key

    signal state_reg: std_logic_vector(3 downto 0) := "0000";

begin

    logic : process(CLK) -- RESET isn't needed in sensitivity list for synchronous reset
    begin
        if rising_edge(CLK) then
            if RESET = '1' then
                state_reg <= INIT_STATE;
            else
                if state_reg = "0000" then -- HOLE
                    if KEYS(IDX_TOP) = '1' then
                        if MASK.bottom = '1' then
                            state_reg <= NEIG_BOTTOM;
                        end if;
                    elsif KEYS(IDX_LEFT) = '1' then
                        if MASK.right = '1' then
                            state_reg <= NEIG_RIGHT;
                        end if;
                    elsif KEYS(IDX_RIGHT) = '1' then
                        if MASK.left = '1' then
                            state_reg <= NEIG_LEFT;
                        end if;
                    elsif KEYS(IDX_BOTTOM) = '1' then
                        if MASK.top = '1' then
                            state_reg <= NEIG_TOP;
                        end if;
                    end if;
                else -- NUMBER
                    if KEYS(IDX_TOP) = '1' then
                        if MASK.top = '1' and NEIG_TOP = "0000" then
                            state_reg <= NEIG_TOP;
                        end if;
                    elsif KEYS(IDX_LEFT) = '1' then
                        if MASK.left = '1' and NEIG_LEFT = "0000" then
                            state_reg <= NEIG_LEFT;
                        end if;
                    elsif KEYS(IDX_RIGHT) = '1' then
                        if MASK.right = '1' and NEIG_RIGHT = "0000" then
                            state_reg <= NEIG_RIGHT;
                        end if;
                    elsif KEYS(IDX_BOTTOM) = '1' then
                        if MASK.bottom = '1' and NEIG_BOTTOM = "0000" then
                            state_reg <= NEIG_BOTTOM;
                        end if;
                    end if;
                end if; -- CELL is a hole or a number
            end if; -- RESET or (maybe) KEY
        end if; -- CLK
    end process logic;

    STATE <= state_reg;

end Behavioral;
