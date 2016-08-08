/***************************************************************
 *
 * @file    cy30-com.c
 * @brief    cy30 communication and control file
 * @author    PWE <dicksonliuming@gmail.com>
 * @date    2016-7-12
 *
 ***************************************************************/

#include <stdio.h>
#include <termios.h>
#include <memory.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "cy30_com.h"
#include <stdlib.h>


/**
 * ** LOCAL FUNCTIONS DECLARATION
 */

/**
 * calculateCS: to calculate CS in command.
 * @param   precommand      part of command without CS
 * @param   len             length of precommand
 * @retval                  return CS, 0x00 is fault
 */
static unsigned char calculateCS(unsigned char * precommand, unsigned int len);

/**
 * checkCS: check received data is right with cs bit
 * @param   origin          received data
 * @param   len             lenght of receive command
 * @retval                  0 is right cs, 1 is wrong cs, -1 is fault
 */
static int checkCS(unsigned char * origin, unsigned int len);

/**
 * calculateDistance: used to calculate distance
 * @param   originDist      part chars of distance in received data, always 7 chars
 * @retval                  distance in float form
 */
static float calculateDistance(unsigned char *originDist);
/**
 * setOpt: This function is used to set serial communication argument.
 * @param  fd     serial com file point
 * @param  nSpeed serial transfer speed
 * @param  nBits  serial transfer data bits
 * @param  nEvent serial transfer parity mode
 * @param  nStop  serial transfer stop bit
 * @return        0 is down
 */
static int setOpt(int fd, int nSpeed, int nBits, char nEvent, int nStop);

/**
 * openPort: function for open port, you should change uart device name in it.
 * @param   flag        device flag
 * @return 0 is down
 */
static int openPort(int flag);

/**
 * @func:   uartInit    initialization of uart, contain open_port and set_opt funcs
 * @retval:             int is file descriptor
 * */
static int uartInit(int flag);

/**
 * * LOCAL FUNCTION DEFINATION
 * */

