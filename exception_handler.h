#ifndef EXCEPTION_HANDLER_H
#define EXCEPTION_HANDLER_H

#include <windows.h>
#include <dbghelp.h>
#include <shellapi.h>
#include <shlobj.h>
#include <Strsafe.h>
#include <QString>

static LONG WINAPI HS_UnhandledExceptionFilter(struct _EXCEPTION_POINTERS* ExceptionInfo)
{
    char strTime[255] = {0};

    SYSTEMTIME sys;
    GetLocalTime( &sys );
    sprintf(strTime,"D%4d%02d%02d_%02d%02d%02d_DumpFile.dmp",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute,sys.wSecond);
    QString str(strTime);
    //HANDLE lhDumpFile = CreateFile(L"DumpFile132.dmp", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL ,NULL);
    HANDLE lhDumpFile = CreateFile(str.toStdWString().data(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL ,NULL);
    MINIDUMP_EXCEPTION_INFORMATION loExceptionInfo;
    loExceptionInfo.ExceptionPointers = ExceptionInfo;
    loExceptionInfo.ThreadId = GetCurrentThreadId();
    loExceptionInfo.ClientPointers = TRUE;

    MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(),lhDumpFile, MiniDumpNormal, &loExceptionInfo, NULL, NULL);

    CloseHandle(lhDumpFile);

    ::ShellExecuteW(NULL, NULL, L"BugReport.exe", NULL, NULL, SW_NORMAL);

    return EXCEPTION_EXECUTE_HANDLER;
}

#endif // EXCEPTION_HANDLER_H
