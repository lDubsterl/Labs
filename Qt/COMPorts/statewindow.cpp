#include "statewindow.h"
#include "ui_statewindow.h"

StateWindow::StateWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StateWindow)
{
    ui->setupUi(this);
}

StateWindow::~StateWindow()
{
    delete ui;
}

void StateWindow::amountOfWrittenBytesFrom1Pair(uint amount)
{
    ui->label->setText("Байт записано: " + QString::number(amount));
    //ui->label_2->setText("Байт прочитано: " + QString::number(amount));
}

void StateWindow::amountOfWrittenBytesFrom2Pair(uint amount)
{
    ui->label_2->setText("Байт записано: " + QString::number(amount));
    //ui->label_2->setText("Байт прочитано: " + QString::number(amount));
}

void StateWindow::recievePorts(QSerialPort *port1, QSerialPort *port2, QSerialPort *port3, QSerialPort *port4)
{
    ui->Pair1->setText(port1->portName() + "->" + port2->portName());
    ui->Pair2->setText(port4->portName() + "<-" + port3->portName());

}
