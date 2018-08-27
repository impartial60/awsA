#include "device125.h"

Device125::Device125(QObject *parent) : QObject(parent)
{

   timer = new QTimer;
   connect(timer,SIGNAL(timeout()),this,SLOT(slot_timeout()));
   timer->setSingleShot(true);
   exch = new QUdpSocket;
   connect(exch,SIGNAL(readyRead()),this,SLOT(slot_udpServer()));
    rt.start();
    memset(&tpaz,0,sizeof(tpaz));
    memset(&tpum,0,sizeof(tpum));


}

Device125::~Device125()
{

}

#ifdef test_pult_kprd
inline double Device125::device_getpos_az()  {
    double tmp;
        if(mode == combat)
           tmp = (get_encoder_az_pu(p_receive->enc_angle_pos_az));
            else
                tmp = (lenze_to_double(p_receive->enc_angle_pos_az));
        return tmp;// > 180.0 ? tmp - 360.0 : tmp;
                }
inline double Device125::device_getpos_elv() {
        if(mode == combat)
            return(get_encoder_um_pu (p_receive->motor_encoder_elv));
            else
                return (lenze_to_double(p_receive->motor_encoder_elv));
                   }
#else
inline double device_getpos_az()  {return (lenze_to_double(p_receive->enc_angle_pos_az));}
inline double device_getpos_elv() {return(lenze_to_double(p_receive->motor_encoder_elv));}
#endif

 inline void Device125::device_setpos_az (double pos) {
    if(pos >360.0 || pos < -360.0) return;
    double tmp=pos,tmpoz,tmpz;
    static double oldz;
    qDebug()<<"device125 az 0"<<pos<<" tmp "<<tmp<<" oldz "<<oldz<<" zero_az "<<zero_az<<" "<< p_send->ID_packet;
//      tmp=fabs(tmp-old_pos_az)<180.0 ? tmp-old_pos_az : tmp-old_pos_az < 0.0 ? 360.0+(tmp-old_pos_az):(tmp-old_pos_az)-360.0;

    if(oldz>0.0)
    {if(pos<0.0) {tmpoz = 180.0-oldz; tmpz = 180 + pos;
            if(tmpoz + tmpz < 180) tmp = oldz + tmpoz + tmpz;}
     }
    else
    {if(pos>0.0) {tmpoz = 180.0+oldz; tmpz = 180 - pos;
            if(tmpoz + tmpz < 180) tmp = oldz - (tmpoz + tmpz);}
     }

  //  if(tmp < 0) tmp = 360.0+tmp;

        if(mode == combat)
            tmp = tmp-zero_az;

      p_send->angle_pos_az = double_to_lenze(tmp);
     oldz = tmp;
    qDebug()<<"device125 az 1"<<pos<<" tmp "<<tmp<<" oldz "<<oldz<<" zero_az "<<zero_az<<" "<< p_send->ID_packet;
}

inline void Device125::device_setpos_elv(double pos) {
       double tmp;
      if(pos > elv_limit_up || pos < elv_limit_dn) return;
      if(mode == combat)
      {


    tmp = pos-zero_elv;

    p_send->angle_pos_elv= double_to_lenze(tmp);
      }
      else p_send->angle_pos_elv= double_to_lenze(pos);
      old_pos_elv = pos;
    qDebug()<<"device125 um "<<pos<<" "<<tmp<<" "<<" "<<zero_elv<<" "<< p_send->ID_packet;
}

void Device125::set_on_device(void)//sync
{
     state=0;
}

void Device125::sync(void)
{
 // qDebug()<<count_switch;
    int tmp_pm=get_pult_mode();
    if(old_pult_mode != tmp_pm)
    {
       if(++count_switch > 10)
       {
     if(tmp_pm == combat_mode)  state=0;
        old_pult_mode = tmp_pm;
        count_switch = 0;
          }
    }

    if((mode == combat) && (old_pult_mode == combat_mode))
    {
    switch(state)
    {case 0:
        model_on_of(off);
        device_az_zero_intg(off);
        device_elv_zero_intg(off);
        device_az_on_intg(off);
        device_elv_on_intg(off);
        device_az_en_intg(off);
        device_elv_en_intg(off);
        state++;
        break;
    case 1:
        device_az_zero_intg(on);
        device_elv_zero_intg(on);
        state++;
        break;
    case 2:
        device_az_on_intg(on);
        device_elv_on_intg(on);
        state++;
        break;
    case 3:
        device_az_zero_intg(off);
        device_elv_zero_intg(off);
        zero_az = device_getpos_az();
        zero_elv = device_getpos_elv();
        device_setpos_az(old_pos_az);//new sync,recalculate
        device_setpos_elv(old_pos_elv);
   //     p_send->sync_int_um =1;
        state++;
        break;
    case 4:
        device_az_en_intg(on);
        device_elv_en_intg(on);
 //       p_send->sync_int_um =1;
        state++;
        break;
    default:
        break;}
    }
    else if(mode == training)
    {
        switch(state)
        {case 0:
            model_on_of(on);
            state++;
            break;
        case 1:

      //      device_az_en_intg(off);
      //      device_elv_en_intg(off);
            state++;
            break;
        case 2:
      //      device_az_on_intg(off);
      //      device_elv_on_intg(off);
          //  tpaz.curr_pos = zero_az;
          //  tpum.curr_pos = zero_elv;
            state++;
            break;
        default:
            break;}
        }
        else;
    }

void Device125::slot_timeout(void)
{
emit sig_timeout();
}

uint32_t Device125::get_parameter_lenze(int parameter)
{

}

uint32_t Device125::set_parameter_lenze(int parameter,int parm)
{

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


    p_receive->enc_angle_pos_az = (int)(tpaz.curr_pos_enc*10000.0);
    p_receive->motor_encoder_elv = (int)(tpum.curr_pos_enc*10000.0);

    //  send_packet
    exch->writeDatagram((char*)p_receive,sizeof(*p_receive),adr,port_tmp);

       }
old_ID_packet = send_tmp.ID_packet;
}

void Device125::tp_update(void)
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

        tpaz.curr_pos_enc = tpaz.curr_pos;// < 0.0 ? 360.0 - tpaz.curr_pos : tpaz.curr_pos;
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
        tpum.curr_pos_enc = tpum.curr_pos;// < 0.0 ? 360.0 - tpum.curr_pos : tpum.curr_pos;

}
