#include <QDateTime>
#include <QDir>
#include <math.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "externA.h"

//voronin-aa@zrk-voronin-aa:~$ sudo su
//[sudo] password for voronin-aa:
//root@zrk-voronin-aa:/home/voronin-aa# sh /etc/rc.local

uint8_t oldElevationScanId = 0;

unsigned char mdoArray[sizeof(MDO1_echoData)];
unsigned char mdoTrgtData[sizeof(MDO1_incomingData)];

extern int detectCntr;
extern MDO1_TrgtPlot_Coord grayTrgtPlotCoord[2];

void simulatorMDO1(int mdo1Index, uint8_t elevationScanId, float prolAzimuth)
{
    memset(&unv2apy.mdo1Info[mdo1Index], 0, sizeof(MDO1_incomingData));

    unv2apy.mdo1Info[mdo1Index].header.x55 = 0x55;
    unv2apy.mdo1Info[mdo1Index].header.xAA = 0xAA;
    unv2apy.mdo1Info[mdo1Index].header.deviceAddress = 0x21 + mdo1Index;

    bool trgtNoIsEngaged[16];

    for(int i = 0; i < 16; i++)
    {
        unv2apy.mdo1Info[mdo1Index].rawTrgtData[i].Nceli = i;
        trgtNoIsEngaged[i] = false;
    }

    unv2apy.mdo1Info[mdo1Index].rawTrgtData[0].Nceli = elevationScanId;

    int trgtListSize = rand() % 17;

    //qDebug()<<"simulatorMDO1 trgtListSize:"<<trgtListSize;

    if(trgtListSize == 0)
    {
//        detectCntr++;
//        if(detectCntr >= MAX_DIRECT_COUNT)
//            detectCntr = 0;
        return;
    }

    //return;

    float maxDelta = 2.0;// Rad2Grad 2.0

    // 1-я истинная цель
    int trgtInd = rand() % trgtListSize;//16;       int trgtInd = 0;    //  2018_07_12
    //if(trgtInd >= 16)   qDebug()<<"simulatorMDO1 1 trgtListSize:"<<trgtListSize;
    trgtNoIsEngaged[trgtInd] = true;

    float deltaAz = fabs(convert360angle2PlusMinus180(apyInfoA.nextTocka[0].azimuth360 - prolAzimuth));//srvDrvInfoA.currentAzimuth));    // 131.0

    uint16_t maxEnergy = 65535;
    uint16_t energy;

    //if(deltaAz < maxDelta)
    {
        float arg = deltaAz * M_PI / maxDelta;

        if(deltaAz == 0.0)
            energy = maxEnergy;
        else
            energy = (uint16_t)(maxEnergy * fabs(sin(arg) / arg));

        //qDebug()<<"simulatorMDO1 detectCntr:"<<detectCntr<<"deltaAz:"<<deltaAz<<"energy:"<<energy;
    }

    if(apyInfoA.nextTocka[0].elevationAngle < srvDrvInfoA.currentDirectrisaElevationAngle - 5.0 || apyInfoA.nextTocka[0].elevationAngle > srvDrvInfoA.currentDirectrisaElevationAngle + 5.0)
        energy = 0.0;   //  цель за пределами сектора по углу места
/*  screenShot для Фищука

    typedef struct
    {
        u_int8_t x55;
        u_int8_t xAA;
        u_int8_t deviceAddress;
    } USM_MDO_header;

    typedef struct
    {
        u_int8_t Nceli;
        u_int16_t range;
        u_int8_t scanerYV10ElevAngle;
        u_int16_t energy[3];
        u_int8_t ARY;
    } MDO1_targetData;

    typedef struct
    {
        USM_MDO_header header;
        USM_MDO_timeFrame timeFrame;
        MDO1_targetData targetData[16];
        u_int8_t CRC16[2];
    } MDO1_incomingData;

    MDO1_incomingData mdo1Info[3];  // каждые 40мс по результатам анализа угломестного скана ты присылаешь мне эту структуру,
    // разделённую на 3 порции, от каждого из 3 блоков МДО-1 по одной порции. Как я узнаю, что эти порции относятся к одному и тому же угломестному скану?
    // Могу ли я рассчитывать на то, что у них совпадают номера рабочих сессий чтобы использовать этот признак для отождествления?
    // Правильно ли я понимаю, что номера и очередность целей во всех 3 порциях не совпадает и я должен для отождествления целей анализировать их дальности?

    mdo1Info[0].header.x55, mdo1Info[1].header.x55, mdo1Info[2].header.x55 = 0x55;  //  это просто что-то типа маркера
    mdo1Info[0].header.xAA, mdo1Info[1].header.xAA, mdo1Info[2].header.xAA = 0xAA;  //  это просто что-то типа маркера

    // информация по 1-й цели
    mdo1Info[0].header.deviceAddress = 0x21;    //  адрес устройства 1-го блока МДО-1
    mdo1Info[0].targetData[0].energy[0];        //  1-я цель,   энергия в допплеровском канале  0 кГц
    mdo1Info[0].targetData[0].energy[1];        //  1-я цель,   энергия в допплеровском канале 15 кГц
    mdo1Info[0].targetData[0].energy[2];        //  1-я цель,   энергия в допплеровском канале 30 кГц

    mdo1Info[1].header.deviceAddress = 0x22;    //  адрес устройства 2-го блока МДО-1
    mdo1Info[1].targetData[0].energy[0];        //  1-я цель,   энергия в допплеровском канале 45 кГц
    mdo1Info[1].targetData[0].energy[1];        //  1-я цель,   энергия в допплеровском канале 60 кГц
    mdo1Info[1].targetData[0].energy[2];        //  1-я цель,   энергия в допплеровском канале 75 кГц

    mdo1Info[2].header.deviceAddress = 0x23;    //  адрес устройства 3-го блока МДО-1
    mdo1Info[2].targetData[0].energy[0];        //  1-я цель,   энергия в допплеровском канале -15 кГц
    mdo1Info[2].targetData[0].energy[1];        //  1-я цель,   энергия в допплеровском канале -30 кГц
    mdo1Info[2].targetData[0].energy[2];        //  1-я цель,   энергия в допплеровском канале -45 кГц

    // информация по 2-й цели
    mdo1Info[0].header.deviceAddress = 0x21;    //  адрес устройства 1-го блока МДО-1
    mdo1Info[0].targetData[1].energy[0];        //  2-я цель,   энергия в допплеровском канале  0 кГц
    mdo1Info[0].targetData[1].energy[1];        //  2-я цель,   энергия в допплеровском канале 15 кГц
    mdo1Info[0].targetData[1].energy[2];        //  2-я цель,   энергия в допплеровском канале 30 кГц

    mdo1Info[1].header.deviceAddress = 0x22;    //  адрес устройства 2-го блока МДО-1
    mdo1Info[1].targetData[1].energy[0];        //  2-я цель,   энергия в допплеровском канале 45 кГц
    mdo1Info[1].targetData[1].energy[1];        //  2-я цель,   энергия в допплеровском канале 60 кГц
    mdo1Info[1].targetData[1].energy[2];        //  2-я цель,   энергия в допплеровском канале 75 кГц

    mdo1Info[2].header.deviceAddress = 0x23;    //  адрес устройства 3-го блока МДО-1
    mdo1Info[2].targetData[1].energy[0];        //  2-я цель,   энергия в допплеровском канале -15 кГц
    mdo1Info[2].targetData[1].energy[1];        //  2-я цель,   энергия в допплеровском канале -30 кГц
    mdo1Info[2].targetData[1].energy[2];        //  2-я цель,   энергия в допплеровском канале -45 кГц

*/

    unv2apy.mdo1Info[mdo1Index].rawTrgtData[trgtInd].energy[0] = 0.1 * energy;
    unv2apy.mdo1Info[mdo1Index].rawTrgtData[trgtInd].energy[1] = 0.9 * energy;
    unv2apy.mdo1Info[mdo1Index].rawTrgtData[trgtInd].energy[2] = 0;

    unv2apy.mdo1Info[mdo1Index].rawTrgtData[trgtInd].scanerYV10ElevAngle = 127 + ((apyInfoA.nextTocka[0].elevationAngle - srvDrvInfoA.currentDirectrisaElevationAngle) / 5.0) * 128;    
    unv2apy.mdo1Info[mdo1Index].rawTrgtData[trgtInd].range = (uint16_t)(apyInfoA.nextTocka[0].slantRange_km / CENA_MLADSHEGO_RAZRIADA_DALNOSTI);

    //if(mdo1Index == 2)
        //qDebug()<<"simulatorMDO1: mdo1Index:"<<mdo1Index<<"detectCntr:"<<detectCntr<<"trgtInd:"<<trgtInd<<"deltaAz:"<<deltaAz<<"energy:"<<energy<<"prolAzimuth:"<<prolAzimuth;
        //qDebug()<<"simulatorMDO1: unv2apy.mdo1Info[mdo1Index].rawTrgtData[trgtInd].range:"<<unv2apy.mdo1Info[mdo1Index].rawTrgtData[trgtInd].range;

    if(deltaAz < grayTrgtPlotCoord[0].dAz)
    {
        grayTrgtPlotCoord[0].dAz = deltaAz;
        grayTrgtPlotCoord[0].enrg = maxEnergy;
        grayTrgtPlotCoord[0].az360 = apyInfoA.nextTocka[0].azimuth360;
        //grayTrgtPlotCoord[0].elevAngleF = unv2apy.mdo1Info[mdo1Index].rawTrgtData[trgtInd].scanerYV10ElevAngle;
        grayTrgtPlotCoord[0].elevAngleF = apyInfoA.nextTocka[0].elevationAngle;

        //grayTrgtPlotCoord[0].range = unv2apy.mdo1Info[mdo1Index].rawTrgtData[trgtInd].range * CENA_MLADSHEGO_RAZRIADA_DALNOSTI;
        //grayTrgtPlotCoord[0].range = apyInfoA.nextTocka[0].slantRange_km;
        grayTrgtPlotCoord[0].range_km = CENA_MLADSHEGO_RAZRIADA_DALNOSTI * unv2apy.mdo1Info[mdo1Index].rawTrgtData[trgtInd].range;

//        if(mdo1Index == 2)
//            qDebug()<<"simulatorMDO1: unv2apy.mdo1Info[mdo1Index].rawTrgtData[trgtInd].range:"<<unv2apy.mdo1Info[mdo1Index].rawTrgtData[trgtInd].range
//                    <<"grayTrgtPlotCoord[0].range:"<<grayTrgtPlotCoord[0].range;
    }

    if(trgtListSize == 1)       //  2018_07_12
        return;                 //  2018_07_12

    //return;

    // 2-я истинная цель

    //int metka = -1;

    if(trgtListSize > 2)
    {
        //metka = 10;
        trgtInd = rand() % trgtListSize;
        if(trgtNoIsEngaged[trgtInd])                // trgtInd может совпасть с trgtInd для 1-й истинной цели
        {
            //metka = 11;
            trgtInd = rand() % trgtListSize;        // дадим ещё один шанс, если trgtInd опять совпадёт с 1-й целью - пусть это симитирует пропуск фрагмента пачки по 1-й цели на этом скане
        }
    }
    else                            // целей ровно 2
    {
        //metka = 20;
        if(trgtNoIsEngaged[0])      // trgtInd может совпасть с trgtInd для 1-й истинной цели
            trgtInd = 1;
        else
            trgtInd = 0;
    }

    //  2018_07_12  trgtInd = 1;

    if(trgtNoIsEngaged[trgtInd] == false)
    {
        //if(trgtInd >= 16)   qDebug()<<"simulatorMDO1 2 trgtListSize:"<<trgtListSize;
        trgtNoIsEngaged[trgtInd] = true;

        deltaAz = fabs(convert360angle2PlusMinus180(apyInfoA.nextTocka[1].azimuth360 - prolAzimuth));//srvDrvInfoA.currentAzimuth));    // 135.0

        maxEnergy = 50000;

        //if(deltaAz < maxDelta)
        {
            if(deltaAz == 0.0)
                energy = maxEnergy;
            else
                energy = (uint16_t)(maxEnergy * fabs(sin(deltaAz * M_PI / maxDelta) / deltaAz / M_PI * maxDelta));
        }

        if(apyInfoA.nextTocka[1].elevationAngle < srvDrvInfoA.currentDirectrisaElevationAngle - 5.0 || apyInfoA.nextTocka[1].elevationAngle > srvDrvInfoA.currentDirectrisaElevationAngle + 5.0)
            energy = 0.0;   //  цель за пределами сектора по углу места

        unv2apy.mdo1Info[mdo1Index].rawTrgtData[trgtInd].energy[0] = 0.1 * energy;
        unv2apy.mdo1Info[mdo1Index].rawTrgtData[trgtInd].energy[1] = 0.9 * energy;
        unv2apy.mdo1Info[mdo1Index].rawTrgtData[trgtInd].energy[2] = 0;

        unv2apy.mdo1Info[mdo1Index].rawTrgtData[trgtInd].scanerYV10ElevAngle = 127 + ((apyInfoA.nextTocka[1].elevationAngle - srvDrvInfoA.currentDirectrisaElevationAngle) / 5.0) * 128;
        unv2apy.mdo1Info[mdo1Index].rawTrgtData[trgtInd].range = (uint16_t)(apyInfoA.nextTocka[1].slantRange_km / CENA_MLADSHEGO_RAZRIADA_DALNOSTI);

        if(deltaAz < grayTrgtPlotCoord[1].dAz)
        {
            grayTrgtPlotCoord[1].dAz = deltaAz;
            grayTrgtPlotCoord[1].enrg = maxEnergy;
            grayTrgtPlotCoord[1].az360 = apyInfoA.nextTocka[1].azimuth360;
            //grayTrgtPlotCoord[1].elevAngleF = unv2apy.mdo1Info[mdo1Index].rawTrgtData[trgtInd].scanerYV10ElevAngle;
            grayTrgtPlotCoord[1].elevAngleF = apyInfoA.nextTocka[1].elevationAngle;
            //grayTrgtPlotCoord[1].range = unv2apy.mdo1Info[mdo1Index].rawTrgtData[trgtInd].range * CENA_MLADSHEGO_RAZRIADA_DALNOSTI;
            grayTrgtPlotCoord[1].range_km = CENA_MLADSHEGO_RAZRIADA_DALNOSTI * unv2apy.mdo1Info[mdo1Index].rawTrgtData[trgtInd].range;
        }
    }

    if(trgtListSize == 2)
    {
        //  if(mdo1Index == 0) qDebug()<<"simulatorMDO1: trgtListSize == 2 metka:"<<metka<<"energy:"<<unv2apy.mdo1Info[mdo1Index].targetData[0].energy[1]<<unv2apy.mdo1Info[mdo1Index].targetData[1].energy[1];
        return;
    }

    //return; //  2018_07_12

    float amplPomehi = 0.2;

    for(int i = 0; i < trgtListSize; i++)
    {
        if(trgtNoIsEngaged[i])
            continue;

        unv2apy.mdo1Info[mdo1Index].rawTrgtData[i].energy[0] = amplPomehi * (rand() % 65535);//energPomehi + 3;
        unv2apy.mdo1Info[mdo1Index].rawTrgtData[i].energy[1] = amplPomehi * (rand() % 65535);//energPomehi + 3;
        unv2apy.mdo1Info[mdo1Index].rawTrgtData[i].energy[2] = amplPomehi * (rand() % 65535);//energPomehi + 3;
        unv2apy.mdo1Info[mdo1Index].rawTrgtData[i].scanerYV10ElevAngle = rand() % 256;
        unv2apy.mdo1Info[mdo1Index].rawTrgtData[i].range = rand();
    }

//    if(mdo1Index == 0)
//        qDebug()<<"simulatorMDO1: trgtListSize:"<<trgtListSize<<"metka:"<<metka
//                <<"trgtNoIsEngaged[i]:"<<trgtNoIsEngaged[0]<<"targetData[0].energy[1]:"<<unv2apy.mdo1Info[mdo1Index].targetData[0].energy[1];

}

