-- IVH Project: Patnáctka
-- Author: xcibak00 - Michal Cibák

library IEEE;
use IEEE.std_logic_1164.all;
-- use IEEE.numeric_std.all; -- for modulo operations etc.
-- use IEEE.math_real."ceil";
-- use IEEE.math_real."log2";
use work.game_pack.all;


entity cell_tb is
end cell_tb;


architecture Behavior of cell_tb is
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

    signal CLK: std_logic := '0';
    signal RESET: std_logic := '0';
    signal INIT_STATE: std_logic_vector(3 downto 0) := (others => '0');
    signal KEYS: std_logic_vector(3 downto 0) := (others => '0');
    signal NEIG_TOP: std_logic_vector(3 downto 0) := (others => '0');
    signal NEIG_LEFT: std_logic_vector(3 downto 0) := (others => '0');
    signal NEIG_RIGHT: std_logic_vector(3 downto 0) := (others => '0');
    signal NEIG_BOTTOM: std_logic_vector(3 downto 0) := (others => '0');
    signal STATE_MID: std_logic_vector(3 downto 0);
    signal STATE_CORN: std_logic_vector(3 downto 0);

    constant MASK_MID : mask_t := getmask(1, 2, 4, 4); -- (others => '1')
    constant MASK_CORN : mask_t := getmask(0, 0, 4, 4); -- TOP 0, LEFT 0, RIGHT 1, BOTTOM 1;
    constant CLK_period : time := 10 ns;
    constant CLK_half_period : time := CLK_period / 2;

    -- constant COLUMN : natural := 4;
    -- constant ROW : natural := 4;
    -- constant size : natural := natural(ceil(log2(real(a)))); -- changes in ROM needed as well in case it isn't equal to 4
    -- signal game_state: std_logic_vector(ROWS * COLS * size – 1 downto 0);
    -- signal game_state: std_logic_vector(0 to 63) := "0000000100100011010001010110011110001001101010111100110111101111";

