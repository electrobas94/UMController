#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QAudioOutput>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QThread>
#include <QDebug>
#include <windows.h>
#include <QStringBuilder>
#include <QTextEncoder>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Ставим стандартный текст на форме
    ui->lineEditSimState->setText("Не активна");
    ui->progressBarSignal->setValue(0);
    ui->lineEditModemName->setText("Модем не подключен");
    ui->lineEditStatus->setText("Отключен");
    ui->lineEditNetState->setText("Сеть отсутствует");

    // Дергаем команду обновления списка доступных портов
    // Получаем сиписок и показываем на форме для возможности выбора
    RefreshPortList();


    // Настраеваем таймер который будет раз в 3 секунды вызывать
    updateModemInfo = new QTimer(this);
    connect(updateModemInfo, SIGNAL(timeout()), SLOT(getMainInfo()));
   // updateModemInfo->start(3000);

    // И также настраеваем таймер который будет раз в 20 мили секунд высылать данные с микрофона если мы звоним
    updateAudio = new QTimer(this);
    connect(updateAudio, SIGNAL(timeout()), SLOT(writeAudioData()));
    updateAudio->start(20);


    // Выпляняем настройку параметров для микрофона
    // Стандратные для почти всех gsm модемов
    QAudioFormat format;
    // Set up the format, eg.
    format.setSampleRate(8000); // Частота дискретизации
    format.setChannelCount(1); // колво канало = моно
    format.setSampleSize(16);  // размер семплирования
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::UnSignedInt);

    QAudioDeviceInfo info(QAudioDeviceInfo::defaultInputDevice());
    if (!info.isFormatSupported(format)) {
        qWarning() << "Raw audio format not supported by backend, cannot play audio.";
        return;
    }

    // Применяем настройки иоткрываем микрофонный канал
    m_audioInput = new QAudioInput(info, format, this);
    m_input = m_audioInput->start();
    //m_audioInput->

}

MainWindow::~MainWindow()
{
    delete ui;
}

// Используется для получения инфы о соединении
void MainWindow::getMainInfo()
{
    // Если не подключены то и ничего не спрашиваем у модема
    if(_Modem.isOpen())
    {
        // Узнать уровень сигнала
        SendAtCommand("AT+CSQ", 20);
        // Есть ли сеть, какая и зарегестрированны ли мы в ней
        SendAtCommand("AT^SPN=0", 20);
        // Статуст сим карты
        SendAtCommand("AT+CPIN?", 20);

        //qDebug() <<"Read info modem";
    }

    // если вдруг звоним то отображаем это на форме
    // так же вырубаем кнопку звонка во время звонка
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

// непосредственно пытаемся подконектится к выбранным портам
void MainWindow::ModemConnect()
{
    // Возвращаем текст на форме в нормальное состояние
    ui->lineEditSimState->setText("Не активна");
    ui->progressBarSignal->setValue(0);
    ui->lineEditModemName->setText("Модем не подключен");
    ui->lineEditStatus->setText("Отключен");
    ui->lineEditNetState->setText("Сеть отсутствует");

    // если юзер вдруг не отключился от старого соединения то выпиливаем его, соединение всмысле
    if(_Modem.isOpen())
        _Modem.close();

    // смотрим что юзер выбрал и отрезаем от выбранного только название порта (т.е описание и производитель нам не нужны)
    QString modemNamePort = ui->comboBoxModemPort->currentText().split(":")[0];
    QString dataNamePort  = ui->comboBoxDataPort->currentText().split(":")[0];
    //QString NetPort  = ui->comboBoxIpData->currentText().split(":")[0];

    //netModem = new GSMModem(NetPort);

    // Ставим имя порта которые получили выше
    _Modem.setPortName(modemNamePort);
    //_Modem.setBaudRate(115200);

    // Пытаемся законектится
    if(_Modem.open(QIODevice::ReadWrite))
    {
        QObject::connect(&_Modem, &QSerialPort::readyRead, this, &MainWindow::readData );
        qDebug() <<"Modem port is opened";

        ui->lineEditModemName->setText(ui->comboBoxModemPort->currentText().split(":")[1]);
    }
    else
    {
        // Если не удалось то говорим юзеру что хьстон у нас проблемы
        QMessageBox *mesB = new QMessageBox();
        mesB->setText("Ошибка, возможно вы выбрали не тот порт");
        mesB->show();
        qDebug() <<_Modem.errorString();
        qDebug() <<"Modem port fail open!!!";
    }

    _VoiceChanal.setPortName(dataNamePort);
    //_VoiceChanal.setBaudRate(115200);
}

// Функции обработки текущих микрофонных данных и запихивания их в порт
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

// Функция дергается если нам от модема прилетают данные
// Все ет дело парсится и выводится или на форму или записывается куда нить
void MainWindow::readData()
{
    QString rxData = _Modem.readAll();
    auto list = rxData.split("\n");

    foreach ( QString str, list)
    {
        // если ответ на уровень сигнала прищел
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
       // На информации о сети
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
           // на запрос пин кода
       }else if(str.indexOf("+CPIN") >=0)
       {
            QString tmp = str.split(": ")[1];
            tmp = tmp.trimmed();
            if(tmp == "READY")
                tmp = "Подключена";
            ui->lineEditSimState->setText( tmp );
       }
       // телофонная книга
       else if( str.indexOf("^CPBR") >= 0 )
       {
           QStringList tmpL = str.split(":");
           if(tmpL.length() > 1)
           {
               QString tmp = tmpL[1];

               tmp = tmp.trimmed();

               if(tmp.split(",").length() > 3)
               {
                   //QString::toLocal8Bit()
                   QChar m = 'М';
                   QChar m2 = 0x8105;

                   QString strName = HexToQString(tmp.split(",")[3].split("\"")[1]);

                   QString phoneItems = strName + " - " + tmp.split(",")[1].split("\"")[1];
                   ui->listWidget->addItem( phoneItems );
               }
           }
           // есди любая другая кооманда
       }else if(str.length() > 1)
       {
           if(str.indexOf("OK") < 0 && str.indexOf("ERROR") < 0)
               recivData = str;
       }
    }

    ui->plainTextEditConsole->appendPlainText( QString::fromLocal8Bit( rxData.toStdString().c_str() ) );
}

