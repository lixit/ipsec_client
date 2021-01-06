#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>
#include <QMessageBox>

#include "passwordlineedit.h"
#include "lineedit.h"

#include <QTimer>

class QString;
class QLabel;
class QNetworkAccessManager;
class QNetworkReply;
class QTranslator;

const int ClientApiVersion = 2;

extern QTranslator *qtbaseTranslator;
extern QTranslator *translator;

namespace Ui { class login; }

class LoginWindow : public QDialog
{
    Q_OBJECT

public:
    LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow();

    void loginError(QString message);

protected:
    void changeEvent(QEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private slots:
    void replyFinished(QNetworkReply *reply);

    void on_loginButton_clicked();

//    void on_vsmIPInput_textChanged(const QString &arg1);
//    void on_userNameInput_textChanged(const QString &arg1);

    void vsmIPInput_textChanged(const QString &arg1);
    void passwordInput_textChanged(const QString &arg1);
    void userNameInput_textChanged(const QString &arg1);

    void on_pushButton_clicked();

    void on_languageButton_clicked(bool checked);

private:
    void setLanguage(bool isEnglish);

    void writeSettings();
    void readSettings();

    void checkUpdateRequest();
    void downloadUrlRequest();

    void loginRequest();

    //update needed
    int vsmApiVersion;
    int vsmSystemVersion;
    QString updateUrl;

    //login needed
    QString vsmAddr;
    QString username;
    QString oldPassword;
    QString deviceType;

    LineEdit *vsmInput;
    LineEdit *userNameInput;
    PasswordLineEdit *passwordInput;
    QLabel *labelError;

    QNetworkAccessManager *manager;

    Ui::login *ui;
};
#endif // MAINWINDOW_H
