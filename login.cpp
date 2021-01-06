#include "login.h"
#include "ui_login.h"
#include "changepassword.h"
#include "forgetpassword.h"

#include "update.h"

#include "mainwin.h"


#include <QNetworkAccessManager>
#include <QSslConfiguration>
#include <QSettings>
#include <QCloseEvent>
#include <QDebug>
#include <QUrlQuery>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QTranslator>

LoginWindow::LoginWindow(QWidget *parent)
    : QDialog(parent,  Qt::WindowCloseButtonHint), //  | Qt::WindowStaysOnTopHint
      vsmApiVersion(0),
      vsmSystemVersion(0),
      ui(new Ui::login)
{
    ui->setupUi(this);

    vsmInput = new LineEdit(this);
    vsmInput->setPlaceholderText(QCoreApplication::translate("login", "https://123.123.123.123:8080", nullptr));

//    QRegExpValidator(QRegExp("(http(s)?:\/\/.)?(www\.)?[-a-zA-Z0-9@:%._\+~#=]{2,256}\.[a-z]{2,6}\b([-a-zA-Z0-9@:%_\+.~#?&//=]*)"), this);

//    vsmInput->setValidator();
    ui->inputLayout->insertWidget(0, vsmInput);

    userNameInput = new LineEdit(this);
    userNameInput->setPlaceholderText(QCoreApplication::translate("login", "username@www.netlinkz.com", nullptr));
    ui->inputLayout->insertWidget(1, userNameInput);


    passwordInput = new PasswordLineEdit(this);
    QFont font;
    font.setFamily(QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221"));
    font.setPointSize(14);
    passwordInput->setMinimumSize(QSize(360, 50));
    passwordInput->setFont(font);
    passwordInput->setPlaceholderText(QCoreApplication::translate("login", "password", nullptr));
    passwordInput->setStyleSheet(QString::fromUtf8("QLineEdit{ border-width: 1px; border-style: solid; border-color: white white #D1D7E0 white; }"));
    passwordInput->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
    ui->inputLayout->insertWidget(2, passwordInput);


    connect(passwordInput, SIGNAL(textChanged(const QString &)),
            this, SLOT(passwordInput_textChanged(const QString &)));
    connect(vsmInput, &LineEdit::textChanged, this, &LoginWindow::vsmIPInput_textChanged);
    connect(userNameInput, &LineEdit::textChanged, this, &LoginWindow::userNameInput_textChanged);

    labelError = new QLabel(this);
    QFont font1;
    font1.setPointSize(12);
    labelError->setFont(font1);
    labelError->setWordWrap(true);
    labelError->setStyleSheet( QString::fromUtf8("QLabel { color: red } \n") );
    //setStyleSheet(QString::fromUtf8("QPushButton { color: #666666 }\n"
    readSettings();

    manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished, this, &LoginWindow::replyFinished);
}

void LoginWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
//        vsmInput->setPlaceholderText(QCoreApplication::translate("login", "Please enter server address", nullptr));
//        userNameInput->setPlaceholderText(QCoreApplication::translate("login", "Please enter user name", nullptr));
        passwordInput->setPlaceholderText(QCoreApplication::translate("login", "password", nullptr));
    } else {
        QDialog::changeEvent(event);
    }
}

void LoginWindow::loginError(QString message)
{
    labelError->setText(message);
    ui->inputLayout->addWidget(labelError);
    labelError->setVisible(true);
    QTimer::singleShot(3000, this, [=] { labelError->setVisible(false); });
}

void LoginWindow::setLanguage(bool isEnglish)
{
    qDebug() << "isEnglish" << isEnglish;

    qDebug() << "qtbaseTranslator addr is: " << qtbaseTranslator;

    QString chinese = "zh_CN";
    if (isEnglish) {
        if (qtbaseTranslator != nullptr) {
            qDebug() << qApp->removeTranslator(qtbaseTranslator);
            delete qtbaseTranslator;
            qtbaseTranslator = nullptr;
        }
        if (translator != nullptr) {
            qApp->removeTranslator(translator);
            delete translator;
            translator = nullptr;
        }
    } else {
        // install base translator
        if (qtbaseTranslator != nullptr) {
            delete qtbaseTranslator;
            qtbaseTranslator = new QTranslator;
        } else {
            qtbaseTranslator = new QTranslator;
        }
        qtbaseTranslator->load(QString("qtbase_") + chinese, ":/translations");
        qApp->installTranslator(qtbaseTranslator);

        // install translator
        if (translator != nullptr) {
            delete translator;
            translator = new QTranslator;
        } else {
            translator = new QTranslator;
        }
        translator->load(QString("vpn_cpp_") + chinese, ":/translations");
        qApp->installTranslator(translator);
    }
}