void MainWindow::on_pushButton_clicked()
{
    /*
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
        */
}

// если звук закончился или еще что нить произошло с устройсвом его воспроизведения у его меняется статут
// и автоматом дергается ета штука
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

// Функция начала вызова
void MainWindow::on_pushButtonCall_clicked()
{
    if(_Modem.isOpen())
    {
        SendAtCommand("AT^DDSETEX=2", 40);
        SendAtCommand("ATD"+ui->lineEditPhoneNumber->text()+";", 100);
        SendAtCommand("AT^DDSETEX=2", 40);

        if(_VoiceChanal.open(QIODevice::ReadWrite))
        {
            qDebug() <<"Voice port is opened";
        }else
            qDebug() <<"Voice port fail open!!!";

        if(_VoiceChanal.isOpen())
        {
            // настраеваем формат звука как и у микрона
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

                // И наченаем вещать сразу данными из ком порта
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

// Отрубить вызов
void MainWindow::on_pushButtonBreakCall_clicked()
{
    if(_Modem.isOpen())
    {
        if(_VoiceChanal.isOpen())
        {
            // вырубаем звук
            audio->stop();
            delete audio;
            _VoiceChanal.clear();
            _VoiceChanal.close();
        }
        // говорим модему что все мол, мы договорили
        SendAtCommand("ATH", 20);
        SendAtCommand("AT+CHUP", 20);
    }
    else
        ui->plainTextEditConsole->appendPlainText("Порт модема не открыт\n");
    getMainInfo();
}

// Все что написал пользователь отправляем в консоль
void MainWindow::on_pushButtonSendCommand_clicked()
{
    SendAtCommand(ui->lineEditConsoleCommand->text());
}

// функция отсылки ат команды в модем
void MainWindow::SendAtCommand(QString com, int timeSleep)
{
    auto command = com.toUpper();
    char *dirtyCommand = new char[command.length() + 2];

    for(int i = 0; i < command.length(); i++ )
        dirtyCommand[i] = command[i].toLatin1();

    // добавляем знаки переноса строки и возврата каретки
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

    // ждем определенное время установленное на получения ответа, но при этом выполняемя все события приложения
    // что бы юзер не подумал что мы завислои
    for(int i =0; i< timeSleep; i++)
    {
        QThread::msleep(5);
        qApp->processEvents();
    }
    //QThread::msleep(timeSleep);
}

void MainWindow::on_pushButton_14_clicked()
{
}

void MainWindow::on_pushButtonConnect_clicked()
{
    ui->plainTextEditConsole->clear();
    ModemConnect();

    SendAtCommand("AT^U2DIAG=0", 40);

    // Спрашиваеимя производителя
    recivData ="";
    SendAtCommand("AT+CGMI", 40);
    if(recivData="")
    {
        QMessageBox *a = new QMessageBox();
        a->setText("Ошибка открытия порта. \nВозможно вы выбрание не тот порт в качестве модема");
        a->show();
        _Modem.close();
        return 0;
    }
    ui->lineEditVendor->setText(recivData);

    //  версию софта
    recivData ="";
    SendAtCommand("AT+CGMR", 40);
    ui->lineEditSoftVersion->setText(recivData);

    //  модель
    recivData ="";
    SendAtCommand("AT+CGMM", 40);
    ui->lineEditModel->setText(recivData);

    //  версию железа
    recivData ="";
    SendAtCommand("AT^HWVER", 40);
    if(recivData.indexOf("HWVER")>0)
        recivData = recivData.split(":")[1];
    ui->lineEditHardVersion->setText(recivData);

    //  уникальные данные устройсва и симки
    recivData ="";
    SendAtCommand("AT+CIMI", 40);
    ui->lineEditSIMIMSI->setText(recivData);

    recivData ="";
    SendAtCommand("AT+CGSN", 40);
    ui->lineEditIMEI->setText(recivData);

    // получаем номера записанные с сим карты
    // get number book
    SendAtCommand("AT^CPBR=1,50", 50);
    SendAtCommand("AT^CPBR=51,100", 50);
    SendAtCommand("AT^CPBR=101,150", 50);
    SendAtCommand("AT^CPBR=151,200", 50);
    SendAtCommand("AT^CPBR=201,250", 50);

    // и информации о сети, что бы не ждать 3 секунды с таймера
    getMainInfo();

    updateModemInfo->start(3000);

    if(_Modem.isOpen())
    {
        ui->pushButtonModemDeconect->setEnabled(true);
    }
}

// отрубаем модем если так пользователь пожелал
void MainWindow::on_pushButtonModemDeconect_clicked()
{
     updateModemInfo->stop();
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

// просто занов получаем список портов и показываем пользователю что ему можно выбрать
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
        //ui->comboBoxIpData->addItem(portInfo.portName() + ": "+ portInfo.description() + " :(" + portInfo.manufacturer() + ")");
    }
}


// Соеденяемся с нетом, если дрова есть то соединения винда сама делает
//  и мы просто выполняем консольную команду подключения
void MainWindow::on_pushButtonNetConnect_clicked()
{
    QString str = "rasdial "+ ui->comboBox->currentText() + " callback:*99#";
    system(str.toStdString().c_str());
    //_Modem.SendAtCommand();
    //netModem->Connect();
}

//  Виндосовскую консоль окрыть по настройки соединений
void MainWindow::on_pushButton_2_clicked()
{
    system("ncpa.cpl");
    //QNetworkConfigurationManager a;
    //auto g = a.allConfigurations();
    //foreach (QNetworkConfiguration aa, g) {
      //  qDebug() << aa.name();
    //}
    //netModem->ReceivedDataFromPort();
}

void MainWindow::on_pushButton_3_clicked()
{
    system("rasdial /d");
}

void MainWindow::on_listWidget_clicked(const QModelIndex &index)
{
    QString numer = ui->listWidget->currentItem()->text().split(" - ")[1];

    if(numer.length() == 7 )
        numer = "+37533" + numer;
    ui->lineEditPhoneNumber->setText( numer );
}

QString MainWindow::HexToQString( QString str)
{
   // str = "8004320430043B0435044004300020FF";
    //QString str12="";
    ushort stsr[200];

    int b=0;
    for( int i = 2; i < str.length() - 3; i+=4)
    {
        QString numer = QString(str[i])+ QString(str[i+1]) + QString(str[i+2]) + QString(str[i+3]);
        stsr[b] = numer.toInt(nullptr, 16);
        if((stsr[b] <= 0x044f && stsr[b] >= 0x0401 ) || (stsr[b] <= 0x0071 && stsr[b] >= 0x0021 ))
            b++;
    }

    //char *str = "UTF-8";
    //QTextCodec *codec = QTextCodec::codecForName(str);

    return QString::fromUtf16(stsr, b);
}

QString TextToDigit(QString text)
{
    QString d ="";

    auto s = text.toStdWString();

    for( int i = 0; i < s.length(); i++)
    {
        int a = (int)s[i];

        QString ass =  QString::number(a, 16).toUpper();

        while(ass.length() <4)
            ass = "0" + ass;
        d += ass;
    }

    return d;
}

void MainWindow::on_pushButton_4_clicked()
{
    updateModemInfo->stop();

    QString s = ui->lineEditPhoneNumber_2->text();

    s = s.split("-")[0] + s.split("-")[1] + s.split("-")[2]+ s.split("-")[3]+ s.split("-")[4];

    s[0] = ' ';
    s = s.trimmed();

    QString num = "0001000C91";

    for(int i =0; i < 12; i= i+2)
    {
        num += s[i+1];
        num += s[i];
    }

    QString len = QString::number( ui->plainTextEditSMS->toPlainText().length()*2, 16);
    if(len.length() == 1)
        len = "0" + len;

    num+="0008" + len;
    num+=TextToDigit( ui->plainTextEditSMS->toPlainText() ) + 0x1A;

    SendAtCommand("AT+CMGF=0", 500);
    SendAtCommand("AT+CMGS=" + QString::number(num.length()/2-1), 500);
    SendAtCommand( num, 500);

    QMessageBox *d = new QMessageBox();

    d->setText("Сообщение отправлено");
    d->show();

    //updateModemInfo->start(3000);
}

void MainWindow::on_pushButton_5_clicked()
{
    QString s = ui->lineEditPhoneNumber_2->text();

    s = s.split("-")[0] + s.split("-")[1] + s.split("-")[2]+ s.split("-")[3]+ s.split("-")[4];

    s[0] = ' ';
    s = s.trimmed();

    QString num = "0001000C91";

    for(int i =0; i < 12; i= i+2)
    {
        num += s[i+1];
        num += s[i];
    }

    QString len = QString::number( ui->plainTextEditSMS->toPlainText().length()*2, 16);
    if(len.length() == 1)
        len = "0" + len;

    num+="0008FF" + len + "00";

    ui->plainTextEditSMS->appendPlainText(num);
}
