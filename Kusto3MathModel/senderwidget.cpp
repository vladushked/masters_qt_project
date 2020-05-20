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
        rosReceiverIP.setAddress(jsonData.value("ros_receiver.ip").toString());
        rosReceiverPort=jsonData.value("ros_receiver.port").toInt();
        //выведем полученные значения в окно приложения
        lblQtSenderIPport->setText(QtSenderIP.toString()+":"+QString::number(QtSenderPort));
        lblQtReceiverIPport->setText(QtReceiverIP.toString()+":"+QString::number(QtReceiverPort));
        lblROSSenderIPport->setText(RosSenderIP.toString()+":"+QString::number(RosSenderPort));
        lblROSReceiverIPport->setText(rosReceiverIP.toString()+":"+QString::number(rosReceiverPort));
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
    connect(qtReceiverUdpSocket, SIGNAL(readyRead()), this, SLOT(receive()));
    //timer_send.start(100);
    //connect(&timer_send, SIGNAL(timeout()), this, SLOT(send()));
    connectionEstablished = false;
    send();

    gateDirection = "none";

    // state machine
    connect(startButton, SIGNAL(clicked()), SIGNAL(startPressed()));
    connect(againButton, SIGNAL(clicked()), SIGNAL(againPressed()));
    waitForCommand = new QState();
    search = new QState();
    swim = new QState();
    centering = new QState();
    finish = new QFinalState();
    // state machine transitions
    waitForCommand->addTransition(this, SIGNAL(startPressed()), search);
    search->addTransition(this, SIGNAL(gateFinded()), swim);
    search->addTransition(this, SIGNAL(gateNotFound()), waitForCommand);
    swim->addTransition(this, SIGNAL(startCentering()), centering);
    centering->addTransition(this, SIGNAL(centeringDone()), finish);
    search->addTransition(this, SIGNAL(againPressed()), waitForCommand);
    swim->addTransition(this, SIGNAL(againPressed()), waitForCommand);
    centering->addTransition(this, SIGNAL(againPressed()), waitForCommand);
    // connnect some signals
    connect(waitForCommand, SIGNAL(entered()), this, SLOT(resetSU()));
    connect(search, SIGNAL(entered()), this, SLOT(searchForGate()));
    connect(swim, SIGNAL(entered()), this, SLOT(swimToGate()));
    connect(centering, SIGNAL(entered()), this, SLOT(centeringOnGate()));
    connect(&stateMachine, SIGNAL(finished()), SLOT(finishMission()));
    // set text for each state
    waitForCommand->assignProperty(this, "state", "Ожидание команды");
    search->assignProperty(this, "state", "Поиск ворот");
    swim->assignProperty(this, "state", "Ворота найдены.");
    centering->assignProperty(this, "state", "Центрирование");
    // add states to state machine
    stateMachine.addState(waitForCommand);
    stateMachine.addState(search);
    stateMachine.addState(swim);
    stateMachine.addState(centering);
    stateMachine.addState(finish);
    // set initial state
    stateMachine.setInitialState(waitForCommand);
    stateMachine.start();
}

void SenderWidget::updateSendValues()
{
    messageToRos.angleYaw = 1.5708 + X[42][0];
    messageToRos.angularVelosityY = X[50][0];
    messageToRos.poseY = 2.4 + X[62][0]; // глубина
    messageToRos.linearVelosityY = X[70][0];
    messageToRos.poseZ = 4.0 + X[82][0]; // лаг
    messageToRos.linearVelosityZ = X[90][0];
    qDebug() << X[42][0];
    // qDebug() << messageToRos.angleYaw;
    // qDebug() << messageToRos.poseY;
    // qDebug() << messageToRos.poseZ;
}

void SenderWidget::updateReceivedValues()
{
    if (messageFromRos.isExist)
        emit gateFinded();
}

void SenderWidget::resetSU()
{
    // задающие сигналы по каналам
    K[44] = 1;
    K[45] = 1;
    K[43] = 0;
    K[41] = 0;     // контур курса
    K[64] = 1;
    K[65] = 1;
    K[63] = 0;
    K[61] = 0;   // контур глубины
    K[84] = 1;
    K[85] = 1;
    K[83] = 0;
    K[81] = 0;   // контур лага
    messageFromRos.isExist = false;
    su.resetModel();
}

