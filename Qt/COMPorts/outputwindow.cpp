#include "outputwindow.h"
#include "inputwindow.h"
#include "packet.h"
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
    this->close();
    inputPort2->close();
    outputPort2->close();
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
    PacketSeq data(s);
    ui->stuffedSequence->setText(QString::fromLocal8Bit(data.getStuffedData()));
    data.destuffBytes();
    emit bytesWritten(s.size());
    ui->textEdit->append(data.getDestuffedData());
}

void OutputWindow::on_inputLine_returnPressed()
{
    ui->inputWindow->append(ui->inputLine->text());
    unsentText2.append(ui->inputLine->text());
    ui->inputLine->clear();
    PacketSeq data(unsentText2);
    data.divideToPackets(*inputPort2);
    data.stuffBytes();
    qDebug() << QString::fromLocal8Bit(data.getStuffedData());
    inputPort2->write(data.getStuffedData());
    unsentText2 = "";
    inputPort2->waitForBytesWritten();
    if (inputWindow->isVisible())
        emit dataSent();
}

