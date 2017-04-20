#include "gsmmodem.h"
#include "dictionarycommandat.h"

GSMModem::GSMModem(QString portName)
{
    // Open modem serila port
    modemPort = new AdvancedSerialPort();
    modemPort->setPortName( portName );
    modemPort->OpenWithAutoSpeed();

    connect( modemPort, SIGNAL( readyRead() ), this, SLOT( ReceivedDataFromPort() ) );

    // Start tcp server for retranslete data
    // 127.0.0.1
    tcpServer = new QTcpServer(this);

    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(NewUserConect()));

    if (!tcpServer->listen(QHostAddress::Any, 24))
        qDebug() <<  QObject::tr("Unable to start the server: %1.").arg(tcpServer->errorString());
    else
        qDebug() << QString::fromUtf8("Сервер запущен!");
}


void GSMModem::NewUserConect()
{
        qDebug() << QString::fromUtf8("У нас новое соединение!");

        QTcpSocket* clientSocket=tcpServer->nextPendingConnection();
        int idusersocs=clientSocket->socketDescriptor();
        SClients[idusersocs]=clientSocket;

        connect(SClients[idusersocs],SIGNAL(readyRead()),this, SLOT(ReadClient()));
}

void GSMModem::ReadClient()
{
    QTcpSocket* clientSocket = (QTcpSocket*)sender();
    int idusersocs=clientSocket->socketDescriptor();
    QTextStream os(clientSocket);

    //os.setAutoDetectUnicode(true);

    auto data  =clientSocket->readAll();
    qDebug () <<"Send data";
    qDebug() << data;

    modemPort->write(data);
    dataWaitReceived = true;

    int i = 0;
    while(dataWaitReceived && i < 400)
    {
        qApp->processEvents();
        i++;
        QThread::msleep(10);
    }
    if(i>=400)
    {
        os << "HTTP/1.0 200 Ok\r\n"
              "Content-Type: text/html; charset=\"utf-8\"\r\n"
              "\r\n"
              "<h1>Что то пошло не так. Интернет не доступен...</h1>";
        qDebug() << "nope";
    }
    else
    {
        os<< buf;
        qDebug() << buf;
    }



    // Если нужно закрыть сокет
    //clientSocket->close();
    //SClients.remove(idusersocs);
}

void GSMModem::ReceivedDataFromPort()
{
    buf = modemPort->readAll();
    dataWaitReceived = false;
    qDebug () <<"New data";
    qDebug() << buf;
}

void GSMModem::Connect()
{
    if(modemPort->isOpen())
    {
        //DictionaryCommandAT::SendCommandAT(modemPort, "AT+CGDCONT=1,\"ip\",\"mts\"", 200);
        //qApp->processEvents();
        DictionaryCommandAT::SendCommandAT(modemPort, "AT", 200);
        qApp->processEvents();
        DictionaryCommandAT::SendCommandAT(modemPort, "AT&FE0V1X1&D2&C1S0=0", 200);
        qApp->processEvents();
        DictionaryCommandAT::SendCommandAT(modemPort, "AT", 200);
        qApp->processEvents();
        DictionaryCommandAT::SendCommandAT(modemPort, "AT&FE0V1X1&D2&C1S0=0", 200);
        qApp->processEvents();
        DictionaryCommandAT::SendCommandAT(modemPort, "ATS0=0", 200);
        qApp->processEvents();
        DictionaryCommandAT::SendCommandAT(modemPort, "ATDT*99#", 200);
        qApp->processEvents();
    }
    else
        qDebug() << "Порт модема не открыт";
}
