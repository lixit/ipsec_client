#ifndef FORGETPASSWORD_H
#define FORGETPASSWORD_H

#include <QDialog>

namespace Ui {
class ForgetPassword;
}

class ForgetPassword : public QDialog
{
    Q_OBJECT

public:
    explicit ForgetPassword(QWidget *parent = nullptr);
    ~ForgetPassword();

protected:
    void changeEvent(QEvent *) override;

private slots:
    void on_pushButton_clicked();

private:
    Ui::ForgetPassword *ui;
};

#endif // FORGETPASSWORD_H
