#include "dialogsms.h"
#include "ui_dialogsms.h"

DialogSms::DialogSms(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSms)
{
    ui->setupUi(this);
}

DialogSms::~DialogSms()
{
    delete ui;
}
