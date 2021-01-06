#include "forgetpassword.h"
#include "ui_forgetpassword.h"

ForgetPassword::ForgetPassword(QWidget *parent) :
    QDialog(parent,  Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint),
    ui(new Ui::ForgetPassword)
{
    ui->setupUi(this);
}

ForgetPassword::~ForgetPassword()
{
    delete ui;
}

void ForgetPassword::on_pushButton_clicked()
{
    close();
    delete this;
}

void ForgetPassword::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    } else {
        QDialog::changeEvent(event);
    }
}
