#include "device125.h"

Device125::Device125(QObject *parent) : QObject(parent)
{

   timer = new QTimer;
   connect(timer,SIGNAL(timeout()),this,SLOT(slot_timeout()));
   timer->setSingleShot(true);
   exch = new QUdpSocket;
   connect(exch,SIGNAL(readyRead()),this,SLOT(slot_udpServer()));

    rt.start();

}

Device125::~Device125()
{

}

void Device125::sync(void)
{

}
void Device125::slot_timeout(void)
{
emit sig_timeout();
}



void Device125::slot_udpServer(void)
{
    int len;
    QHostAddress adr;
    quint16 port_tmp;
    while (exch->hasPendingDatagrams()) {

    len = exch->readDatagram((char*)&send_tmp,sizeof(send_tmp),&adr,&port_tmp);
    }

    if(len == sizeof(*p_send) && old_ID_packet+1 == send_tmp.ID_packet)
    {
        p_receive->ID_packet = p_send->ID_packet;

     memcpy(p_send,&send_tmp,sizeof(*p_send));
     timer->start(main_clock_works+time_outs);
       tp_update();


        tpaz.pos_cmd = (double)p_send->angle_pos_az/10000.0;
        tpum.pos_cmd = (double)p_send->angle_pos_elv/10000.0;


    p_receive->enc_angle_pos_az = (int)(tpaz.curr_pos*10000.0);
    p_receive->enc_angle_pos_elv = (int)(tpum.curr_pos*10000.0);

    //  send_packet
    exch->writeDatagram((char*)p_receive,sizeof(*p_receive),adr,port_tmp);

       }
old_ID_packet = send_tmp.ID_packet;
}

void Device125::tp_update()
{

        double max_dv, tiny_dp, pos_err, vel_req,period=rt.interval();

        tpaz.active = 0;

        max_dv = tpaz.max_acc * period;

        tiny_dp = max_dv * period * 0.001;

        if (tpaz.enable) {

        pos_err = tpaz.pos_cmd - tpaz.curr_pos;

        if (pos_err > tiny_dp) {
            vel_req = -max_dv +
                   sqrt(2.0 * tpaz.max_acc * pos_err + max_dv * max_dv);

            tpaz.active = 1;
        } else if (pos_err < -tiny_dp) {
            vel_req =  max_dv -
                   sqrt(-2.0 * tpaz.max_acc * pos_err + max_dv * max_dv);

            tpaz.active = 1;
        } else {

            vel_req = 0.0;
        }
        } else {

        vel_req = 0.0;
        tpaz.pos_cmd = tpaz.curr_pos;
        }

        if (vel_req > tpaz.max_vel) {
            vel_req = tpaz.max_vel;
        } else if (vel_req < -tpaz.max_vel) {
        vel_req = -tpaz.max_vel;
        }

        if (vel_req > tpaz.curr_vel + max_dv) {
        tpaz.curr_vel += max_dv;
        } else if (vel_req < tpaz.curr_vel - max_dv) {
        tpaz.curr_vel -= max_dv;
        } else {
        tpaz.curr_vel = vel_req;
        }
        if (tpaz.curr_vel != 0.0) {
        tpaz.active = 1;
        }

        tpaz.curr_pos += tpaz.curr_vel * period;
//---------------------------um
        tpum.active = 0;

        max_dv = tpum.max_acc * period;

        tiny_dp = max_dv * period * 0.001;

        if (tpum.enable) {

        pos_err = tpum.pos_cmd - tpum.curr_pos;

        if (pos_err > tiny_dp) {
            vel_req = -max_dv +
                   sqrt(2.0 * tpum.max_acc * pos_err + max_dv * max_dv);

            tpum.active = 1;
        } else if (pos_err < -tiny_dp) {
            vel_req =  max_dv -
                   sqrt(-2.0 * tpum.max_acc * pos_err + max_dv * max_dv);

            tpum.active = 1;
        } else {

            vel_req = 0.0;
        }
        } else {

        vel_req = 0.0;
        tpum.pos_cmd = tpum.curr_pos;
        }

        if (vel_req > tpum.max_vel) {
            vel_req = tpum.max_vel;
        } else if (vel_req < -tpum.max_vel) {
        vel_req = -tpum.max_vel;
        }

        if (vel_req > tpum.curr_vel + max_dv) {
        tpum.curr_vel += max_dv;
        } else if (vel_req < tpum.curr_vel - max_dv) {
        tpum.curr_vel -= max_dv;
        } else {
        tpum.curr_vel = vel_req;
        }
        if (tpum.curr_vel != 0.0) {
        tpum.active = 1;
        }

        tpum.curr_pos += tpum.curr_vel * period;


}
