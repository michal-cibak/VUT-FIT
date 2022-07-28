-- fsm.vhd: Finite State Machine
-- Author: xcibak00 - Michal Cibák
--
library IEEE;
use IEEE.std_logic_1164.all;
-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity fsm is
port(
    CLK         : in  std_logic;
    RESET       : in  std_logic;

    -- Input signals
    KEY         : in  std_logic_vector(15 downto 0);
    CNT_OF      : in  std_logic;

    -- Output signals
    FSM_CNT_CE  : out std_logic;
    FSM_MX_MEM  : out std_logic;
    FSM_MX_LCD  : out std_logic;
    FSM_LCD_WR  : out std_logic;
    FSM_LCD_CLR : out std_logic
);
end entity fsm;

-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of fsm is
    type t_state is (
        START,
        FIRST, SECOND, THIRD, FOURTH, FIFTH, SIXTH1, SIXTH2, SEVENTH1, SEVENTH2, EIGHTH1, EIGHTH2, NINTH1, NINTH2, TENTH,
        ERR1, ERR2, ERR3, ERR4, ERR5, ERR6, ERR7, ERR8, ERR9, ERR10,
        PRINT_MESSAGE_C, PRINT_MESSAGE_F,
        FINISH
        );
    signal present_state, next_state: t_state;

begin
-- -------------------------------------------------------
sync_logic : process(RESET, CLK)
begin
    if RESET = '1' then
        present_state <= START;
    elsif CLK'event and CLK = '1' then
        present_state <= next_state;
    end if;
end process sync_logic;
-- -------------------------------------------------------

