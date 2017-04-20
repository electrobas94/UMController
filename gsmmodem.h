#ifndef GSMMODEM_H
#define GSMMODEM_H

#include <QtNetwork>
#include <QTcpSocket>
#include <QObject>
#include <QByteArray>
#include <QDebug>
#include "advancedserialport.h"
#include <QThread>

class GSMModem: QObject
{
    Q_OBJECT
private:
    QByteArray buf;
    AdvancedSerialPort *modemPort;

    QTcpServer *tcpServer;
    QMap<int,QTcpSocket *> SClients;
    bool dataTransferOn = false;
    bool dataWaitReceived = false;

public:
    GSMModem(QString portName);
    void Connect();
public slots:
    void NewUserConect();
    void ReadClient();
    void ReceivedDataFromPort();
};

#endif // GSMMODEM_H
