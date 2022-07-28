-- IVH Project: Patnáctka
-- Author: xcibak00 - Michal Cibák

library IEEE;
use IEEE.std_logic_1164.all;
use work.game_pack.all;


entity game_pack_tb is
end game_pack_tb;


architecture Behavioral of game_pack_tb is
    signal a0, a1, a2, a3, a4: mask_t;
begin
    a0 <= getmask(0,0,5,5);
    a1 <= getmask(4,4,5,5);
    a2 <= getmask(4,0,5,5);
    a3 <= getmask(0,4,5,5);
    a4 <= getmask(2,2,5,5);
end Behavioral;
