#include "inputwindow.h"
#include "outputwindow.h"
#include "qtextcodec.h"
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
    inputPort1->close();
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


void InputWindow::on_lineEdit_returnPressed()
{
    ui->textEdit->append(ui->lineEdit->text());
    unsentText1.append(ui->lineEdit->text() + "\n");
    ui->lineEdit->clear();
    inputPort1->write(unsentText1.toLocal8Bit());
    unsentText1 = "";
    inputPort1->waitForBytesWritten();
    if (outputWindow->isVisible())
        emit dataSent();
}

void InputWindow::dataRecieved()
{
    outputPort1->waitForReadyRead();
    auto s = outputPort1->readAll();
    emit bytesWritten(s.size());
    QTextCodec *tc = QTextCodec::codecForName("Windows-1251");
    ui->outputWindow->append(tc->toUnicode(s));
}
