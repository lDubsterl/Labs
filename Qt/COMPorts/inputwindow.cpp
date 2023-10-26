#include "inputwindow.h"
#include "outputwindow.h"
#include "packet.h"
#include "ui_inputwindow.h"

QSerialPort* inputPort1;
QSerialPort* outputPort1;

InputWindow::InputWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InputWindow)
{
    ui->setupUi(this);
    ui->textEdit->setReadOnly(true);
}

InputWindow::~InputWindow()
{
    this->close();
    inputPort1->close();
    outputPort1->close();
    delete ui;
}

QString unsentText1;

void InputWindow::receivePorts(QSerialPort *port1, QSerialPort *port2)
{
    connect(this, &InputWindow::dataSent, outputWindow, &OutputWindow::dataRecieved);
    connect(this, &InputWindow::bytesWritten, stateWindow, &StateWindow::amountOfWrittenBytesFrom2Pair);
    inputPort1 = port1;
    outputPort1 = port2;
    inputPort1->setBaudRate(QSerialPort::Baud115200);
    outputPort1->setBaudRate(QSerialPort::Baud115200);
}


void InputWindow::on_inputLine_returnPressed()
{
    ui->textEdit->append(ui->inputLine->text());
    unsentText1.append(ui->inputLine->text());
    ui->inputLine->clear();
    PacketSeq data(unsentText1);
    data.divideToPackets(*inputPort1);
    data.stuffBytes();
    qDebug() << QString::fromLocal8Bit(data.getStuffedData());
    inputPort1->write(data.getStuffedData());
    unsentText1 = "";
    inputPort1->waitForBytesWritten();
    if (outputWindow->isVisible())
        emit dataSent();
    else
        unsentText1.append('\n');
}

void InputWindow::dataRecieved()
{
    outputPort1->waitForReadyRead();
    auto s = outputPort1->readAll();
    PacketSeq data(s);
    ui->stuffedSequence->setText(QString::fromLocal8Bit(data.getStuffedData()));
    data.destuffBytes();
    emit bytesWritten(s.size());
    ui->outputWindow->append(data.getDestuffedData());
}
