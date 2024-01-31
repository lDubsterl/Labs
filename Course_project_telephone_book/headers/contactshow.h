#ifndef CONTACTSHOW_H
#define CONTACTSHOW_H

#include "contactinfo.h"
#include <QDialog>

namespace Ui {
class ContactShow;
}
// класс для работы с окном просмотра контактов
class ContactShow : public QDialog
{
    Q_OBJECT

public:
    explicit ContactShow(QWidget *parent = nullptr);

    ~ContactShow();

public slots:
    void slotShowContact(ContactInfo);

private slots:
    void on_checkBox_stateChanged(int arg1);

private:
    Ui::ContactShow *ui;

    void hideWidgets();

    void showWidgets();
};

#endif // CONTACTSHOW_H
