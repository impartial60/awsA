//#include <QPixmap>
#include <QImage>
#include <QPainter>

#include "externA.h"

int detectCntr = 1;

int otstyp_slevaB = 50;
int widthB = 500;
int otstyp_slevaE = otstyp_slevaB + widthB + 50;
int otstyp_sverhyRB = 20;
int otstyp_sverhyEB = 310;
int otstyp_sverhyOsc = 600;

MDO1_Plot primaryPlots[MAX_MDO1_PRIMARY_PLOTS_COUNT]; // плоты за один азимутальный скан
MDO1_Plot_SCR_Coord primaryPlotScrCrd[2][64];
MDO1_TrgtPlot_Coord grayTrgtPlotCoord[2];       // истинные плоты  истинных целей
MDO1_TrgtPlot_Coord grayTrgtTrackCoord[2][64];  // истинные трассы истинных целей
int scanCount = 0;
int plotsCount = 0;

void azimuthPackagePainter(QString commonPartOfName)
{
    const float factorR = 2.77;

    QImage *pckgImg = new QImage(900, 880, QImage::Format_RGB32);//new QImage(1280, 540, QImage::Format_RGB32);//QImage *pckgImg = new QImage(1020, 286, QImage::Format_RGB32);
    QPainter *painter = new QPainter(pckgImg);

    painter->fillRect(QRect(0, 0, pckgImg->width(), pckgImg->height()), QBrush(Qt::black, Qt::SolidPattern));

    painter->setPen(QColor::fromRgba(qRgba(32, 32, 32, 255)));
    //painter->drawRect(otstyp_slevaOsc + j * (256 + 28) - 5, otstyp_sverhyOsc + u * (256 + 8) - 4, 256 + 8, 256 + 4);
    painter->drawRect(otstyp_slevaB - 1, otstyp_sverhyRB - 1, widthB + 2, 256 + 2); // R-β    левый верх
    painter->drawRect(otstyp_slevaB - 1, otstyp_sverhyEB - 1, widthB + 2, 256 + 2); // ε-β    левый средний
    painter->drawRect(otstyp_slevaB - 1, otstyp_sverhyOsc - 1, widthB + 2, 256 + 2);// Osc    левый низ
    //painter->drawRect(otstyp_slevaE - 1, otstyp_sverhyRB - 1,  256 + 2, 256 + 2); // R-ε    правый верх
    painter->drawRect(otstyp_slevaE - 1, otstyp_sverhyEB - 1,  256 + 2, 256 + 2);   // R-ε    правый низ

    float xFactor = 1.0 / 10.0 * widthB;//pckgImg->width();

    float startEdge = 0;

    for(int i = 0; i < detectCntr / 2; i++)
    {
        if(mdo1AllDirections[i].azimuth360 != 0.0)
        {
            startEdge = mdo1AllDirections[i].azimuth360;
            break;
        }
    }

    float finEdge = 0;

    for(int i = detectCntr - 1; i >= detectCntr / 2; i--)
    {
        if(mdo1AllDirections[i].azimuth360 != 0.0)
        {
            finEdge = mdo1AllDirections[i].azimuth360;
            break;
        }
    }

    if(startEdge < finEdge)  // если сканирование слева направо
    {
        primaryPlotScrCrd[0][scanCount].left2Right = true;
    }
    else
    {
        startEdge = finEdge;
        primaryPlotScrCrd[0][scanCount].left2Right = false;
    }

    for(int u = 0; u < 2; u++)  // добавление в трассы новых (на момент прохода максимума ДН по азимуту) координат целей (истинных целей всего две)
    {
        grayTrgtTrackCoord[u][scanCount].az360 = grayTrgtPlotCoord[u].az360;
        grayTrgtTrackCoord[u][scanCount].enrg = grayTrgtPlotCoord[u].enrg;
        grayTrgtTrackCoord[u][scanCount].elevAngleF = grayTrgtPlotCoord[u].elevAngleF;
        grayTrgtTrackCoord[u][scanCount].range_km = grayTrgtPlotCoord[u].range_km;
        grayTrgtPlotCoord[u].dAz = 1001.0;  // признак того, что данные использованы, структура готова к записи новых
    }

    //  О Т Р И С О В К А   "И С Т И Н Н Ы Х"   Ц Е Л Е Й
    for(int i = 1; i <= scanCount; i++)
    {
        int brtn = 128 + 3 * (i - scanCount);
        if(brtn < 32)
            brtn = 32;
        // if(scanCount == 20)  qDebug()<<"azimuthPackagePainter: i:"<<i<<brtn;

        QBrush trgtBrush(QColor::fromRgb(qRgb(brtn, brtn, brtn)), Qt::SolidPattern);

        int dS = 1;
        if(i == scanCount)
        {
            dS = 3;
            trgtBrush.setColor(QColor::fromRgb(qRgb(160, 160, 160)));
        }

        for(int u = 0; u < 2; u++)  //  истинных целей всего две
        {
            int trgtScrX = otstyp_slevaB + widthB / 2 + xFactor * convert360angle2PlusMinus180(grayTrgtTrackCoord[u][i].az360 - srvDrvInfoA.directrisaAzimuth);

            //int trgtScrY = otstyp_sverhyRB + 256 - (uint16_t)(grayTrgtTrackCoord[u][i].range / 256);
            int trgtScrY = otstyp_sverhyRB + 256 - grayTrgtTrackCoord[u][i].range_km * factorR;

            //if(u == 0)
              //  qDebug()<<"azimuthPackagePainter: grayTrgtTrackCoord[u][i].range * factorR"<<grayTrgtTrackCoord[u][i].range * factorR<<"trgtScrY:"<<trgtScrY;

            painter->fillRect(QRect(trgtScrX - dS, trgtScrY - dS, dS * 2, dS * 2),  trgtBrush);  // цели серые

            trgtScrY = otstyp_sverhyOsc + 256 - grayTrgtTrackCoord[u][i].enrg / 256;
            painter->fillRect(QRect(trgtScrX - dS, trgtScrY - dS, dS * 2, dS * 2),  trgtBrush);  // цели серые  Osc

            //trgtScrY = otstyp_sverhyEB + 256 - grayTrgtTrackCoord[u][i].elevAngleF;
            trgtScrY = otstyp_sverhyEB + 128 - 256.0 / 10.0 * (grayTrgtTrackCoord[u][i].elevAngleF - srvDrvInfoA.currentDirectrisaElevationAngle);

            painter->fillRect(QRect(trgtScrX - dS, trgtScrY - dS, dS * 2, dS * 2),  trgtBrush);  // цели серые

            //trgtScrX = otstyp_slevaE + (uint16_t)(grayTrgtTrackCoord[u][i].range / 256);
            trgtScrX = otstyp_slevaE + (uint16_t)(grayTrgtTrackCoord[u][i].range_km * factorR);

            painter->fillRect(QRect(trgtScrX - dS, trgtScrY - dS, dS * 2, dS * 2),  trgtBrush);  // цели серые


            //painter->drawPoint(trgtScrX, trgtScrY);
            //trgtPlotCoord[u][i].dAz = 1001.0;  // признак того, что данные использованы, структура готова к записи новых
        }

    }

    for(int u = 0; u < plotsCount; u++) // последний обнаруженный плот
    {
        primaryPlotScrCrd[u][scanCount].scrB = otstyp_slevaB + xFactor * convert360angle2PlusMinus180(primaryPlots[u].azimuth360 - startEdge);
        //primaryPlotScrCrd[u][scanCount].scrRB = otstyp_sverhyRB + 256 - primaryPlots[u].range / 256;
        primaryPlotScrCrd[u][scanCount].scrRB = otstyp_sverhyRB + 256 - primaryPlots[u].range_km * factorR;
        primaryPlotScrCrd[u][scanCount].scrE = otstyp_sverhyEB + 256 - primaryPlots[u].scanerYV10ElevAngle;
    }

    //  О Т Р И С О В К А   О Б Н А Р У Ж Е Н Н Ы Х   П Л О Т О В

    for(int i = 1; i <= scanCount; i++)
    {
        int brtn = 192 + 3 * (i - scanCount);

        if(primaryPlotScrCrd[0][i].left2Right)
            painter->setPen(QColor::fromRgba(qRgba(brtn, brtn, 0, 128)));    // плоты желтые
        else
            painter->setPen(QColor::fromRgba(qRgba(brtn, 0, brtn, 128)));    // плоты сиреневые

        int dS = 3;
        if(i == scanCount)
        {
            dS = 5;
            if(primaryPlotScrCrd[0][i].left2Right)//if(fromLeft2Right)
                painter->setPen(QColor::fromRgba(qRgba(255, 255, 0, 128)));    // плоты желтые
            else
                painter->setPen(QColor::fromRgba(qRgba(255, 0, 255, 128)));    // плоты сиреневые

        }

        for(int u = 0; u < plotsCount; u++)
        {
            painter->drawRect(primaryPlotScrCrd[u][i].scrB - dS, primaryPlotScrCrd[u][i].scrRB - dS, dS * 2, dS * 2);
            painter->drawRect(primaryPlotScrCrd[u][i].scrB - dS, primaryPlotScrCrd[u][i].scrE  - dS, dS * 2, dS * 2);
        }
    }

    if(scanCount < 64)
        scanCount++;

    //  О Т Р И С О В К А   Д А Н Н Ы Х   М Д О - 1
    for(int j = 0; j < detectCntr; j++)
    {
        int scrX = otstyp_slevaB + xFactor * convert360angle2PlusMinus180(mdo1AllDirections[j].azimuth360 - startEdge);

        if(j == primaryPlots[0].dCntr)
            painter->setPen(QColor::fromRgba(qRgba(0, 160, 255, 80)));
        else
            if(j == primaryPlots[1].dCntr)
                painter->setPen(QColor::fromRgba(qRgba(160, 0, 255, 80)));
            else
                painter->setPen(QColor::fromRgba(qRgba(0, 0, 255, 80)));
        painter->drawLine(scrX, otstyp_sverhyRB, scrX, otstyp_sverhyRB + 255);  // вертикальные линии пролонгированных азимутов на развёртке R
        painter->drawLine(scrX, otstyp_sverhyEB, scrX, otstyp_sverhyEB + 255);  // вертикальные линии пролонгированных азимутов на развёртке ε
        painter->drawLine(scrX, otstyp_sverhyOsc, scrX, otstyp_sverhyOsc + 255);  // вертикальные линии пролонгированных азимутов на развёртке ε
//if(false)
        for(int i = 0; i < mdo1AllDirections[j].nextEScanTargetCount; i++)
        {
            uint8_t srednEn256 = (uint8_t)(255.0 * mdo1AllDirections[j].joinedTrgtData[i].sredEnF);

            //painter->setPen(QColor::fromRgb(qRgb(0, srednEn / 259, 0)));

            //  Д А Л Ь Н О С Т Ь   от азимута
            //int scrY = otstyp_sverhyRB + 256 - mdo1AllDirections[j].joinedTrgtData[i].range / 256;
            int scrY = otstyp_sverhyRB + 256 - mdo1AllDirections[j].joinedTrgtData[i].range_km * factorR;
            painter->fillRect(scrX-1, scrY-1, 3, 3, QBrush(QColor::fromRgb(qRgb(0, srednEn256, 0)), Qt::SolidPattern));

            //  Д А Л Ь Н О С Т Ь   от угла места
            //int scrE = otstyp_slevaE + 256 - slideSignalValues[j].targetData[i].scanerYV10ElevAngle;// + eFactor * slideSignalValues[j].targetData[i].scanerYV10ElevAngle;
            scrY = otstyp_sverhyEB + 256 - mdo1AllDirections[j].joinedTrgtData[i].scanerYV10ElevAngle;
            //int scrR = otstyp_slevaE + mdo1AllDirections[j].joinedTrgtData[i].range / 256;
            int scrR = otstyp_slevaE + mdo1AllDirections[j].joinedTrgtData[i].range_km * factorR;

            for(int uy = scrY - 1; uy <= scrY + 1; uy++)
            {
                if(uy >= 0 && uy < pckgImg->height())
                {
                    for(int ux = scrR - 1; ux <= scrR + 1; ux++)
                    {
                        if(ux >= 0 && ux < pckgImg->width())
                        {
                            uint8_t pixGreen = qGreen(pckgImg->pixel(ux, uy));
                            if(srednEn256 > pixGreen)
                            {
                                painter->setPen(QColor::fromRgb(qRgb(0, srednEn256, 0)));
                                painter->drawPoint(ux, uy);
                            }
                        }
                    }
                }
            }
            //painter->fillRect(scrE-1, scrY-1, 3, 3, QBrush(QColor::fromRgb(qRgba(0, srednEn, 0, 80)), Qt::SolidPattern));

            //  У Г О Л   М Е С Т А  от азимута
            painter->fillRect(scrX-1, scrY-1, 3, 3, QBrush(QColor::fromRgb(qRgb(0, srednEn256, 0)), Qt::SolidPattern));//painter->drawPoint(scrX, scrY);

            //  Э Н Е Р Г И Я   от азимута
            //scrY = otstyp_sverhyOsc + 256 - mdo1AllDirections[j].targetData[i].energy[1] / 256;
            scrY = otstyp_sverhyOsc + 256 - mdo1AllDirections[j].joinedTrgtData[i].sredEnF * 256;

            painter->fillRect(scrX-1, scrY-1, 3, 3, QBrush(QColor::fromRgb(qRgb(0, 255, 0)), Qt::SolidPattern));//painter->drawPoint(scrX, scrY);

        }
    }

    painter->setPen(QColor(255,64,64));


//    QFont fnt("Ubuntu");
//    fnt.setBold(true);
//    fnt.setPointSize(10);
//    painter->setFont(fnt);
//    QString s = QString("b");
//    painter->drawText(50, 50, s);

//    QFont *fnt = new QFont("Arial");
//    fnt->setBold(true);
//    fnt->setPointSize(20);
//    painter->setFont(*fnt);
//    painter->drawText(250, pckgImg->height() - 20, "b");
//    delete fnt;

//    QFont fnt("Arial");
//    fnt.setBold(true);
//    fnt.setPointSize(20);
//    painter->setFont(fnt);
//    painter->drawText(250, pckgImg->height() - 20, "b");
//    painter->drawText(250, pckgImg->height() - 20, QString::fromUtf8("β"));


    //p->drawLine(0,0,100,100);

    QImage *rbeImg = new QImage("rbe.png", NULL);
    painter->drawImage(3, 128, *rbeImg, 2, 0, 16, -1, Qt::AutoColor);   // R
    painter->drawImage(267, otstyp_sverhyEB - 28, *rbeImg, 24, 6, 20, 20, Qt::AutoColor);  // β
    painter->drawImage(267, otstyp_sverhyOsc - 28, *rbeImg, 24, 6, 20, 20, Qt::AutoColor);  // β

    painter->drawImage(4, otstyp_sverhyEB + 120, *rbeImg, 46, 0, 14, -1, Qt::AutoColor);   // ε
    painter->drawImage(267, pckgImg->height() - 24, *rbeImg, 24, 4, 20, -1, Qt::AutoColor);  // β

//    painter->drawImage(otstyp_slevaE - 18, 128, *rbeImg, 2, 0, 14, -1, Qt::AutoColor);   // R
//    painter->drawImage(otstyp_slevaE + 125, otstyp_sverhyRB + 258, *rbeImg, 48, 2, -1, -1, Qt::AutoColor);   // ε

    painter->drawImage(otstyp_slevaE - 18,  otstyp_sverhyEB + 120, *rbeImg, 48, 2, -1, -1, Qt::AutoColor);   // ε
    painter->drawImage(otstyp_slevaE + 125, otstyp_sverhyOsc - 28, *rbeImg, 2, 0, 14, -1, Qt::AutoColor);   // R

    delete rbeImg;

    QString pctFileName = commonPartOfName + ".png";
    pckgImg->save(pctFileName,"PNG");
    delete painter;
    delete pckgImg;

//    QPainter painter(&pckgBMP);
//    painter.setBrush(QBrush(Qt::green));
//    painter.drawRect(0, 0, pckgBMP.width(), pckgBMP.height());

//    QFile file(pctFileName);
//    file.open(QIODevice::WriteOnly);
//    pckgBMP.save(&file, "PNG");
}


