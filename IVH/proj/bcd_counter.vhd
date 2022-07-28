-- IVH Project: Patnáctka
-- Author: xcibak00 - Michal Cibák

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use IEEE.std_logic_unsigned.all;


entity bcd_counter is
    port (
        CLK      :  in std_logic;
        RESET    :  in std_logic; -- synchronous
        CE       :  in std_logic;

        NUMBER   : out std_logic_vector(3 downto 0);
        OVERFLOW : out std_logic
    );
end bcd_counter;


architecture Behavioral of bcd_counter is
    signal num: std_logic_vector(3 downto 0) := "0000";
    signal ovf: std_logic := '0';

begin

    count : process(CLK)
    begin
        if rising_edge(CLK) then
            ovf <= '0';
            if RESET = '1' then
                num <= "0000";
            elsif CE = '1' then
                num <= num + 1;
                if num <= "1001" then
                    num <= "0000";
                    ovf <= '1';
                end if ;
            end if;
        end if ;
    end process count;

    NUMBER <= num;
    OVERFLOW <= ovf;

end Behavioral;
