#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>

#define BUFFER_SIZE 65536



// typedef struct {
//     int value;
//     int min_value;
//     int max_value;
// } Register;

// // Array to store registers
// Register registers[10]; // Adjust the size as needed

// // Function to initialize registers
// void init_registers() {
//     // Initialize registers with default values and ranges
//     registers[0].value = 10;
//     registers[0].min_value = 0;
//     registers[0].max_value = 100;

//     // ... Initialize other registers
// }




// Registers
int reg1 = 1;
int reg2 = 2;
int N = 10;

void process_command(char *command, char *response) {
    // Clear the response buffer
   // memset(response, 0, BUFFER_SIZE);

    // Handle REG1 commands
    
    if(strcmp(command, "help") == 0){
        sprintf(response,"\nAvailable Commands for the %d Registers are:\n---  AT+REG<N>: Read the Nth register's value\n---  AT+REG<N>=?: Read the list of all allowed values for the Nth register\n---  REG<N>=<int>: Write the provided integer to Nth register\n",N);
    }else if (strcmp(command, "AT+REG1=?") == 0) {
        sprintf(response, "0-16535\n");
    } else if (strncmp(command, "AT+REG1=", 8) == 0) {
        // Parse the integer value from the command
        char *value_str = command + 8;
        char *endptr;
        int new_value = strtol(value_str, &endptr, 10);

        if (*endptr == '\0' && new_value >= 0 && new_value <= 16535) { // Validate range and input
            reg1 = new_value;
            sprintf(response, "OK\n");
        } else {
            sprintf(response, "InvalidInput\n");
        }
    } else if (strcmp(command, "AT+REG1") == 0) {
        sprintf(response, "%d\n", reg1);
    }
    // Handle REG2 commands
    else if (strcmp(command, "AT+REG2=?") == 0) {
        sprintf(response, "1|2|3\n");
    } else if (strncmp(command, "AT+REG2=", 8) == 0) {
        char *value_str = command + 8;
        char *endptr;
        int new_value = strtol(value_str, &endptr, 10);

        if (*endptr == '\0' && (new_value == 1 || new_value == 2 || new_value == 3)) { // Validate discrete values
            reg2 = new_value;
            sprintf(response, "OK\n");
        } else {
            sprintf(response, "InvalidInput\n");
        }
    } else if (strcmp(command, "AT+REG2") == 0) {
        sprintf(response, "%d\n", reg2);
    } else {
        // Invalid command
        sprintf(response, "Invalid Command\n");
    }
}

void configure_serial_port(int fd) {
    struct termios tty;
    memset(&tty, 0, sizeof(tty));

    if (tcgetattr(fd, &tty) != 0) {
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }

    // Configure baud rate, 8N1, and no flow control
    cfsetospeed(&tty, B9600);
    cfsetispeed(&tty, B9600);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8; // 8 bits per byte
    tty.c_iflag &= ~IGNBRK;                    // Disable break processing
    tty.c_lflag = 0;                           // No signaling chars, no echo
    tty.c_oflag = 0;                           // No remapping, no delays
    tty.c_cc[VMIN] = 1;                        // Read blocks until 1 byte is available
    tty.c_cc[VTIME] = 1;                       // Timeout in deciseconds

    tty.c_iflag &= ~(IXON | IXOFF | IXANY);    // Shut off xon/xoff ctrl
    tty.c_cflag |= (CLOCAL | CREAD);           // Enable receiver, ignore modem status lines

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }
}
int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <serial_port>\n", argv[0]);
        return 1;
    }
    const char *serial_port = argv[1];
    printf("Server running on port: %s\n", argv[1]);
    int serial_fd = open(serial_port, O_RDWR | O_NOCTTY | O_SYNC);
    if (serial_fd < 0) {
        perror("Open serial port");
        return EXIT_FAILURE;
    }

    configure_serial_port(serial_fd);

    char buffer[BUFFER_SIZE];
    char response[BUFFER_SIZE];

    //init_registers();

    while (1) {
        // Clear buffers
        memset(buffer, 0, BUFFER_SIZE);
        memset(response, 0, BUFFER_SIZE);

        // Read data from the serial port
        ssize_t n = read(serial_fd, buffer, BUFFER_SIZE - 1);
        if (n > 0) {
            buffer[n] = '\0'; // Null-terminate the input
            printf("Received: %s\n", buffer);

            // Process the command and prepare the response
            process_command(buffer, response);

            // Write the response back to the serial port
            write(serial_fd, response, strlen(response));
        }
    }

    close(serial_fd);
    return EXIT_SUCCESS;
}



// #include <stdio.h>
// #include <stdlib.h>
// #include <fcntl.h>
// #include <unistd.h>
// #include <termios.h>
// #include <string.h>

// #define SERIAL_PORT "/dev/pts/5" // Adjust as needed
// #define BUFFER_SIZE 65536



