#include "externA.h"
#include "srcKram/Drive125.h"

//#define real_work

//--------------------------ADD Kramarenko A.V.-----19.03.2018----------------------------
extern  Drive125 *drive_unit; //Drive125 drive; //extern Drive125 * drive;//
//----------------------------------------------------------------------------------------

extern void azimuthPakageDetector();

bool unvClockwiseScanDirection = true;

float directFactor = 1.0;

int cntr = 0;


//int komJustExecuteCounter[] = {-1,-1};

void servoDriveExecuter()
{
    //////////////////////qDebug()<<"servoDriveExecuter: apyInfoA.rejimPoiskaSNR:"<<apyInfoA.rejimPoiskaSNR;

    //qDebug()<<"servoDriveExecuter ********** settings->servoDriveDiagnostic:"<<settings->servoDriveDiagnostic;

    //printf("printf servoDriveExecuter: %d\n", settings->servoDriveDiagnostic);

    cntr++;
   // komKomA.servoDriveSimulation?drive_unit->unv->set_mode(drive_unit->unv->training):drive_unit->unv->set_mode(drive_unit->unv->combat);

//  2018-06-16    if(apyInfoA.rejimRabotiZRK == RejimRabotiZRK::TR_A)
//  2018-06-16        srvDrvInfoA.targetIndexCY = oficerNaved2Console.guidanceOfficerCmds.targetIndexCY;
//    else
//        srvDrvInfoA.simulatorTargetIndexCY = 0;

    //  2018-06-19    apyInfoA.rejimSoprovojdeniaSNR = oficerNaved2Console.guidanceOfficerCmds.rejimSoprovojdeniaSNR;

    if(apyInfoA.rejimPoiskaSNR == RejimPoiskaSNR::POISK_PEREKLYCHEN_V_SOPROVOJDENIE)
        srvDrvInfoA.cyOtrabotano = -1;
    else
        if(apyInfoA.rejimPoiskaSNR == RejimPoiskaSNR::POISK_PEREKLYCHEN_V_ZAHVAT && srvDrvInfoA.cyOtrabotano != 1)
            srvDrvInfoA.cyOtrabotano = 0;

    if(apyInfoA.rejimPoiskaSNR == POISK_BSP)
        srvDrvInfoA.azimuthScanSectorHalfWidth = 5.0;
    else
        if(apyInfoA.rejimPoiskaSNR == POISK_MSP)
            srvDrvInfoA.azimuthScanSectorHalfWidth = 2.5;

//    if(srvDrvInfoA.rejimPoiskaSNR != RejimPoiskaSNR::POISK_PEREKLYCHEN_V_ZAHVAT && srvDrvInfoA.rejimPoiskaSNR != RejimPoiskaSNR::POISK_PEREKLYCHEN_V_SOPROVOJDENIE)//if(srvDrvInfoA.zahvat == false)
//    {
//        for(int i = 0; i < 2; i++)
//        {
//            old_state_F[i].Position[0] = 0.0;
//            /////////////////////////////srvDrvInfoA.handWheelFSpeed[i] = 0.0;
//        }
//    }

    //  А З И М У Т   А З И М У Т   А З И М У Т   А З И М У Т   А З И М У Т
    //qDebug()<<"servoDriveExecuter: komKomG.kanalSvoboden:"<<komKomG.kanalSvoboden<<"rejimRabotiZRK:"<<komKomG.rejimRabotiZRK;

    if(apyInfoA.remoteControlZRK) //  РАБОТА ПО ЦУ ОТ КП
    {
        if(vkp2zrkG.komandiPoCelevomyKanaly1.commandInfoExist)
        {
//  2018-06-15            if(vkp2zrkG.komandiPoCelevomyKanaly1.OCU)
//            {
//                srvDrvInfoA.rejimPoiskaSNR = POISK_VIKLUCHEN;

//                qDebug()<<"servoDriveExecuter: metka 1 - отмена ЦУ"<<"cntr:"<<cntr<<"CU:"<<vkp2zrkG.komandiPoCelevomyKanaly1.CU<<"OCU:"<<vkp2zrkG.komandiPoCelevomyKanaly1.OCU;
//            }

            if(vkp2zrkG.komandiPoCelevomyKanaly1.CU)
            {
// 2018-06-15               if(apyInfoA.rejimPoiskaSNR == POISK_VIKLUCHEN)
//                {
//                    srvDrvInfoA.rejimPoiskaSNR = RejimPoiskaSNR::POISK_PEREKLYCHEN_V_ZAHVAT;
//                }
//                else
                {                    
                    //  2018-06-15  if(vkp2zrkG.komandiPoCelevomyKanaly1.targetInfoExist)
                    {
                        float azCU_grad = atan2(vkp2zrkG.coordYHX.x_km, vkp2zrkG.coordYHX.y_km) * Rad2Grad;

                        float deltaAz = azCU_grad - srvDrvInfoA.currentAzimuth;

                        if(deltaAz > 180)
                            deltaAz -= 360.0;

                        if(deltaAz < -180)
                            deltaAz += 360.0;

                 // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! ne ponal
                        drive_unit->unv->device_setpos_az(/*drive_unit->unv->device_getpos_az()+*/ deltaAz); // p1.pos_cmd = p1.curr_pos + deltaAz;

                        float deltaMax = 2.5;

                        if(srvDrvInfoA.cyOtrabotano == 0 && fabs(deltaAz) < deltaMax)
                        {
                            srvDrvInfoA.cyOtrabotano = 1;   //  2018-06-15  srvDrvInfoA.rejimPoiskaSNR = RejimPoiskaSNR::POISK_PEREKLYCHEN_V_SOPROVOJDENIE;
                        }
//                        else
//                        {
//                            srvDrvInfoA.cyOtrabotano = false;   //  2018-06-15  srvDrvInfoA.rejimPoiskaSNR = RejimPoiskaSNR::POISK_PEREKLYCHEN_V_ZAHVAT;
//                        }

//                        qDebug()<<"servoDriveExecuter: metka 3 cntr:"<<cntr<<"azCU_grad:"<<azCU_grad<<"currentAzimuth"<<srvDrvInfoA.currentAzimuth<<"deltaAz:"<<deltaAz
//                               <<"srvDrvInfoA.rejimPoiskaSNR:"<<srvDrvInfoA.rejimPoiskaSNR<<"targetInfoExist:"<<vkp2zrkG.komandiPoCelevomyKanaly1.targetInfoExist
//                              <<"coord:"<<vkp2zrkG.coordYHX.y_km<<vkp2zrkG.coordYHX.x_km<<"CU:"<<vkp2zrkG.komandiPoCelevomyKanaly1.CU<<"OCU:"<<vkp2zrkG.komandiPoCelevomyKanaly1.OCU;
                    }
                }
            }
        }
    }
    else    // М Е С Т Н О Е   У П Р А В Л Е Н И Е   З Р К
    {
        // О Т Р А Б О Т К А   К Л И К А   М Ы Ш И   П О   Ш К А Л А М   кроме режима захвата и сопровождения
        if(apyInfoA.rejimPoiskaSNR != POISK_PEREKLYCHEN_V_ZAHVAT && apyInfoA.rejimPoiskaSNR != POISK_PEREKLYCHEN_V_SOPROVOJDENIE)
        {
            if(oficerNaved2Console.guidanceOfficerCmds.servoDriveExecute[AZIMUTH])    // отработка клика мыши по шкале азимута
            {
                //p1.pos_cmd = p1.curr_pos + oficerNaved2Console.guidanceOfficerCmds.servoDriveDelta[AZIMUTH];    //  2018-08-07
                double newAz = convert360angle2PlusMinus180(srvDrvInfoA.directrisaAzimuth + oficerNaved2Console.guidanceOfficerCmds.servoDriveDelta[AZIMUTH]);
                drive_unit->unv->device_setpos_az(newAz);   //  2018-05-08
                srvDrvInfoA.directrisaAzimuth = drive_unit->unv->device_getpos_az();   //srvDrvInfoA.directrisaAzimuth = p1.pos_cmd; //  2018-08-07

                qDebug()<<"servoDriveExecuter отработка клика мыши по шкале азимута наведение newAz:"<<newAz<<"komKomA.servoDriveSimulation:"<<komKomA.servoDriveSimulation;

                //qDebug()<<"servoDriveExecuter srvDrvInfoA.directrisaAzimuth:"<<srvDrvInfoA.directrisaAzimuth;
            }

            if(komKomA.srv[AZIMUTH].executeCounter % 10 > 0)    // отработка клика мыши по шкале азимута командиром
            {
                if(komKomA.srv[AZIMUTH].executeCounter / 10 > srvDrvInfoA.komJustExecuteCounter[AZIMUTH])  // это действительно новый клик
                {                    
                    srvDrvInfoA.komJustExecuteCounter[AZIMUTH] = komKomA.srv[AZIMUTH].executeCounter / 10;
/*
//                    if(srvDrvInfoA.rejimPoiskaSNR == RejimPoiskaSNR::POISK_VIKLUCHEN)
//                    {
                        p1.pos_cmd = p1.curr_pos + komKomA.srv[AZIMUTH].delta;
//                    }
//                    else
                        srvDrvInfoA.directrisaAzimuth = p1.pos_cmd;//+= komKomM.srv[AZIMUTH].delta;

                    //  2018-08-07  p1.pos_cmd = convert360angle2PlusMinus180(srvDrvInfoA.directrisaAzimuth + komKomA.srv[AZIMUTH].delta);  //  2018-05-08
                    //  2018-08-07  srvDrvInfoA.directrisaAzimuth = p1.pos_cmd;                        //  2018-05-08                    

*/

                    double newAz = convert360angle2PlusMinus180(srvDrvInfoA.directrisaAzimuth + komKomA.srv[AZIMUTH].delta);
                   qDebug()<<"servoDriveExecuter отработка клика мыши по шкале азимута командиром newAz:"<<newAz<<"komKomA.servoDriveSimulation:"<<komKomA.servoDriveSimulation;

                  drive_unit->unv->device_setpos_az(newAz);  //  2018-05-08
                  srvDrvInfoA.directrisaAzimuth = drive_unit->unv->device_getpos_az();                                                             //  2018-05-08
                }
                else
                    if(komKomA.srv[AZIMUTH].executeCounter / 10 < srvDrvInfoA.komJustExecuteCounter[AZIMUTH])   // случай когда АРМ командира был перезапущен
                        srvDrvInfoA.komJustExecuteCounter[AZIMUTH] = komKomA.srv[AZIMUTH].executeCounter / 10 - 1;
            }

            if(oficerNaved2Console.guidanceOfficerCmds.servoDriveExecute[ELEVANG])    // отработка клика мыши по угломестной шкале
            {
                //p2.pos_cmd = p2.curr_pos + oficerNaved2Console.guidanceOfficerCmds.servoDriveDelta[ELEVANG];
                drive_unit->unv->device_setpos_elv(drive_unit->unv->device_getpos_elv() + oficerNaved2Console.guidanceOfficerCmds.servoDriveDelta[ELEVANG]);
            }

            if(komKomA.srv[ELEVANG].executeCounter % 10 > 0)    // отработка клика мыши по угломестной шкале командиром
            {
                if(komKomA.srv[ELEVANG].executeCounter / 10 > srvDrvInfoA.komJustExecuteCounter[ELEVANG])  // это действительно новый клик
                {                    
                    srvDrvInfoA.komJustExecuteCounter[ELEVANG] = komKomA.srv[ELEVANG].executeCounter / 10;
                    drive_unit->unv->device_setpos_elv(drive_unit->unv->device_getpos_elv() + komKomA.srv[ELEVANG].delta);//p2.pos_cmd = p2.curr_pos + komKomA.srv[ELEVANG].delta;
                }
                else
                    if(komKomA.srv[ELEVANG].executeCounter / 10 < srvDrvInfoA.komJustExecuteCounter[ELEVANG])   // случай когда АРМ командира был перезапущен
                        srvDrvInfoA.komJustExecuteCounter[ELEVANG] = komKomA.srv[ELEVANG].executeCounter / 10 - 1;
            }
        }

        //  2018-06-16  if(oficerNaved2Console.guidanceOfficerCmds.rejimPoiskaSNR == RejimPoiskaSNR::POISK_VIKLUCHEN)  // старая или новая команда офицера наведения выключить и поиск и сопровождение
        if(apyInfoA.rejimPoiskaSNR == RejimPoiskaSNR::POISK_VIKLUCHEN)  //  2018-06-16
        {
//  2018-06-16  if(apyInfoA.rejimPoiskaSNR == RejimPoiskaSNR::POISK_BSP ||      // сканирования было включено,
//  2018-06-16      apyInfoA.rejimPoiskaSNR == RejimPoiskaSNR::POISK_MSP)        // выключаем сканирование и возвращаем УНВ на директрису
            {
    //            drive_unit->unv->device_setpos_az(srvDrvInfoA.directrisaAzimuth);//p1.pos_cmd = srvDrvInfoA.directrisaAzimuth;
            }
        }        
        else    // офицер наведения включил или поиск или захват
        {
            //  2018-06-16  if(oficerNaved2Console.guidanceOfficerCmds.rejimPoiskaSNR == RejimPoiskaSNR::POISK_BSP || oficerNaved2Console.guidanceOfficerCmds.rejimPoiskaSNR == RejimPoiskaSNR::POISK_MSP)  // офицер наведения включил поиск
            //  2018-06-16  {
                if(apyInfoA.rejimPoiskaSNR == RejimPoiskaSNR::POISK_BSP || apyInfoA.rejimPoiskaSNR == RejimPoiskaSNR::POISK_MSP) // сканирование продолжается
                {
                    drive_unit->unv->device_setpos_az(srvDrvInfoA.directrisaAzimuth + directFactor * srvDrvInfoA.azimuthScanSectorHalfWidth);//p1.pos_cmd = srvDrvInfoA.directrisaAzimuth + directFactor * srvDrvInfoA.azimuthScanSectorHalfWidth;

                    if(fabs(convert360angle2PlusMinus180(drive_unit->unv->device_getpos_az() - srvDrvInfoA.currentAzimuth)) < 0.1)  // дошли до границы сектора - возвращаемся
                    {
                        unvClockwiseScanDirection = !unvClockwiseScanDirection;

                        azimuthPakageDetector();

                        directFactor = 1.0;
                        if(unvClockwiseScanDirection == false)
                            directFactor = -1.0;
                    }
                }
                //  2018-06-16  else    // сканирование начинается
                //  2018-06-16  {
                //  2018-06-16    unvClockwiseScanDirection = true;
                //  2018-06-16      p1.pos_cmd = convert360angle2PlusMinus180(srvDrvInfoA.directrisaAzimuth + srvDrvInfoA.azimuthScanSectorHalfWidth);    //  2018-05-08
                //  2018-06-16  }
            //  2018-06-16  }
            else    // офицер наведения включил захват, в режиме тренажа при АС сопровождение вкючится само, когда сервопривод отработает азимут
            {                
                if(oficerNaved2Console.guidanceOfficerCmds.targetIndexCY >= 0 && oficerNaved2Console.guidanceOfficerCmds.targetIndexCY < MAXTOCKANUM)   //  эта проверка на тот случай, если simulatorTargetIndexCY == -1,
                {                                                                                                                                   //  так быть не должно никогда
                    //if(srvDrvInfoA.cyOtrabotano != 1)
                        //  2018-06-20  qDebug()<<"servoDriveExecuter: apyInfoA.rejimSoprovojdeniaSNR"<<apyInfoA.rejimSoprovojdeniaSNR<<"apyInfoA.rejimPoiskaSNR:"<<apyInfoA.rejimPoiskaSNR;

                    if(apyInfoA.rejimSoprovojdeniaSNR == SOPROVOJDENIE_AS ||            // в режиме тренажа АС по углам
                            apyInfoA.rejimPoiskaSNR == POISK_PEREKLYCHEN_V_ZAHVAT)      // и отработка ЦУ по углам в РС без ошибок
                    {
//                        p1.pos_cmd = apyInfoA.nextTocka[oficerNaved2Console.guidanceOfficerCmds.targetIndexCY].azimuth360;
//                        p2.pos_cmd = apyInfoA.nextTocka[oficerNaved2Console.guidanceOfficerCmds.targetIndexCY].elevationAngle;

                        drive_unit->unv->device_setpos_az(apyInfoA.nextTocka[oficerNaved2Console.guidanceOfficerCmds.targetIndexCY].azimuth360);
                        drive_unit->unv->device_setpos_elv(apyInfoA.nextTocka[oficerNaved2Console.guidanceOfficerCmds.targetIndexCY].elevationAngle);

                    }
                    else    // РС по углам ведется по штурвалам операторов
                    {                        
//                        if(apyInfoA.rejimSoprovojdeniaSNR == SOPROVOJDENIE_AS ||            // в режиме тренажа АС по углам
//                                apyInfoA.rejimPoiskaSNR == POISK_PEREKLYCHEN_V_ZAHVAT)      // и отработка ЦУ по углам в РС без ошибок


                        float dAzimSpeed = apyInfoA.handWheelFSpeed[0] * SIN45COS45 +  apyInfoA.handWheelFSpeed[1] * SIN45COS45;    //  штатный режим
                        float dElAngSpeed = apyInfoA.handWheelFSpeed[0] * SIN45COS45 - apyInfoA.handWheelFSpeed[1] * SIN45COS45;    //  штатный режим

                        //float dElAngSpeed = 0; float dAzimSpeed = 0.01 * shMemU2C.guidanceOfficerCmds.integerValue4Test;    //  тест с помощью слайдера

//                        p1.pos_cmd = p1.curr_pos + dAzimSpeed;    //  штатный режим
//                        p2.pos_cmd = p2.curr_pos + dElAngSpeed;   //  штатный режим

                        drive_unit->unv->device_setpos_az(drive_unit->unv->device_getpos_az() + dAzimSpeed);    //  штатный режим
                        drive_unit->unv->device_setpos_elv(drive_unit->unv->device_getpos_elv() + dElAngSpeed);   //  штатный режим


                        //qDebug()<<"servoDriveExecuter: РС по углам ведется по штурвалам операторов dAzimSpeed:"<<dAzimSpeed<<"p1.pos_cmd:"<<p1.pos_cmd<<"p1.curr_pos:"<<p1.curr_pos;
                    }

//                    srvDrvInfoA.directrisaAzimuth = p1.pos_cmd;
//                    srvDrvInfoA.currentDirectrisaElevationAngle = p2.pos_cmd;
                    srvDrvInfoA.directrisaAzimuth = drive_unit->unv->device_getpos_az();
                    srvDrvInfoA.currentDirectrisaElevationAngle = drive_unit->unv->device_getpos_elv();

                    float deltaMax = 0.3;
                    float deltaAz = apyInfoA.nextTocka[oficerNaved2Console.guidanceOfficerCmds.targetIndexCY].azimuth360 - srvDrvInfoA.currentAzimuth;
                    if(deltaAz > 180)
                        deltaAz -= 360.0;

                    float deltaElAngle = apyInfoA.nextTocka[oficerNaved2Console.guidanceOfficerCmds.targetIndexCY].elevationAngle - srvDrvInfoA.currentDirectrisaElevationAngle;

                    if(srvDrvInfoA.cyOtrabotano == 0 && fabs(deltaAz) <= deltaMax && fabs(deltaElAngle) <= deltaMax)
                    {
                        srvDrvInfoA.cyOtrabotano = 1;   //  2018-06-19
                        qDebug()<<"servoDriveExecuter: srvDrvInfoA.cyOtrabotano = 1";
                    }
                }
            }
        }
    }

//  2018-06-16    if(oficerNaved2Console.guidanceOfficerCmds.rejimPoiskaSNR != POISK_PEREKLYCHEN_V_ZAHVAT ||     // после отработки команды синхронизируем srvDrvInfo с guidanceOfficerCmds
//  2018-06-16      (oficerNaved2Console.guidanceOfficerCmds.rejimPoiskaSNR == POISK_PEREKLYCHEN_V_ZAHVAT &&     // кроме переключения в сопровождение, которое происходит автоматически
//  2018-06-16       apyInfoA.rejimPoiskaSNR != POISK_PEREKLYCHEN_V_SOPROVOJDENIE))
//  2018-06-16    {
//  2018-06-16        srvDrvInfoA.rejimPoiskaSNR = oficerNaved2Console.guidanceOfficerCmds.rejimPoiskaSNR;
//  2018-06-16    }

#if 0
    //  А З И М У Т
    p1.max_acc = 10;
    p1.max_vel = 10;

    //  У Г О Л   М Е С Т А
    p2.max_acc = 10;
    p2.max_vel = 10;

    if(p2.pos_cmd > 84.0)
        p2.pos_cmd = 84.0;

    if(p2.pos_cmd < -2.0)
        p2.pos_cmd = -2.0;

    drive.get_realtime();

    tp_update(&p2, drive.time_diff);
    tp_update(&p1, drive.time_diff);  //trajectory planer

    if(komKomA.servoDriveSimulation == false)   // Р А Б О Т А   С   Р Е А Л Ь Н Ы М   С Е Р В О П Р И В О Д О М
    {        
        drive.unv_drive_az_on();
        drive.unv_drive_elv_on();
        drive.unv_drive_az_en();
        drive.unv_drive_elv_en();

    //   p1.curr_pos = drive.unv_getpos_az();
    //   p2.curr_pos = drive.unv_getpos_elv();

       drive.unv_setpos_az (p1.curr_pos);
       drive.unv_setpos_elv(p2.curr_pos);

       drive.slot_target_motion();
    }
#endif

    if(drive_unit->unv->device_getpos_elv()/*p2.pos_cmd>84*/ > 72.0)
        drive_unit->unv->device_setpos_elv(72.0)/*p2.pos_cmd = 84.0*/;

    if(drive_unit->unv->device_getpos_elv()/*p2.pos_cmd*/ < -2.0)
        drive_unit->unv->device_setpos_elv(/*p2.pos_cmd =*/ -2.0);



    //programmStartTime_2.elapsed();//
    //float deltaTime_sec = 0.001 * programmStartTime_2.restart();// programmStartTime.currentTime();
    double secSinceMidnight = 0.001 * programmStartTime.currentTime().msecsSinceStartOfDay();//.restart();
    double deltaT = secSinceMidnight - srvDrvInfoA.currentAzimuthLastMeasurementTime;
    srvDrvInfoA.currentAzimuthLastMeasurementTime = secSinceMidnight;//0.001 * programmStartTime_2.restart(); // deltaTime_sec;//0.001 * programmStartTime_2.msecsSinceStartOfDay();

    //qDebug()<<"servoDriveExecuter: srvDrvInfoA.currentAzimuthLastMeasurementTime:"<<srvDrvInfoA.currentAzimuthLastMeasurementTime;

    if(deltaT > 0)
        //srvDrvInfoA.currentAzimuthScanSpeed = (p1.curr_pos - srvDrvInfoA.currentAzimuth) / deltaT;
        srvDrvInfoA.currentAzimuthScanSpeed = (drive_unit->unv->device_getpos_az() - srvDrvInfoA.currentAzimuth) / deltaT;

//    qDebug()<<"deltaT:"<<deltaT<<"secSinceMidnight:"<<secSinceMidnight<<"srvDrvInfoA.currentAzimuth:"<<srvDrvInfoA.currentAzimuth<<"p1.curr_pos:"<<p1.curr_pos
//           <<"srvDrvInfoA.currentAzimuthLastMeasurementTime:"<<srvDrvInfoA.currentAzimuthLastMeasurementTime<<"srvDrvInfoA.currentAzimuthScanSpeed:"<<srvDrvInfoA.currentAzimuthScanSpeed;


    //float secsSinceStOfDay = programmStartTime_2.hour() * 3600 + programmStartTime_2.minute() * 60 + programmStartTime_2.second() + 0.001 * programmStartTime_2.msec();

    //qDebug()<<"programmStartTime:"<<programmStartTime_2<<srvDrvInfoA.currentAzimuthLastMeasurementTime<<"secsSinceStOfDay:"<<secsSinceStOfDay<<deltaTime_sec;
    //qDebug()<<"programmStartTime_2.msec():"<<programmStartTime_2.minute() * 60 + programmStartTime_2.second() + 0.001 * programmStartTime_2.msec();

    //qDebug()<<"p1.pos_cmd:"<<p1.pos_cmd<<"srvDrvInfoA.currentAzimuth:"<<srvDrvInfoA.currentAzimuth<<"deltaTime_sec:"<<deltaTime_sec;
    //qDebug()<<"srvDrvInfoA.currentAzimuthLastMeasurementTime:"<<srvDrvInfoA.currentAzimuthLastMeasurementTime;
    //float Vb = 0;
    //if(srvDrvInfoA.currentAzimuthLastMeasurementTime != 0)
      //  Vb = (p1.curr_pos - srvDrvInfoA.currentAzimuth) / srvDrvInfoA.currentAzimuthLastMeasurementTime;

    //qDebug()<<"p1.curr_pos:"<<p1.curr_pos<<"srvDrvInfoA.currentAzimuth:"<<srvDrvInfoA.currentAzimuth<<"srvDrvInfoA.currentAzimuthLastMeasurementTime:"<<srvDrvInfoA.currentAzimuthLastMeasurementTime<<"Vb:"<<Vb;

    srvDrvInfoA.currentAzimuth = drive_unit->unv->device_getpos_az();//p1.curr_pos;

    //  2018_07_03  srvDrvInfoA.currentDirectrisaElevationAngle = p2.curr_pos;
    srvDrvInfoA.currentDirectrisaElevationAngle = drive_unit->unv->device_getpos_elv();

    int popravka_na_oboroti = srvDrvInfoA.currentAzimuth / 360.0;
    srvDrvInfoA.currentAzimuth -= 360.0 * popravka_na_oboroti;

    if(srvDrvInfoA.currentAzimuth < 0)
        srvDrvInfoA.currentAzimuth += 360.0;

    if(srvDrvInfoA.currentAzimuth >= 360.0)
        srvDrvInfoA.currentAzimuth -= 360.0;

    //if(settings->servoDriveDiagnostic == 12345)
      //  qDebug()<<"servoDriveExecuter settings->servoDriveDiagnostic:"<<settings->servoDriveDiagnostic;
}

