#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>

int main() {
    int uart = open("/dev/ttyS1", O_RDONLY | O_NOCTTY);
    if (uart < 0) {
        perror("Không mở được UART1");
        return 1;
    }

    struct termios options;
    tcgetattr(uart, &options);

    cfsetispeed(&options, B9600);
    options.c_cflag &= ~PARENB;  // không parity
    options.c_cflag &= ~CSTOPB;  // 1 stop bit
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;      // 8 bit data
    options.c_cflag |= CREAD | CLOCAL; // bật nhận

    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // raw input
    options.c_iflag &= ~(IXON | IXOFF | IXANY);         // tắt flow control
    options.c_oflag &= ~OPOST;                          // raw output

    tcsetattr(uart, TCSANOW, &options);

    printf("Đang chờ nhận UART1 @9600bps...\n");

    char buf[100];
    while (1) {
        int n = read(uart, buf, sizeof(buf) - 1);
        if (n > 0) {
            buf[n] = '\0';
            printf(">> %s", buf);
        }
    }

    close(uart);
    return 0;
}

