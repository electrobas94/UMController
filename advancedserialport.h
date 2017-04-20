#ifndef ADVANCEDSERIALPORT_H
#define ADVANCEDSERIALPORT_H
#include <QSerialPort>

class AdvancedSerialPort:public QSerialPort
{
public:
    void OpenWithAutoSpeed();
    AdvancedSerialPort();
};

#endif // ADVANCEDSERIALPORT_H
