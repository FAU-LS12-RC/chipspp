---------------------------------------------------------------------------
-- Company     : FAU
-- Author(s)   : Andreas Becher <andreas.becher@fau.de>
-- 
-- Creation Date : 23/05/2017
-- File          : generic_fwft_fifo.vhd
--
-- Abstract : 
--
---------------------------------------------------------------------------

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.numeric_std.all;
-- blasynthesis translate_off
Library xpm;
use xpm.vcomponents.xpm_fifo_async;
use xpm.vcomponents.xpm_fifo_sync;
-- blasynthesis translate_on

--constant FIFO_DATA_WIDTH : integer := 32;
--constant FIFO_DEPTH_BITS : integer := 9;  --9 would be 512 deep
--constant FIFO_PROG_EMPTY : integer := 7;
--constant FIFO_PROG_FULL  : integer := 7;
--constant FIFO_HAS_LAST   : boolean := false; --use x_last signals
--constant FIFO_ASYNC      : boolean := false;
--constant FIFO_DEBUG      : boolean := false;
--
--signal s_fifo_in_data        : std_logic_vector(FIFO_DATA_WIDTH-1 downto 0);
--signal s_fifo_in_last        : std_logic;
--signal s_fifo_in_valid       : std_logic;
--signal s_fifo_in_ready       : std_logic;
--signal s_fifo_in_avail       : std_logic_vector(FIFO_DEPTH_BITS downto 0);
--signal s_fifo_in_full        : std_logic;
--signal s_fifo_in_prog_full   : std_logic;
--signal s_fifo_out_data       : std_logic_vector(FIFO_DATA_WIDTH-1 downto 0);
--signal s_fifo_out_last       : std_logic;
--signal s_fifo_out_valid      : std_logic;
--signal s_fifo_out_ready      : std_logic;
--signal s_fifo_out_avail      : std_logic_vector(FIFO_DEPTH_BITS downto 0);
--signal s_fifo_out_empty      : std_logic;
--signal s_fifo_out_prog_empty : std_logic;
--signal s_fifo_in_clk         : std_logic; --if async=false, this is the only clk
--signal s_fifo_out_clk        : std_logic := '0'; -- if async=false, this clk is unused
--signal s_fifo_async_rst      : std_logic

--fifo_i : generic_fwft_fifo
--generic map(
--               DATA_WIDTH => FIFO_DATA_WIDTH,
--               DEPTH_BITS => FIFO_DEPTH_BITS,
--               PROG_EMPTY => FIFO_PROG_EMPTY,
--               PROG_FULL  => FIFO_PROG_FULL ,
--               HAS_LAST   => FIFO_HAS_LAST  ,
--               ASYNC      => FIFO_ASYNC     ,
--               DEBUG      => FIFO_DEBUG      
--           )
--port map(
--            in_data              => s_fifo_in_data       ,
--            in_last              => s_fifo_in_last       ,
--            in_valid             => s_fifo_in_valid      ,
--            in_ready             => s_fifo_in_ready      ,
--            in_avail             => s_fifo_in_avail      ,
--            in_full              => s_fifo_in_full       ,
--            in_prog_full         => s_fifo_in_prog_full  ,
--            out_data             => s_fifo_out_data      ,
--            out_last             => s_fifo_out_last      ,
--            out_valid            => s_fifo_out_valid     ,
--            out_ready            => s_fifo_out_ready     ,
--            out_avail            => s_fifo_out_avail     ,
--            out_empty            => s_fifo_out_empty     ,
--            out_prog_empty       => s_fifo_out_prog_empty,
--            in_clk               => s_fifo_in_clk        ,
--            out_clk              => s_fifo_out_clk       ,
--            async_rst            => s_fifo_async_rst      
--        );





---------------------------------------------------------------------------
Entity generic_fwft_fifo is 
    ---------------------------------------------------------------------------
    generic(
               DATA_WIDTH : integer range 1 to 4096 := 32;
               DEPTH_BITS : integer range 4 to 21 := 10; --9 would be 512 deep
               PROG_EMPTY : integer range 7 to 4096 := 7;
               PROG_FULL  : integer range 7 to 4096 := 7;
               HAS_LAST   : boolean := false; --use last
               ASYNC      : boolean := false;
               DEBUG      : boolean := false
           );
    port 
    (
        in_data        : in  std_logic_vector(DATA_WIDTH-1 downto 0);
        in_last        : in  std_logic := '0';
        in_valid       : in  std_logic;
        in_ready       : out std_logic;
        in_avail       : out std_logic_vector(DEPTH_BITS downto 0);
        in_full        : out std_logic;
        in_prog_full   : out std_logic;
        out_data       : out std_logic_vector(DATA_WIDTH-1 downto 0);
        out_last       : out std_logic := '0';
        out_valid      : out std_logic;
        out_ready      : in  std_logic;
        out_avail      : out std_logic_vector(DEPTH_BITS downto 0);
        out_empty      : out std_logic;
        out_prog_empty : out std_logic;
        in_clk         : in  std_logic; --if async=false, this is the only clk
        out_clk        : in  std_logic := '0'; -- if async=false, this clk is unused
        async_rst      : in  std_logic
    );
