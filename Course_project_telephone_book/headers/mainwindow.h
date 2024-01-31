#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "contactButton.h"
#include <QMainWindow>
#include <QMouseEvent>
#include "List.h"
#include <QFile>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE
// класс для работы с основным окном
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

    ~MainWindow();

private slots:
    void readFromFile();

    void saveFile();

    void saveAs();

    void on_addButton_clicked();

    void on_singleClicked();

    void on_rightClicked();

    void on_checkBox_stateChanged(int arg1);

    void editContact(ContactInfo, ContactButton*);

    void editContactRequest(ContactButton*);

    void on_lineEdit_textChanged(const QString &arg1);

signals:
     void signalShowContact(ContactInfo);

     void on_menuEditContact_click(ContactButton *);

public slots:
    void slotReceiveData(ContactInfo info, bool addMode);

    void deleteContactSlot(ContactButton*);

    void on_doubleClicked();

private:
     List<ContactButton*> list; // объект списка

    QFile file;

    void closeEvent(QCloseEvent *event) override;

    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
