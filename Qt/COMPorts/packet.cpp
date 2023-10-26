#include "packet.h"
#include "qserialport.h"

PacketSeq::PacketSeq(QString data)
{
    rawData = data;
}

PacketSeq::PacketSeq(QByteArray packetSeq)
{
    dataSeq = packetSeq;
}

uchar PacketSeq::Packet::flag = 'z' + 29;

PacketSeq::Packet::Packet(QSerialPort &port)
{
    sourceAddress = port.portName().back().cell();
}

QByteArray PacketSeq::Packet::convertToByteArray()
{
    QByteArray arr;
    arr.append(flag);
    arr.append(destinationAddress);
    arr.append(sourceAddress);
    arr.append(data);
    arr.append(fcs);
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
            while (packet.data.size() < 29)
                packet.data.append('\0');
            rawData = rawData.remove(0, rawData.size());
            dataSeq.append(packet.convertToByteArray());
        }
    }
}

void PacketSeq::stuffBytes()
{
    int j, size = 33;
    for (int i = 0; i < dataSeq.size(); i++)
    {
        if ((uchar)dataSeq[i] == Packet::flag)
        {
            for (j = i + 1; j < i + size; j++)
            {
                if ((uchar)dataSeq[j] == Packet::flag)
                {
                    dataSeq = dataSeq.insert(j, '*');
                    j++;
                    size--;
                    continue;
                }
                if ((uchar)dataSeq[j] == '*')
                {
                    dataSeq = dataSeq.insert(j, '*');
                    j++;
                    size--;
                }
            }
            i = j;
        }
    }
}

void PacketSeq::destuffBytes()
{
    dataSeq = dataSeq.remove(0, 3);
    for (int i = 0; i < dataSeq.size() - 1; i++)
    {
        if (((uchar)dataSeq[i] != '*' && (uchar)dataSeq[i + 1] == Packet::flag))
        {
            dataSeq = dataSeq.remove(i + 1, 3);
            i--;
            continue;
        }
        if ((uchar)dataSeq[i] == '*' && (uchar)dataSeq[i + 1] == Packet::flag)
        {
            dataSeq = dataSeq.remove(i, 1);
            i--;
            continue;
        }
        if ((uchar)dataSeq[i] == '*' && (uchar)dataSeq[i + 1] == '*')
        {
            dataSeq = dataSeq.remove(i, 1);
        }
    }
    rawData = QString::fromLocal8Bit(dataSeq);
}

QByteArray PacketSeq::getStuffedData()
{
    return dataSeq;
}
QString PacketSeq::getDestuffedData()
{
    return rawData;
}
