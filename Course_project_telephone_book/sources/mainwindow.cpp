#include "../headers/mainwindow.h"
#include "../headers/contactdialog.h"
#include "../headers/contactedit.h"
#include "../headers/contactshow.h"
#include "../headers/fileexception.h"
#include "qfiledialog.h"
#include "ui_mainwindow.h"
#include "../headers/contactButton.h"
#include <QTimer>
#include <QGraphicsOpacityEffect>
#include <QMessageBox>

void MainWindow::readFromFile()
{
    QFileDialog fileName;
    QString name;
    name = fileName.getOpenFileName(this, "Открыть файл", "", "Файл контактов (*.txt *.cont)"); // получение полного пути файлла
    file.setFileName(name); // установка пути файла
    try
    {
    if (!file.open(QIODevice::ReadWrite)) // попытка открытия файла в режиме чтения/записи
        throw FileException<MainWindow>(Errors::fileIsNotOpen, this, "Не удалось открыть файл");
    if (list.begin() != nullptr) // если в списке есть кнопки - он очищается
    {
        for (MyIterator it = list.begin(); it != list.end() + 1; ++it)
               deleteContactSlot(*it);
    }
    QTextStream in(&file);
    ContactInfo element, emptyElement;
    while (true)
    {
        if(in.atEnd()) // считывание до символа конца файла
            break;
        in >> element; // ввод данных из файла
        if (in.status() == QTextStream::ReadCorruptData)
            throw FileException<MainWindow>(Errors::incorrectInput, this, "Неверная структура файла.\nВозможно, он поврежден");
        slotReceiveData(element, 0); // добавление кнопки в окно
    }
    ui->statusBar->showMessage("Файл открыт", 4000); // вывод сообщения в строку состояния
    setWindowTitle("Телефонный справочник" " - " + name);
    }
    catch(FileException<MainWindow> &obj)
    {
        if (obj.getErrorCode() == Errors::incorrectInput)
        {
            int count = ui->verticalLayout->count();
            for (int i = 0; i < count; i++)
            {
                list.deleteElement(0);
                ContactButton *btn=qobject_cast<ContactButton*>(ui->verticalLayout->itemAt(0)->widget()); // выбор кнопки из окна
                btn->hide(); // скрытие кнопки
                delete btn;
            }
        }
        obj.show();
        return;
    }

}

void MainWindow::saveAs()
{
    QFileDialog fileName;
    QString name;
    name = fileName.getSaveFileName(this, "Сохранить файл", "", "Файл контактов (*.cont *.txt)");
    file.setFileName(name);
    if (!file.open(QIODevice::WriteOnly)) // открытие файла в режиме записи
    {
        QMessageBox::warning(this, "Внимание","Не удалось сохранить файл");
        return;
    }
    QTextStream out(&file);
    for (MyIterator it = list.begin(); it != list.end() + 1; ++it)
    {
        ContactInfo element = (*it)->getContact(); // получение элемента из списка
        out << element; // запись его в файл
    }
    ui->statusBar->showMessage("Файл сохранен", 4000); // вывод сообщения в окно состояния
}

void MainWindow::saveFile()
{
    if (file.isOpen())
    {
        QTextStream out(&file);
        file.resize(0); // установка размера файла 0
        for (MyIterator it = list.begin(); it != list.end() + 1; ++it)
        {
            ContactInfo element = (*it)->getContact();
            out << element;
        }
    }
    else
        saveAs(); // сохранить файл как...
    ui->statusBar->showMessage("Файл сохранен", 4000);
}

