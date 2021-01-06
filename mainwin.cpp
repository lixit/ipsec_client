#include "mainwin.h"
#include "ui_mainwin.h"

#include <QtWidgets>
#include <QNetworkAccessManager>
#include <QSslCertificate>
#include <QUrlQuery>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QMessageBox>
#include <QSystemTrayIcon>

#include "login.h"

#ifdef Q_OS_WIN
#include <windows.h> // for Sleep
#endif

using namespace std::string_literals;


MainWin::MainWin(const QString &username,
                 const QString &oldPassword,
                 const QString &vsmAddr,
                 const QJsonObject &jsonObj,
                 QWidget *parent) :
    QMainWindow(parent, Qt::CustomizeWindowHint | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint),
    ui(new Ui::MainWin),
    ras(nullptr),
    vid(nullptr),
    caInstalled(false)
{

    // set win10 register
    if (!fixReg()) {
        QMessageBox::warning(this, tr("please run as admin"),
                             tr("<h2>please run as admin</h2>"));
        close();
        QTimer::singleShot(1000, this, SLOT(close()));
    }

    this->vsmAddr=vsmAddr;
    this->username=username;
    this->oldPassword=oldPassword;

    if (jsonObj.contains("result") && jsonObj["result"].isObject()) {
        auto rstObj = jsonObj["result"].toObject();
        if (rstObj.contains("userData") && rstObj["userData"].isObject()) {
            auto dataObj = rstObj["userData"].toObject();
            tenantName = dataObj["tenantName"].toString();
            tenantId = dataObj["tenantId"].toString();
            userId = dataObj["userId"].toString();


            // returned username is same as one used in login, so not useful.
            //username = dataObj["username"].toString();
        }
        if (rstObj.contains("token") && rstObj["token"].isString()) {
            accessToken = rstObj["token"].toString();
        }
    }
    deviceType = QString::number(3);

    readSettings();

    setWindowIcon(QIcon(":/images/icon.png"));
    font.setPointSize(12);

    ui->setupUi(this);
    ui->labelWelcome->setText(tr("Welcome ") + tenantName);

    hideGridLayout();

    manager = new QNetworkAccessManager(this);
    manager->setTransferTimeout(3000);

    connectActions();


    createActions();
    createTrayIcon();

    getVpnListRequest();
}

void MainWin::connectActions()
{
    connect(manager, &QNetworkAccessManager::finished, this, &MainWin::replyFinished);
    connect(ui->exitButton, &QPushButton::clicked, this, &MainWin::buttonLogout);

//    connect(ui->actionabout, &QAction::triggered, this, &MainWin::about);
//    connect(ui->actionAbout_Qt, &QAction::triggered, qApp, &QApplication::aboutQt);
}

void MainWin::hideGridLayout()
{
    ui->breakButton->hide();
    ui->connectionLabel->show();

    ui->nameLabel->hide();
    ui->ipLabel->hide();
    ui->ipData->hide();
    ui->maskLabel->hide();
    ui->maskData->hide();
    ui->iconLayout->removeItem(ui->gridLayout);
    ui->iconLayout->removeItem(ui->thirdSpaceer);
}

void MainWin::isRunning()
{
    QMessageBox::warning(nullptr, tr("warning"), tr("<p>the program is running."));
}

void MainWin::changePasswordRequest(QString original, QString newpass)
{
    QNetworkRequest request;
    request.setUrl(QUrl(vsmAddr+"/vsm/mobile/password"));
    request.setRawHeader("Content-Type", "application/json");
    request.setRawHeader("accessToken", accessToken.toUtf8());

    //save the new password to MainWin
    newPassword = newpass;
    QJsonObject json;
    json.insert("tenantId", tenantId);
    json.insert("userId", userId);
    json.insert("devicetype", deviceType);
    json.insert("oldPassword", original);
    json.insert("newPassword", newpass);
    QJsonDocument jsonDoc(json);

    QByteArray jsonData= jsonDoc.toJson();

    qDebug() << "query paramters are\n" << jsonDoc;
    manager->put(request, jsonData);
}

