#ifndef RASCREATEVPN_H
#define RASCREATEVPN_H

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#pragma message("library is linking with \"rasapi32.lib\"")
#pragma comment(lib, "rasapi32.lib")

#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <Ras.h>
#include <RasError.h>

#include "rasdlg.h"
#include <tchar.h>
#include "strsafe.h"

#include <QObject>
#include <QWinEventNotifier>
#include <QDebug>

#define PHONE_NUMBER_LENGTH 20
#define DEVICE_NAME_LENGTH 100
#define DEVICE_TYPE_LENGTH 16


/*
    vpn使用代码
    RasCreateVpn *ras1 = new RasCreateVpn(L"test183", L"182.92.222.196");
	ras1->addVpn();
	ras1->setCredentials(L"xitong", L"Vpp123");
	ras1->connect();
	ras1->disConnect();
	ras1->removeVpn();
*/
class RasCreateVpn : public QObject
{
    Q_OBJECT

public:
    RasCreateVpn(PCWSTR vpnName, PCWSTR vpnServer, QObject *parent = nullptr);

    bool addVpn();
    bool addVpnDefaultGateway();
    bool setCredentials(PCWSTR userName, PCWSTR password);
    bool connect();
    bool getStatus();
    bool disConnect();
    bool removeVpn();

    QWinEventNotifier *winNotifier;


private:
    static void WINAPI RasDialFunc(UINT unMsg, RASCONNSTATE rasconnstate, DWORD dwError);
    PCWSTR _vpnName;
    PCWSTR _vpnServer;
    RASCREDENTIALS _rascredentials;
    HRASCONN _hRasConn;
};

bool fixReg();  // return false if failed



#endif //RASCREATEVPN_H
