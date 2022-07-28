library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use IEEE.std_logic_unsigned.all;

entity ledc8x8 is
port (
    -- Sem doplnte popis rozhrani obvodu.
    SMCLK : in std_logic;
    RESET : in std_logic;

    ROW : out std_logic_vector(7 downto 0);
    LED : out std_logic_vector(7 downto 0)
);
end ledc8x8;

architecture main of ledc8x8 is
    -- Sem doplnte definice vnitrnich signalu.
    signal row_freq, led_row, led_column : std_logic_vector(7 downto 0);
    signal state : std_logic_vector(1 downto 0);
    signal state_freq : std_logic_vector(12 downto 0);
    signal row_state : std_logic_vector(2 downto 0);

begin
    -- Sem doplnte popis obvodu. Doporuceni: pouzivejte zakladni obvodove prvky
    -- (multiplexory, registry, dekodery,...), jejich funkce popisujte pomoci
    -- procesu VHDL a propojeni techto prvku, tj. komunikaci mezi procesy,
    -- realizujte pomoci vnitrnich signalu deklarovanych vyse.

    -- DODRZUJTE ZASADY PSANI SYNTETIZOVATELNEHO VHDL KODU OBVODOVYCH PRVKU,
    -- JEZ JSOU PROBIRANY ZEJMENA NA UVODNICH CVICENI INP A SHRNUTY NA WEBU:
    -- http://merlin.fit.vutbr.cz/FITkit/docs/navody/synth_templates.html.

    -- Nezapomente take doplnit mapovani signalu rozhrani na piny FPGA
    -- v souboru ledc8x8.ucf.

--perioda zmeny riadku - 256 period SMCLK
row_frequency_counter : process(RESET, SMCLK)
begin
    if (RESET = '1') then
        row_freq <= "00000000";
    elsif (SMCLK'event) and (SMCLK = '1') then
        row_freq <= row_freq + 1;
    end if;
end process;

--register stavu (zobrazovaneho pismena) - kazdu periodu zmeny stavu
state_logic : process(RESET, SMCLK)
begin
    if (RESET = '1') then
        state <= "00";
    elsif (SMCLK'event) and (SMCLK = '1') then
        if (state_freq = "0000000011111") then
            state <= state + 1;
        end if;
    end if;
end process;

--posuvny register riadkov displeja - kazdu periodu zmeny riadku
led_row_logic : process(RESET, SMCLK)
begin
    if (RESET = '1') then
        led_row <= "00000001";
    elsif (SMCLK'event) and (SMCLK = '1') then
        if (row_freq = "11111111") then
            led_row <= led_row(6 downto 0) & led_row(7);
        end if;
    end if;
end process;

--register riadkov displeja
row_logic : process(RESET, SMCLK)
begin
    if (RESET = '1') then
        row_state <= "000";
    elsif (SMCLK'event) and (SMCLK = '1') then
        if (row_freq = "11111111") then
            row_state <= row_state + 1;
        end if;
    end if;
end process;

--perioda zmeny znaku - 1/4 sekundy - 7200 zmien riadku na znak
--7372800 Hz / 256 = 28800 Hz
--28800 Hz / 4 = 7200 Hz
state_frequency_counter : process(RESET, SMCLK)
begin
    if (RESET = '1') then
        state_freq <= "0000000000000";
    elsif (SMCLK'event) and (SMCLK = '1') then
        if (row_freq = "11111111") then
            if (state_freq = "0000000011111") then
                state_freq <= "0000000000000";
            else
                state_freq <= state_freq + 1;
            end if;
        end if;
    end if;
end process;

--priradenie lediek riadku
leds : process(row_state, state)
begin
    led_column <= "11111111";

    if (state = "00") then
        if (row_state="000") then
            led_column <= "01110111";
        elsif (row_state="001") then
            led_column <= "00100111";
        elsif (row_state="010") then
            led_column <= "01010111";
        elsif (row_state="010") then
            led_column <= "01110111";
        elsif (row_state="011") then
            led_column <= "01110111";
        end if;
    elsif (state = "10") then
        if (row_state="000") then
            led_column <= "10001111";
        elsif (row_state="001") then
            led_column <= "01110111";
        elsif (row_state="010") then
            led_column <= "11110111";
        elsif (row_state="010") then
            led_column <= "01110111";
        elsif (row_state="011") then
            led_column <= "10001111";
        end if;
	end if;
end process;

--vystup
output_logic : process(led_row, led_column)
begin
    ROW <= led_row;
    LED <= led_column;
end process;

end main;
