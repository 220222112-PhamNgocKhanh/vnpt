#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <time.h>

#define GPIO1_BASE        0x4804C000
#define GPIO_SIZE         0x1000
#define GPIO_OE           0x134
#define GPIO_DATAIN       0x138
#define GPIO_SETDATAOUT   0x194
#define GPIO_CLEARDATAOUT 0x190
#define GPIO_INDEX        28    // GPIO1_28 (P9_12)

#define DMTIMER2_BASE     0x48040000
#define DMTIMER_SIZE      0x1000
#define DMTIMER_TCLR      0x038
#define DMTIMER_TCRR      0x03C
#define DMTIMER_TLDR      0x040
#define DMTIMER_TIOCP_CFG 0x010
#define DMTIMER_TSICR     0x054

#define TIMEOUT_US        200
#define BIT_THRESHOLD_US  35

static volatile uint32_t *gpio = NULL;
static volatile uint32_t *dmtimer = NULL;

static int init_gpio() {
    int fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd < 0) {
        perror("open /dev/mem (gpio)");
        return -1;
    }
    gpio = mmap(NULL, GPIO_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, GPIO1_BASE);
    close(fd);
    return (gpio == MAP_FAILED) ? -1 : 0;
}

static int init_dmtimer() {
    int fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd < 0) {
        perror("open /dev/mem (timer)");
        return -1;
    }
    dmtimer = mmap(NULL, DMTIMER_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, DMTIMER2_BASE);
    close(fd);
    if (dmtimer == MAP_FAILED) return -1;

    dmtimer[DMTIMER_TIOCP_CFG / 4] = 0x3;
    while (dmtimer[DMTIMER_TIOCP_CFG / 4] & 1); // wait reset complete
    dmtimer[DMTIMER_TSICR / 4] = 0x0;
    dmtimer[DMTIMER_TCLR / 4] = 0x0;
    dmtimer[DMTIMER_TLDR / 4] = 0;
    dmtimer[DMTIMER_TCRR / 4] = 0;
    dmtimer[DMTIMER_TCLR / 4] = 0x3;  // enable + autoreload
    return 0;
}

static void delay_us(unsigned int us) {
    uint32_t start = dmtimer[DMTIMER_TCRR / 4];
    while ((dmtimer[DMTIMER_TCRR / 4] - start) < us * 24);
}

static void set_input()  { gpio[GPIO_OE / 4] |=  (1 << GPIO_INDEX); }
static void set_output() { gpio[GPIO_OE / 4] &= ~(1 << GPIO_INDEX); }
static void digital_write(int val) {
    if (val)
        gpio[GPIO_SETDATAOUT / 4] = (1 << GPIO_INDEX);
    else
        gpio[GPIO_CLEARDATAOUT / 4] = (1 << GPIO_INDEX);
}

static int digital_read() {
    return (gpio[GPIO_DATAIN / 4] & (1 << GPIO_INDEX)) ? 1 : 0;
}

static int wait_level(int level, int timeout_us) {
    struct timespec start, now;
    clock_gettime(CLOCK_MONOTONIC, &start);
    while (1) {
        if (digital_read() == level) return 0;
        clock_gettime(CLOCK_MONOTONIC, &now);
        long dt = (now.tv_sec - start.tv_sec) * 1000000 + (now.tv_nsec - start.tv_nsec) / 1000;
        if (dt > timeout_us) return -1;
    }
}

static int read_dht11(uint8_t data[5]) {
    memset(data, 0, 5);
    set_output();
    digital_write(0);
    delay_us(18000);
    digital_write(1);
    delay_us(40);
    set_input();

    if (wait_level(0, TIMEOUT_US) < 0 || wait_level(1, TIMEOUT_US) < 0) return -1;

    for (int i = 0; i < 40; i++) {
        if (wait_level(0, TIMEOUT_US) < 0 || wait_level(1, TIMEOUT_US) < 0) return -1;
        struct timespec t1, t2;
        clock_gettime(CLOCK_MONOTONIC, &t1);
        while (digital_read()) {
            clock_gettime(CLOCK_MONOTONIC, &t2);
            if (((t2.tv_sec - t1.tv_sec) * 1000000 + (t2.tv_nsec - t1.tv_nsec) / 1000) > TIMEOUT_US) return -1;
        }
        long pulse = (t2.tv_sec - t1.tv_sec) * 1000000 + (t2.tv_nsec - t1.tv_nsec) / 1000;
        int bit = (pulse > BIT_THRESHOLD_US);
        data[i / 8] <<= 1;
        data[i / 8] |= bit;
    }

    uint8_t chk = data[0] + data[1] + data[2] + data[3];
    return (chk & 0xFF) == data[4] ? 0 : -2;
}

int main() {
    if (init_gpio() < 0 || init_dmtimer() < 0) return 1;

    while (1) {
        uint8_t d[5];
        int res = read_dht11(d);
        if (res == 0) {
            printf("Humidity: %d.%d%%  Temperature: %d.%d°C\n", d[0], d[1], d[2], d[3]);
        } else {
            printf("DHT11 read failed (code %d)\n", res);
        }
        delay_us(2000000);  // Delay 2s
    }

    return 0;
}

