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

void * ProcessThreadMainServer( void * temp_pt )
{
    struct sockaddr_in si_me, si_other;

    int s, slen = sizeof(si_other);// , recv_len;
    socklen_t clilen = sizeof(si_other);

    bool connectStatus = true;

    //create a UDP socket
    if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        qDebug()<<"ProcessThreadMainServer: socket == -1";
        connectStatus = false;//die("socket");
    }
    fcntl(s, F_SETFL, O_NONBLOCK);

    // zero out the structure
    memset((char *) &si_me, 0, sizeof(si_me));

    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(settings->port_mainServer);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);

    //bind socket to port
    if(connectStatus &&  bind(s , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1)
    {
        connectStatus = false;//die("bind");

        qDebug()<<"ProcessThreadMainServer: bind == -1";
    }

    unv2apy.cntrMCA2D = 2000;

    qDebug()<<"ProcessThreadMainServer UDP Server port:"<<settings->port_mainServer<<"connectStatus:"<<connectStatus;

    //int newDispatcherCounter = -1;

    int cntrUSM_test = 0;

    while(connectStatus)
    {
        usleep(35 * 1000);
        ////////////printf("Waiting for data...");
        ////////////fflush(stdout);

        ssize_t totalReadBytes = 0;
        ssize_t nextReadBytes;
        int readCirclCounter = 0;

        while (totalReadBytes < abs(sizeof(MainServer2ServerUNV)) && readCirclCounter++ < 10)
        {
            nextReadBytes = recvfrom(s, &apy2unv + totalReadBytes, sizeof(MainServer2ServerUNV) - totalReadBytes, 0, (struct sockaddr *) &si_other, &clilen);

            if(nextReadBytes < 0)
                nextReadBytes = 0;

            totalReadBytes += nextReadBytes;
        }

        if(totalReadBytes == abs(sizeof(MainServer2ServerUNV)))
        {            
            memcpy(&oficerNaved2Console, &apy2unv.komandi_NAVED, sizeof(KOMANDI_OFICERA_NAVED)); //  2018_06_14
            memcpy(&komKomA, &apy2unv.komKom, sizeof(KOMANDI_KOMANDIRA));    //  2018_06_14
            memcpy(&apyInfoA, &apy2unv.apyInfo, sizeof(APY_INFO));           //  2018_06_16

            ////////////////////////qDebug()<<"A: ProcessThreadMainServer:"<<(oficerNaved2Console.usmCmds.sync[7] & 0x08)<<"scannerUV10IsOn:"<<oficerNaved2Console.guidanceOfficerCmds.scannerUV10IsOn
                                                                                                 ///////////<<"scannerUV11IsOn:"<<oficerNaved2Console.guidanceOfficerCmds.scannerUV11IsOn;


//            qDebug()<<"A: ProcessThreadMainServer:"<<oficerNaved2Console.guidanceOfficerCmds.servoDriveExecute[0]<<oficerNaved2Console.guidanceOfficerCmds.servoDriveExecute[1]
//                    <<oficerNaved2Console.guidanceOfficerCmds.servoDriveDelta[0]<<oficerNaved2Console.guidanceOfficerCmds.servoDriveDelta[1];


//            qDebug()<<"A: ProcessThreadMainServer:"
//                    <<apy2unv.komandi_NAVED.guidanceOfficerCmds.servoDriveExecute[0]
//                    <<apy2unv.komandi_NAVED.guidanceOfficerCmds.servoDriveExecute[1]
//                    <<apy2unv.komandi_NAVED.guidanceOfficerCmds.servoDriveDelta[0]
//                    <<apy2unv.komandi_NAVED.guidanceOfficerCmds.servoDriveDelta[1];

            //qDebug()<<"ProcessThreadMainServer: apyInfoA.rejimPoiskaSNR:"<<apyInfoA.rejimPoiskaSNR;
//            qDebug()<<"ProcessThreadMainServer: oficerNaved2Console.guidanceOfficerCmds.scannerUV10IsOn:"
//                    <<oficerNaved2Console.guidanceOfficerCmds.scannerUV10IsOn
//                    <<"scannerUV11IsOn:"<<oficerNaved2Console.guidanceOfficerCmds.scannerUV11IsOn;

            unv2apy.cntrMCA2D++;

            memcpy(&unv2apy.srvDrvInfo, &srvDrvInfoA, sizeof(SERVO_DRIVE_INFO));       //  2018_06_14            

            //  2018_08_04 cntrUSM_test++;                             //  для тестирования передачи данных    //  2018_07_07
            //  2018_08_04 unv2apy.usmInfo.usm02[9] = cntrUSM_test;    //  awsA125Server->awsM125Server->awsG125DiagMonitor
            //  2018_08_04 unv2apy.usmInfo.sync[7] = cntrUSM_test;     //  awsA125Server->awsM125Server->awsG125User

//            qDebug()<<"A: ProcessThreadMainServer:"<<unv2apy.joinedTrgtData[0].elevAngleF<<unv2apy.joinedTrgtData[1].elevAngleF<<unv2apy.joinedTrgtData[2].elevAngleF
//                      <<unv2apy.joinedTrgtData[3].elevAngleF<<unv2apy.joinedTrgtData[4].elevAngleF<<unv2apy.joinedTrgtData[5].elevAngleF<<unv2apy.joinedTrgtData[6].elevAngleF;

            sendto(s, &unv2apy, sizeof(ServerUNV2MainServer), 0, (struct sockaddr*) &si_other, slen);

            servoDriveExecuter();                                           //  перенесено из threadGuidanceOfficer 2018_06_14

            //qDebug()<<"ProcessThreadMainServer: apyInfoA.rejimRabotiZRK:"<<apyInfoA.rejimRabotiZRK<<"komKomM.rejimRabotiZRK:"<<komKomM.rejimRabotiZRK;
            //qDebug()<<"ProcessThreadMainServer: totalReadBytes:"<<totalReadBytes<<"apy2unv.cntrD2MCA:"<<apy2unv.cntrD2MCA;
        }
    }

    close(s);
    return temp_pt;
}
