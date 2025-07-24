#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define PWM_CHIP "/sys/class/pwm/pwmchip4"
#define PWM_PATH PWM_CHIP "/pwm-4:0"
#define PERIOD_NS 1000000 // 1ms (1kHz)

void writeToFile(const char *path, const char *value)
{
    int fd = open(path, O_WRONLY);
    if (fd < 0)
    {
        perror(path);
        exit(1);
    }
    write(fd, value, strlen(value));
    close(fd);
}

void initPWM()
{
    // Export PWM if not yet exported
    if (access(PWM_PATH, F_OK) == -1)
    {
        writeToFile(PWM_CHIP "/export", "0");
        sleep(1); // Wait for sysfs to populate
    }

    // Set period
    writeToFile(PWM_PATH "/period", "1000000"); // 1ms

    // Enable PWM
    writeToFile(PWM_PATH "/enable", "1");
}

void setDutyCycle(int percent)
{
    char dutyStr[20];
    int duty_ns = PERIOD_NS * percent / 100;
    sprintf(dutyStr, "%d", duty_ns);
    writeToFile(PWM_PATH "/duty_cycle", dutyStr);
}

int main()
{
    int input;
    initPWM();

    while (1)
    {
        printf("Nhập giá trị (1: 100%%, 2: 50%%, 3: 0%%): ");
        scanf("%d", &input);

        if (input == 1)
            setDutyCycle(100);
        else if (input == 2)
            setDutyCycle(50);
        else if (input == 3)
            setDutyCycle(0);
        else
            printf("Giá trị không hợp lệ.\n");
    }

    return 0;
}
