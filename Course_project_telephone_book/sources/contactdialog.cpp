#include "../headers/contactdialog.h"
#include <QPushButton>
#include "../headers/inputexception.h"
#include "ui_contactdialog.h"
#include "../headers/contactinfo.h"

ContactDialog::ContactDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ContactDialog)
{
    ui->setupUi(this);
    connect(ui->buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked(bool)), this, SLOT(on_buttonBox_accepted()));
    ui->telephoneLineEdit->setInputMask("+999 (99) 999 99 99;_");
    setWindowIcon(QIcon(":/images/plus.png"));
    hideWidgets();
    setWindowTitle("Добавить контакт");
}

void ContactDialog::hideWidgets()
{
    ui->nickLineEdit->hide();
    ui->mailLineEdit->hide();
    ui->nickLabel->hide();
    ui->mailLabel->hide();
}

void ContactDialog::showWidgets()
{
    ui->nickLineEdit->show();
    ui->mailLineEdit->show();
    ui->nickLabel->show();
    ui->mailLabel->show();
}


ContactDialog::~ContactDialog()
{
    delete ui;
}

void ContactDialog::on_buttonBox_accepted()
{
    try
    {
        if (ui->nameLineEdit->text() == "" && ui->telephoneLineEdit->text() == "+ ()   ")
            throw InputException(Errors::emptyLines, this, "Некорректный ввод");
    ContactInfo info;
    info.setName(ui->nameLineEdit->text());
    info.setTelephone(ui->telephoneLineEdit->text());
    info.setAddress(ui->addressLineEdit->text());
    info.setNickname(ui->nickLineEdit->text());
    info.setEmail(ui->mailLineEdit->text());
    emit sendInfo(info, 0);
        accept();
    }
    catch(InputException<ContactDialog> &obj)
    {
        obj.show();
        return;
    }
}





void ContactDialog::on_checkBox_stateChanged(int boxState)
{
    if (boxState == 2)
        showWidgets();
    else
        hideWidgets();
}