-- -------------------------------------------------------
next_state_logic : process(present_state, KEY, CNT_OF)
begin
    case present_state is
    -- - - - - - - - - - - - - - - - - - - - - - -
    when PRINT_MESSAGE_F =>
        next_state <= PRINT_MESSAGE_F;
        if CNT_OF = '1' then
            next_state <= FINISH;
        end if;
    -- - - - - - - - - - - - - - - - - - - - - - -
    when PRINT_MESSAGE_C =>
        next_state <= PRINT_MESSAGE_C;
        if CNT_OF = '1' then
            next_state <= FINISH;
        end if;
    -- - - - - - - - - - - - - - - - - - - - - - -
    when FINISH =>
        next_state <= FINISH;
        if KEY(15) = '1' then
            next_state <= START;
        end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
    when START =>
        next_state <= START;
        if KEY /= "0000000000000000" then
            if KEY(15) = '1' then
                next_state <= PRINT_MESSAGE_F;
            elsif KEY(1) = '1' then
                next_state <= FIRST;
            else
                next_state <= ERR1;
            end if;
        end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
    when ERR1 =>
        next_state <= ERR1;
        if KEY /= "0000000000000000" then
            if KEY(15) = '1' then
                next_state <= PRINT_MESSAGE_F;
            else
                next_state <= ERR2;
            end if;
        end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
    when FIRST =>
        next_state <= FIRST;
        if KEY /= "0000000000000000" then
            if KEY(15) = '1' then
                next_state <= PRINT_MESSAGE_F;
            elsif KEY(3) = '1' then
                next_state <= SECOND;
            else
                next_state <= ERR2;
            end if;
        end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
    when ERR2 =>
        next_state <= ERR2;
        if KEY /= "0000000000000000" then
            if KEY(15) = '1' then
                next_state <= PRINT_MESSAGE_F;
            else
                next_state <= ERR3;
            end if;
        end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
    when SECOND =>
        next_state <= SECOND;
        if KEY /= "0000000000000000" then
            if KEY(15) = '1' then
                next_state <= PRINT_MESSAGE_F;
            elsif KEY(8) = '1' then
                next_state <= THIRD;
            else
                next_state <= ERR3;
            end if;
        end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
    when ERR3 =>
        next_state <= ERR3;
        if KEY /= "0000000000000000" then
            if KEY(15) = '1' then
                next_state <= PRINT_MESSAGE_F;
            else
                next_state <= ERR4;
            end if;
        end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
    when THIRD =>
        next_state <= THIRD;
        if KEY /= "0000000000000000" then
            if KEY(15) = '1' then
                next_state <= PRINT_MESSAGE_F;
            elsif KEY(0) = '1' then
                next_state <= FOURTH;
            else
                next_state <= ERR4;
            end if;
        end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
    when ERR4 =>
        next_state <= ERR4;
        if KEY /= "0000000000000000" then
            if KEY(15) = '1' then
                next_state <= PRINT_MESSAGE_F;
            else
                next_state <= ERR5;
            end if;
        end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
    when FOURTH =>
        next_state <= FOURTH;
        if KEY /= "0000000000000000" then
            if KEY(15) = '1' then
                next_state <= PRINT_MESSAGE_F;
            elsif KEY(9) = '1' then
                next_state <= FIFTH;
            else
                next_state <= ERR5;
            end if;
        end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
    when ERR5 =>
        next_state <= ERR5;
        if KEY /= "0000000000000000" then
            if KEY(15) = '1' then
                next_state <= PRINT_MESSAGE_F;
            else
                next_state <= ERR6;
            end if;
        end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
    when FIFTH =>
        next_state <= FIFTH;
        if KEY /= "0000000000000000" then
            if KEY(15) = '1' then
                next_state <= PRINT_MESSAGE_F;
            elsif KEY(1) = '1' then
                next_state <= SIXTH1;
            elsif KEY(6) = '1' then
                next_state <= SIXTH2;
            else
                next_state <= ERR6;
            end if;
        end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
    when ERR6 =>
        next_state <= ERR6;
        if KEY /= "0000000000000000" then
            if KEY(15) = '1' then
                next_state <= PRINT_MESSAGE_F;
            else
                next_state <= ERR7;
            end if;
        end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
    when SIXTH1 =>
        next_state <= SIXTH1;
        if KEY /= "0000000000000000" then
            if KEY(15) = '1' then
                next_state <= PRINT_MESSAGE_F;
            elsif KEY(9) = '1' then
                next_state <= SEVENTH1;
            else
                next_state <= ERR7;
            end if;
        end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
    when SIXTH2 =>
        next_state <= SIXTH2;
        if KEY /= "0000000000000000" then
            if KEY(15) = '1' then
                next_state <= PRINT_MESSAGE_F;
            elsif KEY(4) = '1' then
                next_state <= SEVENTH2;
            else
                next_state <= ERR7;
            end if;
        end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
    when ERR7 =>
        next_state <= ERR7;
        if KEY /= "0000000000000000" then
            if KEY(15) = '1' then
                next_state <= PRINT_MESSAGE_F;
            else
                next_state <= ERR8;
            end if;
        end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
    when SEVENTH1 =>
        next_state <= SEVENTH1;
        if KEY /= "0000000000000000" then
            if KEY(15) = '1' then
                next_state <= PRINT_MESSAGE_F;
            elsif KEY(2) = '1' then
                next_state <= EIGHTH1;
            else
                next_state <= ERR8;
            end if;
        end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
    when SEVENTH2 =>
        next_state <= SEVENTH2;
        if KEY /= "0000000000000000" then
            if KEY(15) = '1' then
                next_state <= PRINT_MESSAGE_F;
            elsif KEY(2) = '1' then
                next_state <= EIGHTH2;
            else
                next_state <= ERR8;
            end if;
        end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
    when ERR8 =>
        next_state <= ERR8;
        if KEY /= "0000000000000000" then
            if KEY(15) = '1' then
                next_state <= PRINT_MESSAGE_F;
            else
                next_state <= ERR9;
            end if;
        end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
    when EIGHTH1 =>
        next_state <= EIGHTH1;
        if KEY /= "0000000000000000" then
            if KEY(15) = '1' then
                next_state <= PRINT_MESSAGE_F;
            elsif KEY(7) = '1' then
                next_state <= NINTH1;
            else
                next_state <= ERR9;
            end if;
        end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
    when EIGHTH2 =>
        next_state <= EIGHTH2;
        if KEY /= "0000000000000000" then
            if KEY(15) = '1' then
                next_state <= PRINT_MESSAGE_F;
            elsif KEY(5) = '1' then
                next_state <= NINTH2;
            else
                next_state <= ERR9;
            end if;
        end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
    when ERR9 =>
        next_state <= ERR9;
        if KEY /= "0000000000000000" then
            if KEY(15) = '1' then
                next_state <= PRINT_MESSAGE_F;
            else
                next_state <= ERR10;
            end if;
        end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
    when NINTH1 =>
        next_state <= NINTH1;
        if KEY /= "0000000000000000" then
            if KEY(15) = '1' then
                next_state <= PRINT_MESSAGE_F;
            elsif KEY(5) = '1' then
                next_state <= TENTH;
            else
                next_state <= ERR10;
            end if;
        end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
    when NINTH2 =>
        next_state <= NINTH2;
        if KEY /= "0000000000000000" then
            if KEY(15) = '1' then
                next_state <= PRINT_MESSAGE_F;
            elsif KEY(0) = '1' then
                next_state <= TENTH;
            else
                next_state <= ERR10;
            end if;
        end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
    when ERR10 =>
        next_state <= ERR10;
        if KEY(15) = '1' then
            next_state <= PRINT_MESSAGE_F;
        end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
    when TENTH =>
        next_state <= TENTH;
        if KEY(15) = '1' then
            next_state <= PRINT_MESSAGE_C;
        end if;
    -- - - - - - - - - - - - - - - - - - - - - - -
    when others =>
        next_state <= START;
    -- - - - - - - - - - - - - - - - - - - - - - -
    end case;
