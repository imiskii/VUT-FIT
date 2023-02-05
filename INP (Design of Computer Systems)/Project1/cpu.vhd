-- cpu.vhd: Simple 8-bit CPU (BrainFuck interpreter)
-- Copyright (C) 2022 Brno University of Technology,
--                    Faculty of Information Technology
-- Author(s): Michal Ľaš
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
   CLK   : in std_logic;  -- Clock signal
   RESET : in std_logic;  -- asynchronous processor reset
   EN    : in std_logic;  -- processor activity enabled
 
   -- synchronous RAM memory
   DATA_ADDR  : out std_logic_vector(12 downto 0); -- address to memory
   DATA_WDATA : out std_logic_vector(7 downto 0);  -- mem[DATA_ADDR] <- DATA_WDATA if DATA_EN='1'
   DATA_RDATA : in std_logic_vector(7 downto 0);   -- DATA_RDATA <- ram[DATA_ADDR] if DATA_EN='1'
   DATA_RDWR  : out std_logic;                     -- read (0) / write (1)
   DATA_EN    : out std_logic;                     -- activity enabled
   
   -- input port
   IN_DATA   : in std_logic_vector(7 downto 0);    -- IN_DATA <- keyboard status if IN_VLD='1' and IN_REQ='1'
   IN_VLD    : in std_logic;                       -- valid data
   IN_REQ    : out std_logic;                      -- data entry request
   
   -- output port
   OUT_DATA : out  std_logic_vector(7 downto 0);   -- recorded data
   OUT_BUSY : in std_logic;                        -- LCD is busy (1), enable to write
   OUT_WE   : out std_logic                        -- LCD <- OUT_DATA if OUT_WE='1' and OUT_BUSY='0'
 );
end cpu;


-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of cpu is

  signal pc_reg  : std_logic_vector(12 downto 0);
  signal pc_inc  : std_logic;
  signal pc_dec  : std_logic;

  signal ptr_reg  : std_logic_vector(12 downto 0);
  signal ptr_inc  : std_logic;
  signal ptr_dec  : std_logic;

  signal cnt_out  : std_logic_vector(7 downto 0);
  signal cnt_inc  : std_logic;
  signal cnt_dec  : std_logic;

  signal sel      : std_logic_vector(1 downto 0) := "00";

  type fsm_state is (START, FETCH_0, FETCH_1, DECODE, 

  SFT_R, SFT_L, 

  INC_DATA_LOAD, INC_DATA_INC, INC_DATA_WRITE,
  DEC_DATA_LOAD, DEC_DATA_DEC, DEC_DATA_WRITE,
  PRINT_DATA_LOAD, PRINT_DATA_SEND, 
  LOAD_VAL_REQ, LOAD_VAL_SET, LOAD_VAL_WRITE,

  CYCLE_WHILE_START_1, CYCLE_WHILE_START_2, CYCLE_WHILE_START_3, CYCLE_WHILE_START_4, CYCLE_WHILE_START_5,
  CYCLE_WHILE_END_1, CYCLE_WHILE_END_2, CYCLE_WHILE_END_3, CYCLE_WHILE_END_4, CYCLE_WHILE_END_5, CYCLE_WHILE_END_6,
  DO_WHILE_START,
  DO_WHILE_END_1, DO_WHILE_END_2, DO_WHILE_END_3, DO_WHILE_END_4, DO_WHILE_END_5, DO_WHILE_END_6,

  HALT, STATE_OTHERS);
  
--  DO_WHILE_S, DO_WHILE_E, WHILE_S, WHILE_E, LOAD,

  signal current_state : fsm_state;
  signal next_state    : fsm_state;
  
