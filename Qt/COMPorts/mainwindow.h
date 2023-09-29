#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "inputwindow.h"
#include "outputwindow.h"
#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include "statewindow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    OutputWindow *out;
    InputWindow *in;
    StateWindow *state;

signals:
    void send1Pair(QSerialPort *port1, QSerialPort *port2);
    void send2Pair(QSerialPort *port1, QSerialPort *port2);
    void sendAllPorts(QSerialPort *port11, QSerialPort *port12, QSerialPort *port21, QSerialPort *port22);

private slots:

    void on_input_clicked();

    void on_output_clicked();

    void on_ParityOff_triggered();

    void on_ParityEven_triggered();

    void on_ParityOdd_triggered();

    void on_state_clicked();

private:
    Ui::MainWindow *ui;
    void setPorts();
};

#endif // MAINWINDOW_H
