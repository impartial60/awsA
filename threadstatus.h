#ifndef THREADSTATUS_H
#define THREADSTATUS_H

enum threedStatus {cReate, inuse, cancel, notuse};

class ThreadStatus
{    
    public:
    threedStatus thrStatus_CnvEth485;
    threedStatus thrStatus_MDO1;
    threedStatus thrStatus_MDO1Echo;
    threedStatus thrStatus_MainServer;
    threedStatus thrStatus_Fishchuk;

    ThreadStatus()
    {
        thrStatus_CnvEth485 = cReate;//notuse;//
        thrStatus_MDO1 = cReate; //notuse;//
        thrStatus_MDO1Echo = cReate; //notuse;//
        thrStatus_MainServer = cReate; //notuse;//
        thrStatus_Fishchuk = notuse;//cReate;//
    }
};

#endif // THREADSTATUS_H
