#ifndef INPUTWINDOW_H
#define INPUTWINDOW_H

#include "qserialport.h"
#include "statewindow.h"
#include <QDialog>
#include <QTranslator>

class OutputWindow;

namespace Ui {
class InputWindow;
}

class InputWindow : public QDialog
{
    Q_OBJECT

public:
    explicit InputWindow(QWidget *parent = nullptr);
    ~InputWindow();
    OutputWindow *outputWindow;
    StateWindow *stateWindow;

private slots:

    void on_inputLine_returnPressed();

signals:
    void dataSent();
    void bytesWritten(uint amount);

public slots:
    void receivePorts(QSerialPort *port1, QSerialPort *port2);
    void dataRecieved();

private:
    Ui::InputWindow *ui;
};

#endif // INPUTWINDOW_H
