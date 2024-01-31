#include "../headers/mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv); // экземпляр приложения
    MainWindow w; // создание экземпляра главного окна
    w.show(); // показ окна
    return a.exec(); // выполнение приложения
}
