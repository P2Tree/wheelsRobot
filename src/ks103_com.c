#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <string.h>

#include "ks103_com.h"

/**
 * *    LOCAL FUNCTION DECLARATION
 * */

/**
 * @func:   set_i2c_register    write a register in ks103 sensor through i2c
 * @param:  fd                  file descriptor
 * @param:  addr                address of sensor
 * @param:  reg                 number of register in sensor
 * @param:  value               value which to write to register
 * @retval:                     0 is down
 * */
static unsigned int set_i2c_register(unsigned int const fd, unsigned char const addr, unsigned char const reg, unsigned char const value);

/**
 * @func:   get_i2c_register    read a register in ks103 sensor through i2c
 * @param:  fd                  file descriptor
 * @param:  addr                address of sensor
 * @param:  reg                 number of register in sensor
 * @param:  value               reading result
 * @retval:                     0 is down
 * */
static unsigned int get_i2c_register(unsigned int const fd, unsigned char const addr, unsigned char const reg, unsigned char *val);

/**
 * @func:   get_distance        get a distance value from sensor
 * @param:  fd                  file descriptor
 * @param:  addr                address of sensor
 * @param:  reg                 number of regisetr in sensor
 * @param:  distance            distance result
 * @retval:                     0 is down
 * */
static unsigned int get_distance(unsigned int const fd, unsigned char const addr, unsigned char const reg, unsigned int *distance);

/**
 * @func:   change_directory    change i2c address of sensor
 * @param:  fd                  file descriptor
 * @param:  addr                address of sensor in i2c
 * @param:  new_addr            address will be changed to
 * @retval:                     0 is down
 * */
static unsigned char change_directory(unsigned int const fd, unsigned char const addr, unsigned char const new_addr);

/**
 * *    LOCAL FUNCTION DEFINITION
 * */
static unsigned int set_i2c_register(unsigned int fd, unsigned char addr, unsigned char reg, unsigned char value) {
    unsigned char outbuf[2];
    struct i2c_rdwr_ioctl_data  packets;
    struct i2c_msg  messages[1];

    messages[0].addr    = addr;
    messages[0].flags   = 0;
    messages[0].len     = sizeof(outbuf);
    messages[0].buf     = outbuf;

    // The first byte indicates which register we'll write
    outbuf[0] = reg;

    // The second byte indicates the value to write
    outbuf[1] = value;

    // Transfer the i2c packets to the kernel and verify it worked
    packets.msgs = messages;
    packets.nmsgs = 1;
    if ( ioctl(fd, I2C_RDWR, &packets) < 0 ) {
        perror("Unable to send data");
        return 1;
    }
    return 0;
}

static unsigned int get_i2c_register(unsigned int fd, unsigned char addr, unsigned char reg, unsigned char *val) {
    unsigned char inbuf, outbuf;
    struct i2c_rdwr_ioctl_data  packets;
    struct i2c_msg  messages[2];

    // In order to read a register, we first do a "dummy write" by writing
    // 0 bytes to the register we want to read from.
    outbuf = reg;
    messages[0].addr    = addr;
    messages[0].flags   = 0;
    messages[0].len     = sizeof(outbuf);
    messages[0].buf     = &outbuf;

    messages[1].addr    = addr;
    messages[1].flags   = I2C_M_RD;     // | I2C_M_NOSTART
    messages[1].len     = sizeof(inbuf);
    messages[1].buf     = &inbuf;

    // Send the request to the kernel and get the result back
    packets.msgs    = messages;
    packets.nmsgs   = 2;
    if (ioctl(fd, I2C_RDWR, &packets) < 0 ) {
        perror("Unable to send data");
        return 1;
    }
    *val = inbuf;
    return 0;
}

static unsigned int get_distance(unsigned int fd, unsigned char addr, unsigned char reg, unsigned int *distance) {
    unsigned char distance_high;
    unsigned char distance_low;

    set_i2c_register(fd, addr, reg, 0xb4);
    usleep(100000);     // 100ms
    if (get_i2c_register(fd, addr, reg, &distance_high))
        return 1;
    *distance = distance_high << 8;
    if (get_i2c_register(fd, addr, reg+1, &distance_low))
        return 1;
    *distance |= distance_low;

    return 0;
}

static unsigned char change_directory(unsigned int fd, unsigned char addr, unsigned char new_addr) {
    if (set_i2c_register(fd, addr, REG, 0x9a)) {
        return 1;
    }
    usleep(1000);  // 1ms
    if (set_i2c_register(fd, addr, REG, 0x92)) {
        return 1;
    }
    usleep(1000);  // 1ms
    if (set_i2c_register(fd, addr, REG, 0x9e)) {
        return 1;
    }
    usleep(1000);  // 1ms
    if (set_i2c_register(fd, addr, REG, new_addr)) {
        return 1;
    }
    usleep(100000); // 100ms

    return 0;

}

/**
 * *    PUBLIC FUNCTION DEFINITION
 * */
unsigned int distanceAKS103(unsigned int fd, const char *argAddr) {
    unsigned int addr = strtol(argAddr, NULL, 0);
    unsigned int distance = 0;
    if ( get_distance(fd, addr, REG, &distance)) {
        printf("Unable to get distance\n");
        return -1;
    }
    else {
        printf("distance is %d mm\n", distance);
        return distance;
    }
    sleep(1);
}

void distanceMultipleKS103(unsigned int fd, const unsigned char argAddr, const unsigned int sensorNum, int *result) {
    unsigned char begin_addr = argAddr;
    unsigned int i2c_devices_number = sensorNum;
    unsigned char addr_count = 0;
    unsigned int distance = 0;
    for(addr_count = 0; addr_count < i2c_devices_number; addr_count++) {
        if (get_distance(fd, begin_addr+addr_count, REG, &distance)) {
            printf("Unable to get distance, %x", begin_addr+addr_count);
        }
        else {
            /* printf("sensor.%d distance is %d mm\n", addr_count+1, distance); */
            result[addr_count] = distance;
        }
    }
}

void changeKS103Address(unsigned int fd, const char *argAddr, const char *argNewAddr) {
    unsigned char addr = strtol(argAddr, NULL, 0);
    unsigned char new_addr = strtol(argNewAddr, NULL, 0);
    if (change_directory(fd, addr, new_addr)) {
        printf("Unable to change directory\n");
    }
    else {
        printf("old directory is %x, new directory is %x\n", addr, new_addr);
    }
}

unsigned char readKS103Register(unsigned int fd, const char *argAddr, const char *argReg) {
    unsigned int addr = strtol(argAddr, NULL, 0);
    unsigned int reg = strtol(argReg, NULL ,0);
    unsigned char value;
    if ( get_i2c_register(fd, addr, reg, &value) ) {
        printf("Unable to get register!\n");
        return -1;
    }
    else {
        printf("Register %d: %d (%x)\n", reg, (unsigned int)value, (unsigned int)value);
        return value;
    }
}

void writeKS103Register(unsigned int fd, const char *argAddr, const char *argReg, const char *argValue) {
    unsigned int addr = strtol(argAddr, NULL, 0);
    unsigned int reg = strtol(argReg, NULL, 0);
    unsigned int value = strtol(argValue, NULL, 0);
    if (set_i2c_register(fd, addr, reg, value)) {
        printf("Set register %x: %d (%x)\n", reg, value, value);
    }
}

unsigned int KS103Init() {
    int fd;
    fd = open("/dev/i2c-2", O_RDWR);
    if (fd < 0) {
        printf("ks103 i2c open error\n");
        exit(1);
    }
    return fd;
}

