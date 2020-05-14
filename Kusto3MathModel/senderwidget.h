#ifndef SENDERWIDGET_H
#define SENDERWIDGET_H

#include <QString>
#include <QWidget>
#include <QHostAddress>
#include <QUdpSocket>
#include <QNetworkDatagram>
#include "ui_senderwidget.h"
#include "su_rov.h"

#include <QVBoxLayout>
#include <QWebEngineView>

struct ToRos {
    float linearVelosityX;
    float linearVelosityY;
    float linearVelosityZ;
    float angularVelosityX;
    float angularVelosityY;
    float angularVelosityZ;
};

struct FromRos {
    bool isExist;
    float x_start;
    float y_start;;
    float x_end;
    float y_end;
    float x_center;
    float y_center;
};

class SenderWidget : public QWidget, Ui::SenderWidget {
    Q_OBJECT
public:
    explicit SenderWidget(QWidget *parent = 0);
    ~SenderWidget(){};
    SU_ROV su;

private:
    bool connectionEstablished;
    QUdpSocket *qtSenderUdpSocket;
    QUdpSocket *qtReceiverUdpSocket;
    QString PATH;//путь к файлу (прошлое занятие)
    QHostAddress QtSenderIP, QtReceiverIP, RosSenderIP, RosReceiverIP;//переменные для работы с IP-адресами
    int QtSenderPort, QtReceiverPort, RosSenderPort, RosReceiverPort;//номера портов отправителя и получателя
    ToRos messageToRos;
    FromRos messageFromRos;

private slots:
    void send();
    void receive();
};
#endif // SENDERWIDGET_H
