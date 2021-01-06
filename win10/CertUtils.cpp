#include "CertUtils.h"

CertUtils::CertUtils() {

};

DWORD CertUtils::installCaCert(LPCTSTR szFileName) {
	HANDLE hFile = INVALID_HANDLE_VALUE;
	BYTE pByte[4096] = { 0 };
	BYTE pBinByte[8192] = { 0 };
	unsigned long bytesRead = 0;
	unsigned long binBytes = 4096;
	hFile = CreateFile(szFileName, FILE_READ_DATA, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
	if (INVALID_HANDLE_VALUE == hFile) {
		return CERT_ERROR;
	}

	ReadFile(hFile, pByte, 4096, &bytesRead, NULL);
	if (hFile)
		CloseHandle(hFile);
	CryptStringToBinaryA((LPCSTR)pByte, bytesRead, CRYPT_STRING_BASE64HEADER, pBinByte, &binBytes, NULL, NULL);
	
	return installCaCert(pBinByte, binBytes);
}

DWORD CertUtils::installCaCert(BYTE* pbCertEncoded, unsigned long cbCertEncoded) {

	HCERTSTORE  hSystemStore;
	PCCERT_CONTEXT  pCertContext;
	
	pCertContext = CertCreateCertificateContext(MY_ENCODING_TYPE, pbCertEncoded, cbCertEncoded);

	if (!pCertContext) {
		return CERT_ERROR;
	}

	hSystemStore = CertOpenStore(
		CERT_STORE_PROV_SYSTEM,
		//CERT_STORE_PROV_SYSTEM, // System store will be a 
								// virtual store
		0,                      // Encoding type not needed 
								// with this PROV
		NULL,                   // Accept the default HCRYPTPROV
		CERT_STORE_OPEN_EXISTING_FLAG | CERT_SYSTEM_STORE_LOCAL_MACHINE,
		// Set the system store location in the
		// registry
		L"Root");
	/*hSystemStore = CertOpenSystemStore(
		0,
		L"CA");*/

	if (!hSystemStore) {
		return CERT_ERROR;
	}

	if (!CertAddCertificateContextToStore(
		hSystemStore,                // Store handle
		pCertContext,                // Pointer to a certificate
		CERT_STORE_ADD_REPLACE_EXISTING,
		NULL))
	{
		return CERT_ERROR;
	}

	if (pCertContext)
		CertFreeCertificateContext(pCertContext);

	if (hSystemStore)
		CertCloseStore(
			hSystemStore,
			CERT_CLOSE_STORE_CHECK_FLAG);

	return CERT_SUCCESS;
}

DWORD CertUtils::uninstall(LPCTSTR delCertName) {
	HCERTSTORE hSystemStore;
	PCCERT_CONTEXT  pCertContext = NULL;
	PCCERT_CONTEXT  pDupCertContext;
	char pszNameString[256];

	hSystemStore = CertOpenStore(
		CERT_STORE_PROV_SYSTEM,
		//CERT_STORE_PROV_SYSTEM, // System store will be a 
								// virtual store
		0,                      // Encoding type not needed 
								// with this PROV
		NULL,                   // Accept the default HCRYPTPROV
		CERT_STORE_OPEN_EXISTING_FLAG | CERT_SYSTEM_STORE_LOCAL_MACHINE,
		// Set the system store location in the
		// registry
		L"Root");

	if (!hSystemStore) {
		return CERT_ERROR;
	}

	while (pCertContext = CertEnumCertificatesInStore(
		hSystemStore,
		pCertContext)) // on the first call to the function,
					   //  this parameter is NULL
					   // on all subsequent
					   //  calls, it is the last pointer returned by 
					   //  the function
	{
		if (!CertGetNameString(
			pCertContext,
			CERT_NAME_SIMPLE_DISPLAY_TYPE,
			0,
			NULL,
			(LPWSTR)pszNameString,
			128))
		{
			return CERT_ERROR;
		}
		
		if (_wcsicmp(((LPCTSTR)pszNameString), delCertName) == 0) {
			pDupCertContext = CertDuplicateCertificateContext(pCertContext);

			if (!CertDeleteCertificateFromStore(pDupCertContext)) {
				return CERT_ERROR;
			}

			CertFreeCertificateContext(pDupCertContext);
			goto END;
		}

	}
END:
	CertCloseStore(hSystemStore,0);

	return CERT_SUCCESS;
}
