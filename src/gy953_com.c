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
#include "gy953_uart.h"
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
 * @func: constructCommand  construct a command to send to target device
 * @param: hexCommand   function mark of device
 * @param: command      constructed command
 * @retval:             0 is down;
 */
static int constructCommand(int hexCommand, unsigned char *command);

/**
 * @func: uartInit      use to initialization uart, call functions from gy953_uart file
 * @retval:             0 is down
 * */
static int uartInit(void);

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

static int constructCommand(int hexCommand, unsigned char *command) {
    int i;
    for (i=0; i<3; i++) {
        command[i] = (unsigned char)((hexCommand >> ((2-i) * 8)) & 0x0000ff);
        /* printf("%02x\n", command[i]); */
    }
    return 0;
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

/**
 * *    PUBLIC FUNCTION DEFINATION
 * */
int gy953Close(int fd) {
    close(fd);
    return 0;
}


int gy953Init(unsigned char *command) {
    int fd;
    fd = uartInit();
    if (-1 == fd) {
        printf("gy953 uart init error.\n");
        return -1;
    }
    /* constructCommand(AUTODATA_ACCELEROMETER, command); */
    /* constructCommand(AUTODATA_EULERANGLE, command); */
    constructCommand(EULERANGLE, command);
    return fd;
}

int gy953ReceiveData(int fd, unsigned char *command, unsigned int maxlen) {
    unsigned int nread = 0;
    nread = read(fd, command, maxlen);
    if (!nread)
        return -1;
    tcflush(fd, TCIFLUSH);
    return nread;
}

int gy953SendCommand(int fd, unsigned char *command, unsigned int len) {
    int flag = 0;
    flag = write(fd, command, len);
    tcflush(fd, TCOFLUSH);
    return flag;
}

int analysisEulerangle(unsigned char *originData, int len, float *data1, float *data2, float *data3) {
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

int analysisAccelerometer(unsigned char *originData, int len, int *data1, int *data2, int *data3) {
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