void MainWindow::closeEvent(QCloseEvent *event) // перегрузка метода закрытия окна через "крестик"
{
    event->ignore(); // игнорирование закрытия
    QMessageBox mes;
    QAbstractButton *yes = mes.addButton("Да", QMessageBox::YesRole); // создание кнопки подтверждения
    mes.addButton("Нет", QMessageBox::NoRole); // добавление ее в диалоговое окно
    mes.setText("Вы уверены?\n""Несохраненные данные будут утеряны");
    mes.exec(); // вызов окна
    if(mes.clickedButton() == yes) // если кнопка подтверждения
        event->accept(); // закрытие окна
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Телефонный справочник");
    auto * container = new QWidget();
    ui->scrollArea->setWidget(container); // установка виджета в виджет прокрутки
    ui->verticalLayout = new QVBoxLayout(container); // установка родительского виджета для layout
    ui->checkBox->hide(); // скрытие кнопки выбора сортировки
    connect(ui->menu_3, &QMenu::aboutToShow, this, &MainWindow::readFromFile); // соединение кнопок с действием
    connect(ui->menu_4, &QMenu::aboutToShow, this, &MainWindow::saveFile);
    connect(ui->menu_5, &QMenu::aboutToShow, this, &MainWindow::saveAs);
    ui->statusBar->setStyleSheet("QStatusBar{border-top: 1px outset grey;}"); // установка толщины границы статусной строки
    setWindowIcon(QIcon(":/images/sprav.png")); // установка иконки окна
}
MainWindow::~MainWindow()
{
    int count = ui->verticalLayout->count();
    for (int i = 0; i < count; i++)
    {
        list.deleteElement(0);
        ContactButton *btn=qobject_cast<ContactButton*>(ui->verticalLayout->itemAt(0)->widget()); // выбор кнопки из окна
        btn->hide(); // скрытие кнопки
        delete btn;
    }
    delete ui; // удаление окна
    file.close(); // закрытие файла
}

void MainWindow::slotReceiveData(ContactInfo info, bool addMode)
{
    if(ui->verticalLayout->count() == 0)
        ui->checkBox->show(); // если в окне нет кнопок - скрытие кнопки сортировки
    ContactButton *btn = new ContactButton;
    btn->setText(info.getName());
    btn->setContact(info);
    btn->setContextMenuPolicy(Qt::CustomContextMenu); // установка политики контекстного меню
    QFont font;
    font.setFamily("Comic Sans MS");
    font.setPixelSize(12);
    btn->setFont(font); // установка шрифта
    bool isExist = false; // кнопка не существует
    if (list.begin() != nullptr && !addMode) // если список не пустой и контакт добавляется с проверкой его существования
    for (MyIterator it = list.begin(); it != list.end() + 1; ++it)
        if ((*it)->getContact().getName() == btn->getContact().getName() &&
                (*it)->getContact().getTelephone() == btn->getContact().getTelephone()) // если имя и телефон совпадают с существующим контактом
        {
            isExist = true;
            break;
        }
    if (!isExist && !addMode) // если контакт не существует и была проверка его существования
    {
        ui->verticalLayout->addWidget(btn); // добавление контакта в окно
        list.push(btn); // добавление контакта в список
    }
    if (addMode) // если проверка существования не нужна
        ui->verticalLayout->addWidget(btn);
    connect(btn, &ContactButton::singleClick, this, &MainWindow::on_singleClicked);
    connect(btn, &ContactButton::doubleClickEvent, this, &MainWindow::on_doubleClicked);
    connect(btn, &ContactButton::customContextMenuRequested, this, &MainWindow::on_rightClicked);
}

void MainWindow::on_singleClicked() // действие при одинарном нажатии
{
    ContactButton *btn = (ContactButton*)sender(); // получение нажатой кнопки
    btn->setStyleSheet("background-color: rgb(118, 139, 249); color: white"); // установка цвета заднего фона и шрифта
    connect(btn, &ContactButton::on_deleteKey_press, [this, btn]{(deleteContactSlot(btn));});
}

void MainWindow::on_doubleClicked() // действие при двойном нажатии
{
    ContactButton *btn = (ContactButton*)sender();
    btn ->setStyleSheet(styleSheet()); // обнуление стиля
    ContactShow contactShow;
    connect(this, &MainWindow::signalShowContact, &contactShow, &ContactShow::slotShowContact);
    emit signalShowContact(btn->getContact()); // вызов сигналом действия
    contactShow.exec(); // вызов окна
}

void MainWindow::on_addButton_clicked()// действие при нажатии кнопки добавления
{
    ContactDialog contactDialog;
    connect(&contactDialog, &ContactDialog::sendInfo, this, &MainWindow::slotReceiveData);
    contactDialog.exec(); // вызов окна
}