void SenderWidget::send()
{
    lblYaw->setText(QString("%1").arg(QString::number(X[42][0]*180/3.14, 'f', 1)));
    lblGateIsExist->setText(messageFromRos.isExist ? "true" : "false");
    QTimer::singleShot(100, this, SLOT(send()));
    updateSendValues();
    QByteArray baDatagram;
    qtSenderUdpSocket->writeDatagram((char*)&messageToRos, sizeof (messageToRos), rosReceiverIP, rosReceiverPort);
}

void SenderWidget::receive()
{
    while (qtReceiverUdpSocket->hasPendingDatagrams()) {
        qtReceiverUdpSocket->readDatagram((char*)&messageFromRos, sizeof (messageFromRos));
    }

    if (!connectionEstablished) {
        connectionEstablished = true;
        qDebug() << "Connection established, receiving done";
        txtBrFile->append("Connection established, receiving done");
    }

    //qDebug() << "Data received";
    //updateReceivedValues();
    qDebug() << messageFromRos.isExist;

    // TODO: метод для выдачи сигнала о нахождении ворот после 10 обнаружений
}

void SenderWidget::searchForGate()
{
    K[44] = 0;
    K[43] = 1;
    K[41] = 0.2;
    gateDirection = "left";
    searchingMetod();
}

void SenderWidget::swimToGate()
{
    // set yaw = 0
    txtBrFile->append("Обнуление курса.");
    K[44] = 3.5;
    K[43] = 0;
    K[41] = 0;
    checkYaw();
}

void SenderWidget::centeringOnGate()
{
    K[64] = 0;
    K[65] = 0;
    K[63] = 0.005;
    K[61] = 0;

    K[84] = 0;
    K[85] = 0;
    K[83] = 0.005;
    K[81] = 0;
    centeringMetodLag();
    centeringMetodDepth();
}

void SenderWidget::finishMission()
{
    txtBrFile->append("Centering done. Mission complete!");
    stateMachine.stop();
}

void SenderWidget::searchingMetod()
{
    if (!messageFromRos.isExist){
        if (gateDirection == "left") {
            if (X[42][0] >= 1){
                K[41] = -0.2;
                gateDirection = "right";
            }
            QTimer::singleShot(1, this, SLOT(searchingMetod()));
        }
        else if (gateDirection == "right") {
            if (X[42][0] <= -1) {
                K[44] = 1;
                K[43] = 0;
                K[41] = 0;
                gateDirection = "none";
                qWarning() << "Gate not found";
            }
            QTimer::singleShot(1, this, SLOT(searchingMetod()));
        }
        else if (gateDirection == "none") {
            txtBrFile->append("Ворота не найдены.");
            emit gateNotFound();
        }
    }
    else
        emit gateFinded();
}

void SenderWidget::checkYaw()
{
    if (-0.01 > X[42][0] > 0.01) {
        QTimer::singleShot(1, this, SLOT(checkYaw()));
    }
    else {
        messageFromRos.isExist = false;
        QTimer::singleShot(20000, this, SLOT(swimmingMethod()));

    }

}

void SenderWidget::swimmingMethod()
{
    messageFromRos.isExist = false;
    if (gateDirection == "right"){
        txtBrFile->append("Движение по лагу к воротам. right");
        K[84] = 0;
        K[83] = 1;
        K[81] = 4;
        checkGate();
    }
    else if (gateDirection == "left"){
        txtBrFile->append("Движение по лагу к воротам. left");
        K[84] = 0;
        K[83] = 1;
        K[81] = -4;
        checkGate();
    }
}

void SenderWidget::checkGate()
{
    if (!messageFromRos.isExist)
        QTimer::singleShot(1, this, SLOT(checkGate()));
    else
        emit startCentering();
}

void SenderWidget::centeringMetodLag()
{
    if ((messageFromRos.x_center < 300) or (messageFromRos.x_center > 340)){
        K[81] = messageFromRos.x_center - 320;
        QTimer::singleShot(1, this, SLOT(centeringMetodLag()));
    }
    else {
        K[81] = 0;
    }
}

void SenderWidget::centeringMetodDepth()
{
    if ((messageFromRos.y_center < 220) or (messageFromRos.y_center > 260)){
        K[61] = -(messageFromRos.y_center - 240);
        QTimer::singleShot(1, this, SLOT(centeringMetodDepth()));
    }
    else {
        K[61] = 0;
    }
}



