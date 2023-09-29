#include "mainwindow.h"
#include "inputwindow.h"
#include "outputwindow.h"
#include "statewindow.h"
#include "ui_mainwindow.h"
#include <QtSerialPort/QSerialPortInfo>

QSerialPort COM11, COM12, COM21, COM22;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    setPorts();
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setPorts()
{
   COM11.setPortName("COM3");
   COM12.setPortName("COM6");
   COM21.setPortName("COM5");
   COM22.setPortName("COM4");
   COM11.open(QSerialPort::WriteOnly);
   COM21.open(QSerialPort::WriteOnly);
   COM12.open(QSerialPort::ReadOnly);
   COM22.open(QSerialPort::ReadOnly);
}

void MainWindow::on_input_clicked()
{
    connect(this, &MainWindow::send1Pair, in, &InputWindow::receivePorts);
    emit send1Pair(&COM11, &COM22);
    in->setWindowTitle("Ввод");
    in->setModal(false);
    in->show();
}


void MainWindow::on_output_clicked()
{
    connect(this, &MainWindow::send2Pair, out, &OutputWindow::receivePorts);
    out->setWindowTitle("Вывод");
    emit send2Pair(&COM21, &COM12);
    out->setModal(false);
    out->show();
}


void MainWindow::on_ParityOff_triggered()
{
    COM11.setParity(QSerialPort::NoParity);
    COM12.setParity(QSerialPort::NoParity);
    COM21.setParity(QSerialPort::NoParity);
    COM22.setParity(QSerialPort::NoParity);
    ui->statusbar->showMessage("Проверка парита отключена");
}

void MainWindow::on_ParityEven_triggered()
{
    COM11.setParity(QSerialPort::EvenParity);
    COM12.setParity(QSerialPort::EvenParity);
    COM21.setParity(QSerialPort::EvenParity);
    COM22.setParity(QSerialPort::EvenParity);
    ui->statusbar->showMessage("Включена проверка паритета на четность");
}


void MainWindow::on_ParityOdd_triggered()
{
    COM11.setParity(QSerialPort::OddParity);
    COM12.setParity(QSerialPort::OddParity);
    COM21.setParity(QSerialPort::OddParity);
    COM22.setParity(QSerialPort::OddParity);
    ui->statusbar->showMessage("Включена проверка паритета на нечетность");
}


void MainWindow::on_state_clicked()
{
    connect(this, &MainWindow::sendAllPorts, state, &StateWindow::recievePorts);
    state->setWindowTitle("Состояние");
    state->setModal(false);
    emit sendAllPorts(&COM11, &COM12, &COM21, &COM22);
    state->show();
}
