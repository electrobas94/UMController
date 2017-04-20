#include "dictionarycommandat.h"

void DictionaryCommandAT::SendCommandAT(AdvancedSerialPort *port, QString command, int delay=30 )
{
    command = command.toUpper();
    char *dirtyCommand = new char[command.length() + 2];

    for(int i = 0; i < command.length(); i++ )
        dirtyCommand[i] = command[i].toLatin1();

    dirtyCommand[command.length()] = 0x0D;
    dirtyCommand[command.length() + 1] = 0x0A;

    if( port->isOpen() )
        port->write( dirtyCommand, command.length() + 2 );

    QThread::msleep(delay);
}