void MainWin::replyFinished(QNetworkReply *reply)
{
    /**
      getVpnList() -> getCA()
      */

    if (reply->error() == QNetworkReply::NoError) {
        // if reply did not wrong

        if (reply->url() == QUrl(vsmAddr+"/vsm/mobile/v1/ca")) {
            //if reply is a getCARequest()

            QString strReply = (QString)reply->readAll();
            QJsonDocument jsonResponse = QJsonDocument::fromJson(strReply.toUtf8());
            QJsonObject jsonObj = jsonResponse.object();

            if (jsonObj["code"].toInt() == 10000) {
                // if we get a CA
                if (jsonObj.contains("result") && jsonObj["result"].isObject()) {

                    QJsonObject rstObj = jsonObj["result"].toObject();
                    if (rstObj.contains("cer") && rstObj["cer"].isString()) {

                        QString cer = rstObj["cer"].toString();

                        //get caName
                        auto certObj = QSslCertificate(cer.toUtf8(), QSsl::Pem);
                        caName = certObj.subjectDisplayName();
                        qDebug() << "caName: " << caName;

                        //save CA certificate to file
                        QFile file(caFile);
                        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                            qDebug() << "failed to open " << caFile;
                        }
                        QTextStream out(&file);
                        out << cer;
                        file.close();


                        //install CA cert
                        if (CERT_SUCCESS != CertUtils::installCaCert(L"root_CA.cer")) {
                            qDebug() << "failed to install CA";
                            QMessageBox::warning(this, tr("install CA"), tr("failed to install CA"));
                        } else {
                            caInstalled = true;
                            qDebug() << "CA installed";
                        }

                        //connect a vpn
                        connectVpn();
                    }
                }
            } else if (jsonObj["code"].toInt() == 10010) {
                // not login
                //QMessageBox::warning(this, tr("failed to install CA"), jsonObj["message"].toString());
                QMessageBox::warning(this, tr("failed to install CA"), tr("Login has expired, please log in again."));

                close();

                //Todo: create login window here
                LoginWindow *loginWin = new LoginWindow(nullptr);
                loginWin->show();
                loginWin->raise();
                loginWin->activateWindow();

            } else {
                // if we don't get a CA
                QMessageBox::warning(this, tr("failed to install CA"), jsonObj["message"].toString());
                connectVpn();
            }
        } else if (reply->url() == QUrl(vsmAddr+"/vsm/mobile/vpns")) {

            QString strReply = (QString)reply->readAll();
            QJsonDocument jsonResponse = QJsonDocument::fromJson(strReply.toUtf8());

            QJsonObject jsonObj = jsonResponse.object();

            if (jsonObj["code"].toInt() == 10000) {
                // if getVpnList successfully
                if (jsonObj.contains("result") && jsonObj["result"].isArray()) {
                    QJsonArray rstArry = jsonObj["result"].toArray();
                    parseVpnList(rstArry);
                }
            } else {
                QMessageBox::warning(this, tr("failed to get vpn list"), jsonObj["message"].toString());
                buttonLogout();
            }

        } else if (reply->url() == QUrl(vsmAddr+"/vsm/mobile/logout")) {
            // if reply correspond to logout
            // don't do anything

            //            QString strReply = (QString)reply->readAll();
            //            QJsonDocument jsonResponse = QJsonDocument::fromJson(strReply.toUtf8());
            //            QJsonObject jsonObj = jsonResponse.object();
            //            if (jsonObj["code"].toInt() == 10000) {
            //                // if logout successfully
            //                loginWin->loginError(tr("logout successfully"));
            //            }

        } else if (reply->url() == QUrl(vsmAddr+"/vsm/mobile/password")) {
            //if reply is changePassword
//            QString strReply = (QString)reply->readAll();
//            QJsonDocument jsonResponse = QJsonDocument::fromJson(strReply.toUtf8());
//            QJsonObject jsonObj = jsonResponse.object();
//            if (jsonObj["code"].toInt() == 10000) {
//                // if changed password, logout.
//                changePasswordWin->clear();
//                changePasswordWin->accept();
//                //loginWin->clearPasswordInput();
//                buttonLogout();
//                //loginWin->loginError(tr("changed password, please login again"));
//            } else {
//                if (jsonObj["code"].toInt() == 200204) {
//                    changePasswordWin->errorInfo(tr("original password is wrong"));
//                }
//                else if (jsonObj["code"].toInt() == 200207) {
//                    changePasswordWin->errorInfo(tr("new password's format is wrong.\nIt must include uppercase, lowercase\nletters and numeric digits,\nand must be 8-32 characters."));
//                }
//                else {
//                    changePasswordWin->errorInfo(jsonObj["message"].toString());
//                }
//            }
        }

    } // if reply did not wrong

    else {
        // if reply is error
        //QMessageBox::warning(loginWin, tr("Network error"), reply->errorString());
        qDebug() << "mainWin reply error: " << reply->url() << reply->error();
        reply->deleteLater();
    }
}



