#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>

#define SERIAL_PORT "/dev/pts/3"
#define BUFFER_SIZE 1024

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

int main() {
    int serial_fd = open(SERIAL_PORT, O_RDWR | O_NOCTTY | O_SYNC);
    if (serial_fd < 0) {
        perror("Open serial port");
        return EXIT_FAILURE;
    }

    configure_serial_port(serial_fd);

    char buffer[BUFFER_SIZE];
    while (1) {
        ssize_t n = read(serial_fd, buffer, sizeof(buffer) - 1);
        if (n > 0) {
            buffer[n] = '\0';
            printf("Received: %s\n", buffer);

            // Echo response
            char response[] = "Message received\n";
            write(serial_fd, response, strlen(response));
        }
    }

    close(serial_fd);
    return EXIT_SUCCESS;
}
