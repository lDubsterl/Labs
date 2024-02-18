#ifndef CONTACTBUTTON_H
#define CONTACTBUTTON_H

#include "contactinfo.h"
#include <QPushButton>
// класс кнопки, хранящей контакт
class ContactButton: public QPushButton
{
    Q_OBJECT

public:
    ContactButton(QWidget *parent = nullptr);

    ~ContactButton() = default;

    ContactInfo getContact();

    void setContact(ContactInfo);

    ContactButton& operator=(const ContactButton& button);
    bool operator<(ContactButton& button);

    bool operator>(ContactButton& button);

    bool operator>=(ContactButton& button);

    bool operator!=(ContactButton& data);

private:
    ContactInfo contact;

    void singleClickEvent();

    QTimer *timerOfClick;

protected:
    void mousePressEvent(QMouseEvent *event) override; // перегрузка нажатий мыши

    void keyPressEvent(QKeyEvent *event) override; // перегрузка нажатий на клавиатуре

signals:
    void doubleClickEvent();

    void singleClick();

    void callContextMenu();

    void on_deleteKey_press();
};


#endif // CONTACTBUTTON_H
