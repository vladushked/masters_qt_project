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
        //считаем из объекта IP адрес и порт отправителя файла:
        senderIP.setAddress(jsonData.value("sender.ip").toString());
        senderPort = jsonData.value("sender.port").toInt();
        //считаем IP адрес и порт получателя файла:
        receiverIP.setAddress(jsonData.value("receiver.ip").toString());
        receiverPort=jsonData.value("receiver.port").toInt();
        //выведем полученные значения в окно приложения
        lblSenderIPport->setText(senderIP.toString()+":"+QString::number(senderPort));
        lblReceiverIPport->setText(receiverIP.toString()+":"+QString::number(receiverPort));
    }
    else {
        qDebug()<<"can't open config.file";
    }

    //web widget
    widget->load(QUrl(QStringLiteral("https://www.qt.io")));
}
