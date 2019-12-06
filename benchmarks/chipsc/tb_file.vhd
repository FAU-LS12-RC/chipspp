---------------------------------------------------------------------------
-- Company     : FAU
-- Author(s)   : Andreas Becher <andreas.becher@fau.de>
-- 
-- Creation Date : 09/08/2017
-- File          : tb_file.vhd
--
-- Abstract : 
--
---------------------------------------------------------------------------

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.numeric_std.all;


library xil_defaultlib;
--use xil_defaultlib.foo;
use xil_defaultlib.file_master;
use xil_defaultlib.file_slave;

---------------------------------------------------------------------------
Entity tb_file is 
    ---------------------------------------------------------------------------
    generic (
                DATA_WIDTH :  integer := 32;
                DATA_FILE_IS_HEX : boolean := true;
                STIM_FILE :    string := "../../../in0.in";
                RESULT_FILE :  string := "../../../out0.out"
            );
--            port ( bla : in std_logic := '0');
end entity;


---------------------------------------------------------------------------
Architecture tb_file_1 of tb_file is
    ---------------------------------------------------------------------------

    constant clk_period : time := 10ns;

    --controlling signals
    signal run : std_logic := '0';
    signal sim_done : std_logic := '0';

    signal clk : std_logic := '1';
    signal rst : std_logic := '1';

--CHIPS UUT HERE

    control_proc:process
    begin
        wait until rising_edge(clk);

        run <= not rst;

        sim_done <= d_out_last_0 and d_out_valid_0 and d_out_ready_0;
        if(sim_done = '1') then --one clock later
            assert false report "simulation done" severity failure; -- we're done here
        end if;

    end process;


    --d_out_ready <= '1' after (clk_period*50+1ns);

    clk <= not clk after clk_period/2;
    rst <= '0' after (clk_period*30-0.1ns);

end architecture tb_file_1;

