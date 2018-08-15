#include <QtCore>
#include <QDebug>
#include <QtCore>
#include <QtConcurrent/QtConcurrent>
#include "srcKram/Drive125.h"
#include <QDebug>



#include <QtCore>
#include <QtConcurrent/QtConcurrent>
#include "srcKram/Drive125.h"

extern int main_console(int argc,char *argv[]);
Drive125 *drive_unit;

int main(int argc,char *argv[])
{
 QCoreApplication a(argc,argv);

 drive_unit = new Drive125;

 /*QFuture<int> future =*/ QtConcurrent::run(main_console,argc,argv);
 return a.exec();
}
