#ifndef DEVICE125_H
#define DEVICE125_H

#include <QObject>
#include <QtCore>
#include <QUdpSocket>
#include <QTimer>
#include <QThread>
#include <QtCore/qglobal.h>

#include "clockrealtime.h"
#include <stdint.h>
#include <math.h>
#include <memory.h>
#include <fcntl.h>

#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define START_PACK _Pragma ("pack(push, 1)")
#define END_PACK   _Pragma ("pack(pop)")

#define port_125 (uint16_t)50000
#define main_clock_works 20
#define time_outs 10

#define max_vel_az_unv 18.0
#define max_vel_um_unv 15.0
#define max_acc_az_unv 3.0
#define max_acc_az_unv 3.0

#define cmax_vel_az  18.0*10800
#define cmax_acc_az  5.0*10800
#define cmax_vel_um  15.0*10800
#define cmax_acc_um  5.0*10800

#define max_vel_az_p 7.0
#define max_vel_um_p 4.0
#define max_acc_az_p 3.0
#define max_acc_um_p 3.0
 class QObject;
class Device125:public QObject
{
   Q_OBJECT
public:
    explicit Device125(QObject *parent = nullptr);
    ~Device125();


    enum device_type {unv,pusk,nodevice};
    enum device_mode {combat,training};


    inline int32_t double_to_lenze(double n){return ((int)(n*10000.0));}
    inline double lenze_to_double(int32_t n){return (double(n)/10000.0);}

    inline double get_encoder_az(int az) {return (double)az/10800.0;}
    inline double get_encoder_um(int um) {return (double)um/10800.0;}

    inline double get_resolver_az(int az) {return (double)az/10000.0;}
    inline double get_resolver_um(int um) {return (double)um/10000.0;}

    inline double get_integrator_az(int irg) {return (double)irg/10000.0;}
    inline double get_integrator_um(int irg) {return (double)irg/10000.0;}
 //--------------------------------------------------------------------
    inline double get_encoder_az_p(int az) {return ((360.0/65536.0)*(double)(az&0x0000ffff));}
    inline double get_encoder_um_p(int um) {return ((360.0/65536.0)*(double)(um&0x0000ffff));}

    inline double get_resolver_az_p(int az) {return (double)az/10000.0;}
    inline double get_resolver_um_p(int um) {return (double)um/10000.0;}


    inline double get_integrator_az_p(int irg) {return (double)irg/10000.0;}
    inline double get_integrator_um_p(int irg) {return (double)irg/10000.0;}


   // inline int32_t double_to_lenze(double n){return ((int)(n*10000));}
    inline int32_t double_to_lenze_eps(double n){return ((int)(n*10000));}

    inline int32_t double_to_lenze_p(double n){return ((int)(n*10000));}
    inline int32_t double_to_lenze_eps_p(double n){return ((int)(n*10000));}

    double polynom(double x) {return
 (((x*x*x)*4.51561467696784e-7)-((x*x)*0.000198086819613)+(x*0.02827381765438)-0.040144620068631);}

    inline int32_t double_to_lenze_eps_p_poly( double n) {return ((int)((n-polynom(n))*10000.0));}

    void get_realtime() {time_diff = rt.elapsed();rt.start();}

    inline double device_getpos_az()  {return (lenze_to_double(p_receive->enc_angle_pos_az));}
    inline double device_getpos_elv() {return(lenze_to_double (p_receive->enc_angle_pos_elv));}

    inline void device_setpos_az (double pos) {p_send->angle_pos_az = double_to_lenze(pos);}
    inline void device_setpos_elv(double pos) {p_send->angle_pos_elv= double_to_lenze(pos);}

    inline void device_az_on() {p_send->az_on = 1;}
    inline void device_az_off() {p_send->az_on = 0;}

    inline void device_elv_on() {p_send->elv_on = 1;}
    inline void device_elv_off() {p_send->elv_on = 0;}

    inline void device_az_en() {p_send->az_en = 1;}
    inline void device_az_dis() {p_send->az_en = 0;}

    inline void device_elv_en() {p_send->elv_en = 1;}
    inline void device_elv_dis() {p_send->elv_en = 0;}
    double time_diff;

    int mode=combat;
    inline void set_mode(int modein) {
                                      mode = modein;

                                      if(mode == combat)
                                      {p_receive = &receive;
                                        p_send = &send;
                                         p_ip = &ip_combat;
                                      model_on_of(off);
                                      }
                                        else {p_receive = &receive_training;
                                               p_send = &send_training;
                                                p_ip = &ip_training;


                                      if(type == unv)
                                          set_parameter_model(cmax_vel_az,
                                                              cmax_acc_az,
                                                              cmax_vel_um,
                                                              cmax_acc_um
                                                      );
                                      else
                                         set_parameter_model(max_vel_az_p,
                                                             max_acc_az_p,
                                                             max_vel_um_p,
                                                             max_acc_um_p
                                                  );

                                            model_on_of(on);
                                      }

               exch->close();
               exch->bind(*p_ip,port_125);

                                     }
    inline int get_mode(void){return mode;}

