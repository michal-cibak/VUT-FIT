-- cpu.vhd: Simple 8-bit CPU (BrainF*ck interpreter)
-- Copyright (C) 2019 Brno University of Technology,
--                    Faculty of Information Technology
-- Author(s): xcibak00 - Michal Cibák
--

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity cpu is
 port (
   CLK   : in std_logic;  -- hodinovy signal
   RESET : in std_logic;  -- asynchronni reset procesoru
   EN    : in std_logic;  -- povoleni cinnosti procesoru

   -- synchronni pamet RAM
   DATA_ADDR  : out std_logic_vector(12 downto 0); -- adresa do pameti
   DATA_WDATA : out std_logic_vector(7 downto 0); -- mem[DATA_ADDR] <- DATA_WDATA pokud DATA_EN='1'
   DATA_RDATA : in std_logic_vector(7 downto 0);  -- DATA_RDATA <- ram[DATA_ADDR] pokud DATA_EN='1'
   DATA_RDWR  : out std_logic;                    -- cteni (0) / zapis (1)
   DATA_EN    : out std_logic;                    -- povoleni cinnosti

   -- vstupni port
   IN_DATA   : in std_logic_vector(7 downto 0);   -- IN_DATA <- stav klavesnice pokud IN_VLD='1' a IN_REQ='1'
   IN_VLD    : in std_logic;                      -- data platna
   IN_REQ    : out std_logic;                     -- pozadavek na vstup data

   -- vystupni port
   OUT_DATA : out  std_logic_vector(7 downto 0);  -- zapisovana data
   OUT_BUSY : in std_logic;                       -- LCD je zaneprazdnen (1), nelze zapisovat
   OUT_WE   : out std_logic                       -- LCD <- OUT_DATA pokud OUT_WE='1' a OUT_BUSY='0'
 );
end cpu;


-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of cpu is

    signal pc     : std_logic_vector(11 downto 0) := (others => '0'); -- address of instruction
    signal inc_pc : std_logic := '0';
    signal dec_pc : std_logic := '0';

    signal ptr     : std_logic_vector(11 downto 0) := (others => '0'); -- address of data
    signal inc_ptr : std_logic := '0';
    signal dec_ptr : std_logic := '0';

    signal sel_addr : std_logic := '0'; -- 0 = PC, 1 = PTR
        signal addr : std_logic_vector(12 downto 0) := (others => '0');
    signal sel_tmp  : std_logic := '0'; -- 0 = PC/PTR, 1 = TMP

    signal sel_data : std_logic_vector(1 downto 0) := "00";
        signal inced_data : std_logic_vector(7 downto 0) := (others => '0');
        signal deced_data : std_logic_vector(7 downto 0) := (others => '0');

    signal cnt     : std_logic_vector(11 downto 0) := (others => '0'); -- count of brackets (maybe change the size later)
    signal inc_cnt : std_logic := '0';
    signal dec_cnt : std_logic := '0';

    type t_state is (
        -- after CPU restart
        START,
        -- read instruction
        FETCH,
        -- decode instruction
        DECODE,
        -- > (increment data ptr)
        PTR_INC,
        -- < (decrement data ptr)
        PTR_DEC,
        -- + (increment data)
        DATA_INC_R, DATA_INC_W,
        -- - (decrement data)
        DATA_DEC_R, DATA_DEC_W,
        -- [ (loop start / skip instruction inside)
        OPEN_READ, OPEN_COMP,
        OPEN_FETCH, OPEN_DECODE,
        -- ] (loop end / go back to beginning)
        CLOSE_READ, CLOSE_COMP,
        CLOSE_FETCH, CLOSE_DECODE,
        -- . (print character)
        PRINT_READ, PRINT_WAIT, PRINT_WRITE,
        -- , (load character)
        READ_REQ, READ_WRITE,
        -- $ (load data into tmp)
        TMP_LOAD_R, TMP_LOAD_W,
        -- ! (stora data from tmp)
        TMP_STORE_R, TMP_STORE_W,
        -- null (end execution)
        FINISH
        );
    signal present_state, next_state : t_state;

