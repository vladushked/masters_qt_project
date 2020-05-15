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

#include <QStateMachine>
#include <QFinalState>

struct ToRos {
    float linearVelosityX = 0;
    float linearVelosityY = 0;
    float linearVelosityZ = 0;
    float angularVelosityX = 0;
    float angularVelosityY = 0;
    float angularVelosityZ = 0;
};

struct FromRos {
    bool isExist = false;
    float x_start = 0;
    float y_start = 0;
    float x_end = 0;
    float y_end = 0;
    float x_center = 0;
    float y_center = 0;
};

class SenderWidget : public QWidget, Ui::SenderWidget {
    Q_OBJECT
    Q_PROPERTY(QString state READ getState WRITE setState NOTIFY stateChanged)

signals:
    void startPressed();
    void stateChanged();
    void gateFinded();
    void centeringDone();

public:
    explicit SenderWidget(QWidget *parent = 0);
    ~SenderWidget(){};
    SU_ROV su;
    QString getState(){return state;}
    void setState(QString st){
        state = st;
        txtBrFile->setText(st);
    }

private:
    // udp
    bool connectionEstablished;
    QUdpSocket *qtSenderUdpSocket;
    QUdpSocket *qtReceiverUdpSocket;
    QHostAddress QtSenderIP, QtReceiverIP, RosSenderIP, rosReceiverIP;//переменные для работы с IP-адресами
    int QtSenderPort, QtReceiverPort, RosSenderPort, rosReceiverPort;//номера портов отправителя и получателя
    ToRos messageToRos;
    FromRos messageFromRos;
    // state machine
    QStateMachine stateMachine;
    QState *waitForCommand, *searchGate, *swimToGate, *centering;
    QFinalState *finish;
    QString state;

private slots:
    void send();
    void receive();
    void finishMission();
};
#endif // SENDERWIDGET_H
