/*****************************************************************
 *
 * @file    gy953_com.c
 * @brief   functions packet of gy953 control.
 * @author  PWE dicksonliuming@gmail.com
 * @date    2016-7-19
 *
 * ***************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <termios.h>        // POSIX terminal input/output header
#include <fcntl.h>
#include "gy953_com.h"

/**
 * *    LOCAL FUNCTION DECLARATION
 * */

/**
 * @func:   analysisData    use to check out all of data from received data(string)
 * @param:  originData      received data(string)
 * @param:  len             length of received data
 * @param:  data1           data 1 in received string, will return to what call it
 * @param:  data2           data 2 in received string, will return to what call it
 * @param:  data3           data 3 in received string, will return to what call it
 * @retval:                 0 is down, -1 is data wrong
 * */
static int analysisData(unsigned char *originData, int len, int *data1, int *data2, int *data3);

/**
 * @func: checkCS       check cs bit in received data
 * @param: data         received data
 * @param: len          length of received data
 * @retval:             0 is right and -1 is wrong cs.
 */
static int checkCS(unsigned char *data, int len);

/**
 * @func: uartInit      use to initialization uart, call functions from gy953_uart file
 * @retval:             0 is down
 * */
static int uartInit(void);

/**
 * @func: setOpt    use to setting uart options
 * @param: fd       file descriptor
 * @param: nSpeed   uart transfer speed
 * @param: nBits    uart transfer bits
 * @param: nEvent   uart parity bit
 * @param: nStop    uart stop bit
 * @param:          0 is down
 **/
static int setOpt(int fd, int nSpeed, int nBits, char nEvent, int nStop);

/**
 * @func: openPort  use to open uart
 * @retval:         0 is down
 **/
static int openPort(void);

/**
 * @func: gy953ReceiveData   use to receive date from device
 * @param: fd           file descriptor
 * @param: command      string used to catch data
 * @param: maxlen       allowed maximum length of receive data
 * @retval:             if catch data, return length of data, else return -1
 * */
static int gy953ReceiveData(int fd, unsigned char *command, unsigned int maxlen);

/**
 * @func: gy953SendCommand   use to send data to device
 * @param: fd           file descriptor
 * @param: command    data
 * @param: len        length of data
 * @retval:         0 is down
 * */
static int gy953SendCommand(int fd, unsigned char *command, unsigned int len);

/**
 * @func:   analysisEulerangle      analysis data of euler angle
 * @param:  originData              received data
 * @param:  len                     length of received data
 * @param:  data1                   return angle x axis
 * @param:  data2                   return angle y axis
 * @param:  data3                   return angle z axis
 * @retval:                         0 is down, -1 is error
 * */
static int analysisEulerangle(unsigned char *originData, int len, float *data1, float *data2, float *data3);

/**
 * @func:   analysisAccelerometer   analysis data of accelerometer
 * @param:  originData              received data
 * @param:  len                     length of received data
 * @param:  data1                   return data group 1
 * @param:  data2                   return data group 2
 * @param:  data3                   return data group 3
 * @retval:                         0 is down and -1 is wrong
 **/
static int analysisAccelerometer(unsigned char *originData, int len, int *data1, int *data2, int *data3) __attribute__ ((unused));

/**
 * *    LOCAL FUNCTION DEFINATION
 * */

static int analysisData(unsigned char *originData, int len, int *data1, int *data2, int *data3) {
    int i=0;
    if (checkCS(originData, len)) {
        printf("data wrong\n");
        return -1;
    }
    *data1 = (unsigned int)(originData[i+4] << 8 | originData[i+5]);
    *data2 = (unsigned int)(originData[i+6] << 8 | originData[i+7]);
    *data3 = (unsigned int)(originData[i+8] << 8 | originData[i+9]);
    return 0;
}

