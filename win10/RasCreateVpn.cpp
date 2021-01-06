#include "RasCreateVpn.h"

RasCreateVpn::RasCreateVpn(PCWSTR vpnName, PCWSTR vpnServer, QObject *parent):
    QObject(parent),
    winNotifier(nullptr),
    _vpnName(vpnName),
    _vpnServer(vpnServer) {

}

bool RasCreateVpn::addVpn() {

    DWORD dwError = ERROR_SUCCESS;
    LPCTSTR lpszDeviceName = L"Vpn";

    // Allocate heap memory and initialize RASENTRY structure

    DWORD dwBufferSize = 0;
    RasGetEntryProperties(NULL, L"", NULL, &dwBufferSize, NULL, NULL);
    LPRASENTRY lpentry = (LPRASENTRY)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwBufferSize);

    // Allocate heap memory and initialize RASDIALDLG structure
    LPRASDIALDLG lpInfo = (LPRASDIALDLG)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(RASDIALDLG));
    if (lpentry == NULL || lpInfo == NULL) {
        wprintf(L"HeapAlloc failed");
        HeapFree(GetProcessHeap(), 0, lpentry);
        HeapFree(GetProcessHeap(), 0, lpInfo);
        return false;
    }
    // The RASDIALDLG and RASENTRY dwSize members have to be initialized or the RasDialDlg()
    // RasSetEntryProperties() APIs will fail below.
    lpInfo->dwSize = sizeof(RASDIALDLG);
    //ZeroMemory(lpentry, sizeof(RASENTRY));
    lpentry->dwSize = dwBufferSize;
    lpentry->dwFramingProtocol = RASFP_Ppp;
    lpentry->dwfNetProtocols = RASNP_Ip | RASNP_Ipv6;
    lpentry->dwfOptions = 0;
    lpentry->dwType = RASET_Vpn;
    lpentry->dwVpnStrategy = VS_Ikev2Only;
    lpentry->dwEncryptionType = ET_Optional;
    lpentry->dwRedialCount = 3;
    lpentry->dwRedialPause = 60;
    lpentry->dwCustomAuthKey = 0;
    lpentry->dwDialExtraPercent =75;
    lpentry->dwHangUpExtraPercent = 10;
    lpentry->dwSubEntries = 0;

    lpentry->dwfOptions = lpentry->dwfOptions | RASEO_ModemLights;
    lpentry->dwfOptions = lpentry->dwfOptions | RASEO_RequireEAP;

    lpentry->dwfOptions = lpentry->dwfOptions | RASEO_PreviewUserPw;
    lpentry->dwfOptions = lpentry->dwfOptions | RASEO_PreviewDomain;
    lpentry->dwfOptions = lpentry->dwfOptions | RASEO_ShowDialingProgress;
    //lpentry->dwfOptions = lpentry->dwfOptions | RASEO_RemoteDefaultGateway;
    lpentry->dwfOptions = lpentry->dwfOptions | RASEO_IpHeaderCompression;

    lpentry->dwfOptions2 = lpentry->dwfOptions2 | RASEO2_IPv6RemoteDefaultGateway;
    lpentry->dwfOptions2 = lpentry->dwfOptions2 | RASEO2_DontNegotiateMultilink; //不为单路连接协商多路链接
    lpentry->dwfOptions2 = lpentry->dwfOptions2 | RASEO2_Internet;
    lpentry->dwfOptions2 = lpentry->dwfOptions2 | RASEO2_ReconnectIfDropped;//断线重拨
    lpentry->dwfOptions2 = lpentry->dwfOptions2 | RASEO2_UsePreSharedKey;
    lpentry->dwfOptions2 = lpentry->dwfOptions2 | RASEO2_UseTypicalSettings;

    dwError |= StringCchCopyN(lpentry->szLocalPhoneNumber, RAS_MaxPhoneNumber, this->_vpnServer, PHONE_NUMBER_LENGTH);
    dwError |= StringCchCopyN(lpentry->szDeviceName, RAS_MaxDeviceName, lpszDeviceName, DEVICE_NAME_LENGTH);
    dwError |= StringCchCopyN(lpentry->szDeviceType, RAS_MaxDeviceType, RASDT_Vpn, DEVICE_TYPE_LENGTH);

    if (dwError != S_OK) {
        wprintf(L"Structure initilization failed: Error = %d\n", dwError);
        HeapFree(GetProcessHeap(), 0, lpentry);
        HeapFree(GetProcessHeap(), 0, lpInfo);
        return false;
    }

    // Validate the new entry's name
    dwError = RasValidateEntryName(NULL, this->_vpnName);
    if (dwError != ERROR_SUCCESS) {
        wprintf(L"RasValidateEntryName failed: Error = %d\n", dwError);
        HeapFree(GetProcessHeap(), 0, lpentry);
        HeapFree(GetProcessHeap(), 0, lpInfo);
        return false;
    }

    // Create and set the new entry's properties
    dwError = RasSetEntryProperties(NULL, this->_vpnName, lpentry, lpentry->dwSize, NULL, 0);
    if (dwError != ERROR_SUCCESS) {
        wprintf(L"RasSetEntryProperties failed: Error = %d\n", dwError);
        HeapFree(GetProcessHeap(), 0, lpentry);
        HeapFree(GetProcessHeap(), 0, lpInfo);
        return false;
    }
    HeapFree(GetProcessHeap(), 0, lpentry);
    HeapFree(GetProcessHeap(), 0, lpInfo);

    return true;
}