  //  inline uint32_t get_ip_adress() {return *p_ip;}
    inline QHostAddress get_ip_adress() {return *p_ip;}

    int type=nodevice;

    inline void set_type(int typein) {type = typein;}
    inline int get_type(void){return type;}


    inline char * get_adr_send(void) {return (char*)p_send;}
    inline int get_len_send(void) {return sizeof(*p_send);}

    inline char * get_adr_receive(void) {return (char*)p_receive;}
    inline int get_len_receive(void) {return sizeof(*p_receive);}

    inline char * get_tmp_adr_receive(void) {return (char*)&receive_tmp;}
    inline int get_tmp_len_receive(void) {return sizeof(receive_tmp);}

    inline void set_id_packet(int pk) {p_send->ID_packet = pk;}
    inline int  get_id_packet(void) {return p_receive->ID_packet;}

    ClockRealTime rt;

    uint16_t      port_dev = port_125;

   QHostAddress ip_combat,ip_training,ip_tmp,*p_ip;

    QUdpSocket *exch;
    QTimer  *timer;


public slots:

void slot_timeout(void);
void slot_udpServer(void);

signals:
void sig_timeout(void);

private:
int old_ID_packet;

  //Структура данных обмена — АПУ->УНВ  и пусковых

    START_PACK
    struct apu_slave
    {
        const uint32_t header = 0xaa55aa55;
    /*4 */uint32_t ID_packet;
    /*8 */int32_t angle_pos_az;               //DINT 	(32) 		 Угловая координата азимута.
    /*12*/int32_t angle_pos_elv ;            //DINT 	(32) 		 Угловая координата угла места.
    /*16*/int32_t angle_speed_az ;           //DINT 	(32) 		 Угловая скорость по азимуту.
    /*20*/int32_t angle_speed_elv ;

  //DINT 	(32) 		 Угловая скорость по углу места.
    /*24*/uint32_t send_SDO1;                 //UDINT[2] (64)         данные для записи по кодам/субкодвм. На передачу.
    /*28*/uint32_t send_SDO2;                 //UDINT[2] (64)	 	 данные для записи по кодам/субкодвм На передачу.
    /*30*/uint16_t code_az;       //WORD (16)		Номер кода  в запросе.(азимут).(Номер кода внутри блока управления приводом)
    /*32*/uint16_t subcode_az;    //WORD (16)		Номер субкода  в запросе.(азимут) (Номер кода внутри блока управления приводом)
    /*34*/uint16_t code_elv;      //WORD (16)		Номер кода  в запросе.(угол места).(Номер кода внутри блока управления приводом)
    /*36*/uint16_t subcode_elv;   //WORD (16)		Номер субкода  в запросе.(угол места) (Номер кода внутри блока управления приводом)
    /*40*/uint32_t data_az;       //UNION(32)		Данные для записи.(угол места)
    /*44*/uint32_t data_elv;      //UNION(32)		Данные для записи.(азимут)
    /*48*/uint32_t                //DWORD (32) 		Управление приводом
        az_on:1,//бит 0 		Включение привода азимута
        elv_on:1,//бит 1 		Включение привода угла места
        az_en:1,//бит 2 		Разрешение работы интегратора азимута
        elv_en:1,//бит 3 		Разрешение работы интегратора угла места
        az_int_clear:1,//бит 4 		Сброс интегратора азимута
        elv_int_clear:1,//бит 5 		Сброс интегратора угла места
        az_pos_speed_controll:1,//бит 6 		Управление по позиции — 0 по скорости 1 (азимут)
        elv_pos_speed_controll:1,//бит 7 		Управление по позиции — 0 по скорости 1 (угол места)
        az_error_clear:1,//бит 8 		Сброс ошибки привода азимута
        elv_error_clear:1,//бит 9		Сброс ошибки привода угла места
        CAN_SDO_ADR:3,//биты 10-12 	Адрес устройства на CAN шине при обмене SDO .
        az_code_write:1,//Бит 13	 	Признак записи для кодов (азимут).
        elv_code_write:1,//Бит 14	 	Признак записи для кодов (угол места).

        sc_UV10_on:1,//bit 15           Включение/выключение сканнера УВ10
        sc_UV11_on:1,//bit 16           Включение/выключение сканнера УВ11

        az_code_read:1,//Бит 17	 	Признак записи для кодов (азимут).
        elv_code_read:1,//Бит 18	 	Признак записи для кодов (угол места).
        not_used:13;        //bit 17-31

