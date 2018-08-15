#include "Drive125.h"


Drive125::Drive125(QObject *parent) : QObject(parent)
{

    unv = new Device125;
    p1 =new Device125;
    p2 =new Device125;
    p3 =new Device125;
    p4 =new Device125;

unv->ip_combat =   QHostAddress(QString("10.5.0.101"));
unv->ip_training = QHostAddress(QString("127.0.0.101"));
p1->ip_combat =   QHostAddress(QString("10.5.0.102"));
p1->ip_training = QHostAddress(QString("127.0.0.102"));
p2->ip_combat =   QHostAddress(QString("10.5.0.103"));
p2->ip_training = QHostAddress(QString("127.0.0.103"));
p3->ip_combat =   QHostAddress(QString("10.5.0.104"));
p3->ip_training = QHostAddress(QString("127.0.0.104"));
p4->ip_combat =   QHostAddress(QString("10.5.0.105"));
p4->ip_training = QHostAddress(QString("127.0.0.105"));

    unv->set_type(unv->unv);
    unv->set_mode(unv->training);
    p1->set_type(p1->pusk);
    p1->set_mode(p1->combat);
    p2->set_type(p2->nodevice);
    p2->set_mode(p2->training);
    p3->set_type(p3->nodevice);
    p3->set_mode(p3->training);
    p4->set_type(p4->nodevice);
    p4->set_mode(p4->training);


    exch = new QUdpSocket;

    connect(unv,SIGNAL(sig_timeout()),this,SLOT(slot_timeout_unv()));
    connect(p1,SIGNAL(sig_timeout()),this,SLOT(slot_timeout_p1()));
    connect(p2,SIGNAL(sig_timeout()),this,SLOT(slot_timeout_p2()));
    connect(p3,SIGNAL(sig_timeout()),this,SLOT(slot_timeout_p3()));
    connect(p4,SIGNAL(sig_timeout()),this,SLOT(slot_timeout_p4()));

    main_timer = new QTimer;

    exch->bind(QHostAddress::AnyIPv4,port_125);

    connect(exch,SIGNAL(readyRead()),this,SLOT(slot_receive_packet()));
    connect(main_timer,SIGNAL(timeout()),this,SLOT(slot_timer_task()));
    main_timer->start(main_clock_works);

}
Drive125::~Drive125()
{

}


void Drive125::slot_timer_task(void)
{
    //qDebug()<<"slot_timer_task unv->get_type():"<<unv->get_type();
    if(unv->get_type() != unv->nodevice)
    {
        unv->set_id_packet(count);
        exch->writeDatagram(unv->get_adr_send(),unv->get_len_send(),*unv->p_ip,port_125);
    }

    if(p1->get_type() != p1->nodevice){
        p1->set_id_packet(count);
        exch->writeDatagram(p1->get_adr_send(),p1->get_len_send(),*p1->p_ip,port_125);
    }
    if(p2->get_type() != p2->nodevice){
        p2->set_id_packet(count);
        exch->writeDatagram(p2->get_adr_send(),p2->get_len_send(),*p2->p_ip,port_125);
    }
    if(p3->get_type() != p3->nodevice){
        p3->set_id_packet(count);
        exch->writeDatagram(p3->get_adr_send(),p3->get_len_send(),*p3->p_ip,port_125);
    }

    if(p4->get_type() != p4->nodevice){
        p4->set_id_packet(count);
        exch->writeDatagram(p4->get_adr_send(),p4->get_len_send(),*p4->p_ip,port_125);

    }
    count++;

}

void Drive125::slot_receive_packet(void)
{
    int len;
    QHostAddress adr;
    quint16 port_tmp;
    while (exch->hasPendingDatagrams()) {

    len = exch->readDatagram(unv->get_tmp_adr_receive(),unv->get_tmp_len_receive(),&adr,&port_tmp);

    if(port_tmp == port_125 && len == unv->get_tmp_len_receive())
        {
   if(adr == *unv->p_ip)
   {memcpy(unv->get_adr_receive(),unv->get_tmp_adr_receive(),unv->get_tmp_len_receive());
        }
    if(adr == *p1->p_ip)
    {memcpy(p1->get_adr_receive(),unv->get_tmp_adr_receive(),unv->get_tmp_len_receive());
        }
      if(adr == *p2->p_ip)
      {memcpy(p2->get_adr_receive(),unv->get_tmp_adr_receive(),unv->get_tmp_len_receive());
        }
        if(adr == *p3->p_ip)
        {memcpy(p3->get_adr_receive(),unv->get_tmp_adr_receive(),unv->get_tmp_len_receive());
            }
          if(adr == *p4->p_ip)
          {memcpy(p4->get_adr_receive(),unv->get_tmp_adr_receive(),unv->get_tmp_len_receive());
            }


    }
    }
}

void Drive125::slot_timeout_unv(void)
{

}
void Drive125::slot_timeout_p1(void)
{

}
void Drive125::slot_timeout_p2(void)
{

}
void Drive125::slot_timeout_p3(void)
{

}
void Drive125::slot_timeout_p4(void)
{

}