uint32_t simulationElevationId32 = 0;

void dopplerChannelIdentification(int detectIndex)//, int mdo1Ind)
{
//    qDebug()<<"dopplerChannelEnergyIdentification: detectCntr - 1:"<<detectCntr - 1<<mdo1AllDirections[detectCntr - 1].joinedTrgtData[47].energy[0]
//            <<mdo1AllDirections[detectCntr - 1].joinedTrgtData[47].energy[1]          <<mdo1AllDirections[detectCntr - 1].joinedTrgtData[47].energy[2];

    //return;

    //for(int j = 0; j < detectCntr; j++)     //  по всем угломестным сканам
    {
        // 2018_07_18   qDebug()<<"dopplerChannelEnergyIdentification: j:"<<j;

        for(int u = 0; u < 3; u++)          //  по всем блокам МДО-1 в очередном скане
        {
            for(int i = 0; i < 16; i++)     //  по всем целям в одном блоке
            {
                if(mdo1AllDirections[detectIndex].rawTrgtData[u * 16 + i].range > 0)    // есть данные по цели
                {
                    // 2018_07_18   qDebug()<<"dopplerChannelEnergyIdentification: есть данные по цели i:"<<i;

                    bool firstAppearence = true;
                    if(u > 0)   // проверка на появление сигнала этой же цели у предыдущих блоков МДО-1
                    {
                        for(int e = 0; e < mdo1AllDirections[detectIndex].nextEScanTargetCount; e++)
                        {
                            // 2018_07_18   qDebug()<<"dopplerChannelEnergyIdentification: e:"<<e;

                            if(abs(mdo1AllDirections[detectIndex].rawTrgtData[u * 16 + i].range - mdo1AllDirections[detectIndex].joinedTrgtData[e].range) < 20 &&
                                abs(mdo1AllDirections[detectIndex].rawTrgtData[u * 16 + i].scanerYV10ElevAngle - mdo1AllDirections[detectIndex].joinedTrgtData[e].scanerYV10ElevAngle) < 2)  //  сигнал был
                            {
                                mdo1AllDirections[detectIndex].joinedTrgtData[e].energy[u * 3    ] = mdo1AllDirections[detectIndex].rawTrgtData[u * 16 + i].energy[0];
                                mdo1AllDirections[detectIndex].joinedTrgtData[e].energy[u * 3 + 1] = mdo1AllDirections[detectIndex].rawTrgtData[u * 16 + i].energy[1];
                                mdo1AllDirections[detectIndex].joinedTrgtData[e].energy[u * 3 + 2] = mdo1AllDirections[detectIndex].rawTrgtData[u * 16 + i].energy[2];

                                mdo1AllDirections[detectIndex].joinedTrgtData[e].range = mdo1AllDirections[detectIndex].rawTrgtData[u * 16 + i].range;
                                mdo1AllDirections[detectIndex].joinedTrgtData[e].scanerYV10ElevAngle = mdo1AllDirections[detectIndex].rawTrgtData[u * 16 + i].scanerYV10ElevAngle;
                                mdo1AllDirections[detectIndex].joinedTrgtData[e].range_km = CENA_MLADSHEGO_RAZRIADA_DALNOSTI * mdo1AllDirections[detectIndex].rawTrgtData[u * 16 + i].range;
                                //mdo1AllDirections[detectIndex].joinedTrgtData[e].elevAngleF = srvDrvInfoA.currentDirectrisaElevationAngle - 5.0 * (1.0 -  1.0 / 256 * mdo1AllDirections[detectIndex].rawTrgtData[u * 16 + i].scanerYV10ElevAngle);
                                mdo1AllDirections[detectIndex].joinedTrgtData[e].elevAngleF = srvDrvInfoA.currentDirectrisaElevationAngle - 5.0  + 10.0 / 255 * mdo1AllDirections[detectIndex].rawTrgtData[u * 16 + i].scanerYV10ElevAngle;   //  2018_07-21

                                // 2018_07_18   qDebug()<<"dopplerChannelEnergyIdentification: firstAppearence: false e:"<<e
                                // 2018_07_18           <<"u:"<<u<<"energy:"<<mdo1AllDirections[j].joinedTrgtData[mdo1AllDirections[j].nextEScanTargetCount].energy[u * 3 + 1];

                                firstAppearence = false;
                                break;
                            }
                        }
                    }

                    if(firstAppearence)
                    {
                        // 2018_07_18   qDebug()<<"dopplerChannelEnergyIdentification: metka1";

                        mdo1AllDirections[detectIndex].joinedTrgtData[mdo1AllDirections[detectIndex].nextEScanTargetCount].energy[u * 3    ] = mdo1AllDirections[detectIndex].rawTrgtData[u * 16 + i].energy[0];
                        mdo1AllDirections[detectIndex].joinedTrgtData[mdo1AllDirections[detectIndex].nextEScanTargetCount].energy[u * 3 + 1] = mdo1AllDirections[detectIndex].rawTrgtData[u * 16 + i].energy[1];
                        mdo1AllDirections[detectIndex].joinedTrgtData[mdo1AllDirections[detectIndex].nextEScanTargetCount].energy[u * 3 + 2] = mdo1AllDirections[detectIndex].rawTrgtData[u * 16 + i].energy[2];

                        mdo1AllDirections[detectIndex].joinedTrgtData[mdo1AllDirections[detectIndex].nextEScanTargetCount].range = mdo1AllDirections[detectIndex].rawTrgtData[u * 16 + i].range;
                        mdo1AllDirections[detectIndex].joinedTrgtData[mdo1AllDirections[detectIndex].nextEScanTargetCount].scanerYV10ElevAngle = mdo1AllDirections[detectIndex].rawTrgtData[u * 16 + i].scanerYV10ElevAngle;

                        mdo1AllDirections[detectIndex].joinedTrgtData[mdo1AllDirections[detectIndex].nextEScanTargetCount].range_km = CENA_MLADSHEGO_RAZRIADA_DALNOSTI * mdo1AllDirections[detectIndex].rawTrgtData[u * 16 + i].range;
                        mdo1AllDirections[detectIndex].joinedTrgtData[mdo1AllDirections[detectIndex].nextEScanTargetCount].elevAngleF =
                        //srvDrvInfoA.currentDirectrisaElevationAngle - 5.0 * (1.0 -  1.0 / 256 * mdo1AllDirections[detectIndex].rawTrgtData[u * 16 + i].scanerYV10ElevAngle);
                        srvDrvInfoA.currentDirectrisaElevationAngle - 5.0  + 10.0 / 255 * mdo1AllDirections[detectIndex].rawTrgtData[u * 16 + i].scanerYV10ElevAngle;   //  2018_07-21

                        mdo1AllDirections[detectIndex].nextEScanTargetCount++;

                        // 2018_07_18   qDebug()<<"dopplerChannelEnergyIdentification: firstAppearence: true j:"<<j<<"nextEScanTargetCount:"<<mdo1AllDirections[j].nextEScanTargetCount
                        // 2018_07_18           <<"u:"<<u<<"energy:"<<mdo1AllDirections[j].joinedTrgtData[mdo1AllDirections[j].nextEScanTargetCount].energy[u * 3 + 1];
                    }

                    mdo1AllDirections[detectIndex].joinedTrgtData[i].sredEnF = 1.0 / 65535.0 * (mdo1AllDirections[detectIndex].rawTrgtData[i].energy[1]);

                    ////////////////////////////////////////////mdo1AllDirections[j].floatSredEn[i] = 1.0 / 65535.0 * (mdo1AllDirections[j].targetData[i].energy[0]) +
                    ////////////////////////////////////////////mdo1AllDirections[j].targetData[i].energy[1] + mdo1AllDirections[j].targetData[i].energy[2]) / 3;
                }
                else
                    break;
            }
        }
        //slideTotalTargetCount += mdo1AllDirections[j].nextEScanTargetCount;
    }

    //  О П Р Е Д Е Л Е Н И Е   М А К С И М У М А   С И Г Н А Л А   И   Г Р У Б О Г О   Д О П Л Е Р А

    //for(int j = 0; j < detectCntr; j++)     //  по всем угломестным сканам
    {
        for(int i = 0; i < mdo1AllDirections[detectIndex].nextEScanTargetCount; i++)  //  по всем отождествлённым целям в трёх блоках
        {
            float srEnF = 0;
            float dplr = 0;
            for(int e = 0; e < 9; e++)  //  по 9-ти доплеровским каналам
            {
                if(mdo1AllDirections[detectIndex].joinedTrgtData[i].energy[e] > srEnF)    //  для простоты выбирается доплеровский канал с максимальным сигналом
                {
                    srEnF = mdo1AllDirections[detectIndex].joinedTrgtData[i].energy[e];

                    if(e < 6)
                        dplr =   15.0 * e;
                    else
                        dplr = - 15.0 * (e - 5);
                }

            }
            mdo1AllDirections[detectIndex].joinedTrgtData[i].sredEnF = 1.0 / 65535.0 * srEnF;
            mdo1AllDirections[detectIndex].joinedTrgtData[i].dopler = dplr;
        }


//       qDebug()<<"dopplerChannelEnergyIdentification:"<<j
//                    <<mdo1AllDirections[j].joinedTrgtData[0].range<<mdo1AllDirections[j].joinedTrgtData[1].range
//                    <<mdo1AllDirections[j].joinedTrgtData[2].range<<mdo1AllDirections[j].joinedTrgtData[3].range
//                    <<mdo1AllDirections[j].joinedTrgtData[4].range<<mdo1AllDirections[j].joinedTrgtData[5].range
//                    <<mdo1AllDirections[j].joinedTrgtData[6].range<<mdo1AllDirections[j].joinedTrgtData[7].range
//                    <<mdo1AllDirections[j].joinedTrgtData[8].range<<mdo1AllDirections[j].joinedTrgtData[9].range;

    }
}


