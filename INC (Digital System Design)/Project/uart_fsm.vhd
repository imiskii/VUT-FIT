-- uart_fsm.vhd: UART controller - finite state machine
-- Author(s): Michal Ľaš
--
library ieee;
use ieee.std_logic_1164.all;

-------------------------------------------------
entity UART_FSM is
port(
   CLK 			: in std_logic;
   RST 			: in std_logic;
	DIN 			: in std_logic;
	CNT_MID_EN  : out std_logic;
	CNT_MID 		: in std_logic;
	CNT_BIT_EN	: out std_logic;
	CNT_BIT 		: in std_logic;
	DO_VLD		: in std_logic;
	DVLD			: out std_logic
   );
end entity UART_FSM;

-------------------------------------------------
architecture behavioral of UART_FSM is

type STATE_t is (WAIT_START_BIT, COUNT_TO_MID, PROCESS_DATA, WAIT_STOP_BIT, VALID);
signal current_state : STATE_t;
signal next_state 	: STATE_t;

begin

	current_state_logic : process (CLK, RST)
	begin
		if RST = '1' then
			current_state <= WAIT_START_BIT;
		elsif rising_edge (CLK) then
			current_state <= next_state;
		end if;
	end process;


	next_state_logic: process (current_state, DIN, CNT_MID, CNT_BIT, DO_VLD)
	begin
		case current_state is
			when WAIT_START_BIT 	=> if DIN = '0' then
												next_state <= COUNT_TO_MID;
											end if;
			when COUNT_TO_MID 	=> if CNT_MID = '1' then
												next_state <= PROCESS_DATA;
											end if;
			when PROCESS_DATA 	=> if CNT_BIT = '1' then
												next_state <= WAIT_STOP_BIT;
											end if;
			when WAIT_STOP_BIT	=> if DO_VLD = '1' then
												next_state <= VALID;
											end if;
			when VALID 				=> next_state <= WAIT_START_BIT;
			when others 			=> null;
		end case;
	end process;
	
	
	output_logic : process (current_state)
	begin
		case current_state is
			when WAIT_START_BIT 	=>
				CNT_MID_EN 	<= '0';
				CNT_BIT_EN 	<= '0';
				DVLD 			<= '0';
			when COUNT_TO_MID 	=>
				CNT_MID_EN 	<= '1';
				CNT_BIT_EN 	<=	'0';
				DVLD 			<= '0';
			when PROCESS_DATA 	=>
				CNT_MID_EN 	<=	'1';
				CNT_BIT_EN 	<= '1';
				DVLD 			<= '0';
			when WAIT_STOP_BIT 	=>
				CNT_MID_EN 	<= '1';
				CNT_BIT_EN 	<= '0';
				DVLD 			<= '0';
			when VALID 				=>
				CNT_MID_EN 	<= '0';
				CNT_BIT_EN 	<= '0';
				DVLD 			<= '1';
			when others 			=>
				null;
		end case;
	end process;

end behavioral;
