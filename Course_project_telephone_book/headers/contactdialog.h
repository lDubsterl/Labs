#ifndef CONTACTDIALOG_H
#define CONTACTDIALOG_H

#include "contactinfo.h"
#include <QDialog>
// класс для работы с окном добавления контакта
namespace Ui {
class ContactDialog;
}

class ContactDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ContactDialog(QWidget *parent = nullptr);

    ~ContactDialog();

signals:
    void sendInfo(ContactInfo, bool addMode);

private slots:
    void on_buttonBox_accepted();

    void on_checkBox_stateChanged(int arg1);

private:
    Ui::ContactDialog *ui;

    void hideWidgets();

    void showWidgets();
};

#endif // CONTACTDIALOG_H
