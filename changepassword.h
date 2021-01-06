#ifndef CHANGEPASSWORD_H
#define CHANGEPASSWORD_H

#include <QDialog>
class QLabel;

namespace Ui {
class ChangePassword;
}

class ChangePassword : public QDialog
{
    Q_OBJECT

public:
    explicit ChangePassword(QWidget *parent = nullptr);

    void errorInfo(const QString &err);

    ~ChangePassword();

protected:
    void changeEvent(QEvent *event) override;

public slots:
    void clear();

signals:
    void buttonSave(QString original, QString newpass);

private slots:
//    void on_inputAgain_editingFinished();

    void on_inputAgain_textChanged(const QString &arg1);

    void on_originalPassword_textChanged(const QString &arg1);

    void on_newPassword_textChanged(const QString &arg1);

    void on_buttonBox_accepted();

private:
    void inputFinished();

    QString originalPassword;
    QString newPassword;
    QString inputAgain;

    QFont font;
    QLabel *labelError;
    Ui::ChangePassword *ui;
};

#endif // CHANGEPASSWORD_H
