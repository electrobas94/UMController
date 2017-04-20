#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QSerialPort>
#include <QTimer>
#include <QAudio>
#include <QAudioOutput>
#include <QAudioInput>
#include "gsmmodem.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();
    void handleStateChanged(QAudio::State newState);

    void on_pushButtonCall_clicked();

    void on_pushButtonBreakCall_clicked();

    void on_pushButtonSendCommand_clicked();

    void on_pushButton_14_clicked();

    void on_pushButtonConnect_clicked();

    void on_pushButtonModemDeconect_clicked();

    void on_pushButtonRefresh_clicked();

    void on_pushButtonNetConnect_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_listWidget_clicked(const QModelIndex &index);

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

public slots:
    void readData();
    void getMainInfo();
    void writeAudioData();

private:
    QString recivData;
    QString HexToQString( QString str);
    qint16 recordBuf[1000];
    Ui::MainWindow *ui;
    QAudioOutput *audio;
    QFile sourceFile;
    QSerialPort _Modem;
    QSerialPort _VoiceChanal;

    QAudioDeviceInfo m_device;
    QAudioInput *m_audioInput;
    QIODevice *m_input;

    QTimer* updateAudio;
    QTimer* updateModemInfo;

    void SendAtCommand(QString com, int timeSleep = 10);
    bool RxFlag = false;

    void ModemConnect();

    void RefreshPortList();

    GSMModem *netModem;
};

#endif // MAINWINDOW_H
