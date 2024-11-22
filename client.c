#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>

#define SERIAL_PORT "/dev/pts/6"
#define BUFFER_SIZE 65536


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

int main() {
int i=0;
    int serial_fd = open(SERIAL_PORT, O_RDWR | O_NOCTTY | O_SYNC);
    if (serial_fd < 0) {
        perror("Open serial port");
        return EXIT_FAILURE;
    }

    configure_serial_port(serial_fd);
	while(1){


char command[BUFFER_SIZE];
        printf("Enter AT command: ");
        fgets(command, BUFFER_SIZE, stdin);
        if (strcmp(command, "quit\n") == 0) {
            break;
        }

        // Remove the newline character
        command[strcspn(command, "\n")] = '\0';

     if (write(serial_fd, command, strlen(command)) < 0) {
            perror("send failed");
            exit(EXIT_FAILURE);
        }

        char response[BUFFER_SIZE];

        if (read(serial_fd,response, BUFFER_SIZE) < 0) {
            perror("recv failed");
            exit(EXIT_FAILURE);
        }

        printf("%s\n", response);
    memset(response, 0, BUFFER_SIZE);

}
    close(serial_fd);
    return EXIT_SUCCESS;
}