void LoginWindow::writeSettings()
{
    //always remember other info
    QSettings settings("iSoftStone", "VSN_login");
    settings.setValue("loginWinGeometry", saveGeometry());

    settings.setValue("vsmAddr", vsmInput->text());
    settings.setValue("username", userNameInput->text());

    //whether remember password depends on user
    if (ui->rememberSettings->isChecked()) {
        settings.setValue("oldPassword", passwordInput->text());
        settings.setValue("rememberPassword", true);
    } else {
        //remove old password
        settings.setValue("oldPassword", "");
        settings.setValue("rememberPassword", false);
    }
}

void LoginWindow::readSettings()
{
    QSettings settings("iSoftStone", "VSN_login");
    //qDebug() << settings.fileName();

    //restore loginWin geometry
    restoreGeometry(settings.value("loginWinGeometry").toByteArray());

    vsmInput->setText(settings.value("vsmAddr", "").toString());
    userNameInput->setText(settings.value("username", "").toString());
    passwordInput->setText(settings.value("oldPassword", "").toString());

    ui->rememberSettings->setChecked(settings.value("rememberPassword", false).toBool());
}

void LoginWindow::checkUpdateRequest()
{
    QNetworkRequest request;
    request.setUrl(QUrl(vsmAddr+"/vsm/release?devicetype=3"));
    qDebug() << "checking for update..."  << request.url();
    request.setTransferTimeout(3000);
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);
    manager->get(request);
}

void LoginWindow::downloadUrlRequest()
{
    QNetworkRequest downloadUrl;
    downloadUrl.setUrl(QUrl(vsmAddr+"/vsm/download?devicetype=3"));
    downloadUrl.setTransferTimeout(3000);
    manager->get(downloadUrl);
}

void LoginWindow::loginRequest()
{
    deviceType = "3"; //we are win10 type

    QNetworkRequest request;
    request.setTransferTimeout(3000);
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);

    switch (vsmApiVersion) {
    case 1:
        request.setUrl(QUrl(vsmAddr+"/vsm/login"));
        break;
    case 2:
        request.setUrl(QUrl(vsmAddr+"/vsm/mobile/login"));
        break;
    default:
        request.setUrl(QUrl(vsmAddr+"/vsm/login"));
    }
    qDebug() << "login url is: " << request.url();

    QSslConfiguration sslConfig = QSslConfiguration::defaultConfiguration();

    qDebug() << "support ssl? " << QSslSocket::supportsSsl();
    qDebug() << "runtime     : " << QSslSocket::sslLibraryVersionString();
    qDebug() << "compile time: " << QSslSocket::sslLibraryBuildVersionString();


    request.setSslConfiguration(sslConfig);
    request.setRawHeader("Content-Type", "application/x-www-form-urlencoded");
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);

    QByteArray base64Password = oldPassword.toUtf8().toBase64();

    qDebug() << ".......login in......";
    qDebug() << username;
    qDebug() << base64Password;
    QUrlQuery params;
    params.addQueryItem("username", username);
    params.addQueryItem("password", base64Password);
    params.addQueryItem("devicetype", deviceType);

    manager->post(request, params.query().toUtf8());
    //    connect(reply, &QIODevice::readyRead, this, &MyClass::slotReadyRead);
    //    connect(reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
    //             this, &MyClass::slotError);
    //    connect(reply, &QNetworkReply::sslErrors,
    //             this, &MyClass::slotSslErrors);
}

