/**
 * @file    cy30-com.h
 * @brief   This file is the header file of cy30 module communication and control.
 * @author  PWE <dicksonliuming@gmail.com>
 * @date    2016-7-12
 */
#ifndef CY30_COM_H
#define CY30_COM_H

/**
 * ** ARGUMENTS
 */
#define READLEN                     12      // The max read string length, and you know, it will catch 11 bits data, the last one bit is \n

// debug flag
#define DEBUG_CY30x

/**
 * ** PARAMENTS
 */
/* commands is always follow the form of: [HEADER] [READ/WRITE] [CONTENT] [C] */

#define DEPLOYHEADER                0XFA
#define DEPLOY_READ                 0X06
#define DEPLOY_WRITE                0X04
// measure header is the address of device which want to measure
// #define MEASUREHEADER
#define MEASURE_READ                0X06
#define MEASURE_WRITE               0X04

// deploy function of read
#define DEPLOY_READ_ARGUMENTS       0X01        // like: FA 06 01 FF
#define DEPLOY_READ_MACHINENUM      0X04        // like: FA 06 04 FC
//measure once and save result in module buffer"
#define DEPLOY_READ_ONCEINBUFF      0X06        // like: FA 06 06 FA

// deploy function of write
#define DEPLOY_WRITE_ADDRESS        0X01        // like: FA 04 01 ADDR(1byte) CS
#define DEPLOY_WRITE_DISTANCECALIBRATION    0X06        // like: FA 04 06 MARK('-' is 0x2d, '+' is 0x2b)(1byte) CALIBEATION(1byte) CS
#define DEPLOY_WRITE_MEAINTERVER    0X05        // like: FA 04 05 MEAINTERVER(1byte) CS
#define DEPLOY_WRITE_POSITION       0X08        // like: FA 04 08 POSITION(1byte) CS
#define DEPLOY_WRITE_RANGE          0X09        // like: FA 04 09 RANGE(05,10,30,50,80,1byte) CS
#define DEPLOY_WRITE_FREQUENCE      0X0A        // like: FA 04 0A FREQ(05,10,20,1byte) CS
#define DEPLOY_WRITE_RESOLUTION     0X0C        // like: FA 04 0C RESOLUTION(1(1mm),2(0.1mm),1byte) CS
#define DEPLOY_WRITE_MEASUREINBOOT  0X0D        // like: FA 04 0D Start(0(close),1(open),1byte) CS

// measure function of read
#define MEASURE_READ_BUFFER         0X07        // like: ADDR 06 07 CS
#define MEASURE_READ_READONCE       0X02        // like: ADDR 06 02 CS
#define MEASURE_READ_READCONTINUOUS 0X03        // like: ADDR 06 03 CS
// other functions
// control laser to open or close
#define MEASURE_READ_LASER          0X05        // like: ADDR 06 05 LASER(0(close),1(open),1byte) CS
// shutdown sensor
#define MEASURE_WRITE_SHUTDOWN      0X02        // like: ADDR 04 02 CS

// receive marks
#define RECEIVE_ARGUMENTS           DEPLOY_READ_ARGUMENTS + 0X80        // like: FA 06 81 ADDR XX XX XX ... CS
#define RECEIVE_MACHINENUM          DEPLOY_READ_MACHINENUM + 0x80       // like: FA 06 84 DAT1 DAT2 ... DAT16 CS

#define RECEIVE_MEAINTERVER         DEPLOY_WRITE_MEAINTERVER + 0X80     // like: FA 04 85 7D(is success)
#define RECEIVE_POSITION            DEPLOY_WRITE_POSITION + 0X80        // like: FA 04 88 7A(is success)
#define RECEIVE_RANGE               DEPLOY_WRITE_RANGE + 0X80           // like: FA 04 89 79(is success)
#define RECEIVE_FREQUENCE           DEPLOY_WRITE_FREQUENCE + 0x80       // like: FA 04 8A 78(is success)
#define RECEIVE_RESOLUTION          DEPLOY_WRITE_RESOLUTION + 0x80      // like: FA 04 8C 76(is success)
#define RECEIVE_MEASUREINBOOT       DEPLOY_WRITE_MEASUREINBOOT + 0x80   // like: FA 04 8D 75(is success)

