/*
* ˵����SPIͨѶʵ��
* ��ʽһ�� ͬʱ���������ʵ�ֺ����� SPI_Transfer()
* ��ʽ������������շֿ���ʵ��
* SPI_Write() ֻ����
* SPI_Read() ֻ����
* ���ַ�ʽ��֮ͬ������ʽһ���ڷ��Ĺ�����Ҳ�ڽ��գ��ڶ��ַ�ʽ�����뷢��������
* Created on: 2013-5-28
* Author: lzy
*/


#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>


//#include "Debug.h"
#define SPI_DEBUG 0


static const char *device = "/dev/spidev1.0";
static uint8_t mode = 0; /* SPIͨ��ʹ��ȫ˫��������CPOL��0��CPHA��0�� */
static uint8_t bits = 8; /* ����i�����д��MSB first��*/
static uint32_t speed = 500000;/* ����12M�����ٶ� */
static uint16_t delay = 0;
static int g_SPI_Fd = 0;

int main(int argc, char** argv)
{
    if (!SPI_Open()) {
        while (1) {
            SPI_LookBackTest();
            usleep( 500000 );
        }
    }
    SPI_Close();
}

static void pabort(const char *s)
{
    perror(s);
    abort();
}


/**
* �� �ܣ�ͬ�����ݴ���
* ��ڲ��� ��
* TxBuf -> ���������׵�ַ
* len -> �������ݵĳ���
* ���ڲ�����
* RxBuf -> �������ݻ�����
* ����ֵ��0 �ɹ�
* ������Ա��Lzy 2013��5��22
*/
int SPI_Transfer(const uint8_t *TxBuf, uint8_t *RxBuf, int len)
{
    int ret;
    int fd = g_SPI_Fd;


    struct spi_ioc_transfer tr ={
        .tx_buf = (unsigned long) TxBuf,
        .rx_buf = (unsigned long) RxBuf,
        .len =len,
        .delay_usecs = delay,
    };


    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
    if (ret < 1)
    perror("can't send spi message");
    else
    {
        #if SPI_DEBUG
        int i;
        printf("nsend spi message Succeed");
        printf("nSPI Send [Len:%d]: ", len);
        for (i = 0; i < len; i++)
        {
            if (i % 8 == 0)
            printf("nt");
            printf("0x%02X ", TxBuf[i]);
        }
        printf("n");


        printf("SPI Receive [len:%d]:", len);
        for (i = 0; i < len; i++)
        {
            if (i % 8 == 0)
            printf("nt");
            printf("0x%02X ", RxBuf[i]);
        }
        printf("n");
        #endif
    }
    return ret;
}


/**
* �� �ܣ���������
* ��ڲ��� ��
* TxBuf -> ���������׵�ַ
��len -> �����볤��
������ֵ��0 �ɹ�
* ������Ա��Lzy 2013��5��22
*/
int SPI_Write(uint8_t *TxBuf, int len)
{
    int ret;
    int fd = g_SPI_Fd;


    ret = write(fd, TxBuf, len);
    if (ret < 0)
    perror("SPI Write errorn");
    else
    {
    #if SPI_DEBUG
    int i;
    printf("nSPI Write [Len:%d]: ", len);
    for (i = 0; i < len; i++)
    {
        if (i % 8 == 0)
        printf("nt");
        printf("0x%02X ", TxBuf[i]);
    }
    printf("n");


    #endif
}


return ret;
}


/**
* �� �ܣ���������
* ���ڲ�����
* RxBuf -> �������ݻ�����
* rtn -> ���յ��ĳ���
* ����ֵ��>=0 �ɹ�
* ������Ա��Lzy 2013��5��22
*/
int SPI_Read(uint8_t *RxBuf, int len)
{
    int ret;
    int fd = g_SPI_Fd;
    ret = read(fd, RxBuf, len);
    if (ret < 0)
    perror("SPI Read errorn");
    else
    {
    #if SPI_DEBUG
    int i;
    printf("SPI Read [len:%d]:", len);
    for (i = 0; i < len; i++)
    {
        if (i % 8 == 0)
        printf("nt");
        printf("0x%02X ", RxBuf[i]);
    }
    printf("n");
    #endif
}


return ret;
}