void LoginWindow::replyFinished(QNetworkReply *reply)
{

    if (reply->error() == QNetworkReply::NoError) {
        // if reply did not wrong

        if (reply->url() == QUrl(vsmAddr+"/vsm/release?devicetype=3")) {
            //if reply is /release

            QString strReply = (QString)reply->readAll();
            QJsonDocument jsonResponse = QJsonDocument::fromJson(strReply.toUtf8());
            if (jsonResponse.isObject()) {
                QJsonObject jsonObj = jsonResponse.object();

                if (jsonObj["code"].toInt() == 10000) {
                    QJsonObject rstObj = jsonObj["result"].toObject();

                    vsmApiVersion = rstObj["apiVersion"].toInt();
                    vsmSystemVersion = rstObj["systemVersion"].toInt();

                    qDebug() << "vsmApiVersion: " << vsmApiVersion;
                    qDebug() << "vsmSystemVersion: " << vsmSystemVersion;

                    if(vsmApiVersion > ClientApiVersion) {
                        downloadUrlRequest();
                    } else {
                        //submit username and password to VSM
                        loginRequest();
                    }

                } else {
                    qDebug() << "reply is not a json obj";
                    // login anyways
                    loginRequest();
                }
            }
        } else if (reply->url() == QUrl(vsmAddr+"/vsm/download?devicetype=3")) {
            qDebug() << "reply url is: " << reply->url();
            QString strReply = (QString)reply->readAll();
            QJsonDocument jsonResponse = QJsonDocument::fromJson(strReply.toUtf8());
            if (jsonResponse.isObject()) {
                QJsonObject jsonObj = jsonResponse.object();

                if (jsonObj["code"].toInt() == 10000) {
                    QJsonObject rstObj = jsonObj["result"].toObject();
                    updateUrl = rstObj["url"].toString();
                    qDebug() << "update url is " << updateUrl;

                    Update *updateWin = new Update(this, vsmApiVersion, ClientApiVersion, updateUrl);
                    updateWin->show();

                }
            }
        } else if (reply->url() == QUrl(vsmAddr+"/vsm/mobile/login") ||
                   reply->url() == QUrl(vsmAddr+"/vsm/login")){
            //if reply correspond to login

            QString strReply = (QString)reply->readAll();
            QJsonDocument jsonResponse = QJsonDocument::fromJson(strReply.toUtf8());
            QJsonObject jsonObj = jsonResponse.object();

            if (jsonObj["code"].toInt() == 10000) {
                // if login successfully, we close login window
                close();

                //show main window here
                MainWin *mainWin = new MainWin(username, oldPassword, vsmAddr, jsonObj);

                mainWin->show();
                mainWin->raise();
                mainWin->activateWindow();

                // clear password based on setting
                if (!ui->rememberSettings->isChecked()) {
                    passwordInput->setText("");
                }

                // delete loginWin
                // delete this;
                deleteLater();

                // if login successful
            } else {
                // if login failed, pass message to loginWin
                if (jsonObj["code"].toInt() == 200101) {
                    loginError(tr("<font color=red>username or password is wrong</font>"));
                } else {
                    loginError(jsonObj["message"].toString());
                }
            }
        } // if reply is login
    } else {
        //reply is error;
        qDebug() << "loginWin reply error: " << reply->url() << reply->errorString();
        if  (reply->url().path() == "/vsm/login" ||
             reply->url().path() == "/vsm/mobile/login") {
            loginError(reply->errorString());
            return;
        }
//        if (reply->error() == QNetworkReply::InsecureRedirectError) {
//            vsmAddr = reply->url().host();
//            qDebug() << "Redirect to: " << reply->url();
//        }

        loginRequest();

        reply->deleteLater();
    }
}

void LoginWindow::closeEvent(QCloseEvent *event)
{
    writeSettings();
    event->accept();
}

LoginWindow::~LoginWindow()
{
    delete ui;
}


void LoginWindow::on_loginButton_clicked()
{
    if (vsmAddr.isEmpty())  {
        loginError(tr("<font color=red>please input server address</font>"));
        return;
    }

    QUrl testUrl = QUrl::fromUserInput(vsmAddr);
    if (testUrl.isValid()) {
        vsmAddr = testUrl.toString();
    } else {
        loginError(tr("<font color=red>server address's format is wrong</font>"));
    }

    if (username.isEmpty())  {
        loginError(tr("<font color=red>please input username</font>"));
        return;
    }
    if (oldPassword.isEmpty())  {
        loginError(tr("<font color=red>please input password</font>"));
        return;
    }

    checkUpdateRequest();
}

//void LoginWindow::on_vsmIPInput_textChanged(const QString &arg1)
//{
//    vsmAddr = arg1;
//    labelError->setVisible(false);
//}

void LoginWindow::vsmIPInput_textChanged(const QString &arg1)
{
    vsmAddr = arg1;
    if (!arg1.isEmpty()) {
        vsmInput->showDelete();
    } else {
        vsmInput->hideDelete();
    }
    labelError->setVisible(false);
}

//void LoginWindow::on_userNameInput_textChanged(const QString &arg1)
//{
//    username = arg1;
//    labelError->setVisible(false);
//}

void LoginWindow::userNameInput_textChanged(const QString &arg1)
{
    username = arg1;
    if (!arg1.isEmpty()) {
        userNameInput->showDelete();
    } else {
        userNameInput->hideDelete();
    }
    labelError->setVisible(false);
}

void LoginWindow::passwordInput_textChanged(const QString &arg1)
{
    oldPassword = arg1;
    if (!arg1.isEmpty()) {
        passwordInput->showEye();
    } else {
        passwordInput->hideEye();
    }
    labelError->setVisible(false);
}

void LoginWindow::on_pushButton_clicked()
{
    ForgetPassword *forgetPassword = new ForgetPassword(this);

    forgetPassword->show();

    //    QMessageBox::warning(this, tr("Contact the admin"),
    //                         tr("<p>Please contact the adminstrator "
    //                            "<p>to retrieve the password."));
}

void LoginWindow::on_languageButton_clicked(bool checked)
{
    if (checked) {
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/images/en.png"), QSize(), QIcon::Normal, QIcon::Off);
        ui->languageButton->setIcon(icon);
        setLanguage(true);

    } else {
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/images/zh.png"), QSize(), QIcon::Normal, QIcon::Off);
        ui->languageButton->setIcon(icon);
        setLanguage(false);
    }
}
