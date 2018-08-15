#include <QDebug>
#include <QCoreApplication>
#include <QTime>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#include <dirent.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/ipc.h>

//#include  "srcKram/tp.h"
#include <sharedMem.h>
#include <sys.h>

#include "srcKram/Drive125.h"

#include "threadstatus.h"
#include "settingsA.h"

//all definition for mechanical
#define max_vel_const  5000.0  /* в делениях выходного энкодера в секунду ( 44(ед/оборот) * 100(об/сек) = 4400 --- 6000 об/мин )*/
#define max_accel_const 88000.0

pthread_t pthreadAll;
void* ret_val_All_Threads;
void * Process_All_Threads( void * temp_pt );

MDO1_nextDirection *mdo1AllDirections;

pthread_mutex_t localConsole_memory_Mutex;
pthread_mutex_t nextTockaMutex;

ThreadStatus *threadStatus;
int restartCntr = 0;

//tp_t p1,p2;

KOMANDI_KOMANDIRA komKomA;
KOMANDI_OFICERA_NAVED oficerNaved2Console;

ServerUNV2MainServer unv2apy;
MainServer2ServerUNV apy2unv;

SNR2SYS snr2sys;
SYS2SNR sys2snr;
VKP2ZRK vkp2zrkG;
SERVO_DRIVE_INFO srvDrvInfoA;
APY_INFO apyInfoA;

QTime programmStartTime;
SettingsConsoleA *settings;

extern  Drive125 *drive_unit;

extern pid_t proc_find(const char* name, long secondPid);

int main_console(int argc, char *argv[])//main(int argc, char *argv[])
{
    (void)argc; (void)argv;

//    float koef = 0.6; for(int i = 0; i < 1000; i++)    {        float vET = 0.1 * i;        float skOst = 0;
//        if(vET < koef * TMAXPEREHOD_NAKAL_GOTOVA)             skOst = TMAXPEREHOD_NAKAL_GOTOVA - vET;
//        else            skOst = (1.0 - koef) * TMAXPEREHOD_NAKAL_GOTOVA + 1.0 - sqrt(vET - koef * TMAXPEREHOD_NAKAL_GOTOVA + 1.0);
//        printf("vET: %f  skOst: %f   \n", vET, skOst);    }

    //   П Р О В Е Р К А   Н А   Е Д И Н С Т В Е Н Н О С Т Ь   З А П У Щ Е Н Н О Г О   Э К З Е М П Л Я Р А   П Р О Г Р А М М Ы
    pid_t own_pid = getpid();           //  это пид самой себя
    printf("awsA125Server own pid: % d\n", own_pid);

    pid_t pid = proc_find("awsA125Server", own_pid);
    if(pid >= 0)    // найден старый работающий экземпляр программы
    {
        printf("!!!OLD awsA125Server DETECTED!!! pid: %d - it will be killed\n", pid);
        kill(pid, SIGTERM);
        sleep(1);
    }
    else
        printf("OLD awsA125Server NOT FOUND - it`s Ok...\n");

    settings = new SettingsConsoleA();

    programmStartTime.start();

    //  2018-06-16  srvDrvInfoA.totalSimTargetNum = 1;

    komKomA.targetIndexCY = -1;
    //srvDrvInfoA.p18GeoPosition.geoY = 99;  //  99 - признак того, что информация о точке стояния отсутствует
    //srvDrvInfoA.trackPointsNumber = 0;

    apyInfoA.rejimPoiskaSNR = POISK_VIKLUCHEN;
    apyInfoA.rejimSoprovojdeniaSNR = SOPROVOJDENIE_AS;

    mdo1AllDirections = (MDO1_nextDirection *)calloc(MAX_DIRECT_COUNT, sizeof(MDO1_nextDirection));
    //slideCorResults = (float*)calloc(MAX_DETECT_COUNT, sizeof(float));
    //qDebug()<<"sizeof(slideSignalValues):"<<sizeof(MDO1_nextDirection) * MAX_DIRECT_COUNT;

    //qDebug()<<"sizeof(MDO1_Plot) * MAX_MDO1_TARGETS_COUNT:"<<sizeof(MDO1_Plot) * MAX_MDO1_TARGETS_COUNT<<sizeof(MDO1_Plot)<<MAX_MDO1_TARGETS_COUNT;

    threadStatus = new ThreadStatus();

    pthread_mutex_init(&localConsole_memory_Mutex, NULL);
    pthread_mutex_init(&nextTockaMutex, NULL);    

//    srvDrvInfoA.directrisaAzimuth = drive_unit->unv->device_getpos_az();
//    qDebug()<<"main_console srvDrvInfoA.directrisaAzimuth:"<<srvDrvInfoA.directrisaAzimuth;

//    drive_unit->unv->device_setpos_az(30.0);
//    for(int i = 0; i < 5; i++)
//    {
//        sleep(1);
//        srvDrvInfoA.directrisaAzimuth = drive_unit->unv->device_getpos_az();
//        qDebug()<<"main_console "<<i<<"srvDrvInfoA.directrisaAzimuth:"<<srvDrvInfoA.directrisaAzimuth;
//        drive_unit->unv->device_setpos_az(30.0);
//    }


    //srvDrvInfoA.directrisaAzimuth = drive_unit.// drive //srvDrvInfoA.directrisaAzimuth = 133.0;
    srvDrvInfoA.currentAzimuth = srvDrvInfoA.directrisaAzimuth;
    srvDrvInfoA.currentDirectrisaElevationAngle = 6.54;//5.34;

    //p1.active=0; p1.curr_pos=srvDrvInfoA.currentAzimuth;                 p1.curr_vel=0;p1.enable=1;p1.max_acc=max_accel_const;p1.max_vel=max_vel_const;p1.pos_cmd=srvDrvInfoA.currentAzimuth;
    //p2.active=0; p2.curr_pos=srvDrvInfoA.currentDirectrisaElevationAngle;p2.curr_vel=0;p2.enable=1;p2.max_acc=max_accel_const;p2.max_vel=max_vel_const;p2.pos_cmd=srvDrvInfoA.currentDirectrisaElevationAngle;

    pthread_create( &pthreadAll, NULL, Process_All_Threads, NULL );
    qDebug()<<"pthreadAll: pthread_create";

    pthread_join( pthreadAll, &ret_val_All_Threads);
    qDebug()<<"pthreadAll: pthread_join";

    return 123;//a.exec();
}
