#ifndef KS103_I2C_H_V0TRSQPI
#define KS103_I2C_H_V0TRSQPI

//#define I2C_ADDRESS    0xe8
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

void distanceMultipleKS103(unsigned int fd, const char *argAddr, const char *sensorNum);

unsigned int distanceAKS103(unsigned int fd, const char *argAddr);

void changeKS103Address(unsigned int fd, const char *argAddr, const char *argNewAddr);

void writeKS103Register(unsigned int fd, const char *argAddr, const char *argReg, const char *argValue);

unsigned int KS103Init();

#endif /* end of include guard: KS103_I2C_H_V0TRSQPI */
