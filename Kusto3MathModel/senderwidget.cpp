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
    connect(qtReceiverUdpSocket, SIGNAL(readyRead()), this, SLOT(socketReceived()));
    connect(&su.timer, SIGNAL(timeout()), this, SLOT(send()));
    connectionEstablished = false;

    // state machine
    connect(startButton, SIGNAL(clicked()), SIGNAL(startPressed()));
    waitForCommand = new QState();
    searchGate = new QState();
    swimToGate = new QState();
    centering = new QState();
    finish = new QFinalState();
    // state machine transitions
    waitForCommand->addTransition(this, SIGNAL(startPressed()), searchGate);
    searchGate->addTransition(this, SIGNAL(gateFinded()), swimToGate);
    swimToGate->addTransition(this, SIGNAL(gateFinded()), centering);
    centering->addTransition(this, SIGNAL(centeringDone()), finish);
    // connnect some signals
    connect(&stateMachine, SIGNAL(finished()), SLOT(finishMission()));
    // set text for each state
    waitForCommand->assignProperty(this, "state", "Waiting for command...");
    searchGate->assignProperty(this, "state", "Searching for gate...");
    swimToGate->assignProperty(this, "state", "Gate finded.\nSwimming to gate...");
    centering->assignProperty(this, "state", "Centering...");
    // add states to state machine
    stateMachine.addState(waitForCommand);
    stateMachine.addState(searchGate);
    stateMachine.addState(swimToGate);
    stateMachine.addState(centering);
    stateMachine.addState(finish);
    // set initial state
    stateMachine.setInitialState(waitForCommand);
    stateMachine.start();
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

void SenderWidget::finishMission()
{
    txtBrFile->setText("Centering done. Mission complete!");
    stateMachine.stop();

}


