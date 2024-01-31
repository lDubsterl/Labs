#include "../headers/contactedit.h"
#include "../headers/contactButton.h"
#include "../headers/contactinfo.h"
#include "../headers/inputexception.h"
#include "ui_contactedit.h"

ContactEdit::ContactEdit(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ContactEdit)
{
    ui->setupUi(this);
    connect(ui->buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked(bool)), this, SLOT(on_buttonBox_accepted()));
    ui->telephoneLineEdit->setInputMask("+999 (99) 999 99 99;_"); // установка маски ввода
    setWindowIcon(QIcon(":/images/edit.png"));
    hideWidgets();
    setWindowTitle("Редактировать контакт");
}

ContactEdit::~ContactEdit()
{
    delete ui;
}

void ContactEdit::hideWidgets()
{
    ui->nickLineEdit->hide();
    ui->mailLineEdit->hide();
    ui->nickLabel->hide();
    ui->mailLabel->hide();
}

void ContactEdit::showWidgets()
{
    ui->nickLineEdit->show();
    ui->mailLineEdit->show();
    ui->nickLabel->show();
    ui->mailLabel->show();
}

void ContactEdit::on_buttonBox_accepted()
{
    try
    {
        if (ui->nameLineEdit->text() == "" && ui->telephoneLineEdit->text() == "+ ()   ") // если имя или телефон не введены
            throw InputException(Errors::emptyLines, this, "Некорректный ввод");
        ContactInfo info;
        info.setName(ui->nameLineEdit->text());
        info.setTelephone(ui->telephoneLineEdit->text());
        info.setAddress(ui->addressLineEdit->text());
        info.setNickname(ui->nickLineEdit->text());
        info.setEmail(ui->mailLineEdit->text());
        emit acceptEditing(info, button); // подтверждение редактирования
        accept();// подтверждение действия в окне
    }
    catch(InputException<ContactEdit> &obj)
    {
        obj.show();
        return;
    }
}

void ContactEdit::on_menuEditContact_clicked(ContactButton* btn) // ввод информации о контакте в поля
{
    ui->nameLineEdit->setText(btn->getContact().getName());
    ui->telephoneLineEdit->setText(btn->getContact().getTelephone());
    ui->addressLineEdit->setText(btn->getContact().getAddress());
    ui->nickLineEdit->setText(btn->getContact().getNickname());
    ui->mailLineEdit->setText(btn->getContact().getEmail());
    button = btn;
}

void ContactEdit::on_checkBox_stateChanged(int boxState)
{
    if (boxState == 2)
        showWidgets();
    else
        hideWidgets();
}
