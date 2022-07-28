-- IVH Project: Patnáctka
-- Author: xcibak00 - Michal Cibák

library IEEE;
use IEEE.std_logic_1164.all;


package game_pack is

    type mask_t is
    record
        top: std_logic;
        left: std_logic;
        right: std_logic;
        bottom: std_logic;
    end record;

    function getmask(X, Y, COLUMNS, ROWS: natural) return mask_t;
    -- matrix of at least 2 rows and 2 columns is required as well as correct coordinates in that matrix for correct output

    function state_position(X, Y: natural) return natural;

end game_pack;


package body game_pack is

    function getmask(X, Y, COLUMNS, ROWS: natural) return mask_t is
        variable mask: mask_t;
    begin
        if X = 0 then
            mask.left := '0';
            mask.right := '1';
        elsif X = COLUMNS - 1 then
            mask.left := '1';
            mask.right := '0';
        else
            mask.left := '1';
            mask.right := '1';
        end if;

        if Y = 0 then
            mask.top := '0';
            mask.bottom := '1';
        elsif Y = ROWS - 1 then
            mask.top := '1';
            mask.bottom := '0';
        else
            mask.top := '1';
            mask.bottom := '1';
        end if;

        return mask;
    end getmask;

    function state_position(X, Y: natural) return natural is
    begin
        return (Y * 4 + X) * 4;
    end state_position;

end game_pack;