// // typedef struct {
// //     int value;
// //     int min_value;
// //     int max_value;
// // } Register;

// // // Array to store registers
// // Register registers[10]; // Adjust the size as needed

// // // Function to initialize registers
// // void init_registers() {
// //     // Initialize registers with default values and ranges
// //     registers[0].value = 10;
// //     registers[0].min_value = 0;
// //     registers[0].max_value = 100;

// //     // ... Initialize other registers
// // }




// // Registers
// int reg1 = 1;
// int reg2 = 2;
// int N = 10;

// void process_command(char *command, char *response) {
//     // Clear the response buffer
//    // memset(response, 0, BUFFER_SIZE);

//     // Handle REG1 commands
    
//     if(strcmp(command, "help") == 0){
//         sprintf(response,"\nAvailable Commands for the %d Registers are:\n---  AT+REG<N>: Read the Nth register's value\n---  AT+REG<N>=?: Read the list of all allowed values for the Nth register\n---  REG<N>=<int>: Write the provided integer to Nth register\n",N);
//     }else if (strcmp(command, "AT+REG1=?") == 0) {
//         sprintf(response, "0-16535\n");
//     } else if (strncmp(command, "AT+REG1=", 8) == 0) {
//         // Parse the integer value from the command
//         char *value_str = command + 8;
//         char *endptr;
//         int new_value = strtol(value_str, &endptr, 10);

//         if (*endptr == '\0' && new_value >= 0 && new_value <= 16535) { // Validate range and input
//             reg1 = new_value;
//             sprintf(response, "OK\n");
//         } else {
//             sprintf(response, "InvalidInput\n");
//         }
//     } else if (strcmp(command, "AT+REG1") == 0) {
//         sprintf(response, "%d\n", reg1);
//     }
//     // Handle REG2 commands
//     else if (strcmp(command, "AT+REG2=?") == 0) {
//         sprintf(response, "1|2|3\n");
//     } else if (strncmp(command, "AT+REG2=", 8) == 0) {
//         char *value_str = command + 8;
//         char *endptr;
//         int new_value = strtol(value_str, &endptr, 10);

//         if (*endptr == '\0' && (new_value == 1 || new_value == 2 || new_value == 3)) { // Validate discrete values
//             reg2 = new_value;
//             sprintf(response, "OK\n");
//         } else {
//             sprintf(response, "InvalidInput\n");
//         }
//     } else if (strcmp(command, "AT+REG2") == 0) {
//         sprintf(response, "%d\n", reg2);
//     } else {
//         // Invalid command
//         sprintf(response, "Invalid Command\n");
//     }
// }

// void configure_serial_port(int fd) {
//     struct termios tty;
//     memset(&tty, 0, sizeof(tty));

//     if (tcgetattr(fd, &tty) != 0) {
//         perror("tcgetattr");
//         exit(EXIT_FAILURE);
//     }

//     // Configure baud rate, 8N1, and no flow control
//     cfsetospeed(&tty, B9600);
//     cfsetispeed(&tty, B9600);

//     tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8; // 8 bits per byte
//     tty.c_iflag &= ~IGNBRK;                    // Disable break processing
//     tty.c_lflag = 0;                           // No signaling chars, no echo
//     tty.c_oflag = 0;                           // No remapping, no delays
//     tty.c_cc[VMIN] = 1;                        // Read blocks until 1 byte is available
//     tty.c_cc[VTIME] = 1;                       // Timeout in deciseconds

//     tty.c_iflag &= ~(IXON | IXOFF | IXANY);    // Shut off xon/xoff ctrl
//     tty.c_cflag |= (CLOCAL | CREAD);           // Enable receiver, ignore modem status lines

//     if (tcsetattr(fd, TCSANOW, &tty) != 0) {
//         perror("tcsetattr");
//         exit(EXIT_FAILURE);
//     }
// }

// int main() {
//     int serial_fd = open(SERIAL_PORT, O_RDWR | O_NOCTTY | O_SYNC);
//     if (serial_fd < 0) {
//         perror("Open serial port");
//         return EXIT_FAILURE;
//     }

//     configure_serial_port(serial_fd);

//     char buffer[BUFFER_SIZE];
//     char response[BUFFER_SIZE];

//     //init_registers();

//     while (1) {
//         // Clear buffers
//         memset(buffer, 0, BUFFER_SIZE);
//         memset(response, 0, BUFFER_SIZE);

//         // Read data from the serial port
//         ssize_t n = read(serial_fd, buffer, BUFFER_SIZE - 1);
//         if (n > 0) {
//             buffer[n] = '\0'; // Null-terminate the input
//             printf("Received: %s\n", buffer);

//             // Process the command and prepare the response
//             process_command(buffer, response);

//             // Write the response back to the serial port
//             write(serial_fd, response, strlen(response));
//         }
//     }

//     close(serial_fd);
//     return EXIT_SUCCESS;
// }