void azimuthPackageWriter()
{
    QString echoDirName = settings->programAbsolutePath + "/PackageData";

    QDir echoDir(echoDirName);
    if (!echoDir.exists())
    {
        echoDir.mkpath(".");
    }

    QDateTime now =  QDateTime::currentDateTime();
    QString dtStr = echoDirName + "/package_" + now.toString("yyyy_MM_dd__") + now.toString("hh_mm_ss_zzz");

    azimuthPackagePainter(dtStr);

    QString echoFileName = dtStr + ".txt";

    //qDebug()<<"azimuthPackageWriter: echoFileName:"<<echoFileName;

    FILE* fp;
    char file_name[100];
    strcpy(file_name,  echoFileName.toStdString().c_str());

    fp = fopen(file_name, "wt");//"a+t" );

    if(fp != NULL )
    {
        fprintf(fp, "detectCntr: %02d startEdge β: %06.2f°\n", detectCntr, mdo1AllDirections[detectCntr - 1].azimuth360);

        for(int u = 0; u < plotsCount; u++)
        {
            fprintf(fp, "плот  %02d β: %06.2f°  R: %06.3fkm  ε: %06.2f° %04.0fkHz  W: %05.2f  dCntr: %03d  indexOsi: %03d\n",
                    u, primaryPlots[u].azimuth360, primaryPlots[u].range_km, primaryPlots[u].elevAngleF, primaryPlots[u].dopler_Hz, primaryPlots[u].packageWeight, primaryPlots[u].dCntr, primaryPlots[u].indexOsi);
        }

        fprintf(fp, "                                    код угла, код дальности, код энергии, энергия азимутального пакета");

        for(int j = 0; j < detectCntr; j++)
        {
            float deltaT = 0;
            float azScanSpeed = 0;

            //slideSignalValues[detectCntr].azimuth360 = srvDrvInfoA.currentAzimuth;
            if(j > 0)
            {
                deltaT = mdo1AllDirections[j].currentAzimuthLastMeasurementTime - mdo1AllDirections[j - 1].currentAzimuthLastMeasurementTime;
                if(deltaT != 0)
                    azScanSpeed = (mdo1AllDirections[j].azimuth360 - mdo1AllDirections[j - 1].azimuth360) / deltaT;
            }

            fprintf(fp, "\nβ%03d:%06.2f° ΔT:%05.3fc Vβ:%+06.2f°/c  %d", j, mdo1AllDirections[j].azimuth360, deltaT, azScanSpeed, mdo1AllDirections[j].nextEScanTargetCount);

            for(int i = 0; i < 1;//mdo1AllDirections[j].nextEScanTargetCount;
                i++)
            {
                //fwrite(&slideSignalValues[0].ARY, sizeof(uint8_t), 1, fp);
                //fwrite(&i, sizeof(uint8_t), 1, fp);
                //fprintf(fp, "\n<<<<<<<<<<<<<<<   Col: %d   >>>>>>>>>>>>>>>>>", i);

//                fprintf(fp, "  ε%d:%03d %05d %05d %05d", i, slideSignalValues[j].targetData[i].scanerYV10ElevAngle,
//                slideSignalValues[j].targetData[i].energy[0], slideSignalValues[j].targetData[i].energy[1], slideSignalValues[j].targetData[i].energy[2]);

                //if(slideSignalValues[j].targetData[i].energy[0] > 0 || slideSignalValues[j].targetData[i].energy[1] > 0 || slideSignalValues[j].targetData[i].energy[2] > 0)

                //uint16_t srednEn = (slideSignalValues[j].targetData[i].energy[0] + slideSignalValues[j].targetData[i].energy[1] + slideSignalValues[j].targetData[i].energy[2]) / 3;

//                fprintf(fp, "  %d:%03d %05d %05.2f %05.2f", i, mdo1AllDirections[j].targetData[i].scanerYV10ElevAngle,
//                                                        mdo1AllDirections[j].targetData[i].range, mdo1AllDirections[j].floatSredEn[i], mdo1AllDirections[j].packageWeight[i]);

                fprintf(fp, "  %d:%03d %05.2f %05.2f %05.2f", i,     //   ves: %05.2f %05.2f   az: %06.2f° %06.2f°  dAzim: %06.2f° %07.3f°", i,
                //код угла,                                             код дальности,                            код энергии,                         энергия азимутального пакета");
                mdo1AllDirections[j].joinedTrgtData[i].scanerYV10ElevAngle, mdo1AllDirections[j].joinedTrgtData[i].range_km, mdo1AllDirections[j].joinedTrgtData[i].sredEnF, mdo1AllDirections[j].packageWeight[i]);
                //mdo1AllDirections[j].ves1[i], mdo1AllDirections[j].ves2[i], mdo1AllDirections[j].az1[i], mdo1AllDirections[j].az2[i], 0.002 * mdo1AllDirections[j].targetData[i].energy[1] - 6.0,
                //   0.001 * mdo1AllDirections[j].targetData[i].energy[2] + 100.0);
            }
        }
        fclose( fp );
    }
}