    ///*70*/const char ID_string[22] = {'K','r','a','m','a','r','e','n','k','o','S','h','c','h','e','r','b','a','k','o','v',0};//{"КрамаренкоЩербаков"};

    } send , send_training,send_tmp,*p_send;
    END_PACK
    //Структура данных обмена —УНВ  и пусковые →АПУ
    START_PACK
    struct slave_apu
    {
        const uint32_t header = 0x55aa55aa;
/*4 */    uint32_t ID_packet;
/*8 */    int32_t enc_angle_pos_az; //DINT 	(32) 		 Угловая координата азимута. (по энкодеру)
/*16 */    int32_t enc_angle_pos_elv;//DINT 	(32) 		 Угловая координата угла места.(по энкодеру)
/*20 */    int32_t enc_angle_speed_az ;//DINT 	(32) 		 Угловая скорость по азимуту.(по энкодеру)
/*24 */    int32_t enc_angle_speed_elv ; //DINT 	(32) 		 Угловая скорость по углу места.(по энкодеру)
/*28 */ uint32_t motor_encoder_az;     //DWORD (32) Энкодер двигателя азимута
/*32 */ uint32_t motor_encoder_elv;    //DWORD (32) Энкодер двигателя угла места
/*36 */ uint32_t receive_SDO1;         // SDO 1,2 На прием.
/*40 */ uint32_t receive_SDO2;
/*42 */ uint16_t code_az;       //WORD (16)		Номер кода  в ответе.(азимут).(Номер кода внутри блока управления приводом)
/*44 */ uint16_t subcode_az;             //WORD (16)		Номер субкода  в ответе.(азимут) (Номер кода внутри блока управления приводом)
/*46 */ uint16_t code_elv;      //WORD (16)		Номер кода  в ответе.(угол места).(Номер кода внутри блока управления приводом)
/*48 */ uint16_t subcode_elv;   //WORD (16)		Номер субкода  в ответе (угол места) (Номер кода внутри блока управления приводом)
/*52 */ uint32_t data_az;       //UNION(32)		Данные по коду.(азимут)
/*56 */ uint32_t data_elv;      //UNION(32)		Данные по коду.(угол места)
/*60 */ uint32_t az_errors;     //UDINT (32) 		Код ошибки привода azimuth
/*64 */ uint32_t elv_errors;     //UDINT (32) 		Код ошибки привода elv

/*68 */ uint32_t            //UDINT (32) 		Состояние привода.
        az_error:1,     //бит 0 		Ошибка привода азимута
        elv_error:1,    //бит 1 		Ошибка привода угла места
        rez1:1,         //биты 2		резерв
        err_rd_az:1,//3
        err_wr_az:1,//4
        err_rd_um:1,//5
        err_wr_um:1,//6
        status_rd_az:1,//7   //биты 7-8 	статусы чтения/записи кодов .
        status_wr_az:1,//8
        status_rd_um:1,//9   //биты 9-10 	статусы чтения/записи кодов .
        status_wr_um:1,//10
     //   rez2:21;        //биты 11-31 	резерв.
                            //состояние унв/пск
         k_l_raz:1,  //11                 //концевик лесницы/ручка азимута
         k_b_rum:1,  //12                 //концевик переднего балкона/ручка угла места
         k_bz:1,     //13               //концевик заднего балкона
         k_umV:1,   //14                //угол места предел низ
         k_umn:1,   //15                //угол места предел верх
         b_otb:12;  //16-27                 //Биты с ОТБ 9 только для пушек

    //char ID_string[22] = {'S','h','c','h','e','r','b','a','k','o','v','K','r','a','m','a','r','e','n','k','o',0};//{"КрамаренкоЩербаков"};

    } receive,receive_training,receive_tmp,*p_receive;
    END_PACK

    struct tp_t {
    double pos_cmd;		/* Позиция в которую необходимо переместиться */
    double max_vel;		/* Лимит скорости */
    double max_acc;		/* Лимит ускорения */
    int enable;		/* Если "0" движение останавливается "ASAP" */
    double curr_pos;	/* Текущая позиция */
    double curr_vel;	/* Текущая скорость */
    int active;		/* "1" если продолжается движение */
    } tpaz,tpum;

inline void set_parameter_model(double max_vel_az,double max_acc_az,double max_vel_um,double max_acc_um)
{
    tpaz.max_vel = max_vel_az;tpaz.max_acc = max_acc_az;
    tpum.max_vel = max_vel_um;tpum.max_acc = max_acc_um;

}
enum on_off {off,on};
inline void model_on_of(int md) {tpaz.enable = md;tpum.enable = md;}

void tp_update(/*tp_t *tp, double period*/);
};// Device125;

#endif // DEVICE125_H