void MainWin::buttonLogout()
{
    logoutRequest();

    //don't wait for response
    removeCurrentConnection();
    cleanLoginInfo();
    uninstallCA();
    close();



    //Todo: create login window here
    LoginWindow *loginWin = new LoginWindow(nullptr);
    loginWin->show();
    loginWin->raise();
    loginWin->activateWindow();

    delete this;
}

void MainWin::logoutRequest()
{
    QNetworkRequest request;
    request.setUrl(QUrl(vsmAddr+"/vsm/mobile/logout"));
    request.setRawHeader("Content-Type", "application/json");
    request.setRawHeader("accessToken", accessToken.toUtf8());

    QJsonObject json;
    json.insert("tenantId", tenantId);
    json.insert("userId", userId);
    json.insert("devicetype", deviceType);
    QJsonDocument jsonDoc(json);
    QByteArray jsonData= jsonDoc.toJson();

    manager->post(request, jsonData);
}

void MainWin::setSpeedLevel(int level)
{
    QIcon icon;

    switch ( level ) {
        case 1:
        icon.addFile(QString::fromUtf8(":/images/icon_signl_three.svg"), QSize(), QIcon::Normal, QIcon::Off);
        ui->breakButton->setIcon(icon);
        break;
    case 2:
        icon.addFile(QString::fromUtf8(":/images/icon_signl_two.svg"), QSize(), QIcon::Normal, QIcon::Off);
        ui->breakButton->setIcon(icon);
        break;
    case 3:
        icon.addFile(QString::fromUtf8(":/images/icon_signl_one.svg"), QSize(), QIcon::Normal, QIcon::Off);
        ui->breakButton->setIcon(icon);
        break;
    default:
        icon.addFile(QString::fromUtf8(":/images/icon_signl_no.svg"), QSize(), QIcon::Normal, QIcon::Off);
        ui->breakButton->setIcon(icon);
    }

}

void MainWin::getVpnListRequest()
{
    QNetworkRequest request;
    request.setUrl(QUrl(vsmAddr+"/vsm/mobile/vpns"));
    request.setRawHeader("Content-Type", "application/json");
    request.setRawHeader("accessToken", accessToken.toUtf8());

    QJsonObject json;
    json.insert("tenantId", tenantId);
    json.insert("userId", userId);
    json.insert("devicetype", deviceType);
    QJsonDocument jsonDoc(json);

    QByteArray jsonData= jsonDoc.toJson(QJsonDocument::Compact);

    manager->post(request, jsonData);
}

void MainWin::parseVpnList(QJsonArray &vpnArray)
{
    ui->vpnLW->clear();
    vpnMap.clear();

    for (auto vpnItem : vpnArray)
    {
        if (!vpnItem.isObject())
        {
            continue;
        }

        auto vpnObj = vpnItem.toObject();
        if (!vpnObj.contains("dev_name"))
        {
            continue;
        }

        QString dev_name = vpnObj["dev_name"].toString();

        ListItem *listItem = new ListItem(ui->vpnLW);
        listItem->setName(dev_name);
        listItem->setIP(vpnObj["service"].toString());
        listItem->setIcmpDelay();
        connect(listItem, &ListItem::clicked, this, &MainWin::connectClicked);        

        QListWidgetItem *item = new QListWidgetItem(ui->vpnLW);
        //item->setFont(font);
        //item->setText(vpnObj["dev_name"].toString() + "\n" + vpnObj["service"].toString());
        //item->setHidden(true);
        item->setFlags(Qt::NoItemFlags);
        item->setSizeHint(listItem->minimumSizeHint());        

        map[listItem] = item;

        ui->vpnLW->addItem(item);
        ui->vpnLW->setItemWidget(item, listItem);

        vpnMap[dev_name] = vpnObj;
    }
}