bool matchedIndexesR[16];
bool matchedIndexesE[16];

void azimuthPakageDetector()
{
    //  2018_07_21  dopplerChannelIdentification();   //  складывание угломестных фрагментов из 3-х блоков МДО-1 в общий список

    memset(&primaryPlots, 0, sizeof(MDO1_Plot) * MAX_MDO1_PRIMARY_PLOTS_COUNT);

    const int packSampleMaxCount8 = 8;
    const int packSampleOtstypOtKraj = packSampleMaxCount8 / 2;

    float bygorValue[packSampleMaxCount8];
    float vesChastiBygra[packSampleMaxCount8];

    // В Ы Ч И С Л Е Н И Е   С У М М А Р Н Ы Х   Э Н Е Р Г И Й   А З И М У Т А Л Ь Н Ы Х   П А Ч Е К
    for(int j = packSampleOtstypOtKraj; j < detectCntr - packSampleOtstypOtKraj; j++)         //  по всем угломестным сканам
    {
        for(int i = 0; i < mdo1AllDirections[j].nextEScanTargetCount; i++)      //  по всем угломестным пачкам на очередном угломестном скане
        {
            memset(&bygorValue,     0, sizeof(float) * packSampleMaxCount8);
            memset(&vesChastiBygra, 0, sizeof(float) * packSampleMaxCount8);

            int indexBygra = 0;

            for(int jj = j - packSampleOtstypOtKraj; jj < j + packSampleOtstypOtKraj; jj++) // сканирование пачки
            {
                int indexDomika = abs(jj - j);

//                if(indexDomika == 0) { mdo1AllDirections[j].correlResult[i] += mdo1AllDirections[j].floatSredEn[i] * domik[indexDomika];   continue;   }

                for(int u = 0; u < mdo1AllDirections[jj].nextEScanTargetCount; u++) // составление списка "пригодных" фрагментов на очередном угломестном скане из числа соседних
                {                    
                    if(abs(mdo1AllDirections[jj].joinedTrgtData[u].range - mdo1AllDirections[j].joinedTrgtData[i].range) <= 100 * indexDomika)
                    //if(abs(mdo1AllDirections[jj].joinedTrgtData[u].range_km - mdo1AllDirections[j].joinedTrgtData[i].range_km) <= CENA_MLADSHEGO_RAZRIADA_DALNOSTI * 100 * indexDomika)
                        matchedIndexesR[u] = true;
                    else
                        matchedIndexesR[u] = false;

                    if(abs(mdo1AllDirections[jj].joinedTrgtData[u].scanerYV10ElevAngle - mdo1AllDirections[j].joinedTrgtData[i].scanerYV10ElevAngle) <= indexDomika)
                    //if(abs(mdo1AllDirections[jj].joinedTrgtData[u].elevAngleF - mdo1AllDirections[j].joinedTrgtData[i].elevAngleF) <= 10.0 / 256 * indexDomika)
                        matchedIndexesE[u] = true;
                    else
                        matchedIndexesE[u] = false;
                }

//                if(jj == j)
//                    qDebug()<<"azPD j:"<<j<<"i:"<<i<<"kysByg:"<<kysokBygra[0]<<kysokBygra[1]<<kysokBygra[2]<<kysokBygra[3]<<kysokBygra[4]<<kysokBygra[5]<<kysokBygra[6];

                int bestMatchedIndex = -1;
                float maxEnergy = 0;

                for(int u = 0; u < mdo1AllDirections[jj].nextEScanTargetCount; u++) // поиск "лучшего" фрагмента из числа "пригодных" на очередном угломестном скане из числа соседних
                {
                    if(matchedIndexesR[u] && matchedIndexesE[u] && mdo1AllDirections[jj].joinedTrgtData[u].sredEnF > maxEnergy)
                    {
                        maxEnergy = mdo1AllDirections[jj].joinedTrgtData[u].sredEnF;
                        bestMatchedIndex = u;
                    }
                }

                if(bestMatchedIndex > - 1)  //  "лучший" фрагмент найден
                {                    
                    bygorValue[indexBygra] = mdo1AllDirections[jj].joinedTrgtData[bestMatchedIndex].sredEnF;//kysokBygra[indexBygra] += mdo1AllDirections[jj].floatSredEn[bestMatchedIndex];
                    //qDebug()<<"azimuthPakageDetector indexBygra:"<<indexBygra<<"kysokBygra[indexBygra]:"<<kysokBygra[indexBygra];
                }
                else
                {
                  bygorValue[indexBygra] = 0;
                }

                if(indexBygra > 0 && bygorValue[indexBygra] > 0 && bygorValue[indexBygra - 1] > 0)
                {
                    ///////////////////////////////kysokBygra[indexBygra] = kysokBygra[indexBygra - 1];
                    vesChastiBygra[indexBygra] = vesChastiBygra[indexBygra - 1] + (bygorValue[indexBygra] + bygorValue[indexBygra - 1]) / 2;
                }

                indexBygra++;
            }

            //qDebug()<<"azPD j:"<<j<<"i:"<<i<<"kysByg:"<<kysokBygra[0]<<kysokBygra[1]<<kysokBygra[2]<<kysokBygra[3]<<kysokBygra[4]<<kysokBygra[5]<<kysokBygra[6];

            mdo1AllDirections[j].packageWeight[i] = vesChastiBygra[packSampleMaxCount8 - 1];  //  вес всего "бугра"
            int indexOsi = -1;
            float vesPoloviniBygra = mdo1AllDirections[j].packageWeight[i] / 2;

            for(int u = 0; u < packSampleMaxCount8; u++) // поиск индекса бугра, следующего за "осью равновесия" азимутальной пачки
            {
                if(vesChastiBygra[u] > vesPoloviniBygra)
                {
                    indexOsi = u;
                    break;
                }
            }

//            if(j >= 22 && j <= 27)
//                qDebug()<<"azimuthPakageDetector j:"<<j<<"i:"<<i<<"bygorValue:"<<bygorValue[0]<<bygorValue[1]<<bygorValue[2]<<bygorValue[3]<<bygorValue[4]<<bygorValue[5]<<bygorValue[6]
//                       <<"   vesChastiBygra:"<<vesChastiBygra[0]<<vesChastiBygra[1]<<vesChastiBygra[2]<<vesChastiBygra[3]<<vesChastiBygra[4]<<vesChastiBygra[5]<<vesChastiBygra[6];

            if(indexOsi > 0 && vesChastiBygra[indexOsi] != vesChastiBygra[indexOsi - 1]) // ищем интерполированный азимут "оси равновесия" азимутальной пачки
            {
                int skvoznojJIndex = j - packSampleOtstypOtKraj + indexOsi;

                float factor = (mdo1AllDirections[skvoznojJIndex].azimuth360 - mdo1AllDirections[skvoznojJIndex - 1].azimuth360) / (vesChastiBygra[indexOsi] - vesChastiBygra[indexOsi - 1]);

                mdo1AllDirections[j].azOsi[i] = mdo1AllDirections[skvoznojJIndex - 1].azimuth360 + factor * (vesPoloviniBygra - vesChastiBygra[indexOsi - 1]);

                mdo1AllDirections[j].indOsi[i] = indexOsi;  // запоминается только для вывода в отладочный файл

                mdo1AllDirections[j].az1[i] = mdo1AllDirections[skvoznojJIndex - 1].azimuth360;
                mdo1AllDirections[j].az2[i] = mdo1AllDirections[skvoznojJIndex    ].azimuth360;

                mdo1AllDirections[j].ves1[i] = vesChastiBygra[indexOsi - 1];
                mdo1AllDirections[j].ves2[i] = vesChastiBygra[indexOsi    ];
            }
            ////////////else
                ////////////qDebug()<<"azimuthPakageDetector: vesChastiBygra[indexOsi] == vesChastiBygra[indexOsi - 1]: indexOsi"<<indexOsi<<"j:"<<j<<"i:"<<i;

            //mdo1AllDirections[j].packageWeight[i] /= cor2;
        }
    }


    //  О Б Н А Р У Ж Е Н И Е   П Л О Т О В
    plotsCount = 0;
    bool checked[MAX_DIRECT_COUNT][16];
    memset(&checked, 0, sizeof(checked));

    //  О Б Н А Р У Ж Е Н И Е   П Л О Т О В

    for(int u = 0; u < MAX_MDO1_PRIMARY_PLOTS_COUNT; u++)      // обнаруживаем 5 самых "увесистых" азимутальных пачек из 960 возможных
    {
        float maxPackageWeight = 0;
        int matchedJ = -1;
        int matchedI = -1;

        //  П О И С К   О Ч Е Р Е Д Н О Й   А З И М У Т А Л Ь Н О Й   П А Ч К И   С   М А К С И М А Л Ь Н О Й   Э Н Е Р Г И Е Й   И З   Ч И С Л А   "О С Т А В Ш И Х С Я"

        for(int j = packSampleOtstypOtKraj; j < detectCntr - packSampleOtstypOtKraj; j++)         //  по всем угломестным сканам
        {
            for(int i = 0; i < mdo1AllDirections[j].nextEScanTargetCount; i++)      //  по всем угломестным пачкам на очередном угломестном скане
            {
                if(checked[j][i] == false && mdo1AllDirections[j].packageWeight[i] > maxPackageWeight)
                {
                    maxPackageWeight = mdo1AllDirections[j].packageWeight[i];
                    matchedJ = j;
                    matchedI = i;
                }
            }
        }

        if(matchedJ > -1 && matchedI > -1)  //  П А Ч К А   Н А Й Д Е Н А
        {
            checked[matchedJ][matchedI] = true;
            primaryPlots[plotsCount].dCntr = matchedJ;
            primaryPlots[plotsCount].azimuth360 = mdo1AllDirections[matchedJ].azOsi[matchedI];
            //primaryPlots[plotsCount].range = mdo1AllDirections[matchedJ].joinedTrgtData[matchedI].range;
            primaryPlots[plotsCount].range_km = mdo1AllDirections[matchedJ].joinedTrgtData[matchedI].range_km;
            primaryPlots[plotsCount].dopler_Hz = mdo1AllDirections[matchedJ].joinedTrgtData[matchedI].dopler;
            primaryPlots[plotsCount].scanerYV10ElevAngle = mdo1AllDirections[matchedJ].joinedTrgtData[matchedI].scanerYV10ElevAngle;
            primaryPlots[plotsCount].elevAngleF = mdo1AllDirections[matchedJ].joinedTrgtData[matchedI].elevAngleF;
            primaryPlots[plotsCount].packageWeight = mdo1AllDirections[matchedJ].packageWeight[matchedI];
            primaryPlots[plotsCount].indexOsi = mdo1AllDirections[matchedJ].indOsi[matchedI];
            //plots[plotsCount].left2Right = fromLeft2Right;

            //  И С К Л Ю Ч Е Н И Е   А З И М У Т А Л Ь Н О Й   П А Ч К И   О Б Н А Р У Ж Е Н Н О Г О   П Л О Т А   И   Е Ё   Б О К О В Ы Х   Л Е П Е С Т К О В
            //  И З   Д А Л Ь Н Е Й Ш Е Г О   Р А С С М О Т Р Е Н И Я
            for(int j = packSampleOtstypOtKraj; j < detectCntr - packSampleOtstypOtKraj; j++)         //  по всем угломестным сканам
            {
                for(int i = 0; i < mdo1AllDirections[j].nextEScanTargetCount; i++)      //  по всем угломестным пачкам на очередном угломестном скане
                {

                    //if(abs(primaryPlots[plotsCount].range - mdo1AllDirections[j].joinedTrgtData[i].range) <= 100 * abs(j - matchedJ) &&
                    if(abs(primaryPlots[plotsCount].range_km - mdo1AllDirections[j].joinedTrgtData[i].range_km) <= 0.04 * abs(j - matchedJ) &&
                       abs(primaryPlots[plotsCount].scanerYV10ElevAngle - mdo1AllDirections[j].joinedTrgtData[i].scanerYV10ElevAngle) <= abs(j - matchedJ))
                            checked[j][i] = true;
                }
            }
            plotsCount ++;
        }
    }

    // подготовка к выдаче на главный сервер обнаруженных плотов
    memcpy(&unv2apy.primaryPlots, &primaryPlots, sizeof(MDO1_Plot) * MAX_MDO1_PRIMARY_PLOTS_COUNT);       //  2018_07_27

    if(oficerNaved2Console.guidanceOfficerCmds.writeDataContinues)
        azimuthPackageWriter();

    // проход по всем 48x200 угломестным пачкам

//    for(int i = 0; i < dCntr / 10; i++)
//        qDebug()<<"azimuthPakageDetector i:"<<i * 10<<"ARY:"
//                <<slideSignalValues[i * 10 + 0].ARY<<slideSignalValues[i * 10 + 1].ARY<<slideSignalValues[i * 10 + 2].ARY<<slideSignalValues[i * 10 + 3].ARY<<slideSignalValues[i * 10 + 4].ARY
//                <<slideSignalValues[i * 10 + 5].ARY<<slideSignalValues[i * 10 + 6].ARY<<slideSignalValues[i * 10 + 7].ARY<<slideSignalValues[i * 10 + 8].ARY<<slideSignalValues[i * 10 + 9].ARY;

    //  обнуляем mdo1AllDirections для дальнейшего использования
    memset(mdo1AllDirections, 0, MAX_DIRECT_COUNT * sizeof(MDO1_nextDirection));

    detectCntr = 1;
}

