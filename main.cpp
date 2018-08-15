#include <QtCore>
#include <QDebug>
#include <QtCore>
#include <QtConcurrent/QtConcurrent>
#include "srcKram/Drive125.h"
#include <QDebug>

#if 0
extern int main_console(int argc, char *argv[]);

Drive125 *drive;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    drive = new Drive125;
    //    // enable unv azimuth
//    drive->unv->device_az_en();
//    drive->unv->device_az_on();

    //    // enable unv elevation
//    drive->unv->device_elv_en();
//    drive->unv->device_elv_on();

    //qDebug()<<"drive->unv->device_getpos_az():"<<drive->unv->device_getpos_az();

    QtConcurrent::run(main_console, argc, argv);
    qDebug()<<"main_console - fork";
    return a.exec();
}

#endif

#include <QtCore>
#include <QtConcurrent/QtConcurrent>
#include "srcKram/Drive125.h"

extern int main_console(int argc,char *argv[]);
Drive125 *drive_unit;

int main(int argc,char *argv[])
{
 QCoreApplication a(argc,argv);

 /*Drive125*/ drive_unit = new Drive125;

 //    // enable unv azimuth
 drive_unit->unv->device_az_en();
 drive_unit->unv->device_az_on();

 //    // enable unv elevation
 drive_unit->unv->device_elv_en();
 drive_unit->unv->device_elv_on();

 qDebug()<<"drive->unv->device_getpos_az():"<<drive_unit->unv->device_getpos_az();



 QFuture<int> future = QtConcurrent::run(main_console,argc,argv);



 return a.exec();
}
