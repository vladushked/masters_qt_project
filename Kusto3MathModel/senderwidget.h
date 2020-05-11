#ifndef SENDERWIDGET_H
#define SENDERWIDGET_H

#include <QString>
#include <QWidget>
#include <QHostAddress>
#include <QUdpSocket>
#include "ui_senderwidget.h"
#include "su_rov.h"

#include <QVBoxLayout>
#include <QWebEngineView>



class SenderWidget : public QWidget, Ui::SenderWidget {
    Q_OBJECT
public:
    explicit SenderWidget(QWidget *parent = 0);
    ~SenderWidget(){};
    SU_ROV su;

private slots:

private:
    QUdpSocket *udp;//указатель на объект сокета
    QString PATH;//путь к файлу (прошлое занятие)
    QHostAddress senderIP, receiverIP;//переменные для работы с IP-адресами
    int senderPort, receiverPort;//номера портов отправителя и получателя
    int sended; //переменная для хранения количества отправленных байт
};
#endif // SENDERWIDGET_H
