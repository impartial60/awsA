#ifndef SETTINGSA_H
#define SETTINGSA_H

#include <QCoreApplication>
#include <QSettings>
#include <QString>
#include <QDir>
#include <QDebug>
//#include <arpa/inet.h>
#include <pwd.h>

#include <zrkServiceInfo.h>

class SettingsConsoleA : QObject
{
    Q_OBJECT
    public:

    ushort port_mainServer;

    int servoDriveDiagnostic;
    QString programAbsolutePath;

    SettingsConsoleA()
    {
        QDir dir = QDir::currentPath();//QCoreApplication::applicationDirPath());
        programAbsolutePath = dir.absolutePath();
        /////////////////////////dir.cdUp();        
        //qDebug()<<"SettingsConsoleG dir.absolutePath():"<<dir.absolutePath();

        QSettings *qSetting = new QSettings(dir.absolutePath() + "/awsA125FactorySettings.conf", QSettings::NativeFormat);

        port_mainServer = qSetting->value("IPAddressesAndPorts/port_mainServer", 30024).toUInt();

        // У П Р А В Л Е Н И Е   О Т Л А Д К О Й
        servoDriveDiagnostic = qSetting->value("Diagnostics/servoDriveDiagnostic", 0).toUInt();

        //qDebug()<<"SettingsConsoleA servoDriveDiagnostic:"<<servoDriveDiagnostic;
    }
};

#endif // SETTINGSA_H
