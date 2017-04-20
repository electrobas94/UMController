#include "advancedserialport.h"

AdvancedSerialPort::AdvancedSerialPort()
{

}

void AdvancedSerialPort::OpenWithAutoSpeed()
{
    int defSpeed[] = {1200, 2400, 4800, 9600, 14400, 19200, 38400, 56000, 57600, 115200, 230400, 460800};//, 921600};

    for ( int i = 11; i >=0; i--)
    {
        setBaudRate(defSpeed[i]);
        open(QIODevice::ReadWrite);
        if(isOpen())
        {
            clear();
            break;
        }
    }
}