int oldMsecsSinceStartOfDay = 0;

void * ProcessThreadMDO1( void * temp_pt )
{
    (void)temp_pt;
    struct sockaddr_in clientAddr_in_1;
    struct sockaddr_in clientAddr_in_2;
    struct sockaddr_in clientAddr_in_3;

    int clientSocketMDO1;

    memset(&clientAddr_in_1, 0, sizeof(clientAddr_in_1));
    memset(&clientAddr_in_2, 0, sizeof(clientAddr_in_3));
    memset(&clientAddr_in_3, 0, sizeof(clientAddr_in_3));

    clientAddr_in_1.sin_family=AF_INET;
    clientAddr_in_2.sin_family=AF_INET;
    clientAddr_in_3.sin_family=AF_INET;

    //clientAddr_in.sin_addr.s_addr = inet_addr("127.0.0.11");//"192.168.20.98");"192.168.3.250");// kontron "192.168.3.135");// lunapark
    char ipSrv_1[255], ipSrv_2[255], ipSrv_3[255];    //strcpy(ipSrv,  ipAddrPort[0].toStdString().c_str());  //
    strcpy(ipSrv_1,  "10.5.0.41");    //вместо 106
    strcpy(ipSrv_2,  "10.5.0.42");
    strcpy(ipSrv_3,  "10.5.0.43");

    clientAddr_in_1.sin_addr.s_addr = inet_addr(ipSrv_1);
    clientAddr_in_1.sin_port=htons(1999);

    clientAddr_in_2.sin_addr.s_addr = inet_addr(ipSrv_2);
    clientAddr_in_2.sin_port=htons(1999);

    clientAddr_in_3.sin_addr.s_addr = inet_addr(ipSrv_3);
    clientAddr_in_3.sin_port=htons(1999);

    //bool ok;
    //clientAddr_in.sin_port = htons(ipAddrPort[1].toUShort(&ok, 10));

    clientSocketMDO1=socket(PF_INET, SOCK_DGRAM, 0);//SOCK_NONBLOCK);
    fcntl(clientSocketMDO1, F_SETFL, O_NONBLOCK);

    int serverSocketMDO1;
    struct sockaddr_in serverAddr_in;

    memset(&serverAddr_in,0,sizeof(serverAddr_in));

    serverAddr_in.sin_family=AF_INET;
    serverAddr_in.sin_port=htons(2000);
    serverAddr_in.sin_addr.s_addr=INADDR_ANY;

    serverSocketMDO1=socket(PF_INET,SOCK_DGRAM,0);
    fcntl(serverSocketMDO1, F_SETFL, O_NONBLOCK);
    bind(serverSocketMDO1,(struct sockaddr *)&serverAddr_in,sizeof(serverAddr_in));

    //struct sockaddr addrFromClient;
    //unsigned int lengthFromClient = sizeof(addrFromClient);

    //MDO1_incomingData nextMDO1Info;

    float prolongAzimuth = 0;
    double curAzLastMeasurementTime = srvDrvInfoA.currentAzimuthLastMeasurementTime;

    qDebug()<<"sizeof(MDO1_incomingData):"<<sizeof(MDO1_incomingData);

    while(1)
    {
        usleep(10 * 1000);//
        //usleep(12 * 1000);

        simulationElevationId32 ++;

        //if((simulationElevationId32 - 1) % 50 == 0 || (simulationElevationId32 - 2) % 50 == 0 || (simulationElevationId32 - 3) % 50 == 0)
          //  qDebug()<<"ProcessThreadMDO1: simulationElevationId32:"<<simulationElevationId32;

        sendto(clientSocketMDO1, &oficerNaved2Console.mdo1Cmds[0], sizeof(MDO1_commands), 0, (sockaddr *)&clientAddr_in_1, sizeof(clientAddr_in_1));
        sendto(clientSocketMDO1, &oficerNaved2Console.mdo1Cmds[0], sizeof(MDO1_commands), 0, (sockaddr *)&clientAddr_in_2, sizeof(clientAddr_in_2));
        sendto(clientSocketMDO1, &oficerNaved2Console.mdo1Cmds[0], sizeof(MDO1_commands), 0, (sockaddr *)&clientAddr_in_3, sizeof(clientAddr_in_3));

        memset(mdoTrgtData, 0, sizeof(MDO1_incomingData));

        uint totalReadBytes = 0;
        ssize_t nextReadBytes;
        int readCirclCounter = 0;

        while (totalReadBytes < sizeof(MDO1_incomingData) && readCirclCounter++ < 1000)
        {
            nextReadBytes = recv(serverSocketMDO1, mdoTrgtData + totalReadBytes, sizeof(MDO1_incomingData) - totalReadBytes, 0);

            if(nextReadBytes < 0)
                nextReadBytes = 0;

            totalReadBytes += nextReadBytes;
        }

        //continue;

        //mdoTrgtData[2] = 0;
        //mdoTrgtData[6] = 0;

        int realMDOind = -1;

        switch(mdoTrgtData[2])
        {
            case 0x21: realMDOind = 0;

//            if(mdoTrgtData[6] < 7)
//                qDebug()<<"ProcessThreadMDO1: "<<mdoTrgtData[3]<<mdoTrgtData[4]<<mdoTrgtData[5]<<"***"<<mdoTrgtData[6]
//                     <<"E:"<<mdoTrgtData[10]<<mdoTrgtData[11]<<mdoTrgtData[12]<<mdoTrgtData[13]<<mdoTrgtData[14]<<mdoTrgtData[15];
                                                 ; break;
            case 0x22: realMDOind = 1; break;
            case 0x23: realMDOind = 2;
        }

        //qDebug()<<"ProcessThreadMDO1: "<<mdoTrgtData[2]<<mdoTrgtData[6];



        int realDetectIndex = -1;

        if(realMDOind > -1)
        {
            if(mdoTrgtData[6] > oldElevationScanId || (mdoTrgtData[6] != 0 && mdoTrgtData[6] + 200 < oldElevationScanId))
            {
                //  2018_07_23  realDetectIndex = detectCntr - 1;

    //            double newSecSinceMidnight = 0.001 * programmStartTime.currentTime().msecsSinceStartOfDay();
    //            float deltaT = newSecSinceMidnight - srvDrvInfoA.currentAzimuthLastMeasurementTime;
    //            prolongAzimuth = srvDrvInfoA.currentAzimuth + srvDrvInfoA.currentAzimuthScanSpeed * (deltaT);

    //            mdo1AllDirections[realDetectIndex].azimuth360 = prolongAzimuth;
    //            mdo1AllDirections[realDetectIndex].currentAzimuthLastMeasurementTime = srvDrvInfoA.currentAzimuthLastMeasurementTime;

                detectCntr++;   //  постинкремент количества угломестных сканов для реальных данных
                if(detectCntr >= MAX_DIRECT_COUNT)
                    detectCntr = 1;

                realDetectIndex = detectCntr - 1;   //  2018_07_23

                int newMsecsSinceStartOfDay = programmStartTime.currentTime().msecsSinceStartOfDay();

                qDebug()<<"METKA 1   PrcsMDO1: mdoTrgtData[6]:"<<mdoTrgtData[6]<<"oldElevationScanId:"<<oldElevationScanId<<"realDetectIndex:"<<realDetectIndex
                        <<"dT:"<<newMsecsSinceStartOfDay - oldMsecsSinceStartOfDay;

                oldMsecsSinceStartOfDay = newMsecsSinceStartOfDay;

                oldElevationScanId = mdoTrgtData[6];
            }
            else
            {
                if(mdoTrgtData[6] == oldElevationScanId)
                {
                    realDetectIndex = detectCntr - 1;

//                    if(realDetectIndex > -1 && realDetectIndex < MAX_DIRECT_COUNT)
//                        qDebug()<<"METKA 2   PrcsMDO1: mdoTrgtData[6]==oldElevationScanId:"<<oldElevationScanId<<"realDetectIndex:"<<realDetectIndex
//                               <<"realMDOind:"<<realMDOind<<"dataExist:"<<mdo1AllDirections[realDetectIndex].dataExist[realMDOind];
//                    else
//                        qDebug()<<"METKA 2   PrcsMDO1: mdoTrgtData[6]==oldElevationScanId:"<<oldElevationScanId<<"realDetectIndex:"<<realDetectIndex;
                }
                else
                {
                    if((mdoTrgtData[6] == 255 && (oldElevationScanId == 1 && detectCntr > 1 || oldElevationScanId == 0 && detectCntr > 0)) ||
                        mdoTrgtData[6] == 254 && oldElevationScanId == 0 && detectCntr > 1)
                    {
                        realDetectIndex = detectCntr - 1;

                        if(mdoTrgtData[6] == 255)
                        {
                            if(oldElevationScanId == 1 && detectCntr > 1)
                            {
                                qDebug()<<"METKA 3   PrcsMDO1: mdoTrgtData[6]==255 && oldElevationScanId==1 && detectCntr > 1   realDetectIndex:"<<realDetectIndex;
                            }
                            else
                            {
                                qDebug()<<"METKA 4   PrcsMDO1: mdoTrgtData[6]==255 && oldElevationScanId==0 && detectCntr > 0   realDetectIndex:"<<realDetectIndex;
                            }
                        }
                        else
                            qDebug()<<"METKA 5   ProcessThreadMDO1: mdoTrgtData[6]==254 && oldElevationScanId == 0 && detectCntr > 1   realDetectIndex:"<<realDetectIndex;
                    }
                    else
                    if((mdoTrgtData[6] > oldElevationScanId - 3 && detectCntr > 3) ||
                       (mdoTrgtData[6] > oldElevationScanId - 2 && detectCntr > 2) ||
                       (mdoTrgtData[6] > oldElevationScanId - 1 && detectCntr > 1)  )
                    {
                        realDetectIndex = detectCntr - oldElevationScanId + mdoTrgtData[6];
                        qDebug()<<"METKA 6   PrcsMDO1: realDetectIndex:"<<realDetectIndex<<"= detectCntr:"<<detectCntr<<"- oldElevationScanId:"<<oldElevationScanId
                                <<"+ mdoTrgtData[6]:"<< + mdoTrgtData[6];
                    }
                }
            }
        }

        //  2018_07_10  double newSecSinceMidnight = 0.001 * programmStartTime.currentTime().msecsSinceStartOfDay();
        //  2018_07_10  float deltaT = newSecSinceMidnight - srvDrvInfoA.currentAzimuthLastMeasurementTime;
        //  2018_07_10  float prolongAzimuth = srvDrvInfoA.currentAzimuth + srvDrvInfoA.currentAzimuthScanSpeed * (deltaT);

        //int resultMDOind = realMDOind;
        //int resultDetectIndex = realDetectIndex;

        //realMDOind = -1;
        //realDetectIndex = -1;

        if(realMDOind != -1 && realDetectIndex != -1 && realDetectIndex < MAX_DIRECT_COUNT)    // если какой-то реальный МДО1 ответил корректно, работаем по его данным
        {
            if(mdo1AllDirections[realDetectIndex].dataExist[realMDOind] == false)
            {
                mdo1AllDirections[realDetectIndex].dataExist[realMDOind] = true;

                memcpy(&unv2apy.mdo1Info[realMDOind], mdoTrgtData, sizeof(MDO1_incomingData));   //  2018_06_21

                memcpy(mdo1AllDirections[realDetectIndex].rawTrgtData + realMDOind * 16, &unv2apy.mdo1Info[realMDOind].rawTrgtData, sizeof(MDO1_targetData) * 16);// 2018_07_18

                double newSecSinceMidnight = 0.001 * programmStartTime.currentTime().msecsSinceStartOfDay();
                float deltaT = newSecSinceMidnight - srvDrvInfoA.currentAzimuthLastMeasurementTime;
                prolongAzimuth = srvDrvInfoA.currentAzimuth + srvDrvInfoA.currentAzimuthScanSpeed * (deltaT);

                mdo1AllDirections[realDetectIndex].azimuth360 = prolongAzimuth;
                mdo1AllDirections[realDetectIndex].currentAzimuthLastMeasurementTime = srvDrvInfoA.currentAzimuthLastMeasurementTime;

                //qDebug()<<"***  mdoInd:"<<realMDOind<<"dInd:"<<realDetectIndex<<"Azimuth:"<<prolongAzimuth<<"V:"<<srvDrvInfoA.currentAzimuthScanSpeed<<"dT:"<<deltaT;//<<"Time:"<<srvDrvInfoA.currentAzimuthLastMeasurementTime;

                dopplerChannelIdentification(realDetectIndex);   // РЕАЛЬНЫЕ ДАННЫЕ: складывание угломестных фрагментов из 3-х блоков МДО-1 в общий список

                memcpy(&unv2apy.joinedTrgtData, mdo1AllDirections[realDetectIndex].joinedTrgtData, sizeof(MDO1_joinedTargetData) * MAX_MDO1_TARGETS_COUNT);   //  2018_07_21

//            qDebug()<<"A: ProcessThreadMDO1: realMDOind:"<<realMDOind<<"realDetectIndex"<<realDetectIndex<<"prolongAzimuth:"<<prolongAzimuth
//                   <<unv2apy.joinedTrgtData[0].scanerYV10ElevAngle<<unv2apy.joinedTrgtData[1].scanerYV10ElevAngle<<unv2apy.joinedTrgtData[2].scanerYV10ElevAngle;
            }
        }
        else                                                            // только для отладки - реальных МДО1 нет
            //if(false)
            if(apyInfoA.rejimRabotiZRK ==  RejimRabotiZRK::TR_A)        // в режиме автономного тренажа
            {                
                uint8_t simMDO1Ind = simulationElevationId32 % 3;

                if(simMDO1Ind == 0)
                {
                    double newSecSinceMidnight = 0.001 * programmStartTime.currentTime().msecsSinceStartOfDay();
                    curAzLastMeasurementTime = srvDrvInfoA.currentAzimuthLastMeasurementTime;
                    float deltaT = newSecSinceMidnight - curAzLastMeasurementTime;
                    prolongAzimuth = srvDrvInfoA.currentAzimuth + srvDrvInfoA.currentAzimuthScanSpeed * (deltaT);

                    detectCntr++;   //  постинкремент количества угломестных сканов для имитированных данных

                    if(detectCntr >= MAX_DIRECT_COUNT)
                        detectCntr = 1;
                }

                uint8_t simDetectIndex = detectCntr - 1;

                mdo1AllDirections[simDetectIndex].azimuth360 = prolongAzimuth;
                mdo1AllDirections[simDetectIndex].currentAzimuthLastMeasurementTime = curAzLastMeasurementTime;

                //if(simDetectIndex == 0)
                //    qDebug()<<"simDetInd:"<<simDetectIndex<<"simInd:"<<simMDO1Ind<<"["<<simulationElevationId32<<"] prolAz:"<<prolongAzimuth<<"MeasTime:"<<curAzLastMeasurementTime;

                if(simMDO1Ind == 0)
                    simulatorMDO1(simMDO1Ind, (simulationElevationId32 / 3) % 256, prolongAzimuth);  // добавляем две имитированные "истинные" цели и две "ложные"

                // 2018_07_18   qDebug()<<"ProcessThreadMDO1: simDetectIndex:"<<simDetectIndex<<"simMDO1Ind:"<<simMDO1Ind<<(simulationElevationId32 / 3) % 256;

//                if(simMDO1Ind == 2)
//                {
//                    unv2apy.mdo1Info[simMDO1Ind].rawTrgtData[15].energy[0] = 4567;
//                    unv2apy.mdo1Info[simMDO1Ind].rawTrgtData[15].energy[1] = 5678;
//                    unv2apy.mdo1Info[simMDO1Ind].rawTrgtData[15].energy[2] = 1000 + simDetectIndex;
//                }

//                if(simMDO1Ind == 0)  qDebug()<<simDetectIndex
//                    <<unv2apy.mdo1Info[0].targetData[0].energy[1]<<unv2apy.mdo1Info[0].targetData[1].energy[1]<<unv2apy.mdo1Info[0].targetData[2].energy[1]<<unv2apy.mdo1Info[0].targetData[3].energy[1]
//                    <<unv2apy.mdo1Info[0].targetData[4].energy[1]<<unv2apy.mdo1Info[0].targetData[5].energy[1]<<unv2apy.mdo1Info[0].targetData[6].energy[1]<<unv2apy.mdo1Info[0].targetData[7].energy[1]
//                    <<unv2apy.mdo1Info[0].targetData[8].energy[1]<<unv2apy.mdo1Info[0].targetData[9].energy[1]<<unv2apy.mdo1Info[0].targetData[10].energy[1]<<unv2apy.mdo1Info[0].targetData[11].energy[1]
//                    <<unv2apy.mdo1Info[0].targetData[12].energy[1]<<unv2apy.mdo1Info[0].targetData[13].energy[1]<<unv2apy.mdo1Info[0].targetData[14].energy[1]<<unv2apy.mdo1Info[0].targetData[15].energy[1];

                //memcpy(&mdo1AllDirections[simDetectIndex].targetData[simMDO1Ind * 16], &unv2apy.mdo1Info[simMDO1Ind * 16].targetData, sizeof(MDO1_targetData) * 16);

                // 2018_07_18   if(simDetectIndex >= 195)
                    // 2018_07_18   qDebug()<<"ProcessThreadMDO1: Metaka1 simDetectIndex:"<<simDetectIndex<<"simMDO1Ind:"<<simMDO1Ind<<(simulationElevationId32 / 3) % 256;

                memcpy(mdo1AllDirections[simDetectIndex].rawTrgtData + simMDO1Ind * 16, &unv2apy.mdo1Info[simMDO1Ind].rawTrgtData, sizeof(MDO1_targetData) * 16);

                dopplerChannelIdentification(simDetectIndex);   // ИМИТАЦИЯ: складывание угломестных фрагментов из 3-х блоков МДО-1 в общий список

                memcpy(&unv2apy.joinedTrgtData, mdo1AllDirections[simDetectIndex].joinedTrgtData, sizeof(MDO1_joinedTargetData) * MAX_MDO1_TARGETS_COUNT);   //  2018_07_21

//                qDebug()<<"A:   PrcsMDO1:"
//                       <<mdo1AllDirections[simDetectIndex].joinedTrgtData[0].elevAngleF<<mdo1AllDirections[simDetectIndex].joinedTrgtData[1].elevAngleF
//                       <<mdo1AllDirections[simDetectIndex].joinedTrgtData[2].elevAngleF<<mdo1AllDirections[simDetectIndex].joinedTrgtData[3].elevAngleF;

//                          unv2apy.joinedTrgtData[0].elevAngleF<<unv2apy.joinedTrgtData[1].elevAngleF<<unv2apy.joinedTrgtData[2].elevAngleF
//                          <<unv2apy.joinedTrgtData[3].elevAngleF<<unv2apy.joinedTrgtData[4].elevAngleF<<unv2apy.joinedTrgtData[5].elevAngleF<<unv2apy.joinedTrgtData[6].elevAngleF;


                // 2018_07_18   memcpy(&mdo1AllDirections[simDetectIndex].rawTrgtData[simMDO1Ind * 16], &unv2apy.mdo1Info[simMDO1Ind].rawTrgtData, sizeof(MDO1_targetData) * 16);

                // 2018_07_18   for(int i = 0; i < 16; i++)
                    // 2018_07_18   memcpy(&mdo1AllDirections[simDetectIndex].rawTrgtData[simMDO1Ind * 16 + i], &unv2apy.mdo1Info[simMDO1Ind].rawTrgtData[i], sizeof(MDO1_targetData));


//                if(simMDO1Ind == 2)
//                    qDebug()<<simDetectIndex<<unv2apy.mdo1Info[simMDO1Ind].rawTrgtData[0].range<<unv2apy.mdo1Info[simMDO1Ind].rawTrgtData[1].range
//                            <<unv2apy.mdo1Info[simMDO1Ind].rawTrgtData[2].range<<unv2apy.mdo1Info[simMDO1Ind].rawTrgtData[3].range
//                            <<unv2apy.mdo1Info[simMDO1Ind].rawTrgtData[4].range<<unv2apy.mdo1Info[simMDO1Ind].rawTrgtData[5].range
//                            <<unv2apy.mdo1Info[simMDO1Ind].rawTrgtData[6].range<<unv2apy.mdo1Info[simMDO1Ind].rawTrgtData[7].range
//                            <<unv2apy.mdo1Info[simMDO1Ind].rawTrgtData[8].range<<unv2apy.mdo1Info[simMDO1Ind].rawTrgtData[9].range
//                            <<unv2apy.mdo1Info[simMDO1Ind].rawTrgtData[10].range<<unv2apy.mdo1Info[simMDO1Ind].rawTrgtData[11].range
//                            <<unv2apy.mdo1Info[simMDO1Ind].rawTrgtData[12].range<<unv2apy.mdo1Info[simMDO1Ind].rawTrgtData[13].range
//                            <<unv2apy.mdo1Info[simMDO1Ind].rawTrgtData[14].range<<unv2apy.mdo1Info[simMDO1Ind].rawTrgtData[15].range;

//                if(simMDO1Ind == 2)
//                    qDebug()<<simDetectIndex
//                           <<mdo1AllDirections[simDetectIndex].rawTrgtData[simMDO1Ind * 16 + 0].range<<mdo1AllDirections[simDetectIndex].rawTrgtData[simMDO1Ind * 16 + 1].range
//                           <<mdo1AllDirections[simDetectIndex].rawTrgtData[simMDO1Ind * 16 + 2].range<<mdo1AllDirections[simDetectIndex].rawTrgtData[simMDO1Ind * 16 + 3].range
//                           <<mdo1AllDirections[simDetectIndex].rawTrgtData[simMDO1Ind * 16 + 4].range<<mdo1AllDirections[simDetectIndex].rawTrgtData[simMDO1Ind * 16 + 5].range
//                           <<mdo1AllDirections[simDetectIndex].rawTrgtData[simMDO1Ind * 16 + 6].range<<mdo1AllDirections[simDetectIndex].rawTrgtData[simMDO1Ind * 16 + 7].range
//                           <<mdo1AllDirections[simDetectIndex].rawTrgtData[simMDO1Ind * 16 + 8].range<<mdo1AllDirections[simDetectIndex].rawTrgtData[simMDO1Ind * 16 + 9].range
//                           <<mdo1AllDirections[simDetectIndex].rawTrgtData[simMDO1Ind * 16 + 10].range<<mdo1AllDirections[simDetectIndex].rawTrgtData[simMDO1Ind * 16 + 11].range
//                           <<mdo1AllDirections[simDetectIndex].rawTrgtData[simMDO1Ind * 16 + 12].range<<mdo1AllDirections[simDetectIndex].rawTrgtData[simMDO1Ind * 16 + 13].range
//                           <<mdo1AllDirections[simDetectIndex].rawTrgtData[simMDO1Ind * 16 + 14].range<<mdo1AllDirections[simDetectIndex].rawTrgtData[simMDO1Ind * 16 + 15].range;

//                if(simMDO1Ind == 2)
//                    qDebug()<<simDetectIndex
//                            <<mdo1AllDirections[simDetectIndex].joinedTrgtData[0].range<<mdo1AllDirections[simDetectIndex].joinedTrgtData[1].range
//                            <<mdo1AllDirections[simDetectIndex].joinedTrgtData[2].range<<mdo1AllDirections[simDetectIndex].joinedTrgtData[3].range
//                            <<mdo1AllDirections[simDetectIndex].joinedTrgtData[4].range<<mdo1AllDirections[simDetectIndex].joinedTrgtData[5].range
//                            <<mdo1AllDirections[simDetectIndex].joinedTrgtData[6].range<<mdo1AllDirections[simDetectIndex].joinedTrgtData[7].range
//                            <<mdo1AllDirections[simDetectIndex].joinedTrgtData[8].range<<mdo1AllDirections[simDetectIndex].joinedTrgtData[9].range;


                // 2018_07_18   if(simDetectIndex >= 195)
                    // 2018_07_18   qDebug()<<"ProcessThreadMDO1: Metaka2 simDetectIndex:"<<simDetectIndex<<"simMDO1Ind:"<<simMDO1Ind<<(simulationElevationId32 / 3) % 256;

                //  if(simMDO1Ind == 2)  mdo1AllDirections[simDetectIndex].targetData[47].energy[2] = 10000 + simDetectIndex;


           }
    };

    close(clientSocketMDO1);
    close(serverSocketMDO1);
}

