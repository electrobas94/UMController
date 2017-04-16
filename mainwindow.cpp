#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QAudioOutput>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QThread>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->lineEditSimState->setText("Не активна");
    ui->progressBarSignal->setValue(0);
    ui->lineEditModemName->setText("Модем не подключен");
    ui->lineEditStatus->setText("Отключен");
    ui->lineEditNetState->setText("Сеть отсутствует");

    //a=new myAudio(this);
    RefreshPortList();


    updateModemInfo = new QTimer(this);
    connect(updateModemInfo, SIGNAL(timeout()), SLOT(getMainInfo()));
    updateModemInfo->start(3000);

    updateAudio = new QTimer(this);
    connect(updateAudio, SIGNAL(timeout()), SLOT(writeAudioData()));
    updateAudio->start(20);


    QAudioFormat format;
    // Set up the format, eg.
    format.setSampleRate(8000);
    format.setChannelCount(1);
    format.setSampleSize(16);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::UnSignedInt);

    QAudioDeviceInfo info(QAudioDeviceInfo::defaultInputDevice());
    if (!info.isFormatSupported(format)) {
        qWarning() << "Raw audio format not supported by backend, cannot play audio.";
        return;
    }

    //m_device.defaultInputDevice()

    //m_input = new QIODevice();
    m_audioInput = new QAudioInput(info, format, this);
    m_input = m_audioInput->start();
    //m_audioInput->

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::getMainInfo()
{
    if(_Modem.isOpen())
    {
        SendAtCommand("AT+CSQ", 20);
        SendAtCommand("AT^SPN=0", 20);
        SendAtCommand("AT+CPIN?", 20);

        //qDebug() <<"Read info modem";
    }

    if(_VoiceChanal.isOpen())
    {
        ui->lineEditStatus->setText("Идёт вызов");
        ui->pushButtonCall->setEnabled(false);
    }
    else
    {
        ui->pushButtonCall->setEnabled(true);
        if(_Modem.isOpen())
            ui->lineEditStatus->setText("Режим ожидания");
    }
}

void MainWindow::ModemConnect()
{
    ui->lineEditSimState->setText("Не активна");
    ui->progressBarSignal->setValue(0);
    ui->lineEditModemName->setText("Модем не подключен");
    ui->lineEditStatus->setText("Отключен");
    ui->lineEditNetState->setText("Сеть отсутствует");

    if(_Modem.isOpen())
        _Modem.close();

    QString modemNamePort = ui->comboBoxModemPort->currentText().split(":")[0];
    QString dataNamePort  = ui->comboBoxDataPort->currentText().split(":")[0];

    _Modem.setPortName(modemNamePort);
    _Modem.setBaudRate(115200);

    if(_Modem.open(QIODevice::ReadWrite))
    {
        QObject::connect(&_Modem, &QSerialPort::readyRead, this, &MainWindow::readData );
        qDebug() <<"Modem port is opened";

        ui->lineEditModemName->setText(ui->comboBoxModemPort->currentText().split(":")[1]);
    }
    else
    {
        qDebug() <<_Modem.errorString();
        qDebug() <<"Modem port fail open!!!";
    }

    _VoiceChanal.setPortName(dataNamePort);
    _VoiceChanal.setBaudRate(115200);
}

void MainWindow::writeAudioData()
{
    if(_VoiceChanal.isOpen())
    {
        //qApp->processEvents();
        if(m_input)
        {
        auto buf = m_input->readAll();

        if(buf.length() < 320)
            return;
/*
        for(int i =0; i< 320; i++)
        {
            if( buf.length() > i )
                recordBuf[i] = buf.toStdString().c_str()[i];
            else
                recordBuf[0];
        }
*/
        _VoiceChanal.write(buf.toStdString().c_str(), 320 );

        //QFile file("test.raw");
        //file.open(QIODevice::WriteOnly);
        //file.write(buf);
        //file.close();

        //qDebug() <<buf.length();
        //qDebug() <<"write voice";
        }
        else{
            qDebug() <<"Microphone not found";
        }
    }
}

void MainWindow::readData()
{
    QString rxData = _Modem.readAll();
    auto list = rxData.split("\n");

    foreach ( QString str, list)
    {
       if(str.indexOf("+CSQ") >=0)
       {
            QString tmp = str.split(": ")[1];
            tmp = tmp.trimmed();
            if(tmp != "")
            {
                QLocale german(QLocale::German);
                double value = german.toDouble(tmp);
                if( value == 99)
                    value = 0;
                else
                    value = (value - 2.0) * 100.0 / 29.0;

                ui->progressBarSignal->setValue( value );
            }
       }
       else if(str.indexOf("^SPN") >=0)
       {
           try
           {
               QString tmp = str.split(":")[1].split(",")[2];
               tmp = tmp.trimmed();
               ui->lineEditNetState->setText(tmp);
           }
           catch(...)
           {}
       }else if(str.indexOf("+CPIN") >=0)
       {
            QString tmp = str.split(": ")[1];
            tmp = tmp.trimmed();
            if(tmp == "READY")
                tmp = "Подключена";
            ui->lineEditSimState->setText( tmp );
       }
    }

    ui->plainTextEditConsole->appendPlainText( QString::fromLocal8Bit( rxData.toStdString().c_str() ) );
}

