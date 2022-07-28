-- Autor reseni: Michal Cibák - xcibak00

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use IEEE.std_logic_unsigned.all;

entity ledc8x8 is
port (
    SMCLK : in std_logic;
    RESET : in std_logic;

    ROW : out std_logic_vector(7 downto 0);
    LED : out std_logic_vector(0 to 7)
);
end ledc8x8;

architecture main of ledc8x8 is

    type t_state is (s_short_show, s_blank, s_long_show);
    signal pstate, nstate: t_state;

    signal blank: std_logic;

    signal num_f: std_logic_vector(7 downto 0);
    signal num_hs: std_logic_vector(13 downto 0);
    signal of_f, of_hs: std_logic;

    signal rol_num: std_logic_vector(7 downto 0);
    
    signal addr: std_logic_vector(2 downto 0);
    signal data: std_logic_vector(7 downto 0);

    type memory_t is array(0 to 7) of std_logic_vector(7 downto 0);
    constant mem: memory_t := (
        "01110111",
        "00100111",
        "01010111",
        "01110001",
        "01110110",
        "11110111",
        "11110110",
        "11111001"
    );

begin

    fsm_sync : process (RESET, SMCLK)
    begin
        if RESET = '1' then
            pstate <= s_short_show;
        elsif rising_edge(SMCLK) then
            pstate <= nstate;
        end if;
    end process;

    fsm_output_next : process (pstate, of_hs)
    begin
        blank <= '0';
        case pstate is
            when s_short_show =>
                nstate <= s_short_show;
                if of_hs = '1' then
                    nstate <= s_blank;
                end if;
            when s_blank =>
                nstate <= s_blank;
                blank <= '1';
                if of_hs = '1' then
                    nstate <= s_long_show;
                end if;
            when s_long_show =>
                nstate <= s_long_show;
            when others =>
                nstate <= s_short_show;
        end case;
    end process;

    cnt_f : process (RESET, SMCLK) -- counter for frequency
    begin
        if RESET = '1' then
            num_f <= (others => '0');
            of_f <= '0';
        elsif rising_edge(SMCLK) then
            of_f <= '0';
            num_f <= num_f + 1;
            if num_f = "11111111" then
                of_f <= '1';
            end if;
        end if;
    end process; -- cnt_f

    cnt_hs : process (RESET, SMCLK) -- counter for time - counts half a second
    begin
        if RESET = '1' then
            num_hs <= (others => '0');
            of_hs <= '0';
        elsif rising_edge(SMCLK) then
            of_hs <= '0';
            if of_f = '1' then -- EN
                num_hs <= num_hs + 1;
                if num_hs = "11100000111111" then
                    num_hs <= "00000000000000";
                    of_hs <= '1';
                end if;
            end if;
        end if;
    end process; -- cnt_hs

    rol_reg : process (RESET, SMCLK)
    begin
        if RESET = '1' then
            rol_num <= "00000001";
        elsif rising_edge(SMCLK) then
            if of_f = '1' then -- CE
                rol_num <= rol_num(6 downto 0) & rol_num(7);
            end if;
        end if;
    end process; -- rol_reg

    addr <= "000" when rol_num = "00000001" else
            "001" when rol_num = "00000010" else
            "010" when rol_num = "00000100" else
            "011" when rol_num = "00001000" else
            "100" when rol_num = "00010000" else
            "101" when rol_num = "00100000" else
            "110" when rol_num = "01000000" else
            "111" when rol_num = "10000000" else
            "000";

    data <= mem(conv_integer(addr));

    ROW <= rol_num;

    LED <= "11111111" when blank = '1' else data;

end main;




-- ISID: 75579