void MainWin::getCARequest()
{
    QNetworkRequest request;
    request.setUrl(QUrl(vsmAddr+"/vsm/mobile/v1/ca"));
    request.setRawHeader("Content-Type", "application/json");
    request.setRawHeader("accessToken", accessToken.toUtf8());

    QJsonObject json;
    json.insert("tenantId", tenantId);
    json.insert("userId", userId);
    json.insert("devicetype", deviceType);
    json.insert("devSerialNumber", devSerialNumber);

    QJsonDocument jsonDoc(json);
    QByteArray jsonData= jsonDoc.toJson(QJsonDocument::Compact);

    manager->post(request, jsonData);
}

void MainWin::connectVpn()
{
    LPCWSTR vpn_name = reinterpret_cast<LPCWSTR>(vpnName.utf16());
    LPCWSTR ip = reinterpret_cast<LPCWSTR>(serverIp.utf16());

    LPCWSTR vpnUsername = reinterpret_cast<LPCWSTR>(username.utf16());
    LPCWSTR vpnPassword = reinterpret_cast<LPCWSTR>(oldPassword.utf16());

    qDebug() << "---------connecting vpn ------------";
    qDebug() << "vpnName: " << vpnName;
    qDebug() << "vpn Ip:" << serverIp;
    qDebug() << "username" << username;
    qDebug() << "oldPassword" << oldPassword;

    if (!ras) {
        ras = std::make_shared<RasCreateVpn>(vpn_name, ip);
    }

    // whether default gateway depends on subnets
    if (subnetList.isEmpty()) {
        //add default gateway version
        qDebug() << "cidr is empty, adding default Gateway version";
        addDefaultGateway();
        setRoute = false;
    } else {
        QStringList::const_iterator constIterator;
        for (constIterator = subnetList.constBegin(); constIterator != subnetList.constEnd(); ++constIterator) {
             qDebug() << "Subnet: "  << (*constIterator);
            if ((*constIterator) == "0.0.0.0/0") {
                break;
            }
        }

        if (constIterator == subnetList.constEnd()) {
            //if search through end, didn't find 0.0.0.0/0. add no default route
            qDebug() << "no 0.0.0.0/0 find, add no default gateway";
            noDefaultGateway();
            setRoute = true;
        } else {
            qDebug() << "find 0.0.0.0/0, add default gateway";
            addDefaultGateway();
            setRoute = false;
        }
    }

    if (ras->setCredentials(vpnUsername, vpnPassword) != true ) {
        //QMessageBox::warning(this, tr("vpn info"), tr("failed to setCredentials"));
    }

    if (ras->connect() != true) {
        QMessageBox::warning(this, tr("vpn info"), tr("failed to connect"));
        ras->disConnect();
        ras->removeVpn();
        ras.reset();

    } else {
        //connected vpn
        currentVPNConn = serverIp;

        if(setRoute) {
            //add route
            vid =  new VpnIpAddrS();
            vid->VpnServer = serverIp.toStdString().c_str();
            IpRouteUtils ipu;
            if (ipu.GetIpAddr(vpn_name, vid) == 0) {
                qDebug() << "get ip addr succeed";
            } else {
                qDebug() << "get ip addr failed";
            }

            for (QString cidr : subnetList) {

                QString tempary = cidr;
                QString myIp = cidr.remove(QRegExp("[/][0-9]{1,3}$"));
                QString myMask = tempary.remove(QRegExp("^[0-9]{1,3}[.][0-9]{1,3}[.][0-9]{1,3}[.][0-9]{1,3}[/]"));

                qDebug() << "setting route....";
                QByteArray array1 = myIp.toLocal8Bit();
                vid->ForwardDest = array1.constData();
                qDebug() << "ForwardDest = " << vid->ForwardDest;

                QByteArray array2 = getMask(myMask).toLocal8Bit();
                vid->ForwardMask = array2.constData();
                qDebug() << "ForwardMask = " << vid->ForwardMask;

                vid->ForwardNextHop = "0.0.0.0";

                if (ipu.CreateIpRoute(vid) == 0) {
                    qDebug() << "add route succeed";
                } else {
                    qDebug() << "add route failed";
                }
            }

        }  // if(setRoute)

        setConnectionStatus();

        connect(ras->winNotifier, &QWinEventNotifier::activated, this, &MainWin::on_breakButton_clicked);
        //ipu.DeleteIpRoute(&vid);
    }
}

void MainWin::addDefaultGateway()
{
    if (ras->addVpnDefaultGateway() != true) {
        QMessageBox::warning(this, tr("vpn info"), tr("failed to addVpn"));
        ras->disConnect();
        ras->removeVpn();
        ras->addVpnDefaultGateway();
    }
}

