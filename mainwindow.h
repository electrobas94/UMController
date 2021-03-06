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
#include <QtXml>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    void traverseNode(const QDomNode& node);
    void LoadStorage();
    explicit MainWindow(QWidget *parent = 0);
    QString FindNameByNumber( QString number );
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

    void on_pushButtonAutoConnect_clicked();

    void on_pushButtonProOn_clicked();

    void on_pushButtonAutoDisconnect_clicked();

    void on_pushButton_d1_clicked();

    void on_pushButton_d2_clicked();

    void on_pushButton_d3_clicked();

    void on_pushButton_d4_clicked();

    void on_pushButton_d5_clicked();

    void on_pushButton_d6_clicked();

    void on_pushButton_d7_clicked();

    void on_pushButton_d8_clicked();

    void on_pushButton_d9_clicked();

    void on_pushButton_d0_clicked();

    void on_pushButton_dplus_clicked();

    void on_pushButton_ddel_clicked();

    void on_pushButtonSaveNumber_clicked();

    void on_listWidgetKontakt_clicked(const QModelIndex &index);

    void on_tableWidgetCallTable_cellClicked(int row, int column);

    void on_MainWindow_destroyed();

    void on_MainWindow_destroyed(QObject *arg1);

    void on_centralWidget_destroyed();

    void on_tabWidget_tabBarClicked(int index);

    void on_pushButtonStuleSet_clicked();

    void on_tabWidget_3_tabBarClicked(int index);

    void on_tabWidget_3_currentChanged(int index);

    void on_tabWidget_5_tabBarClicked(int index);

    void on_lineEditConsoleCommand_returnPressed();

public slots:
    void closeWindow();
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
