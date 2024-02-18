#include "../headers/contactshow.h"
#include "../headers/contactinfo.h"
#include "ui_contactshow.h"

ContactShow::ContactShow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ContactShow)
{
    ui->setupUi(this); // установка граф окна
    setWindowIcon(QIcon(":/images/show.png"));
    hideWidgets(); // сокрытие доп информации
    setWindowTitle("Информация о контакте");
}

void ContactShow::hideWidgets()
{
    ui->nickLineEdit->hide();
    ui->mailLineEdit->hide();
    ui->nickLabel->hide();
    ui->mailLabel->hide();
}

void ContactShow::showWidgets()
{
    ui->nickLineEdit->show();
    ui->mailLineEdit->show();
    ui->nickLabel->show();
    ui->mailLabel->show();
}

ContactShow::~ContactShow()
{
    delete ui;
}

void ContactShow::slotShowContact(ContactInfo info) // установка в поля данных контакта
{
    ui->nameLabel->setText(info.getName());
    ui->telephoneLabel->setText(info.getTelephone());
    ui->addressLabel->setText(info.getAddress());
    ui->nickLineEdit->setText(info.getNickname());
    ui->mailLineEdit->setText(info.getEmail());
}

void ContactShow::on_checkBox_stateChanged(int boxState) // изменение состояние кнопки раскрытия доп информации
{
    if (boxState == 2)
        showWidgets();
    else
        hideWidgets(); // показать доп информацию
}
