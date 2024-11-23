# serial-communication


This project implements a system with an **AT-Command based server** and a **cli-enabled client** that communicates with it via a serial port.



This system consists of two main components:
- **Server (`server.c`)**: A program that listens on a serial port, processes commands from a client, and manages a set of registers. The server supports reading and writing values to registers, as well as querying for allowed values for discrete registers and sending the response back to the client.
- **Client (`client.c`)**: A simple command-line interface to send AT commands to the server over a serial connection.

### Features
- **Dynamic Register Management**: The server maintains up to 99 registers, each of which can be configured to either accept discrete values or a range of values.
- **AT Commands**: The system accepts commands to interact with registers, such as reading values, setting new values, or querying allowed values.
- **Serial Communication**: Communication between the client and server occurs over a serial interface.


## Instalation
     git clone https://github.com/kellynanou/serial-communication.git

     
## Compile and build

1. ```bash
    cd serial-communication/
1. ```bash
    cmake .
    make

## Run

1.  **In a terminal**
    ```bash 
    socat -d -d pty,raw,echo=0 pty,raw,echo=0
to open the serial communication

2. **In a 2nd terminal** 
    ```bash
     cd serial-communication/
    ./server /dev/pts/X
where X is the first serial port that you can see in the 1st terminal    


3. **In a 3rd terminal**
    ```bash
     cd serial-communication/
    ./client /dev/pts/Y
where Y is the second serial port that you can see in the 1st terminal.
And in this terminal you type the commands.



### Parametrization
You alter the Number of Regs (2-99) and also the Number of the discrete Values by the #define in the server.c code before compiling the code.

## Clean Up
To remove the files that have been created at the end of the execution
```bash
     make cleanall