end entity;


---------------------------------------------------------------------------
Architecture generic_fwft_fifo_1 of generic_fwft_fifo is
    ---------------------------------------------------------------------------

    constant FIFO_MEMORY_TYPE : string := "auto";
    constant ECC_MODE : string := "no_ecc";
    constant READ_MODE : string := "fwft";

    constant C_ZERO : std_logic := '0';

    constant MAXADDR : unsigned(DEPTH_BITS-1 downto 0) := (others => '1');
    constant SIZE : integer := to_integer(MAXADDR)+1;

    constant DOUT_RESET_VALUE : string := "0";

    signal rd_rst_busy : std_logic := '0';
    signal wr_rst_busy : std_logic := '0';

    signal s_in_full : std_logic;
    signal s_full : std_logic;
    signal s_out_empty : std_logic;
    signal s_empty : std_logic;

    signal s_in_avail : std_logic_vector(DEPTH_BITS downto 0);
    signal s_out_avail : std_logic_vector(DEPTH_BITS downto 0);

    signal s_in_corr : std_logic_vector(0 downto 0) := "0";
    signal s_out_corr : std_logic_vector(0 downto 0) := "0";


    function calc_int(x : integer; last : boolean) return integer is
    begin
        if(last) then
            return x+1;
        else
            return x;
        end if;
    end function;

    constant INTERNAL_DATA_WIDTH : integer := calc_int(DATA_WIDTH,HAS_LAST);
    signal s_in_data : std_logic_vector(INTERNAL_DATA_WIDTH-1 downto 0);
    signal s_out_data : std_logic_vector(INTERNAL_DATA_WIDTH-1 downto 0);

    --for correct simulation
    signal s_in_valid : std_logic;
    --signal s_dout_ready : std_logic; --TODO not sure about this one

    ATTRIBUTE MARK_DEBUG : boolean;
    ATTRIBUTE MARK_DEBUG of s_in_avail : SIGNAL IS DEBUG;
    ATTRIBUTE MARK_DEBUG of in_avail : SIGNAL IS DEBUG;

    ATTRIBUTE MARK_DEBUG of s_out_avail : SIGNAL IS DEBUG;
    ATTRIBUTE MARK_DEBUG of out_avail : SIGNAL IS DEBUG;


    ATTRIBUTE MARK_DEBUG of in_valid : SIGNAL IS DEBUG;
    ATTRIBUTE MARK_DEBUG of s_in_full : SIGNAL IS DEBUG;
    ATTRIBUTE MARK_DEBUG of out_ready : SIGNAL IS DEBUG;
    ATTRIBUTE MARK_DEBUG of s_out_empty : SIGNAL IS DEBUG;

