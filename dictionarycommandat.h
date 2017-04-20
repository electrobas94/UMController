#ifndef DICTIONARYCOMMANDAT_H
#define DICTIONARYCOMMANDAT_H

#include <QSerialPort>
#include <QThread>
#include <advancedserialport.h>

class DictionaryCommandAT
{
public:
    static void SendCommandAT(AdvancedSerialPort*, QString, int );
};

#endif // DICTIONARYCOMMANDAT_H
