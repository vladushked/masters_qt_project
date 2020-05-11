#include <QApplication>
#include "senderwidget.h"
#include "su_rov.h"

double X[2000][2];

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SenderWidget w;
    w.show();
    return a.exec();
}
