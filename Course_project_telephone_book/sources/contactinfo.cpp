#include "../headers/contactinfo.h"
#include <string>

void ContactInfo::setName(QString name)
{
    contactName=name;
}

QString &ContactInfo::getName()
{
    return contactName;
}

void ContactInfo::setTelephone(QString telephone)
{
    contactTelephone=telephone;
}

QString &ContactInfo::getTelephone()
{
    return contactTelephone;
}

void ContactInfo::setAddress(QString address)
{
    contactAddress=address;
}

QString &ContactInfo::getAddress()
{
    return contactAddress;
}

bool ContactInfo::operator==(ContactInfo info)
{
    if (info.contactName != contactName)
        return false;
    if (info.contactTelephone != contactTelephone)
        return false;
    if (info.contactAddress != contactAddress)
        return false;
    return true;
}

void ContactInfo::setNickname(QString newNickname)
{
    nickname = newNickname;
}

QString &ContactInfo::getNickname()
{
    return nickname;
}

QString &ContactInfo::getEmail()
{
    return email;
}

void ContactInfo::setEmail(QString newEmail)
{
    email = newEmail;
}

QTextStream& operator>>(QTextStream &in, ContactInfo &info)
{
    info.contactName = in.readLine(32768);
    info.contactTelephone = in.readLine(20);
    if (info.contactTelephone[0] != '+')
    {
        in.setStatus(QTextStream::ReadCorruptData);
        return in;
    }
    info.contactAddress = in.readLine(32768);
    info.nickname = in.readLine(32768);
    info.email = in.readLine(32768);
    in.readLine(10);
    return in;
}

QTextStream& operator<<(QTextStream &out, ContactInfo &info)
{
    out << info.contactName << Qt::endl << info.contactTelephone << Qt::endl <<  info.contactAddress << Qt::endl << info.nickname << Qt::endl << info.email << Qt::endl << Qt::endl;
    return out;
}