begin

    s_in_full <= s_full or wr_rst_busy;
    in_ready <= not s_in_full;
    in_full <= s_in_full;

    s_out_empty <= s_empty or rd_rst_busy;
    out_valid <= not s_out_empty;
    out_empty <= s_out_empty;

    in_avail <= (in_avail'range => '0') when s_in_avail(s_in_avail'high) = '1' else
                std_logic_vector(to_unsigned(SIZE+1,s_in_avail'length)-unsigned(s_in_avail)- unsigned(s_in_corr));

    s_in_corr(0) <= (s_in_valid and (not s_in_full)) when rising_edge(in_clk);

    out_avail <= (out_avail'range => '0') when s_out_empty = '1' else 
                 std_logic_vector(unsigned(s_out_avail) - unsigned(s_out_corr));

    s_in_valid <= in_valid;

    gen_last:if HAS_LAST = true generate
        s_in_data <= in_last & in_data;
        out_data <= s_out_data(out_data'range);
        out_last <= s_out_data(s_out_data'high);
    end generate;

    ngen_last:if HAS_LAST = false generate
        s_in_data <= in_data;
        out_data <= s_out_data;
    end generate;


    gen_async : if ASYNC = true generate

        s_out_corr(0) <= (out_ready and (not s_out_empty)) when rising_edge(out_clk);

        xpm_fifo_async_inst : xpm_fifo_async
        generic map (

                        FIFO_MEMORY_TYPE        => FIFO_MEMORY_TYPE,           --string; "auto", "block", or "distributed";
                        ECC_MODE                => ECC_MODE,         --string; "no_ecc" or "en_ecc";
                        RELATED_CLOCKS          => 0,                --positive integer; 0 or 1
                        FIFO_WRITE_DEPTH        => SIZE,             --positive integer
                        WRITE_DATA_WIDTH        => INTERNAL_DATA_WIDTH,               --positive integer
                        WR_DATA_COUNT_WIDTH     => DEPTH_BITS+1,               --positive integer
                        PROG_FULL_THRESH        => PROG_FULL,               --positive integer
                        FULL_RESET_VALUE        => 1,--0,                --positive integer; 0 or 1;
                        READ_MODE               => READ_MODE,            --string; "std" or "fwft";
                        FIFO_READ_LATENCY       => 0,                --positive integer;
                        READ_DATA_WIDTH         => INTERNAL_DATA_WIDTH,               --positive integer
                        RD_DATA_COUNT_WIDTH     => DEPTH_BITS+1,               --positive integer
                        PROG_EMPTY_THRESH       => PROG_EMPTY,               --positive integer
                        DOUT_RESET_VALUE        => DOUT_RESET_VALUE,              --string
                        CDC_SYNC_STAGES         => 2,                --positive integer
                        WAKEUP_TIME             => 0                 --positive integer; 0 or 2;
                    )
        port map (

                     sleep            => C_ZERO,
                     rst              => async_rst,
                     wr_clk           => in_clk,
                     wr_en            => s_in_valid,
                     din              => s_in_data,
                     full             => s_full,
                     --    overflow         => overflow,
                     wr_rst_busy      => wr_rst_busy,
                     rd_clk           => out_clk,
                     rd_en            => out_ready,
                     dout             => s_out_data,
                     empty            => s_empty,
                     --    underflow        => underflow,
                     rd_rst_busy      => rd_rst_busy,
                     prog_full        => in_prog_full,
                     wr_data_count    => s_in_avail,
                     prog_empty       => out_prog_empty,
                     rd_data_count    => s_out_avail,
                     injectsbiterr    => C_ZERO,
                     injectdbiterr    => C_ZERO,
                     sbiterr          => open,
                     dbiterr          => open
                 );

    end generate;


    gen_sync : if ASYNC = false generate

        s_out_corr(0) <= (out_ready and (not s_out_empty)) when rising_edge(in_clk);

        xpm_fifo_sync_inst : xpm_fifo_sync
        generic map (

                        FIFO_MEMORY_TYPE         => FIFO_MEMORY_TYPE,           --string; "auto", "block", "distributed", or "ultra" ;
                        ECC_MODE                 => ECC_MODE,         --string; "no_ecc" or "en_ecc";
                        FIFO_WRITE_DEPTH        => SIZE,             --positive integer
                        WRITE_DATA_WIDTH        => INTERNAL_DATA_WIDTH,               --positive integer
                        WR_DATA_COUNT_WIDTH     => DEPTH_BITS+1,               --positive integer
                        PROG_FULL_THRESH        => PROG_FULL,               --positive integer
                        FULL_RESET_VALUE         => 1,--0,                --positive integer; 0 or 1;
                        READ_MODE                => READ_MODE,            --string; "std" or "fwft";
                        FIFO_READ_LATENCY        => 0,                --positive integer;
                        READ_DATA_WIDTH         => INTERNAL_DATA_WIDTH,               --positive integer
                        RD_DATA_COUNT_WIDTH     => DEPTH_BITS+1,               --positive integer
                        PROG_EMPTY_THRESH       => PROG_EMPTY,               --positive integer
                        DOUT_RESET_VALUE         => "0",              --string
                        WAKEUP_TIME              => 0                 --positive integer; 0 or 2;
                    )
        port map (

                     rst              => async_rst,
                     wr_clk           => in_clk,
                     wr_en            => s_in_valid,
                     din              => s_in_data,
                     full             => s_full,
                     --                     overflow         => overflow,
                     wr_rst_busy      => wr_rst_busy,
                     rd_en            => out_ready,
                     dout             => s_out_data,
                     empty            => s_empty,
                     --                     underflow        => underflow,
                     rd_rst_busy      => rd_rst_busy,
                     prog_full        => in_prog_full,
                     wr_data_count    => s_in_avail,
                     prog_empty       => out_prog_empty,
                     rd_data_count    => s_out_avail,
                     sleep            => C_ZERO,
                     injectsbiterr    => C_ZERO,
                     injectdbiterr    => C_ZERO,
                     sbiterr          => open,
                     dbiterr          => open
                 );

    end generate;


end architecture generic_fwft_fifo_1;

