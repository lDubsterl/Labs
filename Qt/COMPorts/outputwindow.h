#ifndef OUTPUTWINDOW_H
#define OUTPUTWINDOW_H

#include "qserialport.h"
#include "statewindow.h"
#include <QDialog>

class InputWindow;

namespace Ui {
class OutputWindow;
}

class OutputWindow : public QDialog
{
    Q_OBJECT

public:
    explicit OutputWindow(QWidget *parent = nullptr);
    ~OutputWindow();
    StateWindow *stateWindow;
    InputWindow *inputWindow;

signals:
    void bytesWritten(uint amount);
    void dataSent();

public slots:
    void receivePorts(QSerialPort *port1, QSerialPort *port2);
    void dataRecieved();

private slots:
    void on_inputLine_returnPressed();

private:
    Ui::OutputWindow *ui;
};

#endif // OUTPUTWINDOW_H
