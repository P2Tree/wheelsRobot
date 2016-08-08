#ifndef KS103_I2C_H_V0TRSQPI
#define KS103_I2C_H_V0TRSQPI

#define KS103ADDRESS_1  0x74
#define KS103ADDRESS_2  0x75
#define KS103ADDRESS_3  0x76
#define KS103ADDRESS_4  0x77

#define REG             0x02
#define I2C_FILE_NAME   "/dev/i2c-0"
#define USAGE_MESSAGE \
        "Usage: \n" \
        " '%s -d [addr]'  " \
        " to get distance from ks103, like: ./ks103-i2c-test -d 0x74 \n" \
        " '%s -c [addr] [new addr]'  " \
        " to change ks103 directory, like: ./ks103-i2c-test -c 0x74 0x75\n" \
        " '%s -r [addr] [register]'  " \
        " to read value from [register]\n" \
        " '%s -w [addr] [register] [value]'  " \
        " to write value to [register]\n" \
        ""

unsigned char readKS103Register(unsigned int fd, const char *argAddr, const char *argReg);

void distanceMultipleKS103(unsigned int fd, const unsigned char argAddr, const unsigned int sensorNum, int *result);

unsigned int distanceAKS103(unsigned int fd, const char *argAddr);

void changeKS103Address(unsigned int fd, const char *argAddr, const char *argNewAddr);

void writeKS103Register(unsigned int fd, const char *argAddr, const char *argReg, const char *argValue);

/**
 * @func:   KS103Init       initialization of ks103 code module
 * @param:  port            uart port name
 * @retval: int             0 is down.
 * */
int KS103Init(const char *port);

#endif /* end of include guard: KS103_I2C_H_V0TRSQPI */
