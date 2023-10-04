#ifndef PACKET_H
#define PACKET_H
#include "qserialport.h"
#include <QByteArray>
#include <QString>

class PacketSeq
{
public:

    PacketSeq(QString data);

    PacketSeq(QByteArray packetSeq);

    void divideToPackets(QSerialPort &port);

    void stuffBytes();

    void destuffBytes();

    QByteArray getStuffedData();

    QString getDestuffedData();
private:

    QString rawData;

    QByteArray dataSeq;


    class Packet
    {
    public:
        Packet();

        Packet(QSerialPort &port);

        static uchar flag;

        uchar destinationAddress = 0;

        uchar sourceAddress = 0;

        QByteArray data;

        uchar fcs = 0;

        QByteArray convertToByteArray();
    };
};

#endif // PACKET_H