void MainWin::noDefaultGateway()
{
    if (ras->addVpn() != true) {
        QMessageBox::warning(this, tr("vpn info"), tr("failed to addVpn"));
        ras->disConnect();
        ras->removeVpn();
        ras->addVpn();
    }

}

QString MainWin::getMask(const QString& arg) {
    switch (arg.toInt()) {
    case 0:
        return "0.0.0.0";
    case 8:
        return "255.0.0.0";
    case 16:
        return "255.255.0.0";
    case 24:
        return "255.255.255.0";
    case 32:
        return "255.255.255.255";
    default:
        return "";
    }
}

void MainWin::about()
{
    QMessageBox::about(this, tr("About VSN"),
                       tr("<h2>VSN 0.1</h2>"
                          "<p>Copyright &copy; 2020 iSoftStone."
                          "<p>VSN is a vpn application developed "
                          "by iSoftStone."
                          ));

}

void MainWin::writeSettings()
{
    //QSettings settings("iSoftStone", "VSN_main");
    QSettings settings_login("iSoftStone", "VSN_login");

    settings_login.setValue("loginWinGeometry", saveGeometry());
}

void MainWin::readSettings()
{
    //QSettings settings("iSoftStone", "VSN_main");
    QSettings settings_login("iSoftStone", "VSN_login");

    //restore mainWin geometry
    restoreGeometry(settings_login.value("loginWinGeometry").toByteArray());
}

void MainWin::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);

    } else {
        QWidget::changeEvent(event);
    }
}

void MainWin::closeEvent(QCloseEvent *event)
{
    removeCurrentConnection();
    logoutRequest();
    writeSettings();
    event->accept();
}

void MainWin::setConnectionStatus()
{
    qDebug() << "setting status....";

    // hide no connection label
    ui->connectionLabel->hide();

    //show break button
    ui->breakButton->show();

    //should hide item here
    currItemWidget->hide();
//    ui->vpnLW->removeItemWidget(currItem);
    int row = ui->vpnLW->row(currItem);
    currItem = ui->vpnLW->takeItem(row);

    map.remove(currItemWidget);

    // show connection info
    ui->nameLabel->setText(vpnName);

    VpnIpAddrS temp;
    temp.VpnServer = serverIp.toStdString().c_str();
    IpRouteUtils ipu;
    LPCWSTR vpn_name = reinterpret_cast<LPCWSTR>(vpnName.utf16());
    ipu.GetIpAddr(vpn_name, &temp);
    ui->ipData->setText(temp.IP);

    ui->nameLabel->show();
    ui->ipLabel->show();
    ui->ipData->show();
    ui->maskLabel->show();
    ui->maskData->show();

    ui->iconLayout->insertItem(3, ui->gridLayout);
    ui->iconLayout->insertItem(4, ui->thirdSpaceer);

    //show subnet info
//    if (!subnetList.isEmpty()) {
//        for (QString subnet : subnetList) {
//            QLabel *subnetLabel = new QLabel(ui->centralwidget);
//            subnetLabel->setFont(font);
//            subnetLabel->setText(tr("subnet: ") + subnet);
//            connectionLayout->addWidget(subnetLabel);
//        }
//    } else {
//        QLabel *subnetLabel = new QLabel(ui->centralwidget);
//        subnetLabel->setFont(font);
//        subnetLabel->setText(tr("no"));
//        connectionLayout->addWidget(subnetLabel);
//    }

    //horizontalSpacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

    //ui->iconLayout->addLayout(connectionLayout);
    //ui->iconLayout->addItem(horizontalSpacer);

    //show break button
}

void MainWin::removeCurrentConnection()
{
    if (!currentVPNConn.isEmpty()) {
        qDebug() << "removing current connection.. ";
        ras->disConnect();
        ras->removeVpn();
        ras.reset();

        delete vid;

        hideGridLayout();

        currentVPNConn = "";
        subnetList.clear();
    }
}

void MainWin::cleanLoginInfo()
{
    deviceType = "";
    tenantId = "";
    userId = "";
    devSerialNumber = "";
    accessToken = "";

    tenantName = "";
    vpnMap = QMap<QString, QJsonObject>();

    trayIcon->hide();
}

