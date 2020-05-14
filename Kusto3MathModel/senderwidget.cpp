#include "senderwidget.h"
#include "ui_senderwidget.h"
#include <QFile>
#include <QJsonObject>
#include <QJsonDocument>

SenderWidget::SenderWidget(QWidget *parent) : QWidget(parent) {
    setupUi(this);

    //прочитаем конфигурационные данные из файла
    QFile configFile ("config.json");
    QJsonDocument jsonDoc;
    //проверяем существование файла
    if (configFile.exists()) {
        //если файл существует, то открываем его для чтения и работаем
        configFile.open(QFile::ReadOnly);
        //считаем конфигурационные данные из файла
        //используя метод fromJson, который преобразует текст (QByteArray)
        //в формат JSON
        jsonDoc = QJsonDocument().fromJson(configFile.readAll());
        configFile.close();
        //создадим JSON-объект, в который считаем содержимое jsonDoc
        QJsonObject jsonData = jsonDoc.object();
        //qt sender ip, port:
        QtSenderIP.setAddress(jsonData.value("qt_sender.ip").toString());
        QtSenderPort = jsonData.value("qt_sender.port").toInt();
        //qt receiver ip, port:
        QtReceiverIP.setAddress(jsonData.value("qt_receiver.ip").toString());
        QtReceiverPort=jsonData.value("qt_receiver.port").toInt();
        //ros sender ip, port:
        RosSenderIP.setAddress(jsonData.value("ros_sender.ip").toString());
        RosSenderPort = jsonData.value("ros_sender.port").toInt();
        //ros receiver ip, port:
        RosReceiverIP.setAddress(jsonData.value("ros_receiver.ip").toString());
        RosReceiverPort=jsonData.value("ros_receiver.port").toInt();
        //выведем полученные значения в окно приложения
        lblQtSenderIPport->setText(QtSenderIP.toString()+":"+QString::number(QtSenderPort));
        lblQtReceiverIPport->setText(QtReceiverIP.toString()+":"+QString::number(QtReceiverPort));
        lblROSSenderIPport->setText(RosSenderIP.toString()+":"+QString::number(RosSenderPort));
        lblROSReceiverIPport->setText(RosReceiverIP.toString()+":"+QString::number(RosReceiverPort));
    }
    else {
        qDebug()<<"can't open config.file";
    }

    // web widget
    widget->load(QUrl(QStringLiteral("http://localhost:8080/stream_viewer?topic=/dnn/image")));
    // sockets
    qtSenderUdpSocket = new QUdpSocket();
    qtReceiverUdpSocket = new QUdpSocket();
    qtSenderUdpSocket->bind(QtSenderIP, QtSenderPort);
    qtReceiverUdpSocket->bind(QtReceiverIP, QtReceiverPort);
    // signals
    connect(qtReceiverUdpSocket, SIGNAL(readyRead()), this, SLOT(socketReceived()));
    connect(&su.timer, SIGNAL(timeout()), this, SLOT(send()));
    connectionEstablished = false;
}

void SenderWidget::send()
{
    QByteArray baDatagram;
    qtSenderUdpSocket->writeDatagram((char*)&messageToRos, sizeof (messageToRos), RosReceiverIP, RosReceiverPort);
}

void SenderWidget::receive()
{
    while (qtReceiverUdpSocket->hasPendingDatagrams()) {
        qtReceiverUdpSocket->readDatagram((char*)&messageFromRos, sizeof (messageFromRos));

    }

    if (!connectionEstablished) {
        connectionEstablished = true;
        qDebug() << "Connection established, receiving done";
        txtBrFile->setText("Connection established, receiving done");
    }
}