bool RasCreateVpn::addVpnDefaultGateway() {

    DWORD dwError = ERROR_SUCCESS;
    LPCTSTR lpszDeviceName = L"Vpn";

    // Allocate heap memory and initialize RASENTRY structure

    DWORD dwBufferSize = 0;
    RasGetEntryProperties(NULL, L"", NULL, &dwBufferSize, NULL, NULL);
    LPRASENTRY lpentry = (LPRASENTRY)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwBufferSize);

    // Allocate heap memory and initialize RASDIALDLG structure
    LPRASDIALDLG lpInfo = (LPRASDIALDLG)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(RASDIALDLG));
    if (lpentry == NULL || lpInfo == NULL) {
        wprintf(L"HeapAlloc failed");
        HeapFree(GetProcessHeap(), 0, lpentry);
        HeapFree(GetProcessHeap(), 0, lpInfo);
        return false;
    }
    // The RASDIALDLG and RASENTRY dwSize members have to be initialized or the RasDialDlg()
    // RasSetEntryProperties() APIs will fail below.
    lpInfo->dwSize = sizeof(RASDIALDLG);
    //ZeroMemory(lpentry, sizeof(RASENTRY));
    lpentry->dwSize = dwBufferSize;
    lpentry->dwFramingProtocol = RASFP_Ppp;
    lpentry->dwfNetProtocols = RASNP_Ip | RASNP_Ipv6;
    lpentry->dwfOptions = 0;
    lpentry->dwType = RASET_Vpn;
    lpentry->dwVpnStrategy = VS_Ikev2Only;
    lpentry->dwEncryptionType = ET_Optional;
    lpentry->dwRedialCount = 3;
    lpentry->dwRedialPause = 60;
    lpentry->dwCustomAuthKey = 0;
    lpentry->dwDialExtraPercent =75;
    lpentry->dwHangUpExtraPercent = 10;
    lpentry->dwSubEntries = 0;

    lpentry->dwfOptions = lpentry->dwfOptions | RASEO_ModemLights;
    lpentry->dwfOptions = lpentry->dwfOptions | RASEO_RequireEAP;

    lpentry->dwfOptions = lpentry->dwfOptions | RASEO_PreviewUserPw;
    lpentry->dwfOptions = lpentry->dwfOptions | RASEO_PreviewDomain;
    lpentry->dwfOptions = lpentry->dwfOptions | RASEO_ShowDialingProgress;
    lpentry->dwfOptions = lpentry->dwfOptions | RASEO_RemoteDefaultGateway;
    lpentry->dwfOptions = lpentry->dwfOptions | RASEO_IpHeaderCompression;

    lpentry->dwfOptions2 = lpentry->dwfOptions2 | RASEO2_IPv6RemoteDefaultGateway;
    lpentry->dwfOptions2 = lpentry->dwfOptions2 | RASEO2_DontNegotiateMultilink; //不为单路连接协商多路链接
    lpentry->dwfOptions2 = lpentry->dwfOptions2 | RASEO2_Internet;
    lpentry->dwfOptions2 = lpentry->dwfOptions2 | RASEO2_ReconnectIfDropped;//断线重拨
    lpentry->dwfOptions2 = lpentry->dwfOptions2 | RASEO2_UsePreSharedKey;
    lpentry->dwfOptions2 = lpentry->dwfOptions2 | RASEO2_UseTypicalSettings;

    dwError |= StringCchCopyN(lpentry->szLocalPhoneNumber, RAS_MaxPhoneNumber, this->_vpnServer, PHONE_NUMBER_LENGTH);
    dwError |= StringCchCopyN(lpentry->szDeviceName, RAS_MaxDeviceName, lpszDeviceName, DEVICE_NAME_LENGTH);
    dwError |= StringCchCopyN(lpentry->szDeviceType, RAS_MaxDeviceType, RASDT_Vpn, DEVICE_TYPE_LENGTH);

    if (dwError != S_OK) {
        wprintf(L"Structure initilization failed: Error = %d\n", dwError);
        HeapFree(GetProcessHeap(), 0, lpentry);
        HeapFree(GetProcessHeap(), 0, lpInfo);
        return false;
    }

    // Validate the new entry's name
    dwError = RasValidateEntryName(NULL, this->_vpnName);
    if (dwError != ERROR_SUCCESS) {
        wprintf(L"RasValidateEntryName failed: Error = %d\n", dwError);
        HeapFree(GetProcessHeap(), 0, lpentry);
        HeapFree(GetProcessHeap(), 0, lpInfo);
        return false;
    }

    // Create and set the new entry's properties
    dwError = RasSetEntryProperties(NULL, this->_vpnName, lpentry, lpentry->dwSize, NULL, 0);
    if (dwError != ERROR_SUCCESS) {
        wprintf(L"RasSetEntryProperties failed: Error = %d\n", dwError);
        HeapFree(GetProcessHeap(), 0, lpentry);
        HeapFree(GetProcessHeap(), 0, lpInfo);
        return false;
    }
    HeapFree(GetProcessHeap(), 0, lpentry);
    HeapFree(GetProcessHeap(), 0, lpInfo);

    return true;
}