//bool writeData = false;

int ind = 0;

int cntr4096 = 0;

void * ProcessThreadMDO1Echo( void * temp_pt )
{
    (void)temp_pt;
    int serverSocket;
    struct sockaddr_in serverAddr_in;//, client={AF_INET,INADDR_ANY,INADDR_ANY};

    memset(&serverAddr_in,0,sizeof(serverAddr_in));

    serverAddr_in.sin_family=AF_INET;
    serverAddr_in.sin_port=htons(2001);
    serverAddr_in.sin_addr.s_addr=INADDR_ANY;
//    char ipSrv[255];
//    ////////////////strcpy(ipSrv,  settings->ipAddressARMVL.toStdString().c_str());    //strcpy(ipSrv,  "192.168.3.250");//    strcpy(ipSrv,  "192.168.21.185");
//    serverAddr_in.sin_addr.s_addr = inet_addr(ipSrv);

    serverSocket=socket(PF_INET,SOCK_DGRAM,0);
    fcntl(serverSocket, F_SETFL, O_NONBLOCK);   //  без плат МДО раскомментировать
    bind(serverSocket,(struct sockaddr *)&serverAddr_in,sizeof(serverAddr_in));

    //struct sockaddr addrFromClient;
    //unsigned int lengthFromClient = sizeof(addrFromClient);

    //if(false)
    while(1)
    {
        usleep(35 * 1000);

        memset(mdoArray, 0, sizeof(MDO1_echoData));

        uint totalReadBytes = 0;
        ssize_t nextReadBytes;
        int readCirclCounter = 0;

        while (totalReadBytes < sizeof(MDO1_echoData) && readCirclCounter++ < 1000)
        {
            //nextReadBytes = recvfrom(serverSocket, &shMemC2U.mdo1Echo.x55 + totalReadBytes, sizeof(MDO1_echoData) - totalReadBytes, 0, &addrFromClient, &lengthFromClient);

            //nextReadBytes = recvfrom(serverSocket, mdoArray + totalReadBytes, sizeof(MDO1_echoData) - totalReadBytes, 0, &addrFromClient, &lengthFromClient);
            nextReadBytes = recv(serverSocket, mdoArray + totalReadBytes, sizeof(MDO1_echoData) - totalReadBytes, 0);

            if(nextReadBytes < 0)
                nextReadBytes = 0;

            //qDebug()<<"ProcessThreadMDO1Echo nextReadBytes:"<<nextReadBytes;

            totalReadBytes += nextReadBytes;

            //rsltStr += QString("crclN: %1  nextReadBytes: %2  totalReadBytes: %3  ").arg(crclN++).arg(nextReadBytes).arg(totalReadBytes);
        }

        //qDebug()<<"ProcessThreadMDO1Echo sizeof(MDO1_echoData):"<<sizeof(MDO1_echoData)<<"totalReadBytes:"<<totalReadBytes;

        if(totalReadBytes == sizeof(MDO1_echoData))
        {

//            for(int i = 0; i < sizeof(MDO1_echoData); i++)
//                mdoArray[i] = i % 255;            

            memcpy(&unv2apy.mdo1Echo, mdoArray, sizeof(MDO1_echoData));  //  2018_06_21

//            qDebug()<<"ProcessMDO1Echo totalReadBytes:"<<totalReadBytes<<cnsl2DM.mdo1Echo.header.x55<<cnsl2DM.mdo1Echo.header.xAA
//               <<cnsl2DM.mdo1Echo.header.deviceAddress<<"   "<<cnsl2DM.mdo1Echo.echo[0][0]<<cnsl2DM.mdo1Echo.echo[0][1]<<cnsl2DM.mdo1Echo.echo[0][2]
//                    <<cnsl2DM.mdo1Echo.echo[0][3]<<cnsl2DM.mdo1Echo.echo[0][4]<<cnsl2DM.mdo1Echo.echo[0][5]<<cnsl2DM.mdo1Echo.echo[0][6]
//                    <<cnsl2DM.mdo1Echo.echo[0][7]<<cnsl2DM.mdo1Echo.echo[0][8]<<cnsl2DM.mdo1Echo.echo[0][9]<<cnsl2DM.mdo1Echo.echo[0][10]<<" ... "
//                    <<cnsl2DM.mdo1Echo.echo[0][252]<<cnsl2DM.mdo1Echo.echo[0][253]<<cnsl2DM.mdo1Echo.echo[0][254]<<cnsl2DM.mdo1Echo.echo[0][255];
        }
        else    if(false)   // только для тестирования прохождения информации
        {   //  З А П О Л Н Е Н И Е   П И Л О Й   К О Г Д А   Н Е Т   Д А Н Н Ы Х
            for(int j = 0; j < 16; j++)
            {
                for(int i = 0; i < 256; i ++)
                {
                    if((i + cntr4096) % 16 == 0)//if(cntr4096 % 16 == 0)
                        unv2apy.mdo1Echo.echo[j][i] = cntr4096;
                    else
                        unv2apy.mdo1Echo.echo[j][i] = - cntr4096;
                }
            }

            cntr4096++;

        }
    };

    close(serverSocket);
}
