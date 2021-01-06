#ifndef MAINWIN_H
#define MAINWIN_H

#include <QMainWindow>
#include <QListWidget>

#include "win10/RasCreateVpn.h"
#include "win10/CertUtils.h"
#include "win10/IpRouteUtils.h"

#include "listitem.h"

class QNetworkReply;
class QNetworkAccessManager;
class LoginWindow;
class QSpacerItem;
class QTranslator;
class QSystemTrayIcon;

extern QTranslator *qtbaseTranslator;
extern QTranslator *translator;

namespace Ui {
class MainWin;
}

/**
  * program routine
  *
  * 1. getVpnListRequest() -> parseVpnList()
  * 2. connectClicked() -> on_breakButton_clicked() -> getCARequest() -> connectVpn() -> setConnectionStatus()
  * 3. on_breakButton_clicked() -> removeCurrentConnection()
  * 4. logoutRequest() -> removeCurrentConnection() -> cleanLoginInfo()
  * 5. buttonLogout() ->
  *
  * */
class MainWin : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWin(const QString &username,
                     const QString &oldPassword,
                     const QString &vsmAddr,
                     const QJsonObject &jsonObj,
                     QWidget *parent = nullptr);
    void isRunning();
    ~MainWin();

protected:
    void changeEvent(QEvent *) override;
    void closeEvent(QCloseEvent *event) override;

private slots:
    void changePasswordRequest(QString original, QString newpass);

    void buttonLogout();

    void setSpeedLevel(int level);

    void about();
    void connectClicked(const QString &name, ListItem * myItem);
    void on_breakButton_clicked();

    void replyFinished(QNetworkReply *reply);

private:
    void writeSettings();
    void readSettings();

    void connectActions();
    void hideGridLayout();

    //login routine
    void setWin10();
    void getVpnListRequest();  //get available vpn list
    void getCARequest();
    void parseVpnList(QJsonArray&);
    void connectVpn();
    void addDefaultGateway();
    void noDefaultGateway();
    void setConnectionStatus();
    QString getMask(const QString& arg);

    //logout routine
    void logoutRequest();
    void removeCurrentConnection();
    void cleanLoginInfo();
    void uninstallCA();

    // Tray
    void createActions();
    void createTrayIcon();

    Ui::MainWin *ui;
    QNetworkAccessManager *manager;

    //login needed
    QString vsmAddr;
    QString username;
    QString oldPassword;
    QString deviceType;

    //logout & vpnList needed
    QString tenantId;
    QString userId;

    //getCA needed
    QString devSerialNumber;

    //current connection info
    std::shared_ptr<RasCreateVpn> ras;
    QString serverIp;
    QString vpnName;
    QStringList subnetList;
    QStringList dns;
    VpnIpAddrS *vid;  //vpn's virtual ip addr

    //change password needed
    QString newPassword;
    QString accessToken;
    QString tenantName;

    //vpnList needed
    //QVector<QListWidgetItem *> items;
    //QVector<ListItem *> itemWidgets;
    QMap<ListItem *, QListWidgetItem *> map;

    QMap<QString, QJsonObject> vpnMap;
    QString currentVPNConn;
    QListWidgetItem *currItem;
    ListItem *currItemWidget;

    const QString caFile = "root_CA.cer";
    QString caName;

    //status
    bool caInstalled;
    bool setRoute;

    //font
    QFont font;

    // Tray
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;

    QAction *minimizeAction;
    //QAction *maximizeAction;
    QAction *restoreAction;
    QAction *quitAction;

};

#endif // MAINWIN_H
