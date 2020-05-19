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

extern QVector<double> K;
extern double X[2000][2];

struct ToRos {
    float poseY = 0; // Глубина
    float poseZ = 0; // Лаг
    float angleYaw = 0; //
    float linearVelosityY = 0;
    float linearVelosityZ = 0;
    float angularVelosityY = 0;
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
    void againPressed();
    void stateChanged();
    void gateFinded();
    void gateNotFound();
    void startCentering();
    void centeringDone();

public:
    explicit SenderWidget(QWidget *parent = 0);
    ~SenderWidget(){};
    SU_ROV su;
    QString getState(){return state;}
    void setState(QString st){
        state = st;
        txtBrFile->append(st);
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
    QState *waitForCommand, *search, *swim, *centering;
    QFinalState *finish;
    QString state;
    bool gateFound;
    QString gateDirection;

    void updateSendValues();
    void updateReceivedValues();

    // initial values
    //float initialX = 0.5; // марш
    //float initialY = 2.0; // глубина
    //float initialZ = 4.0; // лаг
    //float initialRoll = 1.5708;
    //float initialYaw = 1.5708;

private slots:
    void send();
    void receive();
    void searchForGate();
    void swimToGate();
    void centeringOnGate();
    void finishMission();
    void searchingMetod();
    void checkYaw();
    void swimmingMethod();
};
#endif // SENDERWIDGET_H
