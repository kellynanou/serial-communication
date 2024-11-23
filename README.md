# serial-communication


This project implements a system with an **AT-Command based server** and a **cli-enabled client** that communicates with it via a serial port.



This system consists of two main components:
- **Server (`server.c`)**: A program that listens on a serial port, processes commands from a client, and manages a set of registers. The server supports reading and writing values to registers, as well as querying for allowed values for discrete registers and sending the response back to the client.
- **Client (`client.c`)**: A simple command-line interface to send AT commands to the server over a serial connection.

### Features
- **Dynamic Register Management**: The server maintains up to 99 registers, each of which can be configured to either accept discrete values or a range of values.
- **AT Commands**: The system accepts commands to interact with registers, such as reading values, setting new values, or querying allowed values.
- **Serial Communication**: Communication between the client and server occurs over a serial interface.


## Installation

1. Clone this repository:
    git clone [git@github.com:kellynanou/serial-communication.git]{https://github.com/kellynanou/serial-communication.git}
2. 