bool RasCreateVpn::setCredentials(PCWSTR userName, PCWSTR password) {
    DWORD dwError = ERROR_SUCCESS;
    RASCREDENTIALSW theRascredentials;
    //bit flags of using username and password
    theRascredentials.dwMask = RASCM_UserName| RASCM_Password;
    //StringCchCopy(theRascredentials.szUserName, sizeof (userName) + 1, userName);
    lstrcpyW(theRascredentials.szUserName, userName);
    lstrcpyW(theRascredentials.szPassword, password);
    theRascredentials.dwSize = sizeof(theRascredentials);

    //用户密码保存
    dwError = RasSetCredentials(NULL, this->_vpnName, &theRascredentials, TRUE);
    _rascredentials = theRascredentials;
    return ERROR_SUCCESS == dwError;
}

bool RasCreateVpn::connect() {
    DWORD Ret;

    RASDIALPARAMS theRasDialParams = {};
    lstrcpyW(theRasDialParams.szUserName, _rascredentials.szUserName);
    lstrcpyW(theRasDialParams.szPassword, _rascredentials.szPassword);
    lstrcpyW(theRasDialParams.szEntryName, this->_vpnName);
    theRasDialParams.dwSize = sizeof(theRasDialParams);   

    // RasDial: should be Asynchronous Mode
    HRASCONN hRasConn = NULL;  //a handle to the RAS connection
    if ((Ret = RasDial(NULL, NULL, &theRasDialParams, 0, NULL, &hRasConn)) != ERROR_SUCCESS) {
        printf("RasDial failed with error %u\n", Ret);
        return false;
    }

    this->_hRasConn = hRasConn;

    // setup vpn event
    HANDLE vpnEvent = CreateEvent(
                NULL,               // default security attributes
                FALSE,               // auto-reset event
                FALSE,              // initial state is nonsignaled
                TEXT("vpn_status")  // object name
                );

    if (vpnEvent == NULL) {
        printf("CreateEvent failed (%d)\n", GetLastError());
        return false;
    }

    RasConnectionNotification(hRasConn, vpnEvent, RASCN_Connection | RASCN_Disconnection);

    winNotifier = new QWinEventNotifier(vpnEvent);

    return true;
}



bool RasCreateVpn::getStatus() {

    RASCONNSTATUS connectionStatus;
    connectionStatus.dwSize = sizeof(connectionStatus);

    if (RasGetConnectStatus(_hRasConn, &connectionStatus) == ERROR_SUCCESS) {
        qDebug() << "get status succeeded";
        if (connectionStatus.rasconnstate == RASCS_Connected) {
            qDebug() << "is connected";
            return true;
        } else {
            qDebug() << "disconnected";
            return false;
        }
    } else {
        qDebug() << "get status failed";
        return false;
    }

}