/**
* �� �ܣ����豸 ����ʼ���豸
* ��ڲ��� ��
* ���ڲ�����
* ����ֵ��0 ��ʾ�Ѵ� 0XF1 ��ʾSPI�Ѵ� ��������
* ������Ա��Lzy 2013��5��22
*/
int SPI_Open(void)
{
    int fd;
    int ret = 0;


    if (g_SPI_Fd != 0) /* �豸�Ѵ� */
    return 0xF1;


    fd = open(device, O_RDWR);
    if (fd < 0)
    pabort("can't open device");
    else
    printf("SPI - Open Succeed. Start Init SPI...n");


    g_SPI_Fd = fd;
    /*
    * spi mode
    */
    ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
    if (ret == -1)
    pabort("can't set spi mode");


    ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
    if (ret == -1)
    pabort("can't get spi mode");


    /*
    * bits per word
    */
    ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
    if (ret == -1)
    pabort("can't set bits per word");


    ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
    if (ret == -1)
    pabort("can't get bits per word");


    /*
    * max speed hz
    */
    ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
    if (ret == -1)
    pabort("can't set max speed hz");


    ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
    if (ret == -1)
    pabort("can't get max speed hz");


    printf("spi mode: %dn", mode);
    printf("bits per word: %dn", bits);
    printf("max speed: %d KHz (%d MHz)n", speed / 1000, speed / 1000 / 1000);


    return ret;
}


/**
* �� �ܣ��ر�SPIģ��
*/
int SPI_Close(void)
{
int fd = g_SPI_Fd;


if (fd == 0) /* SPI�Ƿ��Ѿ���*/
return 0;
close(fd);
g_SPI_Fd = 0;


return 0;
}


/**
* �� �ܣ��Է����ղ��Գ���
* ���յ��������뷢�͵����������һ�� ����ʧ��
* ˵����
* ��Ӳ������Ҫ��������������Ŷ���
* ������Ա��Lzy 2013��5��22
*/
//int SPI_LookBackTest(void)
//{
//    int ret, i;
//    const int BufSize = 16;
//    uint8_t tx[BufSize], rx[BufSize];
//
//
//    bzero(rx, sizeof(rx));
//    for (i = 0; i < BufSize; i++)
//    tx[i] = i;
//
//
//    printf("nSPI - LookBack Mode Test...n");
//    ret = SPI_Transfer(tx, rx, BufSize);
//    if (ret > 1)
//    {
//        ret = memcmp(tx, rx, BufSize);
//        if (ret != 0)
//        {
//            perror("LookBack Mode Test errorn");
//            //pabort("error");
//        }
//    else
//        printf("SPI - LookBack Mode OKn");
//    }
//
//    return ret;
//}

int SPI_LookBackTest(void)
{
    int ret, i;
    int dataLen = 5;
    int BufSize = 128;
    uint8_t tx[128];
    uint8_t rx[128];

    uint8_t tmp[2] = { 0x00, dataLen };
    memcpy( tx, tmp, 2 );

    uint8_t direction;
    int8_t speed; // pulse
    uint8_t radius; //cm
    
    scanf("%d %d %d",&direction,&speed,&radius);
    //fgets(str,7,stdin);
    //str[0] -= 0x30;
    //memcpy( tx+2, str, 5 );
    tx[2] = direction;
    tx[3] = (uint8_t)speed;
    tx[4] = radius;

    bzero(rx, sizeof(rx));
//  for (i = 0; i < BufSize; i++)
//  tx[i] = i;


    //printf("nSPI - LookBack Mode Test...\n");
    ret = SPI_Transfer(tx, rx, BufSize);

    uint8_t j;
    for ( j=0; j<10; j++ ) {
        printf("[%d]:%c\n",j,rx[j]);
    }
    printf("\n");

    



//  if (ret > 1)
//  {
//      ret = memcmp(tx, rx, BufSize);
//      if (ret != 0)
//      {
//          perror("LookBack Mode Test errorn");
//          //pabort("error");
//      }
//  else
//      printf("SPI - LookBack Mode OKn");
//  }

    return ret;
} 