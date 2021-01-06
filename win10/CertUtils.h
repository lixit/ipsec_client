#ifndef CERTUTILS_H
#define CERTUTILS_H

#pragma comment(lib, "crypt32.lib")

#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <Wincrypt.h>

#define MY_ENCODING_TYPE  (PKCS_7_ASN_ENCODING | X509_ASN_ENCODING)
#define CERT_SUCCESS 1
#define CERT_ERROR -1
/*
	CertUtils cert;
	cert.installCaCert(L"C:\\Users\\QingWang\\Desktop\\ca-182.cer");
	cert.uninstall(L"Linkz CA");
*/
class CertUtils
{
public:
	CertUtils();
    static DWORD installCaCert(LPCTSTR szFileName);
    static DWORD installCaCert(BYTE* pBinByte, unsigned long binBytes);
    static DWORD uninstall(LPCTSTR delCertName);
};

#endif // CERTUTILS_H