bool RasCreateVpn::disConnect() {
    DWORD dwError;
    RASCONNSTATUS connectionStatus;
    connectionStatus.dwSize = sizeof(connectionStatus);

    qDebug() << "disconnecting...";

    dwError = RasHangUp(this->_hRasConn);

    //wait for connection is properly closed
    while ((RasGetConnectStatus(_hRasConn, &connectionStatus) == ERROR_SUCCESS) &&
           (connectionStatus.rasconnstate == RASCS_Connected) ) {
        Sleep(0);
        qDebug() << "we are waiting";
    }

    return ERROR_SUCCESS == dwError;
}


bool RasCreateVpn::removeVpn() {
    DWORD dwError = RasDeleteEntry(NULL, this->_vpnName);
    if (dwError != ERROR_SUCCESS) {
        wprintf(L"RasDeleteEntry failed: Error = %d\n", dwError);

        return false;
    }
    return true;
}

void WINAPI RasCreateVpn::RasDialFunc(UINT unMsg, RASCONNSTATE rasconnstate, DWORD dwError)
{

    char  szRasString[256]; // Buffer for error string
    if (dwError) {
        RasGetErrorStringA((UINT)dwError, szRasString, 256);
        printf("Error: %d - %s\n",dwError, szRasString);
        return;
    }

    // Map each of the RasDial states and display on the
    // screen the next state that RasDial is entering

    switch (rasconnstate) {
    case RASCS_ConnectDevice:
        printf ("Connecting device...\n");
        break;
    case RASCS_Connected:
        qDebug() << "RASCS_Connected.\n";
        break;

    default:
        std::cout << "Unmonitored RAS activity.";
        printf ("Unmonitored RAS activity.\n");
        break;
    }
}


bool fixReg() {
    HKEY hKEY;//定义有关的hKEY,在查询结束时要关闭

    LPCTSTR data_Set_Rasman = _T("System\\CurrentControlSet\\Services\\Rasman\\Parameters");
    LSTATUS result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, data_Set_Rasman, 0, KEY_ALL_ACCESS, &hKEY);
    if (ERROR_SUCCESS == result) {
        // if openReg succeed
        DWORD dwValue=1;
        if (ERROR_SUCCESS != ::RegSetValueEx(hKEY, _T("NegotiateDH2048_AES256"), 0, REG_DWORD, (CONST BYTE*)&dwValue, sizeof(DWORD))) {
            qDebug() << "failed to set NegotiateDH2048_AES256";
            return false;
        }
        //程序结束，关闭打开的hKEY
        ::RegCloseKey(hKEY);
    } else {
        // if openReg failed
        qDebug() << "failed to RegOpen";
        LPTSTR errorText = NULL;

        //parse error message
        FormatMessage(
                    // use system message tables to retrieve error text
                    FORMAT_MESSAGE_FROM_SYSTEM
                    // allocate buffer on local heap for error text
                    |FORMAT_MESSAGE_ALLOCATE_BUFFER
                    // Important! will fail otherwise, since we're not
                    // (and CANNOT) pass insertion parameters
                    |FORMAT_MESSAGE_IGNORE_INSERTS,
                    NULL,    // unused with FORMAT_MESSAGE_FROM_SYSTEM
                    result,
                    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                    (LPTSTR)&errorText,  // output
                    0, // minimum size for output buffer
                    NULL);
        if ( NULL != errorText ) {

            // ... do something with the string `errorText` - log it, display it to the user, etc.


            // release memory allocated by FormatMessage()
            LocalFree(errorText);
            errorText = NULL;
        }


        return false;
    }


    LPCTSTR data_Set_PolicyAgent = _T("System\\CurrentControlSet\\Services\\PolicyAgent");
    if (ERROR_SUCCESS == ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, data_Set_PolicyAgent, 0, KEY_ALL_ACCESS, &hKEY)) {
        DWORD dwValue = 2;
        if (ERROR_SUCCESS != ::RegSetValueEx(hKEY, _T("AssumeUDPEncapsulationContextOnSendRule"), 0, REG_DWORD, (CONST BYTE*)&dwValue, sizeof(DWORD))) {
            qDebug() << "failed to set AssumeUDPEncapsulationContextOnSendRule";
            return false;
        }
        //程序结束，关闭打开的hKEY
        ::RegCloseKey(hKEY);
    } else {
        qDebug() << "failed to RegOpen";
        return false;
    }

    return true;
}
