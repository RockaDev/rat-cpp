#pragma once
#include <string>
#include <Windows.h>

std::wstring getSysOpType()
{
    std::wstring ret;
    NTSTATUS(WINAPI * RtlGetVersion)(LPOSVERSIONINFOEXW);
    OSVERSIONINFOEXW osInfo;

    *(FARPROC*)&RtlGetVersion = GetProcAddress(GetModuleHandleA("ntdll"), "RtlGetVersion");

    if (NULL != RtlGetVersion)
    {
        osInfo.dwOSVersionInfoSize = sizeof(osInfo);
        RtlGetVersion(&osInfo);
        ret = L" Windows " + std::to_wstring(osInfo.dwMajorVersion);
    }
    return ret;
}