#pragma once

#include <stdio.h>
#include <Windows.h>
BOOL AesEncrypt(BYTE* pPassword, BYTE* pData, DWORD& dwDataLength, DWORD dwBufferLength);
BOOL AesDecrypt(BYTE* pPassword, BYTE* pData, DWORD& dwDataLength, DWORD dwBufferLength);