end process next_state_logic;
-- -------------------------------------------------------

-- -------------------------------------------------------
output_logic : process(present_state, KEY)
begin
    FSM_CNT_CE  <= '0';
    FSM_MX_MEM  <= '0';
    FSM_MX_LCD  <= '0';
    FSM_LCD_WR  <= '0';
    FSM_LCD_CLR <= '0';

    case present_state is
    -- - - - - - - - - - - - - - - - - - - - - - -
    when PRINT_MESSAGE_F =>
        FSM_CNT_CE <= '1';
        FSM_MX_LCD <= '1';
        FSM_LCD_WR <= '1';
	-- - - - - - - - - - - - - - - - - - - - - - -
    when PRINT_MESSAGE_C =>
        FSM_CNT_CE <= '1';
		FSM_MX_MEM <= '1';
        FSM_MX_LCD <= '1';
        FSM_LCD_WR <= '1';
    -- - - - - - - - - - - - - - - - - - - - - - -
    when FINISH =>
        if KEY(15) = '1' then
            FSM_LCD_CLR <= '1';
        end if;
    -- - - - - - - - - - - - - - - - - - - - - - -
    when TENTH =>
        if KEY(15) = '1' then
			FSM_LCD_CLR <= '1';
        end if;
    -- - - - - - - - - - - - - - - - - - - - - - -
    when ERR10 =>
        if KEY(15) = '1' then
			FSM_LCD_CLR <= '1';
        end if;
    -- - - - - - - - - - - - - - - - - - - - - - -
    when others =>
		if KEY /= "0000000000000000" then
            if KEY(15) = '1' then
                FSM_LCD_CLR <= '1';
            else
                FSM_LCD_WR  <= '1';
            end if;
		end if;
    -- - - - - - - - - - - - - - - - - - - - - - -
    end case;
end process output_logic;
-- -------------------------------------------------------
end architecture behavioral;
