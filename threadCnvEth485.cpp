#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#include "externA.h"

//int executionDelDetectCounter = 0;
//float oldCurAz = -1000;

//int oldAzCounter = 0;
//float oldAbsDltAz = 1000;
//int delayAzExecCounter = 0;

int plcNeOtvechaet = 0;

//extern void clientConnect(int *sock, int port, const char *ipaddr, const char *NameOfThread);

int cntr11 = 0;

void * ProcessThreadCnvEth485(void * temp_pt )
{ 
    (void)temp_pt;
    int tcpSocketFileDescriptor = -1;

    unsigned char readBytes[sizeof(USM_Info)];

    clientConnect(&tcpSocketFileDescriptor, 9760, "10.5.0.31", "ProcessThreadCnvEth485 initial connect");
    //fcntl(tcpSocketFileDescriptor, F_SETFL, O_NONBLOCK);

    int vsego_propyschenih_periodov = 0;

    while(1)
    {        
        usleep(350 * 1000);        

        //memset(readBytes, 0x2C, INPUT_TOTAL_LENGTH);

        //bool connectionStatus = true;
        //int n = -1;

        //ssize_t
                int totalReadBytes = 0;

        ssize_t totalSentBytes = send(tcpSocketFileDescriptor, &oficerNaved2Console.usmCmds, sizeof(USM_Commands), 0);

//        QString b78 = "";
//        for(int i = 0; i < 11; i++)
//            b78 += " " + QString::number(i) + ":0x" + QString("%1").arg(shMemU2C.usmCmds.usm02[i], 2, 16, QChar('0')).toUpper();
//        qDebug()<<"ProcessCnvEth485:   totalSentBytes:"<<totalSentBytes<<b78;

//        QString b78 = "";
//        for(int i = 0; i < 10; i++)
//            b78 += " " + QString::number(i) + ":0x" + QString("%1").arg(shMemU2C.usmCmds.usm08_sid[i], 2, 16, QChar('0')).toUpper();
//        qDebug()<<"ProcessCnvEth485:   totalSentBytes:"<<totalSentBytes<<b78;

        //if(totalSentBytes > 0)
          //qDebug()<<"ProcessCnvEth485:   totalSentBytes:"<<totalSentBytes<<"cntr11:"<<cntr11++;

        if(totalSentBytes < 0)
        {
            clientConnect(&tcpSocketFileDescriptor, 9760, "10.5.0.31", "CnvEth485 Thread    reconnect");
            usleep(500 * 1000);
        }
//        else
        //{            
            //usleep(1 * 1000);

            while (totalReadBytes < sizeof(USM_Info) && vsego_propyschenih_periodov <= 9115)
            {
                ssize_t nextReadBytes = recv(tcpSocketFileDescriptor, readBytes + totalReadBytes, sizeof(USM_Info) - totalReadBytes, 0);

                if(nextReadBytes < 0)
                {
                     //qDebug()<<"CnvEth485 Thread    reconnect";
                     clientConnect(&tcpSocketFileDescriptor, 9760, "10.5.0.31", "CnvEth485 Thread    reconnect");
                     break;
                }

                totalReadBytes += nextReadBytes;

                if(nextReadBytes <= 0)
                {
                    vsego_propyschenih_periodov++;

                    //qDebug()<<"CnvEth485: vsego_propyschenih_periodov:"<<vsego_propyschenih_periodov;
                }
                else
                {
                    vsego_propyschenih_periodov = 0;
                }

                //rsltStr += QString("crclN: %1  nextReadBytes: %2  totalReadBytes: %3  ").arg(crclN++).arg(nextReadBytes).arg(totalReadBytes);

//                if(nextReadBytes < INPUT_TOTAL_LENGTH)
//                    qDebug()<<"ProcessCnvEth485:   nextReadBytes:"<<nextReadBytes;
            }

//            rsltStr += QString("readBytes:  0x%1 0x%2 0x%3").arg(readBytes[22], 2, 16, QChar('0')).toUpper() //  Первый байт маркера заголовка   0x55
//                                                            .arg(readBytes[23], 2, 16, QChar('0')).toUpper() //  Второй байт маркера заголовка   0xAA
//                                                            .arg(readBytes[24], 2, 16, QChar('0')).toUpper();//  Адрес отвечающего устройства    0x18

//            rsltStr += QString("  index22: %4").arg(index22);
//            qDebug()<<rsltStr;

//            qDebug()<<"ProcessCnvEth485: tcpSocketFileDescriptor:"<<tcpSocketFileDescriptor<<"totalSentBytes:"<<totalSentBytes<<"totalReadBytes:"<<totalReadBytes;

            //srvDrvInfoA.totalReadBytes = totalReadBytes;
            //qDebug()<<"ProcessCnvEth485: srvDrvInfoA.totalReadBytes:"<<srvDrvInfoA.totalReadBytes;

            //memset(readBytes, 0, INPUT_TOTAL_LENGTH);

            pthread_mutex_lock(&localConsole_memory_Mutex);

                if(totalReadBytes == sizeof(USM_Info))
                {
                    //memcpy(localConsole_memory  + SERVICE_TOTAL_LENGTH + OUTPUT_TOTAL_LENGTH, readBytes, INPUT_TOTAL_LENGTH);

                    int index22 = -1;

                    for(int i = 0; i < sizeof(USM_Info) - 2; i++)
                    {
                        if(readBytes[i] == 0x55 && readBytes[i + 1] == 0xAA && readBytes[i + 2] == 0x18)
                        {
                            index22 = i;
                            break;
                        }
                    }

                    int delta = index22 - 22;

                    if(index22 >= 0)
                    {
                        memcpy(&unv2apy.usmInfo, readBytes + sizeof(USM_Info) - delta, delta);
                        memcpy(&unv2apy.usmInfo, readBytes + delta, sizeof(USM_Info) - delta);
                    }
                    else
                        memset(&unv2apy.usmInfo, 0, sizeof(USM_Info));
                        //memcpy(localConsole_memory  + SERVICE_TOTAL_LENGTH + OUTPUT_TOTAL_LENGTH, readBytes, INPUT_TOTAL_LENGTH);                    


                    /////////////////////////////////////////////////////*(localConsole_memory  + SERVICE_TOTAL_LENGTH + OUTPUT_TOTAL_LENGTH + 399) = (cntr11++) % 255;  //  для теста
//                    int addr = SERVICE_TOTAL_LENGTH + OUTPUT_TOTAL_LENGTH + 291 + 6 + 258;

//                    qDebug()<<"ProcessCnvEth485: "<<*(localConsole_memory + addr)<<*(localConsole_memory + addr + 1)<<"\t"
//                            <<*(localConsole_memory + addr + 268)<<*(localConsole_memory + addr + 268 + 1)<<"\t"
//                            <<*(localConsole_memory + addr + 268 * 2)<<*(localConsole_memory + addr + 268 * 2 + 1)<<"\t"
//                            <<*(localConsole_memory + addr + 268 * 3)<<*(localConsole_memory + addr + 268 * 3 + 1);
                }

                /////////////////memcpy(localConsole_memory + 6,  &totalReadBytes, sizeof(int));
            pthread_mutex_unlock(&localConsole_memory_Mutex);

        //qDebug()<<"ProcessCnvEth485: tcpSocketFileDescriptor:"<<tcpSocketFileDescriptor<<"n:"<<n;

        if(vsego_propyschenih_periodov++ > 9115)
            break;
            //threadStatus->CnvEth485 == cancel;
    };

    close(tcpSocketFileDescriptor);

    qDebug()<<"ProcessCnvEth485 end close(tcpSocketFileDescriptor);   tcpSocketFileDescriptor:"<<tcpSocketFileDescriptor;
    threadStatus->thrStatus_CnvEth485 = cancel;

//    if(connectionStatus == false)
//        threadStatus->CnvEth485 = cancel;

    return temp_pt;
}
