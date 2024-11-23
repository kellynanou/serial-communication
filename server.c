#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>

#define BUFFER_SIZE 65536
#define NUM_REGISTERS 99 //select numbers 2-99
#define DISCRETE 5 //the number of discrete values

// Register struct
typedef struct {
    int value;        // Current value of the register
    int min_value;    // Minimum valid value (for range type)
    int max_value;    // Maximum valid value (for range type)
    int allowed_values[DISCRETE]; // Array of discrete allowed values (if any)
    int is_discrete;  // Flag: 1 if discrete, 0 if range
} Register;

// Array to store registers
Register registers[NUM_REGISTERS];

// Function to initialize registers
void init_registers() {
    for (int i = 1; i < NUM_REGISTERS+1; i++) {
        registers[i].value = 0; // Default value
        if ((i%2)==0) {
            // Initialize as discrete registers
            registers[i].is_discrete = 1;
            for(int j=0;j<DISCRETE;j++){
            registers[i].allowed_values[j] = i+j;
        }
        } else {
            // Initialize as range registers
            registers[i].is_discrete = 0;
            registers[i].min_value = 0;
            registers[i].max_value = 255;
        }
    }
}

int is_allowed_value(Register reg, int value) {
    for (int i = 0; i < DISCRETE; i++) {
        if (reg.allowed_values[i] == value) {
            return 1;
        }
    }
    return 0;
}

void process_command(char *command, char *response) {
    if (strncmp(command, "AT+REG", 6) == 0) {
        int reg_num;
        if (sscanf(command + 6, "%d", &reg_num) != 1 || reg_num < 1 || reg_num > NUM_REGISTERS) {
            sprintf(response, "Invalid Register\n");
            return;
        }

        Register *reg = &registers[reg_num];

        // Handle "AT+REG<N>=?" command
        if (strcmp(command + 7 + (reg_num >= 10), "=?") == 0) { // Adjust index for multi-digit numbers
            if (reg->is_discrete) {
                char allowed_values_str[BUFFER_SIZE] = {0};
                for (int i = 0; i < DISCRETE; i++) {
                    char temp[10];
                    sprintf(temp, "%d|", reg->allowed_values[i]);
                    strcat(allowed_values_str, temp);
                }
                allowed_values_str[strlen(allowed_values_str) - 1] = '\0'; // Remove trailing '|'
                sprintf(response, "%s\n", allowed_values_str);
            } else {
                sprintf(response, "%d-%d\n", reg->min_value, reg->max_value);
            }
        }
        // Handle "AT+REG<N>=<value>" command
        else if (strncmp(command + 7 + (reg_num >= 10), "=", 1) == 0) {
            int new_value = atoi(command + 8 + (reg_num >= 10));
            if ((reg->is_discrete && is_allowed_value(*reg, new_value)) ||
                (!reg->is_discrete && new_value >= reg->min_value && new_value <= reg->max_value)) {
                reg->value = new_value;
                sprintf(response, "OK\n");
            } else {
                sprintf(response, "InvalidInput\n");
            }
        }
        // Handle "AT+REG<N>" command
        else if (command[7 + (reg_num >= 10)] == '\0') {
            sprintf(response, "%d\n", reg->value);
        } else {
            sprintf(response, "Invalid Command\n");
        }
    } else if (strcmp(command, "help") == 0) {
        sprintf(response, "\nAvailable Commands for the %d Registers are:\n---  AT+REG<N>: Read the Nth register's value\n---  AT+REG<N>=?: Read the list of all allowed values for the Nth register\n---  REG<N>=<int>: Write the provided integer to Nth register\n", NUM_REGISTERS);
    } else {
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

    cfsetospeed(&tty, B9600);
    cfsetispeed(&tty, B9600);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;
    tty.c_iflag &= ~IGNBRK;
    tty.c_lflag = 0;
    tty.c_oflag = 0;
    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 1;

    tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    tty.c_cflag |= (CLOCAL | CREAD);

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

    init_registers();

    char buffer[BUFFER_SIZE];
    char response[BUFFER_SIZE];

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        memset(response, 0, BUFFER_SIZE);

        ssize_t n = read(serial_fd, buffer, BUFFER_SIZE - 1);
        if (n > 0) {
            buffer[n] = '\0';
            printf("Received: %s\n", buffer); 

            process_command(buffer, response);

            write(serial_fd, response, strlen(response));
        }
    }

    close(serial_fd);
    return EXIT_SUCCESS;
}
