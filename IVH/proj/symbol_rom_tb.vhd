-- IVH Project: Patnáctka
-- Author: xcibak00 - Michal Cibák

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith."conv_std_logic_vector";


entity symbol_rom_tb is
end symbol_rom_tb;


architecture Behavior of symbol_rom_tb is
    component symbol_rom
    port(
        ADDRESS : in  std_logic_vector(3 downto 0);
        COLUMN  : in  std_logic_vector(2 downto 0);
        ROW     : in  std_logic_vector(2 downto 0);

        DATA    : out  std_logic
    );
    end component;

    signal ADDRESS: std_logic_vector(3 downto 0) := (others => '0');
    signal COLUMN: std_logic_vector(2 downto 0) := (others => '0');
    signal ROW: std_logic_vector(2 downto 0) := (others => '0');
    signal DATA: std_logic;

    type t_coordinates is array(0 to 1) of integer range 0 to 7; -- ROW, COLUMN
    type t_four_coordinates is array(3 downto 0) of t_coordinates;
    constant coordinates: t_four_coordinates := ((1, 3), (2, 5), (3, 3), (4, 2)); -- 4 pixels which are enough to differentiate 8 from other numbers

begin

    instance_rom : symbol_rom
        port map (ADDRESS, COLUMN, ROW, DATA);

    tb : process
    begin
        -- light of pixel on ROW 3 and COLUMN 3 is OFF for numbers other than 5, 6, 8, 9
        ROW <= "011";
        COLUMN <= "011";
        for number in 0 to 15 loop
            ADDRESS <= conv_std_logic_vector(number, 4);
            wait for 5 ns;

            case number is
                when 5 | 6 | 8 | 9 =>
                    assert (DATA = '1') report "This pixel should be ON!" severity error;
                when others =>
                    assert (DATA = '0') report "This pixel should be OFF!" severity error;
                end case;

            wait for 5 ns;
        end loop;

        -- light of pixels at all "coordinates" is ON for number 8 only (every other number has at least one light OFF):
        ADDRESS <= "1000";
        for index in coordinates'range loop
            ROW <= conv_std_logic_vector(coordinates(index)(0), 3);
            COLUMN <= conv_std_logic_vector(coordinates(index)(1), 3);
            wait for 5 ns;

            assert (DATA = '1') report "This isn't number 8!" severity error;

            wait for 5 ns;
        end loop;

        wait;
    end process tb;

end Behavior;
