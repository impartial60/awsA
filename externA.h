#ifndef EXTERNA_H
#define EXTERNA_H
#include <QTime>
#include <QDebug>

#include "../../aws125CommonInclude/sharedMem.h"
#include "../../aws125CommonInclude/externCommon.h"
#include "../../aws125CommonInclude/sys.h"
#include "threadstatus.h"
#include "srcKram/tp.h"
#include "settingsA.h"

extern MDO1_nextDirection *mdo1AllDirections;

extern ServerUNV2MainServer unv2apy;
extern MainServer2ServerUNV apy2unv;

extern VKP2ZRK vkp2zrkG;
extern SERVO_DRIVE_INFO srvDrvInfoA;
extern APY_INFO apyInfoA;

extern KOMANDI_KOMANDIRA komKomA;
extern KOMANDI_OFICERA_NAVED oficerNaved2Console;

extern SNR2SYS snr2sys;
extern SYS2SNR sys2snr;

extern QTime programmStartTime;

extern tp_t p1,p2;

extern void* ret_val_All_Threads;
extern ThreadStatus *threadStatus;

extern pthread_mutex_t localConsole_memory_Mutex;
extern pthread_mutex_t nextTockaMutex;

extern int restartCntr;

extern SettingsConsoleA *settings;

extern void servoDriveExecuter();
extern uint16_t uint16_t_byte_reorder(uint16_t inputValue);
extern void asterix_I126_050_Coder(uint8_t *sck);   //  I126/050        С О С Т О Я Н И Е   Ц Е Л Е В О Г О   К А Н А Л А   1


extern void * ProcessThreadCommander( void * temp_pt );
extern void * ProcessThreadLauncherOfficer( void * temp_pt );
extern void * ProcessThreadSYS(void * temp_pt );
extern void * ProcessThreadCnvEth485(void * temp_pt );
extern void * ProcessSimulator(void * temp_pt );
extern void * ProcessThreadGuidanceOfficer(void * temp_pt );
extern void * ProcessThreadDiagnMonitor( void * temp_pt );
extern void * ProcessThreadOperF1( void * temp_pt );
extern void * ProcessThreadOperF2( void * temp_pt );
extern void * ProcessThreadMDO1( void * temp_pt );
extern void * ProcessThreadMDO1Echo( void * temp_pt );
extern void * ProcessThreadInfoTable( void * temp_pt );
extern void * ProcessThreadMainServer( void * temp_pt );
extern void * ProcessThreadVKP(void * temp_pt );
extern void * ProcessThreadP18(void * temp_pt );

extern float convert360angle2PlusMinus180(float dltAz360);

#endif // EXTERNA_H
