#ifndef INPUTEXCEPTION_H
#define INPUTEXCEPTION_H

#include "exception.h"

// класс исключений ввода
template <class T>
class InputException :
    public Exception
{   
    T* window;

public:
    InputException<T>(Errors code, T* window, QString message);

    void show() override;
};

template <class T>
InputException<T>::InputException(Errors code, T* window, QString message)
{
    this->code = code;
    this->window = window;
    this->message = message;
}

template <class T>
void InputException<T>::show()
{
    QMessageBox::warning(window, "Ошибка " + QString::number((int)code), message + "\nХотя бы имя или телефон должны быть заполнены"); // вызов предупреждающего окна
}
#endif // INPUTEXCEPTION_H