begin

    instance_cell_mid : cell
        generic map (MASK_MID)
        port map (CLK, RESET, INIT_STATE, KEYS, NEIG_TOP, NEIG_LEFT, NEIG_RIGHT, NEIG_BOTTOM, STATE_MID);

    instance_cell_corn : cell
        generic map (MASK_CORN)
        port map (CLK, RESET, INIT_STATE, KEYS, NEIG_TOP, NEIG_LEFT, NEIG_RIGHT, NEIG_BOTTOM, STATE_CORN);

    CLK_gen : process
    begin
        CLK <= '0';
        wait for CLK_half_period;
        CLK <= '1';
        wait for CLK_half_period;
    end process;

    tb : process
    begin
        wait for CLK_half_period;
        -- CLK is now on its rising edge

        -- ----------- --
        -- MIDDLE CELL --
        -- ----------- --

        -- RESET, initialize to 15, reinitialize to 8
        -- test to see if:
            -- state changes after rising edge of clock
        INIT_STATE <= "1111";
        RESET <= '1';
        wait for CLK_half_period;
        assert (STATE_MID = "1111") report "State of cell should already be 15!" severity error;

            -- state stays the same after period of clock
        wait for CLK_period;
        assert (STATE_MID = "1111") report "State changed after period of clock with reset signal still on!" severity error;

        wait for CLK_half_period;
            -- state stays the same even after reset changes back to 0
        RESET <= '0';
        wait for CLK_half_period;
        assert (STATE_MID = "1111") report "State changed after reset signal changed to 0!" severity error;

        wait for CLK_half_period;
            -- state doesn't change to INIT_STATE while reset isn't active
        INIT_STATE <= "1000";
        wait for CLK_half_period;
        assert (STATE_MID = "1111") report "State reinitialized while reset signal was 0!" severity error;
        wait for CLK_half_period;
        RESET <= '1';
        wait for CLK_half_period;
        assert (STATE_MID = "1000") report "State didn't reinitialize!" severity error;

        wait for CLK_half_period;
        RESET <= '0';
        wait for CLK_period;
        -- STATE is 8, CLK is on its rising edge, all neighbors are holes (default values)

        -- RIGHT key, cell changes to hole
        KEYS <= "0100";
        wait for CLK_period;
        assert (STATE_MID = "0000") report "Cell didn't change into hole!" severity error;

        -- NO key, all neighbors are numbers, cell stays hole
        KEYS <= "0000";
        NEIG_TOP <= "0001";
        NEIG_LEFT <= "0010";
        NEIG_RIGHT <= "0011";
        NEIG_BOTTOM <= "0100";
        wait for CLK_period;
        assert (STATE_MID = "0000") report "Cell changed from hole!" severity error;

        -- DOWN key, TOP neighbor is number 1, cell changes to 1
        KEYS <= "1000";
        wait for CLK_period;
        assert (STATE_MID = "0001") report "Cell didn't change into its top neighbor!" severity error;

        -- still DOWN key, TOP neighbor is now hole, cell doesn't change from 1
        KEYS <= "1000";
        NEIG_TOP <= "0000";
        wait for CLK_period;
        assert (STATE_MID = "0001") report "Cell didn't stay the same!" severity error;

        -- LEFT key, neighbor is number, cell doesn't change from 1
        KEYS <= "0010";
        wait for CLK_period;
        assert (STATE_MID = "0001") report "Cell didn't stay the same!" severity error;

        -- UP key, TOP neighbor is hole, cell changes back to hole
        KEYS <= "0001";
        wait for CLK_period;
        assert (STATE_MID = "0000") report "Cell didn't change into hole!" severity error;

        -- RESET and RIGHT key, cell changes into 8 and not 3
        RESET <= '1';
        KEYS <= "0100";
        wait for CLK_period;
        assert (STATE_MID = "1000") report "Cell didn't reinitialize!" severity error;

        -- ----------- --
        -- CORNER CELL --
        -- ----------- --
        RESET <= '0';
        KEYS <= "0000";
        NEIG_TOP <= "1001";
        NEIG_LEFT <= "0000";
        NEIG_RIGHT <= "1010";
        NEIG_BOTTOM <= "0000";
        wait for CLK_period;
        -- STATE is 8, number is on TOP and RIGHT, hole is on LEFT and BOTTOM, cell has only RIGHT and BOTTOM neighbor

        -- RIGHT key, cell doesn't change (number on right side)
        KEYS <= "0100";
        wait for CLK_period;
        assert (STATE_CORN = "1000") report "Cell changed from number 8, but its right neighbor is a number!" severity error;

        -- LEFT key, cell doesn't change (hole on left side, but it isn't a neighbor)
        KEYS <= "0010";
        wait for CLK_period;
        assert (STATE_CORN = "1000") report "Cell changed from number 8, but it has no neighbor on its left!" severity error;

        -- BOTTOM key, cell changes into hole
        KEYS <= "1000";
        wait for CLK_period;
        assert (STATE_CORN = "0000") report "Cell didn't change into hole!" severity error;

        -- BOTTOM key, cell stays hole (number on top side, but it isn't a neighbor)
        KEYS <= "1000";
        NEIG_BOTTOM <= "1000";
        wait for CLK_period;
        assert (STATE_CORN = "0000") report "Cell changed from hole, but it has no neighbor on its top side!" severity error;

        -- LEFT key, cell changes into number 10
        KEYS <= "0010";
        wait for CLK_period;
        assert (STATE_CORN = "1010") report "Cell didn't change into its right neighbor, number 10!" severity error;

        -- LEFT key again, cell doesn't change (again, hole on left side, but it isn't a neighbor)
        KEYS <= "0010";
        NEIG_RIGHT <= "0000";
        wait for CLK_period;
        assert (STATE_CORN = "1010") report "Cell changed from number 10, but it has no neighbor on its left!" severity error;

        -- TOP key, cell doesn't change (no neighbor on top)
        KEYS <= "0001";
        wait for CLK_period;
        assert (STATE_CORN = "1010") report "Cell changed from number 10, but it has no neighbor on its top side!" severity error;

        -- RIGHT key, cell changes back into hole
        KEYS <= "0100";
        wait for CLK_period;
        assert (STATE_CORN = "0000") report "Cell didn't change back into hole!" severity error;

        KEYS <= "0000";
        wait;
    end process;

end Behavior;