void MainWindow::on_pushButton_clicked()
{
        sourceFile.setFileName("test.raw");
        sourceFile.open(QIODevice::ReadOnly);

        QAudioFormat format;
        // Set up the format, eg.
        format.setSampleRate(8000);
        format.setChannelCount(1);
        format.setSampleSize(16);
        format.setCodec("audio/pcm");
        format.setByteOrder(QAudioFormat::LittleEndian);
        format.setSampleType(QAudioFormat::UnSignedInt);

        QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
        if (!info.isFormatSupported(format)) {
            qWarning() << "Raw audio format not supported by backend, cannot play audio.";
            return;
        }

        audio = new QAudioOutput(format, this);
        connect(audio, SIGNAL(stateChanged(QAudio::State)), this, SLOT(handleStateChanged(QAudio::State)));
        audio->start(&sourceFile);
}

void MainWindow::handleStateChanged(QAudio::State newState)
{
    switch (newState) {
        case QAudio::IdleState:
            // Finished playing (no more data)
            //audio->stop();
            //sourceFile.close();
            //delete audio;
            break;

        case QAudio::StoppedState:
            // Stopped for other reasons
            if (audio->error() != QAudio::NoError) {
                // Error handling
            }
            break;

        default:
            // ... other cases as appropriate
            break;
    }
}

void MainWindow::on_pushButtonCall_clicked()
{
    if(_Modem.isOpen())
    {
        SendAtCommand("AT^DDSETEX=2", 70);
        SendAtCommand("ATD"+ui->lineEditPhoneNumber->text()+";", 400);
        SendAtCommand("AT^DDSETEX=2", 90);

        if(_VoiceChanal.open(QIODevice::ReadWrite))
        {
            qDebug() <<"Voice port is opened";
        }else
            qDebug() <<"Voice port fail open!!!";

        if(_VoiceChanal.isOpen())
        {
            QAudioFormat format;
            // Set up the format, eg.
            format.setSampleRate(8000);
            format.setChannelCount(1);
            format.setSampleSize(16);
            format.setCodec("audio/pcm");
            format.setByteOrder(QAudioFormat::LittleEndian);
            format.setSampleType(QAudioFormat::UnSignedInt);


            //for(int i = 0; i<100;i++)
            {
                QThread::msleep(60);
                qApp->processEvents();

                QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
                if (!info.isFormatSupported(format)) {
                    qWarning() << "Raw audio format not supported by backend, cannot play audio.";
                    return;
                }

                audio = new QAudioOutput(format, this);
                //connect(audio, SIGNAL(stateChanged(QAudio::State)), this, SLOT(handleStateChanged(QAudio::State)));
                audio->start(&_VoiceChanal);
            }
        }
    }
    else
        qDebug() <<"Modem not open";

    getMainInfo();
}

void MainWindow::on_pushButtonBreakCall_clicked()
{
    if(_Modem.isOpen())
    {
        if(_VoiceChanal.isOpen())
        {
            audio->stop();
            delete audio;
            _VoiceChanal.clear();
            _VoiceChanal.close();
        }
        SendAtCommand("ATH", 20);
        SendAtCommand("AT+CHUP", 20);
    }
    else
        ui->plainTextEditConsole->appendPlainText("Порт модема не открыт\n");
    getMainInfo();
}

void MainWindow::on_pushButtonSendCommand_clicked()
{
    SendAtCommand(ui->lineEditConsoleCommand->text());
}

void MainWindow::SendAtCommand(QString com, int timeSleep)
{
    auto command = com.toUpper();
    char *dirtyCommand = new char[command.length() + 2];

    for(int i = 0; i < command.length(); i++ )
        dirtyCommand[i] = command[i].toLatin1();

    dirtyCommand[command.length()] = 0x0D;
    dirtyCommand[command.length() + 1] = 0x0A;

    if(_Modem.isOpen())
    {
        ui->plainTextEditConsole->appendPlainText("Выполняем комманду - " + command +"\n");
        _Modem.write(dirtyCommand, command.length() + 2);
    }
    else
        ui->plainTextEditConsole->appendPlainText("Порт модема не открыт\n");

    delete dirtyCommand;

    qApp->processEvents();
    QThread::msleep(timeSleep);
}

void MainWindow::on_pushButton_14_clicked()
{
}

void MainWindow::on_pushButtonConnect_clicked()
{
    ui->plainTextEditConsole->clear();
    ModemConnect();
    getMainInfo();

    if(_Modem.isOpen())
    {
        ui->pushButtonModemDeconect->setEnabled(true);
    }
}

void MainWindow::on_pushButtonModemDeconect_clicked()
{
    ui->plainTextEditConsole->clear();
    ui->pushButtonModemDeconect->setEnabled(false);

    if(_Modem.isOpen())
        _Modem.close();

    ui->lineEditSimState->setText("Не активна");
    ui->progressBarSignal->setValue(0);
    ui->lineEditModemName->setText("Модем не подключен");
    ui->lineEditStatus->setText("Отключен");
    ui->lineEditNetState->setText("Сеть отсутствует");
}

void MainWindow::on_pushButtonRefresh_clicked()
{
    RefreshPortList();
}

void MainWindow::RefreshPortList()
{
    ui->comboBoxModemPort->clear();
    ui->comboBoxDataPort->clear();

    auto listPort = QSerialPortInfo::availablePorts();

    foreach (auto portInfo, listPort)
    {
        ui->comboBoxModemPort->addItem(portInfo.portName() + ": "+ portInfo.description() + " :(" + portInfo.manufacturer() + ")");
        ui->comboBoxDataPort->addItem(portInfo.portName() + ": "+ portInfo.description() + " :(" + portInfo.manufacturer() + ")");
    }
}