begin

  -- FSM --

  -- Current State register
  
  current_state_logic : process (RESET, CLK, EN)
  begin
    if (RESET = '1') then
      current_state <= START;
    elsif rising_edge(CLK) then
      if (EN = '1') then
        current_state <= next_state;
      end if;
    end if;
  end process;
  
  -- Next State logic + Output logic
  
  next_state_logic : process (EN, current_state, OUT_BUSY, IN_VLD, cnt_out, DATA_RDATA)
  begin

    -- default
    next_state <= START;
    cnt_inc   <= '0';
    cnt_dec   <= '0';
    ptr_inc   <= '0';
    ptr_dec   <= '0';
    pc_inc    <= '0';
    pc_dec    <= '0';
    sel       <= "00";
    DATA_RDWR <= '0';
    DATA_EN   <= '0';
    OUT_DATA  <= "00000000";
    IN_REQ    <= '0';
    OUT_WE    <= '0';


    case( current_state ) is
    
      when START =>
        next_state <= FETCH_1;

      when FETCH_0 =>
        sel <= "00";
        DATA_EN <= '0';
        next_state <= FETCH_1;

      when FETCH_1 =>
        DATA_EN <= '1';
        next_state <= DECODE;
      
      when DECODE =>
        case( DATA_RDATA ) is
        
          when x"3E" =>
            next_state <= SFT_R;
        
          when x"3C" =>
            next_state <= SFT_L;
          
          when x"2B" =>
            sel <= "11";
            next_state <= INC_DATA_LOAD;
          
          when x"2D" =>
            sel <= "11";
            next_state <= DEC_DATA_LOAD;

          when x"2E" =>
            sel <= "11";
            next_state <= PRINT_DATA_LOAD;
          
          when x"2C" =>
            next_state <= LOAD_VAL_REQ;

          when x"5B" =>
            sel <= "11";
            next_state <= CYCLE_WHILE_START_1;

          when x"5D" =>
            sel <= "11";
            next_state <= CYCLE_WHILE_END_1;

          when x"28" =>
            next_state <= DO_WHILE_START;

          when x"29" =>
            sel <= "11";
            next_state <= DO_WHILE_END_1;

          when x"00" =>
            next_state <= HALT;

          when others =>
            next_state <= STATE_OTHERS;
        
        end case ;

        
      -- SHIFT RIGHT

      when SFT_R =>
        ptr_inc <= '1';
        pc_inc  <= '1';
        next_state <= FETCH_0;

      -- SHIFT LEFT

      when SFT_L =>
        ptr_dec <= '1';
        pc_inc  <= '1';
        next_state <= FETCH_0;

      -- INC DATA

      when INC_DATA_LOAD =>
        DATA_EN <= '1';
        DATA_RDWR <= '0';
        next_state <= INC_DATA_INC;

      when INC_DATA_INC =>
        sel <= "10";
        next_state <= INC_DATA_WRITE;
        
      when INC_DATA_WRITE =>
        DATA_EN <= '1';
        DATA_RDWR <= '1';
        pc_inc <= '1';
        next_state <= FETCH_0;

      -- DEC DATA

      when DEC_DATA_LOAD =>
        DATA_EN <= '1';
        DATA_RDWR <= '0';
        next_state <= DEC_DATA_DEC;

      when DEC_DATA_DEC =>
        sel <= "01";
        next_state <= DEC_DATA_WRITE;
        
      when DEC_DATA_WRITE =>
        DATA_EN <= '1';
        DATA_RDWR <= '1';
        pc_inc <= '1';
        next_state <= FETCH_0;

      -- PRINT DATA

      when PRINT_DATA_LOAD =>
        sel <= "11";
        DATA_EN <= '1';
        DATA_RDWR <= '0';
        next_state <= PRINT_DATA_SEND;
      
      when PRINT_DATA_SEND =>
        if (OUT_BUSY = '1') then
          sel <= "11";
          DATA_EN <= '1';
          DATA_RDWR <= '0';
          next_state <= PRINT_DATA_SEND;
        elsif (OUT_BUSY = '0') then
          OUT_DATA <= DATA_RDATA;
          OUT_WE <= '1';
          pc_inc <= '1';
          next_state <= FETCH_0;
        end if;
          
      -- LOAD VALUE

      when LOAD_VAL_REQ =>
        IN_REQ <= '1';
        sel <= "11";
        next_state <= LOAD_VAL_SET;

      when LOAD_VAL_SET =>
        if (IN_VLD = '0') then
          IN_REQ <= '1';
          sel <= "11";
          next_state <= LOAD_VAL_SET;
        elsif (IN_VLD = '1') then
          sel <= "11";
          next_state <= LOAD_VAL_WRITE;
        end if ;

      when LOAD_VAL_WRITE =>
        sel <= "00";
        IN_REQ <= '0';
        DATA_EN <= '1';
        DATA_RDWR <= '1';
        pc_inc <= '1';
        next_state <= FETCH_0;

      -- WHILE CYCLE START

      when CYCLE_WHILE_START_1 =>
        DATA_EN <= '1';
        DATA_RDWR <= '0';
        pc_inc <= '1';
        sel <= "00"; 
        next_state <= CYCLE_WHILE_START_2;

      when CYCLE_WHILE_START_2 =>
        if (DATA_RDATA = "00000000") then
          cnt_inc <= '1';
          next_state <= CYCLE_WHILE_START_3;
        else
          next_state <= FETCH_0;
        end if;

      when CYCLE_WHILE_START_3 =>
        if (cnt_out = "00000000") then
          next_state <= FETCH_0;
        else
          pc_inc <= '1';
          sel <= "00"; 
          DATA_EN <= '1';
          next_state <= CYCLE_WHILE_START_4;
          if (DATA_RDATA = x"5B") then
            cnt_inc <= '1';
          elsif (DATA_RDATA = x"5D") then
            cnt_dec <= '1';
          end if;
        end if;

      when CYCLE_WHILE_START_4 =>
        DATA_EN <= '1';
        DATA_RDWR <= '0';
        sel <= "00";
        next_state <= CYCLE_WHILE_START_5;

      when CYCLE_WHILE_START_5 =>
        DATA_EN <= '1';
        DATA_RDWR <= '0';
        next_state <= CYCLE_WHILE_START_3;

      -- DO WHILE END

      when CYCLE_WHILE_END_1 =>
        DATA_EN <= '1';
        DATA_RDWR <= '0';
        sel <= "00"; 
        next_state <= CYCLE_WHILE_END_2;

      when CYCLE_WHILE_END_2 =>
        if (DATA_RDATA = "00000000") then
          pc_inc <= '1';
          next_state <= FETCH_0;
        else
          cnt_inc <= '1';
          pc_dec <= '1';
          next_state <= CYCLE_WHILE_END_3;
        end if;

      when CYCLE_WHILE_END_3 =>
        if (cnt_out = "00000000") then
          next_state <= FETCH_0;
        else
          sel <= "00";
          next_state <= CYCLE_WHILE_END_4;
        end if;

      when CYCLE_WHILE_END_4 =>
        DATA_EN <= '1';
        DATA_RDWR <= '0';
        sel <= "00";
        next_state <= CYCLE_WHILE_END_5;

      when CYCLE_WHILE_END_5 =>
        if (DATA_RDATA = x"5B") then
          cnt_dec <= '1';
        elsif (DATA_RDATA = x"5D") then
          cnt_inc <= '1';
        end if;
        DATA_EN <= '1';
        next_state <= CYCLE_WHILE_END_6;
          
      when CYCLE_WHILE_END_6 => 
        if (cnt_out = "00000000") then
          pc_inc <= '1';
        else
          pc_dec <= '1';
        end if;
        DATA_EN <= '1';
        next_state <= CYCLE_WHILE_END_3;

      -- DO WHILE LOOP START

      when DO_WHILE_START =>
          pc_inc <= '1';
          next_state <= FETCH_0;

      -- DO WHILE LOOP END

      when DO_WHILE_END_1 =>
        DATA_EN <= '1';
        DATA_RDWR <= '0';
        sel <= "00"; 
        next_state <= DO_WHILE_END_2;

      when DO_WHILE_END_2 =>
        if (DATA_RDATA = "00000000") then
          pc_inc <= '1';
          next_state <= FETCH_0;
        else
          cnt_inc <= '1';
          pc_dec <= '1';
          next_state <= DO_WHILE_END_3;
        end if;

      when DO_WHILE_END_3 =>
        if (cnt_out = "00000000") then
          next_state <= FETCH_0;
        else
          sel <= "00";
          next_state <= DO_WHILE_END_4;
        end if;

      when DO_WHILE_END_4 =>
        DATA_EN <= '1';
        DATA_RDWR <= '0';
        sel <= "00";
        next_state <= DO_WHILE_END_5;

      when DO_WHILE_END_5 =>
        if (DATA_RDATA = x"28") then
          cnt_dec <= '1';
        elsif (DATA_RDATA = x"29") then
          cnt_inc <= '1';
        end if;
        DATA_EN <= '1';
        next_state <= DO_WHILE_END_6;
          
      when DO_WHILE_END_6 => 
        if (cnt_out = "00000000") then
          pc_inc <= '1';
        else
          pc_dec <= '1';
        end if;
        DATA_EN <= '1';
        next_state <= DO_WHILE_END_3;

      --

      when HALT =>
        next_state <= HALT;
        
      when STATE_OTHERS =>
        pc_inc <= '1';
        next_state <= FETCH_0;

    
    end case ;
  end process;


  -- Program counter
  pc_cnt: process (RESET, CLK, pc_inc, pc_dec)
  begin
    if (RESET = '1') then
      pc_reg <= "0000000000000";
    elsif rising_edge(CLK) then
      if (pc_inc = '1') then
        if (pc_reg = "0111111111111") then
          pc_reg <= "0000000000000";
        else
          pc_reg <= pc_reg + 1;
        end if ;
      elsif (pc_dec = '1') then
        if (pc_reg = "0000000000000") then
          pc_reg <= "0111111111111";
        else
          pc_reg <= pc_reg - 1;
        end if ;
      end if;
    end if;
  end process;


  -- Pointer Register
  ptr_cnt: process (RESET, CLK, ptr_inc, ptr_dec)
  begin
    if (RESET = '1') then
      ptr_reg <= "1000000000000";                -- 0x1000
    elsif rising_edge(CLK) then
      if (ptr_inc = '1') then
        if (ptr_reg = "1111111111111") then     --0x1FFF + 1
          ptr_reg <= "1000000000000";
        else
          ptr_reg <= ptr_reg + 1;
        end if;
      elsif (ptr_dec = '1') then
        if (ptr_reg = "1000000000000") then  --0x1000 - 1
          ptr_reg <= "1111111111111";
        else
          ptr_reg <= ptr_reg - 1;
        end if;
      end if;
    end if;
  end process;


  -- CNT CYCLES
  cnt: process (RESET, CLK, cnt_inc, cnt_dec)
  begin
    if (RESET = '1') then
      cnt_out <= "00000000";
    elsif rising_edge(CLK) then
      if (cnt_inc = '1') then
        cnt_out <= cnt_out + 1;
      elsif (cnt_dec = '1') then
        cnt_out <= cnt_out - 1;
      end if ;
    end if;
  end process;

  -- MX1
  mx1: process (RESET, CLK, sel)
  begin
    if (RESET = '1') then
      DATA_ADDR <= "0000000000000";
    elsif rising_edge(CLK) then

      case( sel ) is
      
        when "00" => DATA_ADDR <= pc_reg;
        when "11" => DATA_ADDR <= ptr_reg;
        when "01" => DATA_ADDR <= ptr_reg;
        when "10" => DATA_ADDR <= ptr_reg;
        when others => DATA_ADDR <= (others => '0');
      
      end case ;
    end if;
  end process;


  -- MX2
  mx2: process (RESET, CLK, sel)
  begin
    if (RESET = '1') then
      DATA_WDATA <= x"00";
    elsif rising_edge(CLK) then
    
      case( sel ) is
      
        when "11" => DATA_WDATA <= IN_DATA;
        when "01" => DATA_WDATA <= DATA_RDATA - "00000001";
        when "10" => DATA_WDATA <= DATA_RDATA + "00000001";
        when others => DATA_WDATA <= (others => '0');
      
      end case ;
    end if;
  end process;

end behavioral;

