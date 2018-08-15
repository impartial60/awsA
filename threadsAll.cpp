#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "externA.h"

void * Process_All_Threads(void *temp_pt)
{    
    //printf("Process_All_Threads...\n");

    pthread_t pthreadCnvEth485;
    pthread_t pthreadMDO1;
    pthread_t pthreadMDO1Echo;
    pthread_t pthread_mainServer;

    void* ret_valCnvEth485;
    void* ret_valMDO1;
    void* ret_valMDO1Echo;
    void* ret_val_mainServer;

	while(1)
    {
        //////////////////   З А П У С К   П О Т О К О В   ///////////////////////////
        if(threadStatus->thrStatus_MDO1Echo == cReate)
        {
            threadStatus->thrStatus_MDO1Echo = inuse;
            pthread_create( &pthreadMDO1Echo, NULL, ProcessThreadMDO1Echo, NULL );
            //qDebug()<<"threadStatus->MDO1Echo == cReate";
        }

        if(threadStatus->thrStatus_MDO1 == cReate)
        {
            threadStatus->thrStatus_MDO1 = inuse;
            pthread_create( &pthreadMDO1, NULL, ProcessThreadMDO1, NULL );
            //qDebug()<<"threadStatus->MDO1 == cReate";
        }

        if(threadStatus->thrStatus_CnvEth485 == cReate)
        {
            restartCntr++;
            threadStatus->thrStatus_CnvEth485 = inuse;
            pthread_create( &pthreadCnvEth485, NULL, ProcessThreadCnvEth485, NULL );
            //qDebug()<<"threadStatus->Cnv485Eth == cReate";
        } 

        if(threadStatus->thrStatus_MainServer == cReate)
        {
            threadStatus->thrStatus_MainServer = inuse;
            pthread_create( &pthread_mainServer, NULL, ProcessThreadMainServer, NULL );
            //qDebug()<<"threadStatus->InfoTable == cReate";
        }

        //////////////////   А В А Р И Й Н О Е   З А В Е Р Ш Е Н И Е   П О Т О К О В   ///////////////////////////

        if(threadStatus->thrStatus_MDO1Echo == cancel)
        {
            pthread_cancel(pthreadMDO1Echo);
            //qDebug()<<"threadStatus->MDO1Echo == cancel";
        }

        if(threadStatus->thrStatus_MDO1 == cancel)
        {
            pthread_cancel(pthreadMDO1);
            //qDebug()<<"threadStatus->MDO1 == cancel";
        }                

        if(threadStatus->thrStatus_CnvEth485 == cancel)
        {
            pthread_cancel(pthreadCnvEth485);
            //qDebug()<<"threadStatus->Cnv485Eth == cancel";
        }

        if(threadStatus->thrStatus_MainServer == cancel)
        {
            pthread_cancel(pthread_mainServer);
        }

        //////////////////   Н О Р М А Л Ь Н О Е   З А В Е Р Ш Е Н И Е   П О Т О К О В   ///////////////////////////

        if(threadStatus->thrStatus_MDO1Echo == cancel)
        {
            pthread_join(pthreadMDO1Echo, &ret_valMDO1Echo);

            //if(threadStatus->thrStatus_MDO1Echo == cancel)
                threadStatus->thrStatus_MDO1Echo = notuse;//cReate;
        }

        if(threadStatus->thrStatus_MDO1 == cancel)
        {
            pthread_join(pthreadMDO1, &ret_valMDO1);

            //if(threadStatus->thrStatus_MDO1 == cancel)
                threadStatus->thrStatus_MDO1 = notuse;//cReate;
        }        

        if(threadStatus->thrStatus_CnvEth485 == cancel)
        {
            pthread_join( pthreadCnvEth485, &ret_valCnvEth485);

            //if(threadStatus->thrStatus_CnvEth485 == cancel)
                threadStatus->thrStatus_CnvEth485 = notuse;//cReate;
        }


        if(threadStatus->thrStatus_MainServer == cancel)
        {
            pthread_join(pthread_mainServer, &ret_val_mainServer);
            threadStatus->thrStatus_MainServer = notuse;//cReate;
        }

//        qDebug()<<"Process_All_Threads: threeds_status->encoderE:"<<arm->threeds_status->encoderE
//               <<"threeds_status->encoderB:"<<arm->threeds_status->encoderB
//              <<"threeds_status->Schnaider:"<<arm->threeds_status->Schnaider
//             <<"threeds_status->ASY:"<<arm->threeds_status->ASY
//            <<"threeds_status->ADSBmode:"<<arm->threeds_status->ADSBmode
//           <<"threeds_status->localARMV:"<<arm->threeds_status->localARMV
//          <<"threeds_status->TRIMBLE:"<<arm->threeds_status->TRIMBLE
//         <<"threeds_status->SignalProc:"<<arm->threeds_status->SignalProc;

        usleep(2000 * 1000);
	}
    /////////////////pthread_testcancel();

    return temp_pt;
}
