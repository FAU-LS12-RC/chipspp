----------------------------------------------------------------------------------
-- Company: 
-- Engineer: Andreas Becher
-- 
-- Create Date: 03/04/2016 09:41:00 AM
-- Design Name: 
-- Module Name: file_master - Behavioral
-- Project Name: 
-- Target Devices: 
-- Tool Versions: 
-- Description: 
-- 
-- Dependencies: 
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
-- 
----------------------------------------------------------------------------------


library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

library std;
use std.textio.all;
use IEEE.std_logic_textio.all;
use IEEE.NUMERIC_STD.ALL;


entity file_master is
    Generic (
                FILENAME : string := "input.dat";
                USE_HEX : boolean := true;
                DATA_WIDTH_BYTES : integer range 1 to 32);
    Port ( clk : in STD_LOGIC;
           rst : in STD_LOGIC := '0';
           ready : in STD_LOGIC;
           data : out STD_LOGIC_VECTOR(DATA_WIDTH_BYTES*8-1 downto 0);
           valid : out STD_LOGIC;
           last : out STD_LOGIC);
end file_master;

architecture file_master_beh of file_master is

    type t_char_file is FILE of character;
    file bfile_ptr : t_char_file;
    file cfile_ptr : text;

    signal s_valid : std_logic;

begin

    valid <= s_valid;

    read_proc: process
        variable var : character;
        variable hline : line;
        variable hvar : std_logic_vector(DATA_WIDTH_BYTES*8-1 downto 0);
        variable fstat : file_open_status;
    begin

        wait until rising_edge(clk);

        if(rst = '1') then

            if(USE_HEX=false) then
                file_close(bfile_ptr);
                file_open(fstat,bfile_ptr,FILENAME,READ_MODE);

            else
                file_close(cfile_ptr);
                file_open(fstat,cfile_ptr,FILENAME,READ_MODE);
            end if;
            s_valid <= '0';
            last <= '0';

        else

            if(ready = '1' or s_valid = '0') then
                --assert (fstat /= name_error) report "File not found" & FILENAME severity error;
                assert (fstat = open_ok) report "Unable to open file " & FILENAME & " err: " & file_open_status'image(fstat)  severity failure;
                last <= '0';
                s_valid <= '0';
                if(USE_HEX = false)then
                    if(not endfile(bfile_ptr)) then
                        for i in 0 to DATA_WIDTH_BYTES-1 loop
                            read(bfile_ptr,var);
                            data((i+1) * 8 -1 downto i*8) <= std_logic_vector(to_unsigned(character'pos(var),8)); 
                        end loop;
                        s_valid <= '1';
                        if(endfile(bfile_ptr)) then
                            last <= '1';
                        end if;
                    end if;
                else
                    if(not endfile(cfile_ptr)) then 

                        --TODO handle CR LF space(' ') and comments? '#'
                        readline(cfile_ptr,hline);
                        hread(hline,hvar);
                        data <= hvar;
                        s_valid <= '1';
                        if(endfile(cfile_ptr)) then
                            last <= '1';
                        end if;
                    end if;
                end if;
            end if;
        end if;


    end process;



end file_master_beh;





library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

library std;
use std.textio.all;
use IEEE.std_logic_textio.all;
use IEEE.NUMERIC_STD.ALL;


entity file_slave is
    Generic (
                FILENAME : string := "output.dat";
                USE_HEX : boolean := true;
                DATA_WIDTH_BYTES : integer range 1 to 32);
    Port ( clk : in STD_LOGIC;
           rst : in STD_LOGIC := '0';
           ready : out STD_LOGIC := '1';
           data : in STD_LOGIC_VECTOR(DATA_WIDTH_BYTES*8-1 downto 0);
           valid : in STD_LOGIC;
           last : in STD_LOGIC);
end file_slave;

architecture file_slave_beh of file_slave is

    type t_char_file is FILE of character;
    file bfile_ptr : t_char_file;
    file cfile_ptr : text;

begin


    ready <= not rst;

    read_proc: process
        variable hline : line;
        variable hvar : std_logic_vector(DATA_WIDTH_BYTES*8-1 downto 0);
        variable var : character;
        variable fstat : file_open_status;
    begin

        wait until rising_edge(clk);


        if(rst='1') then
            if(USE_HEX=false) then
                file_close(bfile_ptr);
                file_open(bfile_ptr,FILENAME,WRITE_MODE);
            else
                file_close(cfile_ptr);
                file_open(cfile_ptr,FILENAME,WRITE_MODE);
            end if;
        end if;


        if(valid = '1' and rst = '0') then
            --assert (fstat /= name_error) report "File not found" & FILENAME severity error;
            assert (fstat = open_ok) report "Unable to open file " & FILENAME & " err: " & file_open_status'image(fstat)  severity failure;
            if(USE_HEX = true) then
                hwrite(hline,data,left,data'length/4);
                if(last='1') then
                    write(hline,string'(" #last"));
                end if;
                writeline(cfile_ptr,hline);
            else
                for i in 0 to DATA_WIDTH_BYTES-1 loop
                    var := character'val(to_integer(unsigned(data((i+1) * 8 -1 downto i*8))));
                    write(bfile_ptr,var);
                end loop;
            end if;

            if(last = '1') then
                if(USE_HEX=false) then
                    file_close(bfile_ptr);
                else
                    file_close(cfile_ptr);
                end if;
            end if;

        end if;


    end process;



end file_slave_beh;
