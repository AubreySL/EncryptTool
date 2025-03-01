#include <stdio.h>
#include <Windows.h>
// AES����
BOOL AesEncrypt(BYTE* pPassword, BYTE* pData, DWORD& dwDataLength, DWORD dwBufferLength)
{
	BOOL bRet = TRUE;
	HCRYPTPROV hCryptProv = NULL;
	HCRYPTHASH hCryptHash = NULL;
	HCRYPTKEY hCryptKey = NULL;
	DWORD dwPasswordLength = strlen((char*)pPassword);
	do
	{
		bRet = CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_AES, CRYPT_VERIFYCONTEXT);
		bRet = CryptCreateHash(hCryptProv, CALG_MD5, NULL, 0, &hCryptHash);
		bRet = CryptHashData(hCryptHash, pPassword, dwPasswordLength, 0);
		bRet = CryptDeriveKey(hCryptProv, CALG_AES_128, hCryptHash, CRYPT_EXPORTABLE, &hCryptKey);
		bRet = CryptEncrypt(hCryptKey, NULL, TRUE, 0, pData, &dwDataLength, dwBufferLength);
	} while (FALSE);

	if (hCryptKey || hCryptHash || hCryptProv)
	{
		CryptDestroyKey(hCryptKey);
		CryptDestroyHash(hCryptHash);
		CryptReleaseContext(hCryptProv, 0);
	}
	return bRet;
}

// AES����
BOOL AesDecrypt(BYTE* pPassword, BYTE* pData, DWORD& dwDataLength, DWORD dwBufferLength)
{
	BOOL bRet = TRUE;
	HCRYPTPROV hCryptProv = NULL;
	HCRYPTHASH hCryptHash = NULL;
	HCRYPTKEY hCryptKey = NULL;
	DWORD dwPasswordLength = strlen((char*)pPassword);

	do
	{
		bRet = CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_AES, CRYPT_VERIFYCONTEXT);
		bRet = CryptCreateHash(hCryptProv, CALG_MD5, NULL, 0, &hCryptHash);
		bRet = CryptHashData(hCryptHash, pPassword, dwPasswordLength, 0);
		bRet = CryptDeriveKey(hCryptProv, CALG_AES_128, hCryptHash, CRYPT_EXPORTABLE, &hCryptKey);
		bRet = CryptDecrypt(hCryptKey, NULL, TRUE, 0, pData, &dwDataLength);
	} while (FALSE);

	if (hCryptKey || hCryptHash || hCryptProv)
	{
		CryptDestroyKey(hCryptKey);
		CryptDestroyHash(hCryptHash);
		CryptReleaseContext(hCryptProv, 0);
	}
	return bRet;
}


//int main(int argc, char* argv[])
//{
//	byte pdata[MAX_PATH] = { 0 };
//	DWORD dwdatalength = 0;
//	char* msg = (char*)"hello lyshark";
//
//	strcpy((char*)pdata, msg);
//	dwdatalength = 1 + ::strlen((char*)pdata);
//
//	// aes ����
//	AesEncrypt((byte*)"123", pdata, dwdatalength, MAX_PATH);
//	printf("aes ���ܳ���: %d ���ܺ�: %s \n", dwdatalength, pdata);
//
//	// aes ����
//	AesDecrypt((byte*)"123", pdata, dwdatalength, MAX_PATH);
//	printf("aes ���ܳ���: %d ���ܺ�: %s \n", dwdatalength, pdata);
//
//	system("pause");
//	return 0;
//}