static int checkCS(unsigned char *data, int len) {
    int i;
    unsigned char add = 0x00;
    if (0x5a != data[0] || 0x5a != data[1]) {
        printf("frame header wrong\n");
        return -1;
    }

    for (i=0; i<len-1; i++) {
        add = add + data[i];
    }
    if (add == data[i]) {
        return 0;
    }
    else {
        printf("cs bit wrong\n");
        return -1;
    }
}

static int uartInit(void) {
    int fd;
    if ((fd = openPort()) < 0) {
        perror("open_port error");
        return -1;
    }
    if (setOpt(fd, 115200, 8, 'N', 1) < 0) {
        perror("set_opt error");
        return -1;
    }
    return fd;
}

static int setOpt(int fd, int nSpeed, int nBits, char nEvent, int nStop) {
    struct termios newtio, oldtio;
    if (tcgetattr(fd, &oldtio) != 0) {
        perror("Backup SerialSetting");
        return -1;
    }
    newtio.c_cflag = 0;
    newtio.c_iflag = 0;
    newtio.c_lflag = 0;
    newtio.c_oflag = 0;

    newtio.c_cflag |= CLOCAL | CREAD ;

    // set data bits
    switch(nBits) {
    case 7:
        newtio.c_cflag &= ~CSIZE;           // zero data bits control
        newtio.c_cflag |= CS7;              // set data bits to 7 bits
        break;
    case 8:
        newtio.c_cflag &= ~CSIZE;           // zero data bits control;
        newtio.c_cflag |= CS8;              // set data bits to 8 bits
        break;
    default:
        perror("Set_opt wrong bits");
        return -1;
    }

    // set parity bit
    switch(nEvent) {
    case 'O':
        newtio.c_cflag &= ~PARENB;          // clear even mode
        newtio.c_cflag |= PARODD;           // set parity bit to odd mode
        newtio.c_iflag |= (INPCK |          // allow to execute parity
                                            //error if a parity error occured.
                            ISTRIP);        // reset normal data to 7 bits
                                            //and leave last bit to parity bit.
        break;
    case 'E':
        newtio.c_cflag |= PARENB;           // set parity bit to even mode
        newtio.c_cflag &= ~PARODD;          // clear odd mode
        newtio.c_iflag |= (INPCK | ISTRIP); // same with odd setting code
        break;
    case 'N':
        newtio.c_cflag &= ~PARENB;              // no check bit
        break;
    default:
        perror("Set_opt wrong event");
        return -1;
    }

    // set bandrate
    switch(nSpeed) {
    case 9600:
        cfsetispeed(&newtio, B9600);    // input speed
        cfsetospeed(&newtio, B9600);    // output speed
        break;
    case 115200:
        cfsetispeed(&newtio, B115200);
        cfsetospeed(&newtio, B115200);
        break;
        // you can add other speed items at here.
    default:
        perror("Set_opt wrong speed OR other speed");
        return -1;
    }

    // set stop bits
    if ( 1 == nStop)
        newtio.c_cflag &= ~CSTOPB;      // stop 1 bit
    else if ( 2 == nStop)
        newtio.c_cflag |= CSTOPB;       // stop 2 bits

    newtio.c_cc[VTIME] = 0;             // waitting time
    newtio.c_cc[VMIN] = 0;              // min receive data

    tcflush(fd, TCIOFLUSH);             // before clean input & output buffer

    if ((tcsetattr(fd, TCSANOW, &newtio)) != 0) {   // set argument and take effect at once
        tcsetattr(fd, TCSANOW, &oldtio);    // if wrong, get the old settings
        perror("Set_opt com set error");
        return -1;
    }

    tcflush(fd, TCIOFLUSH);             // after clean input & output buffer
    return 0;
}

