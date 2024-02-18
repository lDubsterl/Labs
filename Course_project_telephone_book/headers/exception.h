#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <QString>
#include <QMessageBox>
// основной класс исключений
enum class Errors
{
    emptyLines = 100,

    fileIsNotOpen = 102,

    incorrectInput = 103,
};

class Exception
{
protected:

    Errors code;

    QString message;

    virtual void show() = 0;

};

#endif // EXCEPTION_H