#define RECEIVE_BUFFER              RECEIVE_READONCE
#define RECEIVE_READONCE            MEASURE_READ_READONCE + 0X80         // like: ADDR 06 82 3X 3X 3X 2E 3X 3X 3X CS
#define RECEIVE_READCONTINUOUS      MEASURE_READ_READCONTINUOUS + 0X80   // like: ADDR 06 83 3X 3X 3X 2E 3X 3X 3X CS

#define RECEIVE_LASER               MEASURE_READ_LASER + 0x80           // like: ADDR 06 85 01 CS(is success)
#define RECEIVE_SHUTDOWN            MEASURE_WRITE_SHUTDOWN + 0X80       // like: ADDR 04 82 CS(is success)

/**
 * ** MARKS
 */

typedef enum {
    Deploy          = 0,
    Measure         = 1
}Mode;

typedef enum {
    ReadArguments   = 0,
    ReadMachineNum  = 1,
    SetAddress      = 2,
    CalibrateDistance   = 3,
    SetMeaInterver  = 4,
    SetPosition     = 5,
    SetRange        = 6,
    SetFrequence    = 7,
    SetResolution   = 8,
    SetMeasureInBoot   = 9,
    MeasureOnceInBuffer = 10,
    ReadBuffer      = 11,
    MeasureOnce     = 12,
    MeasureContinuous   = 13,
    SetLaser        = 14,
    Shutdown        = 15
}Action;

/**
 * ** CONTAINER
 */

typedef struct container{
    unsigned char address;
    float distance;
}DistanceContainer;

typedef struct wrbuffer{
    unsigned char *command; // send to sensor
    unsigned int cmdlen;             // command length
    unsigned char readData[READLEN];// receive from sensor
    unsigned int readlen;
}wrBuffer;

/**
 * @func    cy30Init        initialization cy30 code module
 * @param   port            uart port name
 * @retval  int             0 is down
 * */
int cy30Init(const char *port, wrBuffer *devBuffer);

/**
 * @func    cy30ConstructCommand: to construct command, full command
 * @param   mode            Mode: Depoly or Measure
 * @param   address         address of sensor
 * @param   action          Action: one of action in Action
 * @param   command         return command
 * @retval                  return length of command, -1 is fault
 *
 */
int cy30ConstructCommand(Mode mode, unsigned char address, Action action, unsigned char **cmd );

/**
 * @func    cy30DistanceMultiple    contain catch distance from cy30 sensor with multiple devices, two devices, NOT GOOD ENOUGH TO USE
 * @param   fd1                     fd1 is file descriptor device 1
 * @param   fd2                     fd2 is file descriptor device 2
 * @param   dev1Buffer              send and read buffer of device 1
 * @param   dev2Buffer              send and read buffer of device 2
 * @retval                          0 is down -1 is error read
 * */
int cy30DistanceMultiple(int fd1, int fd2, wrBuffer dev1Buffer, wrBuffer dev2Buffer);
    
/**
 * @func    cy30GetData             catch distance from sensor
 * @param   fd                      file descriptor of device
 * @param   devBuffer               send and read buffer of device
 * @retval                          0 is down -1 is error read
 * */
int cy30GetData(int fd, wrBuffer *devBuffer);

/**
 * cy30ResultProcess: used to analysis result recieved
 * @param   container       a struct form argument to save distance and address
 * @param   origin          received data
 * @param   len             length of received data
 * @param   action          which action return to origin
 * @origin                  0 is down
 */
int cy30ResultProcess(DistanceContainer *container, wrBuffer devBuffer, Action action);

int cy30GetDistance(int fd, wrBuffer *devBuffer, DistanceContainer *container, Action action);

#endif  // CY30-COM
