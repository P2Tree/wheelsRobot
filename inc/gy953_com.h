#ifndef GY953_COM_H_0EYLRUGX
#define GY953_COM_H_0EYLRUGX

/**
 * *    PARAMETER
 */
#define MAXLEN      11
#define WRITELEN    3

/**
 * *    REGISTERS
*/
#define BAUDRATE_115200         0XA5AF54
#define BAUDRATE_9600           0XA5AE53

#define KEY_ACCELEROMETER       0XA551F6
#define KEY_GYROSCOPE           0XA552F7
#define KEY_MAGNETOMETER        0XA553F8
#define ACCGYR_CALIBRATION      0XA557FC
#define MAG_CALIBRATION         0XA558FD
#define RESTOREFACTORY          0XA559FE
#define DATASPEED_50HZ           0XA5A449
#define DATASPEED_100HZ          0XA5A54A
#define DATASPEED_200HZ          0XA5A64B

#define AUTODATA_ACCELEROMETER  0XA515BA
#define AUTODATA_GYROSCOPE      0XA525CA
#define AUTODATA_MAGNETOMETER   0XA535DA
#define AUTODATA_EULERANGLE              0XA545EA
#define AUTODATA_EULERANGLE_CHAR         0XA555FA
#define AUTODATA_QUATERNION              0XA5650A

#define PRECISION_FREQUENCY     0XA5751A
#define RANGE                   0XA5852A
#define EULERANGLE              0XA5953A
#define QUATERNION              0XA5B55A
#define ACCELEROMETER           0XA5C56A
#define GYROSCOPE               0XA5D57A
#define MAGNETOMETER            0XA5E58A

/**
 * *    FUNCTIONS
 * */

extern int gy953Init(unsigned char *command);

/**
 * @func: gy953Close     use to close uart
 * @retval:             0 is down
 * */
extern int gy953Close(int fd);


/**
 * @func: gy953ReceiveData   use to receive date from device
 * @param: fd           file descriptor
 * @param: command      string used to catch data
 * @param: maxlen       allowed maximum length of receive data
 * @retval:             if catch data, return length of data, else return -1
 * */
extern int gy953ReceiveData(int fd, unsigned char *command, unsigned int maxlen);

/**
 * @func: gy953SendCommand   use to send data to device
 * @param: fd           file descriptor
 * @param: command    data
 * @param: len        length of data
 * @retval:         0 is down
 * */
extern int gy953SendCommand(int fd, unsigned char *command, unsigned int len);

/**
 * @func:   analysisEulerangle      analysis data of euler angle
 * @param:  originData              received data
 * @param:  len                     length of received data
 * @param:  data1                   return angle x axis
 * @param:  data2                   return angle y axis
 * @param:  data3                   return angle z axis
 * @retval:                         0 is down, -1 is error
 * */
extern int analysisEulerangle(unsigned char *originData, int len, float *data1, float *data2, float *data3);

/**
 * @func:   analysisAccelerometer   analysis data of accelerometer
 * @param:  originData              received data
 * @param:  len                     length of received data
 * @param:  data1                   return data group 1
 * @param:  data2                   return data group 2
 * @param:  data3                   return data group 3
 * @retval:                         0 is down and -1 is wrong
 **/
extern int analysisAccelerometer(unsigned char *originData, int len, int *data1, int *data2, int *data3);


#endif /* end of include guard: GY953_COM_H_0EYLRUGXGY953_COM_H_0EYLRUGX */
