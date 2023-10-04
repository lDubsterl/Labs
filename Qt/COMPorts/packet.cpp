#include "packet.h"
#include "qserialport.h"

PacketSeq::PacketSeq()
{

}

PacketSeq::PacketSeq(QString data)
{
    for (int i = 0; i < data.size(); i++)
    {
        if (data[i].cell() == Packet::flag)
        {
            data.insert(i - 1, '*');
            i++;
            continue;
        }
        if (data[i].cell() == '*')
        {
            data.insert(i - 1, '*');
            i++;
        }
    }
    rawData = data;
}

uchar PacketSeq::Packet::flag = 'z' + 29;

PacketSeq::Packet::Packet(QSerialPort &port)
{
    sourceAddress = port.portName().back().cell();
}

QByteArray PacketSeq::Packet::convertToByteArray()
{
    QByteArray arr;
    arr.append(flag + destinationAddress + sourceAddress + data + fcs);
    return arr;
}

void PacketSeq::divideToPackets(QSerialPort &port)
{
    Packet packet(port);
    while (!rawData.isEmpty())
    {
        if (rawData.size() > 29)
        {
            packet.data = rawData.first(29).toLocal8Bit();
            rawData = rawData.remove(0, 29);
            dataSeq.append(packet.convertToByteArray());
        }
        else
        {
            packet.data = rawData.toLocal8Bit();
            rawData = rawData.remove(0, rawData.size());
            dataSeq.append(packet.convertToByteArray());
        }
    }
}

QByteArray PacketSeq::getPacketSequence()
{
    return dataSeq;
}

void PacketSeq::destaff()
{

}