void MainWin::uninstallCA()
{
    if (caInstalled) {
        caName = "Linkz CA";
        if (CERT_SUCCESS == CertUtils::uninstall(L"Linkz CA")) {
            caInstalled = false;
            qDebug() << "uninstalled CA";
        }
    }
}

MainWin::~MainWin()
{
    delete ui;
}

// first install a correspond CA.
void MainWin::connectClicked(const QString &name, ListItem *myItem)
{
    on_breakButton_clicked();

    subnetList.clear();
    uninstallCA();

    currItemWidget = myItem;

    currItem = map[myItem];

    // display speed level in Break button;
    connect(currItemWidget, &ListItem::speed, this, &MainWin::setSpeedLevel);

    QJsonObject vpnObj = vpnMap[name];

    //set up current connection
    serverIp = vpnObj["service"].toString();
    vpnName = vpnObj["dev_name"].toString();

    qDebug() << "---------parsing subnet----------";
    //parse subnet to string list
    if (vpnObj.contains("subnet") && vpnObj["subnet"].isArray()) {
        QJsonArray subnetArray = vpnObj["subnet"].toArray();
        for (QJsonValueRef subnetItem : subnetArray) {
            if (subnetItem.isString()) {
                subnetList += subnetItem.toString();
            }
        }
    }
    qDebug() << "subnet list is: " <<subnetList;

    //parse dns to string list
    if (vpnObj.contains("dns") && vpnObj["dns"].isArray()) {
        auto dnsArray = vpnObj["dns"].toArray();
        for (auto dnsItem : dnsArray) {
            if (!dnsItem.isObject()) {
                continue;
            }
            if (dnsItem.isString()) {
                dns += dnsItem.toString();
            }
        }
    }

    //setup serial number to getCA
    devSerialNumber = vpnObj["dev_ser_num"].toString();
    getCARequest();
    qDebug() << "------------installing CA------------";
}

void MainWin::on_breakButton_clicked()
{
    if (!currentVPNConn.isEmpty()) {

        //disconnect(currItemWidget, &ListItem::speed, this, &MainWin::setSpeedLevel);

        QJsonObject currObj = vpnMap[vpnName];

        ListItem *listItem = new ListItem(ui->vpnLW);
        listItem->setName(vpnName);
        listItem->setIP(currObj["service"].toString());
        listItem->setIcmpDelay();


        currItemWidget= listItem;

        map[listItem] = currItem;

        ui->vpnLW->addItem(currItem);
        ui->vpnLW->setItemWidget(currItem, currItemWidget);
        currItemWidget->show();
        connect(listItem, &ListItem::clicked, this, &MainWin::connectClicked);
    }

    //ui->vpnLW->addItem(currItem);
    //ui->vpnLW->setItemWidget(currItem, currItemWidget);
    removeCurrentConnection();



//#ifdef Q_OS_WIN
//    Sleep(uint(500));
//#else
//    struct timespec ts = { ms / 1000, (ms % 1000) * 1000 * 1000 };
//    nanosleep(&ts, NULL);
//#endif
}

void MainWin::createActions()
{
    minimizeAction = new QAction(tr("Mi&nimize"), this);
    connect(minimizeAction, &QAction::triggered, this, &QWidget::hide);

    //maximizeAction = new QAction(tr("Ma&ximize"), this);
    //connect(maximizeAction, &QAction::triggered, this, &QWidget::showMaximized);

    restoreAction = new QAction(tr("&Restore"), this);
    connect(restoreAction, &QAction::triggered, this, &QWidget::showNormal);

    quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
}

void MainWin::createTrayIcon()
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(minimizeAction);
    //trayIconMenu->addAction(maximizeAction);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->setIcon(QIcon(":/images/icon.png"));
    trayIcon->show();

    connect(trayIcon, &QSystemTrayIcon::activated, this,
            [=](QSystemTrayIcon::ActivationReason reason){
        qDebug() << "tray activated: " << reason;
        qDebug() << "before window state is " << windowState();
        if (reason == QSystemTrayIcon::Trigger) {
            qDebug() << "tray was clicked";
            setWindowState(windowState() ^ Qt::WindowMinimized | Qt::WindowActive);
            qDebug() << "after window state is " << windowState();
            if (windowState() == Qt::WindowNoState || windowState() == Qt::WindowActive) {
                show();
                raise();
            } else {
                hide();
            }
        }
    });

}
