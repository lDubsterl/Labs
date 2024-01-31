#ifndef FILEEXCEPTION_H
#define FILEEXCEPTION_H

#include "exception.h"
// класс исключений файлов
template <class T>
class FileException :
    public Exception
{
    T* window;

public:
    FileException<T>(Errors code, T* window, QString message);

    void show() override;

    Errors getErrorCode()
    {
        return code;
    }
};

template <class T>
FileException<T>::FileException(Errors code, T* window, QString message)
{
    this->code = code;
    this->window = window;
    this->message = message;
}

template <class T>
void FileException<T>::show()
{
    QMessageBox::warning(window, "Ошибка " + QString::number((int)code), message);
}

#endif // FILEEXCEPTION_H
