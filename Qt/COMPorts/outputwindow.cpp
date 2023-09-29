#include "outputwindow.h"
#include "inputwindow.h"
#include "ui_outputwindow.h"
#include <QTextCodec>

QSerialPort* inputPort2;
QSerialPort* outputPort2;

OutputWindow::OutputWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OutputWindow)
{
    ui->setupUi(this);
}

OutputWindow::~OutputWindow()
{
    delete ui;
}

QString unsentText2;

void OutputWindow::receivePorts(QSerialPort *port1, QSerialPort *port2)
{
    outputPort2 = port2;
    inputPort2 = port1;
    connect(this, &OutputWindow::bytesWritten, stateWindow, &StateWindow::amountOfWrittenBytesFrom1Pair);
    connect(this, &OutputWindow::dataSent, inputWindow, &InputWindow::dataRecieved);
}

void OutputWindow::dataRecieved()
{
    outputPort2->waitForReadyRead();
    auto s = outputPort2->readAll();
    emit bytesWritten(s.size());
    QTextCodec *tc = QTextCodec::codecForName("Windows-1251");
    ui->textEdit->append(tc->toUnicode(s));
}

void OutputWindow::on_inputLine_returnPressed()
{
    ui->inputWindow->append(ui->inputLine->text());
    unsentText2.append(ui->inputLine->text() + "\n");
    ui->inputLine->clear();

    inputPort2->write(unsentText2.toLocal8Bit());
    unsentText2 = "";
    inputPort2->waitForBytesWritten();
    if (inputWindow->isVisible())
        emit dataSent();
}

