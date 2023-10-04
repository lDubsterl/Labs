#ifndef PACKET_H
#define PACKET_H
#include "qserialport.h"
#include <QByteArray>
#include <QString>

class PacketSeq
{
public:
    PacketSeq();

    PacketSeq(QString data);

    void divideToPackets(QSerialPort &port);

    QByteArray getPacketSequence();

    void destaff();

private:

    void makePacket();

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
