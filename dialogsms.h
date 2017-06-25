#ifndef DIALOGSMS_H
#define DIALOGSMS_H

#include <QDialog>

namespace Ui {
class DialogSms;
}

class DialogSms : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSms(QWidget *parent = 0);
    ~DialogSms();

private:
    Ui::DialogSms *ui;
};

#endif // DIALOGSMS_H
