#ifndef CONTACTINFO_H
#define CONTACTINFO_H

#include <QString>
#include <QTextStream>
// класс информации о контакте

class ContactInfo
{
public:
    ContactInfo() = default;

    void setName(QString);

    QString &getName();

    void setTelephone(QString);

    QString &getTelephone();

    void setAddress(QString);

    QString &getAddress();

    void setNickname(QString newNickname);

    QString &getNickname();

    void setEmail(QString newEmail);

    QString &getEmail();

    friend QTextStream &operator>>(QTextStream &in, ContactInfo &info);

    friend QTextStream &operator<<(QTextStream &out, ContactInfo &info);

    bool operator==(ContactInfo info);

private:
    QString contactName;

    QString contactAddress;

    QString contactTelephone;

    QString nickname;

    QString email;
};

#endif // CONTACTINFO_H