static int uartInit(int flag) {
    int fd;
    if((fd = openPort(flag)) < 0) {
        perror("open_port error");
        return -1;
    }
    if(setOpt(fd, 9600, 8, 'N', 1) < 0) {
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

    newtio.c_cflag |= CLOCAL | CREAD;
    newtio.c_iflag &= ~(INPCK | ISTRIP     // close parity bits setting
                | IUCLC);    // close auto case change

    switch(nBits) {
    case 7:
        newtio.c_cflag &= ~CSIZE;
        newtio.c_cflag |= CS7;
        break;
    case 8:
        newtio.c_cflag &= ~CSIZE;
        newtio.c_cflag |= CS8;
        break;
    default:
        perror("Set_opt wrong bits");
        return -1;
    }

    switch(nEvent) {
    case 'O':
        newtio.c_cflag &= ~PARENB;
        newtio.c_cflag |= PARODD;
        newtio.c_iflag |= (INPCK | ISTRIP);
        break;
    case 'E':
        newtio.c_cflag |= PARENB;
        newtio.c_cflag &= ~PARODD;
        newtio.c_iflag |= (INPCK | ISTRIP);
        break;
    case 'N':
        newtio.c_cflag &= ~PARENB;
        break;
    default:
        perror("Set_opt wrong event");
        return -1;
    }

    switch(nSpeed) {
    case 9600:
        cfsetispeed(&newtio, B9600);
        cfsetospeed(&newtio, B9600);
        break;
    default:
        perror("Set_opt wrong speed OR other speed");
        return -1;
    }
    if ( 1 == nStop)
        newtio.c_cflag &= ~CSTOPB;        // stop 1 bit
    else if ( 2 == nStop)
        newtio.c_cflag |= CSTOPB;        // stop 2 bits
    newtio.c_cc[VTIME] = 0;                // waitting time
    newtio.c_cc[VMIN] = 0;                // min receive data

    tcflush(fd, TCIOFLUSH);
    if ((tcsetattr(fd, TCSANOW, &newtio)) != 0) {
        tcsetattr(fd, TCSANOW, &oldtio);
        perror("Set_opt com set error");
        return -1;
    }
    tcflush(fd, TCIOFLUSH);
    return 0;
}

static int openPort(int flag) {
    int fd;
    if (0 == flag) {
        if (-1 == (fd = open("/dev/ttymxc3", O_RDWR | O_NOCTTY | O_NONBLOCK)) ) {
            perror ("Can't Open Serial Port");
            return -1;
        }
    }
    else if (1 == flag) {
        if (-1 == (fd = open("/dev/ttymxc2", O_RDWR | O_NOCTTY | O_NONBLOCK)) ) {
            perror ("Can't Open Serial Port");
            return -1;
        }
    }
    else {
        printf("wrong device flag! flag = %d\n", flag);
        return -2;
    }
    return fd;
}

static unsigned char calculateCS(unsigned char * precommand, unsigned int len) {
    char cs = 0x00;
    unsigned int i;
    if (len <= 0){
        printf("Error: calculateCS wrong argument: len\n");
        return -1;
    }
    for (i=0; i<len; i++) {
        cs = precommand[i] + cs;
    }
    cs = ~cs + 1;
    return cs;
}

static int checkCS(unsigned char * origin, unsigned int len) {
    char value  = 0x00;
    unsigned int i;
    if (len <= 0) {
        printf("Error: checkCS wrong argument: len\n");
        return -1;
    }

    for (i=0; i<len-1; i++) {
        value = value + origin[i];
    }
    value = ~value + 1;
    if (value == origin[len-1])
        return 0;   // right cs
    else
        return 1;   // wrong cs
}

static float calculateDistance(unsigned char * originDist) {
    return (float)(originDist[0] - '0')*100 + (float)(originDist[1] - '0')*10 + 
        (float)(originDist[2] - '0') + (float)(originDist[4] - '0')*0.1 + 
        (float)(originDist[5] - '0')*0.01 + (float)(originDist[6] - '0')*0.001;
}

int cy30Init(int flag) {
    int fd;
    fd = uartInit(flag);
    if (-1 == fd) {
        printf("cy30 uart init error.\n");
        return -1;
    }
    return fd;
}

int cy30ConstructCommand(Mode mode, unsigned char address, Action action, unsigned char **cmd ) {
    unsigned int len = 0;
    unsigned char cs = 0x00;
    switch(action) {
        case ReadArguments : break;
        case ReadMachineNum : break;
        case SetAddress : break;
        case CalibrateDistance : break;
        case SetMeaInterver : break;
        case SetPosition : break;
        case SetRange : break;
        case SetFrequence : break;
        case SetResolution : break;
        case SetMeasureInBoot : break;
        case MeasureOnceInBuffer : break;
        case ReadBuffer : break;
        case MeasureOnce :
            len = 4;
            *cmd = (unsigned char*)calloc(len, sizeof(unsigned char));
            (*cmd)[0] = address;
            (*cmd)[1] = MEASURE_READ;
            (*cmd)[2] = MEASURE_READ_READONCE;
            len = 3;
            break;
        case MeasureContinuous : break;
        case SetLaser : break;
        case Shutdown : break;
        default:
            printf("Constructing command: Error Action\n");
            return -1;
    }
    cs = calculateCS(*cmd, len);
    (*cmd)[len] = cs;
    len += 1;

    if (len > 0)
        return len;
    else {
        printf("Construct command fault\n");
        return -1;
    }
}

int cy30DistanceMultiple(int fd1, int fd2, wrBuffer dev1Buffer, wrBuffer dev2Buffer) {
    int ret = 0;
    tcflush(fd1, TCOFLUSH);
    write(fd1, dev1Buffer.command, dev1Buffer.cmdlen);
    sleep(1);
    dev1Buffer.readlen = read(fd1, dev1Buffer.readData, READLEN);
    tcflush(fd1, TCIFLUSH);

    tcflush(fd2, TCOFLUSH);
    write(fd2, dev2Buffer.command, dev2Buffer.cmdlen);
    sleep(1);
    dev2Buffer.readlen = read(fd2, dev2Buffer.readData, READLEN);
    tcflush(fd2, TCIFLUSH);

    if (!(dev1Buffer.readlen > 0)) {
        printf("sensor1 read no data\n");
        ret = ret | 0x01;
    }
    else if (!(dev2Buffer.readlen > 0)) {
        printf("sensor2 read no data\n");
        ret = ret | 0x02;
    }

    return ret;
}

int cy30GetData(int fd, wrBuffer *devBuffer) {
#ifdef  DEBUG_CY30
    int i;
    printf("commandlen = %d\n", (*devBuffer).cmdlen);
    printf("command is: ");
    for (i=0; i<4; i++)
        printf("%x ", (*devBuffer).command[i]);
    printf("\n");
#endif

    tcflush(fd, TCOFLUSH);
    write(fd, (*devBuffer).command, (*devBuffer).cmdlen);
    sleep(2);
    (*devBuffer).readlen = read(fd, (*devBuffer).readData, READLEN);
#ifdef  DEBUG_CY30
    printf("readlen = %d\n", (*devBuffer).readlen);
    printf("receive data is: ");
    for (i=0; i<(*devBuffer).readlen; i++)
        printf("%x ", (*devBuffer).readData[i]);
    printf("\n");
#endif
    tcflush(fd, TCIFLUSH);

    if (READLEN != (*devBuffer).readlen+1) {
        /* printf("sensor fd = %d read no data\n", fd); */
        return -1;
    }
    return 0;
}

int cy30ResultProcess(DistanceContainer *container, wrBuffer devBuffer, Action action) {
    unsigned char originDist[7];
    unsigned char *origin = devBuffer.readData;
    unsigned int len = devBuffer.readlen;
    int i;
    // E R R - -
    if (0x45 == origin[3] && 0x52 == origin[4] && 0x52 == origin[5]) {
        /* printf("wrong distance: receive ERR message.\n"); */
        return -1;
    }
#ifdef DEBUG_CY30
    printf("check data down\n");
#endif
    if (checkCS(origin, len)) {
        printf("Error: receive error result, checkCS stop.\n");
        return -2;
    }
#ifdef DEBUG_CY30
    printf("check cs down\n");
#endif
    switch(action) {
        case ReadArguments : break;
        case ReadMachineNum : break;
        case SetAddress : break;
        case CalibrateDistance : break;
        case SetMeaInterver : break;
        case SetPosition : break;
        case SetRange : break;
        case SetFrequence : break;
        case SetResolution : break;
        case SetMeasureInBoot : break;
        case MeasureOnceInBuffer : break;
        case ReadBuffer : break;
        case MeasureOnce :
            if (!(RECEIVE_READONCE == origin[2]))
                return -1;
            (*container).address = origin[0];
            for (i=0; i<7; i++)
                originDist[i] = origin[3+i];
            (*container).distance = calculateDistance(originDist);
            break;
        case MeasureContinuous : break;
        case SetLaser : break;
        case Shutdown : break;
        default:
            printf("analysis command: Error Action\n");
            return -1;
    }
    return 0;
}

int cy30GetDistance(int fd, wrBuffer *devBuffer, DistanceContainer *container, Action action) {
    int ret = 0;
    ret = cy30GetData(fd, devBuffer);
#ifdef  DEBUG_CY30
    printf("cy30GetData ret = %d\n", ret);
#endif
    // catch data
    if ( !ret ) {
        // process data
        if (0 == cy30ResultProcess(container, *devBuffer, MeasureOnce)) {
#ifdef  DEBUG_CY30
            printf("address is 0x%02X , distance is %.3f\n", (*container).address, (*container).distance);
#endif
        }
        else {
            printf("CY30 sensor distance error data\n");
            return -2;
        }
        memset((*devBuffer).readData, 0, 11*sizeof(unsigned char));
        return 0;
    }
    else {
        printf("CY30 sensor distance no data\n");
        return -1;
    }
}


