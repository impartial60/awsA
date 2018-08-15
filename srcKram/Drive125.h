#ifndef Drive125_H
#define Drive125_H

#define Kramarenko

#include <math.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include "clockrealtime.h"
#include "device125.h"
#include "Drive125.h"

#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <sys/times.h>
#include <signal.h>
#include <QCoreApplication>



#define main_clock_works 20
#define time_outs 10

#define N_ID_enc_new        0x3F // (63) 	При наладке адрес энкодера по умолчанию
#define N_ID_incl           0x3E // (62) 	Адрес инклинометра (при запуске нового и в процессе работы одинаковый)
#define N_ID_enc_azimuth    0x3D // (61)	Энкодер азимута.
#define N_ID_enc_elv        0x3C // (60) 	Энкодер угла места
#define N_ID_lenze_az       0x3B // (59) 	Привод азимута.
#define N_ID_lenze_elv      0x3A // (58)	Привод угла места.
#define N_ID_PLC            0x39 // (57)	PLC Schneider.

class Drive125:public QObject//,*/QThread

{
    Q_OBJECT
 public:

    explicit Drive125(QObject *parent = nullptr);
    ~Drive125();

    Device125 *unv,*p1,*p2,*p3,*p4;


public slots:
void slot_receive_packet(void);
void slot_timer_task(void);
void slot_timeout_unv(void);
void slot_timeout_p1(void);
void slot_timeout_p2(void);
void slot_timeout_p3(void);
void slot_timeout_p4(void);


private:


int count=0;
QUdpSocket *exch;
QTimer *mono1,*mono2,*mono3,*mono4,*mono5,*main_timer;


};// Drive125;


#endif // Drive125_H
