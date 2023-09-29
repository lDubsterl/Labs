#ifndef STATEWINDOW_H
#define STATEWINDOW_H

#include "qserialport.h"
#include <QDialog>

namespace Ui {
class StateWindow;
}

class StateWindow : public QDialog
{
    Q_OBJECT

public:
    explicit StateWindow(QWidget *parent = nullptr);
    ~StateWindow();

public slots:
    void amountOfWrittenBytesFrom1Pair(uint amount);
    void amountOfWrittenBytesFrom2Pair(uint amount);
    void recievePorts(QSerialPort *port11, QSerialPort *port12, QSerialPort *port21, QSerialPort *port22);

private:
    Ui::StateWindow *ui;
};

#endif // STATEWINDOW_H
