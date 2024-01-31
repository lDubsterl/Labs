#include "../headers/contactButton.h"
#include "qevent.h"
#include "qtimer.h"

ContactButton::ContactButton(QWidget *parent) :
    QPushButton(parent)
{
    timerOfClick = new QTimer();
    timerOfClick->setInterval(300); // установка таймера
    connect(timerOfClick, &QTimer::timeout, this, &ContactButton::singleClickEvent);
}

bool ContactButton::operator<(ContactButton& button)
{
    if (button.contact.getName() > contact.getName())
        return true;
    if (button.contact.getAddress() > contact.getAddress())
        return true;
    if (button.contact.getTelephone() > contact.getTelephone())
        return true;
    if (button.contact.getNickname() > contact.getNickname())
        return true;
    if (button.contact.getEmail() > contact.getEmail())
        return true;
    return false;
}

bool ContactButton::operator>(ContactButton& button)
{
    if (button.contact.getName() < contact.getName())
        return true;
    if (button.contact.getAddress() < contact.getAddress())
        return true;
    if (button.contact.getTelephone() < contact.getTelephone())
        return true;
    if (button.contact.getNickname() < contact.getNickname())
        return true;
    if (button.contact.getEmail() < contact.getEmail())
        return true;
    return false;
}

bool ContactButton::operator>=(ContactButton& button)
{
    if (button.contact.getName() >= contact.getName())
        return true;
    if (button.contact.getAddress() >= contact.getAddress())
        return true;
    if (button.contact.getTelephone() >= contact.getTelephone())
        return true;
    if (button.contact.getNickname() >= contact.getNickname())
        return true;
    if (button.contact.getEmail() >= contact.getEmail())
        return true;
    return false;
}

bool ContactButton::operator!=(ContactButton& data)
{
    if (contact.getName() != data.contact.getName())
        return true;
    if (contact.getAddress() != data.contact.getAddress())
        return true;
    if (contact.getTelephone() != data.contact.getTelephone())
        return true;
    if (contact.getNickname() != data.contact.getNickname())
        return true;
    if (contact.getEmail() != data.contact.getEmail())
        return true;
    return false;
}

ContactInfo ContactButton::getContact()
{
    return contact;
}

void ContactButton::setContact(ContactInfo info)
{
    contact.setName(info.getName());
    contact.setTelephone(info.getTelephone());
    contact.setAddress(info.getAddress());
    contact.setNickname(info.getNickname());
    contact.setEmail(info.getEmail());
}

void ContactButton::mousePressEvent(QMouseEvent *event) // перегрузка нажатия кнопок мыши
{
    if (event->button() == Qt::LeftButton)
    {
        if (timerOfClick->isActive()) // если время таймера не истекло
        {
            timerOfClick->stop(); // остановка таймера
            emit doubleClickEvent(); // вызов дабл клика
        }
        else
            timerOfClick->start(); // старт таймера сначала
    }
    else
        if(event->button() == Qt::RightButton)
            emit callContextMenu(); // вызов контекстного меню на пкм
}

void ContactButton::keyPressEvent(QKeyEvent *event) // перегрузка нажатия на кнопки клавиатуры
{
    if (event->key() == Qt::Key_Delete) // перегрузка нажатия на delete
        emit on_deleteKey_press(); // удаление контакта
}

void ContactButton::singleClickEvent() // синг клик по лкм
{
    timerOfClick->stop(); // остановка таймера
    emit singleClick(); // вызов действия
}

ContactButton& ContactButton::operator=(const ContactButton& button)
{
    contact = button.contact;
    timerOfClick = button.timerOfClick;
    return *this;
}
