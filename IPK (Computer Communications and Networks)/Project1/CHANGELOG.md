
# Changelog 2023-03-21


## Implemented functionality

The client `ipkcpc` reads the list of commands from STDIN (each instruction on one line) and returns the received response from the server to STDOUT.
  
Possible error messages are written to STDERR.
  
`ipkcpc` is able to use either UDP or TCP depending on the selected mode. The client is able to communicate with any server using IPKCP.


## Known limitations

- In the case of using TCP, a correctly formatted 'BYE\n' message is assumed to terminate the connection from the server side. The message must contain the capital letters 'BYE' and the EOL symbol at the end.

- In the case of using TCP, the program attempts to send a BYE message to the server when interrupted by the C-c signal. The program is set to a maximum of 5 attempts. If a BYE message fails to be sent after five attempts. The program will be terminated despite not closing the connection with the server.

- In the case of using UDP, the response timeout is set to 5 seconds.

- Buffer size for message received from server is set to 4096 bytes. `ipkcpc` client cannot receive larger message.


## Known bugs


### Program arguments

- execution: ```./ipkcpc -h 127.0.1 -p 2023 -m udp```
    - The IP address is not complete but the program runs normally because function
    ```getIPfromHostname()``` is able to 'repair' this ip address to 127.0.0.1

### TCP

- null

### UDP

- null