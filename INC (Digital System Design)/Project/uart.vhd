-- uart.vhd: UART controller - receiving part
-- Author(s): Michal Ľaš
--
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;

-------------------------------------------------
entity UART_RX is
port(	
   CLK		: in std_logic;
	RST		: in std_logic;
	DIN		: in std_logic;
	DOUT		: out std_logic_vector(7 downto 0);
	DOUT_VLD	: out std_logic := '0'
);
end UART_RX;  

-------------------------------------------------
architecture behavioral of UART_RX is

signal cnt_mid_en 	: std_logic;
signal cnt_mid 		: std_logic;
signal cnt_bit_en 	: std_logic;
signal cnt_bit 		: std_logic;
signal do_vld			: std_logic;
signal dvld		 		: std_logic;
signal cnt_mid_v  	: std_logic_vector(4 downto 0) := "00000";
signal cnt_bit_v  	: std_logic_vector(3 downto 0) := "0000";
signal reg_en 			: std_logic_vector(7 downto 0) := "00000000";

begin
	FSM: entity work.UART_FSM(behavioral)
	port map(
		CLK 			=> CLK,
		RST 			=> RST,
		DIN 			=> DIN,
		CNT_MID_EN  => cnt_mid_en,
		CNT_MID 		=> cnt_mid,
		CNT_BIT_EN	=> cnt_bit_en,
		CNT_BIT 		=> cnt_bit,
		DO_VLD 		=> do_vld,
		DVLD			=> dvld
	);

	DOUT_VLD <= dvld;	
	
	count_to_mid: process (CLK, RST)
	begin	
		if rising_edge (CLK) then
			if cnt_mid_en = '1' then 
				cnt_mid_v <= cnt_mid_v + 1;
			elsif RST = '1' then
				cnt_mid_v <= "00000";
			else
				cnt_mid_v <= "00000";
			end if;
			
			if cnt_bit_en = '1' and cnt_mid_v(4) = '1' then
				cnt_mid_v <= "00001";
			end if;
		end if;
	end process;
	
	
	cmp_cnt_to_mid : process(cnt_mid_v)
	begin
		if cnt_mid_v = "11000" then 
			cnt_mid <= '1';
		else 
			cnt_mid <= '0';
		end if;
	end process;
	
	
	count_bits : process (CLK, RST)
	begin
		if rising_edge (CLK) then
			if cnt_bit_en = '1' and cnt_mid_v(4) = '1' then
				cnt_bit_v <= cnt_bit_v + 1;
			elsif cnt_bit_en = '0' then
				cnt_bit_v <= "0000";
			elsif RST = '1' then
				cnt_bit_v <= "0000";
			end if;
		end if;
	end process;
		
	
	decode_data : process(cnt_bit_v, cnt_bit_en, cnt_mid_v)
	begin
		reg_en <= "00000000";
		if cnt_bit_en = '1' and cnt_mid_v(4) = '1' then
			case cnt_bit_v is
				when "0000" => reg_en <= "00000001";
				when "0001" => reg_en <= "00000010";
				when "0010" => reg_en <= "00000100";
				when "0011" => reg_en <= "00001000";
				when "0100" => reg_en <= "00010000";
				when "0101" => reg_en <= "00100000";
				when "0110" => reg_en <= "01000000";
				when "0111" => reg_en <= "10000000";
				when others => reg_en <= "00000000";
			end case;
		end if;
	end process;
	
	reg0 : process(CLK, RST)
	begin
		if RST = '1' then
			DOUT(0) <= '0';
		elsif rising_edge(CLK) then
			if reg_en(0) = '1' then
				DOUT(0) <= DIN;
			end if;
		end if;
	end process;
	
	reg1 : process(CLK, RST)
	begin
		if RST = '1' then
			DOUT(1) <= '0';
		elsif rising_edge(CLK) then
			if reg_en(1) = '1' then
				DOUT(1) <= DIN;
			end if;
		end if;
	end process;
	
	reg2 : process(CLK, RST)
	begin
		if RST = '1' then
			DOUT(2) <= '0';
		elsif rising_edge(CLK) then
			if reg_en(2) = '1' then
				DOUT(2) <= DIN;
			end if;
		end if;
	end process;
	
	reg3 : process(CLK, RST)
	begin
		if RST = '1' then
			DOUT(3) <= '0';
		elsif rising_edge(CLK) then
			if reg_en(3) = '1' then
				DOUT(3) <= DIN;
			end if;
		end if;
	end process;
	
	reg4 : process(CLK, RST)
	begin
		if RST = '1' then
			DOUT(4) <= '0';
		elsif rising_edge(CLK) then
			if reg_en(4) = '1' then
				DOUT(4) <= DIN;
			end if;
		end if;
	end process;
	
	reg5 : process(CLK, RST)
	begin
		if RST = '1' then
			DOUT(5) <= '0';
		elsif rising_edge(CLK) then
			if reg_en(5) = '1' then
				DOUT(5) <= DIN;
			end if;
		end if;
	end process;
	
	reg6 : process(CLK, RST)
	begin
		if RST = '1' then
			DOUT(6) <= '0';
		elsif rising_edge(CLK) then
			if reg_en(6) = '1' then
				DOUT(6) <= DIN;
			end if;
		end if;
	end process;
	
	reg7 : process(CLK, RST)
	begin
		if RST = '1' then
			DOUT(7) <= '0';
		elsif rising_edge(CLK) then
			if reg_en(7) = '1' then
				DOUT(7) <= DIN;
			end if;
		end if;
	end process;
	
	
	cmp_cnt_bits : process (cnt_bit_v)
	begin
		if cnt_bit_v = "1000" then 
			cnt_bit <= '1';
		else 
			cnt_bit <= '0';
		end if;
	end process;
	
	
	count_stop_bit : process (CLK)
	begin
		if rising_edge (CLK) then
			if cnt_mid_v(4) = '1' then
				do_vld <= '1';
			else
				do_vld <= '0';
			end if;
		end if;
	end process;
	
end behavioral;