begin

    ----------------
    -- DATA ROUTE --
    ----------------

    -- PC - program counter - address of current instruction in RAM
    instruction_counter : process (RESET, CLK)
    begin
        if RESET = '1' then
            pc <= (others => '0');
        elsif rising_edge(CLK) then
            if inc_pc = '1' then
                pc <= pc + 1;
            elsif dec_pc = '1' then
                pc <= pc - 1;
            end if;
        end if;
    end process;

    -- PTR - address of data in RAM
    data_counter : process (RESET, CLK)
    begin
        if RESET = '1' then
            ptr <= (others => '0');
        elsif rising_edge(CLK) then
            if inc_ptr = '1' then
                ptr <= ptr + 1;
            elsif dec_ptr = '1' then
                ptr <= ptr - 1;
            end if;
        end if;
    end process;

    -- MULTIPLEXER for selecting between address of instruction or address of data
    addr <= "0" & pc when sel_addr = '0' else "1" & ptr;

    -- MULTIPLEXER for selecting between instruction/data address or address of TMP
    DATA_ADDR <= addr when sel_tmp = '0' else "1000000000000";

    -- MULTIPLEXER for selecting the right data to be written into the RAM
    with sel_data select
    DATA_WDATA <=
        IN_DATA    when "00",
        deced_data when "01",
        inced_data when "10",
        DATA_RDATA when others;

    deced_data <= DATA_RDATA - 1;
    inced_data <= DATA_RDATA + 1;

    -- CNT - counter for while loops
    while_counter : process (RESET, CLK)
    begin
        if RESET = '1' then
            cnt <= (others => '0');
        elsif rising_edge(CLK) then
            if inc_cnt = '1' then
                cnt <= cnt + 1;
            elsif dec_cnt = '1' then
                cnt <= cnt - 1;
            end if;
        end if;
    end process;

    -------------------
    -- CONTROL ROUTE --
    -------------------

    -- FSM - state synchronization
    sync : process (RESET, CLK)
    begin
        if RESET = '1' then
            present_state <= START;
        elsif rising_edge(CLK) then
            if EN = '1' then
                present_state <= next_state;
            end if;
        end if;
    end process;

    -- FSM - logic of output and next state
    output_nstate : process (present_state, DATA_RDATA, cnt, OUT_BUSY, IN_VLD)
    begin
        DATA_RDWR <= '0';
        DATA_EN   <= '0';
        IN_REQ    <= '0';
        OUT_DATA  <= "00000000"; -- comment this line to create LATCH (if that is the desired funcionality)
        OUT_WE    <= '0';
        inc_pc    <= '0';
        dec_pc    <= '0';
        inc_ptr   <= '0';
        dec_ptr   <= '0';
        sel_addr  <= '0';
        sel_tmp   <= '0';
        sel_data  <= "00";
        inc_cnt   <= '0';
        dec_cnt   <= '0';

        case present_state is
        -- - - - - - - - - - - - - - - - -
        when START =>
            next_state <= FETCH;
        -- - - - - - - - - - - - - - - - -
        when FETCH =>
            DATA_EN <= '1';
            next_state <= DECODE;
        -- - - - - - - - - - - - - - - - -
        when DECODE =>
            case DATA_RDATA is
            when X"3E" => -- >
                next_state <= PTR_INC;
            when X"3C" => -- <
                next_state <= PTR_DEC;
            when X"2B" => -- +
                next_state <= DATA_INC_R;
            when X"2D" => -- -
                next_state <= DATA_DEC_R;
            when X"5B" => -- [
                next_state <= OPEN_READ;
            when X"5D" => -- ]
                next_state <= CLOSE_READ;
            when X"2E" => -- .
                next_state <= PRINT_READ;
            when X"2C" => -- ,
                next_state <= READ_REQ;
            when X"24" => -- $
                next_state <= TMP_LOAD_R;
            when X"21" => -- !
                next_state <= TMP_STORE_R;
            when X"00" => -- null
                next_state <= FINISH;
            when others => -- unknown instruction (comment) - SKIP
                inc_pc <= '1';
                next_state <= FETCH;
            end case;
        -- - - - - - - - - - - - - - - - -
        when PTR_INC =>
            inc_ptr <= '1';
            inc_pc <= '1';
            next_state <= FETCH;
        -- - - - - - - - - - - - - - - - -
        when PTR_DEC =>
            dec_ptr <= '1';
            inc_pc <= '1';
            next_state <= FETCH;
        -- - - - - - - - - - - - - - - - -
        when DATA_INC_R =>
            sel_addr <= '1';
            DATA_EN <= '1';
            next_state <= DATA_INC_W;
        -- - - - - - - - - - - - - - - - -
        when DATA_INC_W =>
            sel_addr <= '1';
            sel_data <= "10";
            DATA_RDWR <= '1';
            DATA_EN <= '1';
            inc_pc <= '1';
            next_state <= FETCH;
        -- - - - - - - - - - - - - - - - -
        when DATA_DEC_R =>
            sel_addr <= '1';
            DATA_EN <= '1';
            next_state <= DATA_DEC_W;
        -- - - - - - - - - - - - - - - - -
        when DATA_DEC_W =>
            sel_addr <= '1';
            sel_data <= "01";
            DATA_RDWR <= '1';
            DATA_EN <= '1';
            inc_pc <= '1';
            next_state <= FETCH;
        -- - - - - - - - - - - - - - - - -
        when OPEN_READ =>
            sel_addr <= '1';
            DATA_EN <= '1';
            next_state <= OPEN_COMP;
        -- - - - - - - - - - - - - - - - -
        when OPEN_COMP =>
            inc_pc <= '1';
            if conv_integer(DATA_RDATA) = 0 then -- skip the loop
                next_state <= OPEN_FETCH;
            else -- execute instructions inside the loop
                next_state <= FETCH;
            end if;
        -- - - - - - - - - - - - - - - - -
        when OPEN_FETCH => -- skipping
            DATA_EN <= '1';
            next_state <= OPEN_DECODE;
        -- - - - - - - - - - - - - - - - -
        when OPEN_DECODE => -- skipping
            inc_pc <= '1';
            next_state <= OPEN_FETCH;
            if DATA_RDATA = X"5B" then -- nested loop
                inc_cnt <= '1';
            elsif DATA_RDATA = X"5D" then
                if conv_integer(cnt) = 0 then -- end of skipped loop
                    next_state <= FETCH;
                else -- end of a nested loop
                    dec_cnt <= '1';
                end if;
            end if;
        -- - - - - - - - - - - - - - - - -
        when CLOSE_READ =>
            sel_addr <= '1';
            DATA_EN <= '1';
            next_state <= CLOSE_COMP;
        -- - - - - - - - - - - - - - - - -
        when CLOSE_COMP =>
            if conv_integer(DATA_RDATA) = 0 then -- end of looping, continue with next instruction
                inc_pc <= '1';
                next_state <= FETCH;
            else -- go back to beginning of the loop, skipping the instructions inside
                dec_pc <= '1';
                next_state <= CLOSE_FETCH;
            end if;
        -- - - - - - - - - - - - - - - - -
        when CLOSE_FETCH => -- skipping backwards
            DATA_EN <= '1';
            next_state <= CLOSE_DECODE;
        -- - - - - - - - - - - - - - - - -
        when CLOSE_DECODE => -- skipping backwards
            dec_pc <= '1';
            next_state <= CLOSE_FETCH;
            if DATA_RDATA = X"5D" then -- nested loop
                inc_cnt <= '1';
            elsif DATA_RDATA = X"5B" then
                if conv_integer(cnt) = 0 then -- beginning of the loop, continue looping (execute next instruction)
                    dec_pc <= '0';
                    inc_pc <= '1';
                    next_state <= FETCH;
                else -- end of a nested loop
                    dec_cnt <= '1';
                end if;
            end if;
        -- - - - - - - - - - - - - - - - -
        when PRINT_READ =>
            sel_addr <= '1';
            DATA_EN <= '1';
            next_state <= PRINT_WAIT; -- could go right to PRINT_WRITE if OUT_BUSY = '0', but that doesn't happen if little code is executed between 2 writes
        -- - - - - - - - - - - - - - - - -
        when PRINT_WAIT =>
            next_state <= PRINT_WAIT;
            if OUT_BUSY = '0' then
                next_state <= PRINT_WRITE;
            end if;
        -- - - - - - - - - - - - - - - - -
        when PRINT_WRITE =>
            OUT_DATA <= DATA_RDATA;
            OUT_WE <= '1';
            inc_pc <= '1';
            next_state <= FETCH;
        -- - - - - - - - - - - - - - - - -
        when READ_REQ =>
            IN_REQ <= '1';
            next_state <= READ_REQ;
            if IN_VLD = '1' then
                next_state <= READ_WRITE;
            end if;
        -- - - - - - - - - - - - - - - - -
        when READ_WRITE =>
            sel_addr <= '1';
            sel_data <= "00";
            DATA_RDWR <= '1';
            DATA_EN <= '1';
            inc_pc <= '1';
            next_state <= FETCH;
        -- - - - - - - - - - - - - - - - -
        when TMP_LOAD_R =>
            sel_addr <= '1';
            DATA_EN <= '1';
            next_state <= TMP_LOAD_W;
        -- - - - - - - - - - - - - - - - -
        when TMP_LOAD_W =>
            sel_tmp <= '1';
            sel_data <= "11";
            DATA_RDWR <= '1';
            DATA_EN <= '1';
            inc_pc <= '1';
            next_state <= FETCH;
        -- - - - - - - - - - - - - - - - -
        when TMP_STORE_R =>
            sel_tmp <= '1';
            DATA_EN <= '1';
            next_state <= TMP_STORE_W;
        -- - - - - - - - - - - - - - - - -
        when TMP_STORE_W =>
            sel_addr <= '1';
            sel_data <= "11";
            DATA_RDWR <= '1';
            DATA_EN <= '1';
            inc_pc <= '1';
            next_state <= FETCH;
        -- - - - - - - - - - - - - - - - -
        when FINISH =>
            next_state <= FINISH;
        -- - - - - - - - - - - - - - - - -
        when others => -- shouldn't happen
            next_state <= FINISH;
        -- - - - - - - - - - - - - - - - -
        end case;
    end process;

end behavioral;