static int openPort() {
    int fd;
    fd = open("/dev/ttymxc1", O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (-1 == fd) {
        perror ("Can't Open Serial Port");
        return -1;
    }

    if (isatty(STDIN_FILENO) == 0) {        // check tty
        printf("standard input is not a terminal device \n");
    }
    return fd;
}

static int gy953ReceiveData(int fd, unsigned char *command, unsigned int maxlen) {
    unsigned int nread = 0;
    nread = read(fd, command, maxlen);
    if (!nread)
        return -1;
    tcflush(fd, TCIFLUSH);
    return nread;
}

static int gy953SendCommand(int fd, unsigned char *command, unsigned int len) {
    int flag = 0;
    flag = write(fd, command, len);
    tcflush(fd, TCOFLUSH);
    return flag;
}

static int analysisEulerangle(unsigned char *originData, int len, float *data1, float *data2, float *data3) {
    int da1, da2, da3;
    if (-1 == analysisData(originData, len, &da1, &da2, &da3)) {
        printf("analysis eulerangle wrong\n");
        return -1;
    }
    if (da1 > 32768 || da1 == 32768)
        da1 = -(65535-da1);
    if (da2 > 32768 || da2 == 32768)
        da2 = -(65535-da2);
    if (da3 > 32768 || da2 == 32768)
        da3 = -(65535-da3);
    *data1 = (float)da1 / 100;
    *data2 = (float)da2 / 100;
    *data3 = (float)da3 / 100;
    return 0;
}

static int analysisAccelerometer(unsigned char *originData, int len, int *data1, int *data2, int *data3) {
    if ( -1 == analysisData(originData, len, data1, data2, data3)) {
        printf("analysis accelerometer wrong\n");
        return -1;
    }
    if (*data1 > 32768 || *data1 == 32768) {
        *data1 = -(65535-*data1);
    }
    if (*data2 > 32768 || *data2 == 32768) {
        *data2 = -(65535-*data2);
    }
    return 0;
}

/**
 * *    PUBLIC FUNCTION DEFINATION
 * */

int gy953Init() {
    int fd;
    fd = uartInit();
    if (-1 == fd) {
        printf("gy953 uart init error.\n");
        return -1;
    }
    return fd;
}

int gy953Close(int fd) {
    close(fd);
    return 0;
}

int gy953ConstructCommand(int hexCommand, unsigned char *command) {
    int i;
    for (i=0; i<3; i++) {
        command[i] = (unsigned char)((hexCommand >> ((2-i) * 8)) & 0x0000ff);
        /* printf("%02x\n", command[i]); */
    }
    return 0;
}

void get3AxisEulerAngle(int fd, unsigned char *command, float *result) {
    unsigned char recData[MAXLEN];
    int receiveLen = 0;
    int sendLen = 0;
    float x = 0.0;
    float y = 0.0;
    float z = 0.0;

    sendLen = gy953SendCommand(fd, command, WRITELEN);
    if (-1 == sendLen) {
        printf("send error\n");
#ifdef  DEBUG
        int i;
        /* printf("sendLen = %d", sendLen); */
        /* for (i=0; i<sendLen; i++) */
            /* printf("%02x ", command[i]); */
        /* printf("\n---\n"); */
#endif
        return;
    }
#ifdef  DEBUG
    /* printf("send.\n"); */
#endif
    receiveLen = gy953ReceiveData(fd, recData, MAXLEN);
    if ( -1 == receiveLen) {
        printf("receive error\n");
#ifdef  DEBUG
        int i;
        /* printf("receiveLen: %d\n", receiveLen); */
        /* for (i=0; i<receiveLen; i++) */
            /* printf("%02x ", recData[i]); */
        /* printf("\n---\n"); */
#endif
        return;
    }
#ifdef  DEBUG
    /* printf("receive.\n"); */
    /* printf("receiveLen: %d\n", receiveLen); */
    /* for (i=0; i<receiveLen; i++) */
        /* printf("%02x ", recData[i]); */
    /* printf("\n---\n"); */
#endif
    analysisEulerangle(recData, receiveLen, &x, &y, &z);

    result[0] = x;
    result[1] = y;
    result[2] = z;
}

