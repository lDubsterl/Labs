#ifndef CONTACTEDIT_H
#define CONTACTEDIT_H

#include "contactButton.h"
#include <QDialog>
// класс для работы с окном редактирования контакта
namespace Ui {
class ContactEdit;
}

class ContactEdit : public QDialog
{
    Q_OBJECT

public:
    explicit ContactEdit(QWidget *parent = nullptr);

    ~ContactEdit();

private slots:
    void on_buttonBox_accepted();

    void on_checkBox_stateChanged(int arg1);

public slots:
    void on_menuEditContact_clicked(ContactButton*);

signals:
    void acceptEditing(ContactInfo, ContactButton*);

private:
    Ui::ContactEdit *ui;

    ContactButton* button;

    void hideWidgets();

    void showWidgets();
};

#endif // CONTACTEDIT_H
