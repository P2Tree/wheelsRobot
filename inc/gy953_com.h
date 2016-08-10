#ifndef GY953_COM_H_0EYLRUGX
#define GY953_COM_H_0EYLRUGX

#define DEBUG_GY953

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


/**
 * @func:   gy953Init   initialization of gy953 module
 * @param:  port        uart port name
 * @retval:             0 is down
 * */
int gy953Init(const char *port);

/**
 * @func: gy953Close     use to close uart
 * @retval:             0 is down
 * */
int gy953Close(int fd);

/**
 * @func:   GY953ConstructCommand   construct a command follow the param hexCommand
 * @param:  hexCommand              control the command want to be constructed
 * @param:  command                 result command
 * @retval:                         0 is down
 * */
int gy953ConstructCommand(int hexCommand, unsigned char *command);

/**
 * @func:   get3AxisEulerAngle      can get euler angles of 3 axis
 * @param:  fd                      file descriptor
 * @param:  command                 command want to send to gy953
 * @param:  result                  get result
 * */
void get3AxisEulerAngle(int fd, unsigned char *command, float *result);

#endif /* end of include guard: GY953_COM_H_0EYLRUGXGY953_COM_H_0EYLRUGX */
