#include "mainwindow.h"

#include <QApplication>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QTextCodec>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.in = new InputWindow(&w);
    w.out = new OutputWindow(&w);
    w.state = new StateWindow(&w);
    w.out->stateWindow = w.state;
    w.in->stateWindow = w.state;
    w.in->outputWindow = w.out;
    w.out->inputWindow = w.in;
    w.show();
    return a.exec();
}