void MainWindow::on_rightClicked()
{
    QMenu* menu = new QMenu; // создание контекстного меню
    QAction* deleteContact = new QAction("Удалить", this); // действие удалить
    menu->addAction(deleteContact); // добавление действия в меню
    QAction* editContact = new QAction("Редактировать", this); // действие редактировать
    menu->addAction(editContact);
    QPoint pos = QCursor::pos(); // получение позиции курсора при нажатии на пкм
    menu->popup(pos); // вызов меню в месте курсора
    ContactButton *btn = (ContactButton*)sender();
    connect(deleteContact, &QAction::triggered, this, [this, btn]{(deleteContactSlot(btn));}); // соединение действия и нажатия на кнопку
    connect(editContact, &QAction::triggered, this, [this, btn]{(editContactRequest(btn));});
}
void MainWindow::deleteContactSlot(ContactButton* btn) // удаление контакта
{
    if(ui->verticalLayout->count() == 1) // если удаляется последние контакт - сокрытие кнопки сортировки
        ui->checkBox->hide();
    btn->hide(); // сокрытие контакта
    int i = 0;
    for (MyIterator it = list.begin(); it != list.end() + 1; i++)
    {
        if (*it == btn) // если контакт в окне равен контакту в списке
        {
            --it;
            list.deleteElement(i); // удаление контакта из списка
            break;
        }
        ++it;
    }
    ui->verticalLayout->removeWidget(btn); // удаление контакта из окна
    delete btn;
}

void MainWindow::editContactRequest(ContactButton* btn) // запрос на редактирование контакта
{
    ContactEdit contactEdit;
    connect(this, &MainWindow::on_menuEditContact_click, &contactEdit, &ContactEdit::on_menuEditContact_clicked);
    connect(&contactEdit, &ContactEdit::acceptEditing, this, &MainWindow::editContact);
    emit on_menuEditContact_click(btn); // сигнал редактирования контакта
    contactEdit.exec(); // вызов окна
}

void MainWindow::on_checkBox_stateChanged(int boxState) // нажатие на кнопку сортировки
{
    if (list.begin() == nullptr)
        return; // если список пустой - выход
    int amount = ui->verticalLayout->count();
    for (int i = 0; i < amount; i++) // удаление всех контактов из окна
    {
        ContactButton *btn=qobject_cast<ContactButton*>(ui->verticalLayout->itemAt(0)->widget()); // получение контакта из окна
        ui->verticalLayout->removeWidget(btn);
        btn->hide();
    }
    auto ascending = [](ContactButton* a, ContactButton* b) // функция критерия сортировки
    {
        if (QString::compare(a->getContact().getName(), b->getContact().getName()) < 0 )
            return true;
        else
            return false;
    };
    auto descending = [](ContactButton* a, ContactButton* b)
    {
        if (QString::compare(a->getContact().getName(), b->getContact().getName()) > 0 )
            return true;
        else
            return false;
    };
    if (boxState == 0) // сортировка  списка по возрастанию
        algo::sort(list.begin(), list.end(), *ascending);
    else
        algo::sort(list.begin(), list.end(), *descending); // сортировка списка по убыванию
    for (MyIterator it = list.begin(); it != list.end() + 1; ++it)
        slotReceiveData((*it)->getContact(), 1); // добавление контакта из списка в окно без проверки существования
}

void MainWindow::editContact(ContactInfo info, ContactButton* btn) // редактирование контакта
{
    int i = 0;
    for (MyIterator it = list.begin(); it != list.end() + 1; ++it, i++)
    {
        ContactInfo check = qobject_cast<ContactButton*>(ui->verticalLayout->itemAt(i)->widget())->getContact(); // получение контакта из окна
        if (btn->getContact() == check) // если запрошенный для редактирования контакт совпадает с контактом в окне
        {
            btn->setContact(info); // запись информации в окно
            algo::editElement(it, btn); // изменение контакта в списке
            break;
        }
    }
    btn->setText(info.getName()); // установка имени контакта на кнопку
}

bool strIsEqual(QString lineEditText, QString name) // сравнение строк
{
    lineEditText = lineEditText.toLower();
    name = name.toLower(); // преобразование к нижнему регистру
    int strSize = lineEditText.size();
    strSize = name.size() < strSize ? name.size() : strSize; // выбор минимальной длины
    for (int i = 0; i < strSize; i++) // сравнение символов
        if (lineEditText[i] != name[i])
            return false;
    return true;
}

void MainWindow::on_lineEdit_textChanged(const QString &arg1) // изменение текста в строке поиска контактов
{
    for (MyIterator it = list.begin(); it != list.end() + 1; ++it)
            if (!strIsEqual(arg1, it.getPointer()->info->getContact().getName())  && !strIsEqual(arg1, it.getPointer()->info->getContact().getTelephone()))
                (*it)->hide(); // если контакт соответствует введенному в поиск - спрятать его
            else
                (*it)->show(); // иначе - показать
}
