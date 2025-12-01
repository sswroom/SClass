#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/ByteTool_C.h"
#include "IO/FileUtil.h"
#include "IO/Library.h"
#include "IO/Path.h"
#include "IO/FileStream.h"
#include "IO/StreamReader.h"
#include "IO/WindowsError.h"
#include "Manage/Process.h"
#include "Sync/Interlocked.h"
#include "Sync/SimpleThread.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include <windows.h>
#define PSAPI_VERSION 1
#include <psapi.h>
#include <tlhelp32.h>
#include <shellapi.h>
#include <winternl.h>
#undef DeleteFile
#undef GetCommandLine

#ifndef _WIN32_WCE
#define CloseToolhelp32Snapshot(hand) CloseHandle(hand)
#else
extern "C" DWORD SetProcPermissions(DWORD newperms);
#endif

#ifndef IMAGE_FILE_MACHINE_ARM64
#define IMAGE_FILE_MACHINE_ARM64 0xAA64
#endif

#if !defined(__MINGW32__) && !defined(__CYGWIN__)
typedef struct _SYSTEM_HANDLE_ENTRY {
	ULONG OwnerPid;
	BYTE ObjectType;
	BYTE HandleFlags;
	USHORT HandleValue;
	PVOID ObjectPointer;
	ULONG AccessMask;
} SYSTEM_HANDLE_ENTRY, *PSYSTEM_HANDLE_ENTRY;

typedef struct _SYSTEM_HANDLE_INFORMATION
{
	ULONG Count;
	SYSTEM_HANDLE_ENTRY Handle[1];
} SYSTEM_HANDLE_INFORMATION, *PSYSTEM_HANDLE_INFORMATION;

#define SystemHandleInformation (SYSTEM_INFORMATION_CLASS)16
#define ObjectNameInformation (OBJECT_INFORMATION_CLASS)1
#endif

typedef struct __MyPUBLIC_OBJECT_TYPE_INFORMATION {

    UNICODE_STRING TypeName;

    ULONG Reserved [22];    // reserved for internal use

} MyPUBLIC_OBJECT_TYPE_INFORMATION, *PMyPUBLIC_OBJECT_TYPE_INFORMATION;

#define STATUS_INFO_LENGTH_MISMATCH (NTSTATUS)0xc0000004

typedef NTSTATUS (NTAPI* NtQuerySystemInformationFunc)(
	SYSTEM_INFORMATION_CLASS SystemInformationClass,
	PVOID                    SystemInformation,
	ULONG                    SystemInformationLength,
	PULONG                   ReturnLength
);

typedef NTSTATUS(NTAPI* NtDuplicateObjectFunc)(
	HANDLE SourceProcessHandle,
	HANDLE SourceHandle,
	HANDLE TargetProcessHandle,
	PHANDLE TargetHandle,
	ACCESS_MASK DesiredAccess,
	ULONG Attributes,
	ULONG Options
);

typedef NTSTATUS(NTAPI* NtQueryObjectFunc)(
	HANDLE ObjectHandle,
	OBJECT_INFORMATION_CLASS ObjectInformationClass,
	PVOID ObjectInformation,
	ULONG ObjectInformationLength,
	PULONG ReturnLength
);

enum class ProcessNameType
{
	Default,
	NoName,
	NameDone
};

typedef BOOL(__stdcall* IsWow64Process2Func)(HANDLE hProcess, USHORT* pProcessMachine, USHORT* pNativeMachine);
static Int32 Process_ExecFileId = 0;
static Bool Process_Inited = false;
static IsWow64Process2Func Process_IsWow64Process2;

Manage::Process::Process(UOSInt procId, Bool controlRight)
{
	this->procId = procId;
	DWORD access = PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_DUP_HANDLE;
	if (controlRight)
	{
		access |= PROCESS_TERMINATE;
	}
	this->handle = OpenProcess(access, FALSE, (DWORD)procId);
	if (this->handle == 0)
	{
		access = PROCESS_QUERY_INFORMATION;
		if (controlRight)
		{
			access |= PROCESS_TERMINATE;
		}
		this->handle = OpenProcess(access, FALSE, (DWORD)procId);
	}
	this->needRelease = true;
}

Manage::Process::Process()
{
	this->procId = GetCurrentProcessId();
	this->handle = GetCurrentProcess();
	this->needRelease = false;
}

Manage::Process::Process(UnsafeArray<const UTF8Char> cmdLine)
{
	WChar buff[MAX_PATH];
	WChar progName[MAX_PATH];
	UnsafeArray<const WChar> wptr = Text::StrToWCharNew(cmdLine);
	UnsafeArray<const WChar> cptr = wptr;
	WChar *pptr = progName;
	Bool isQuote = false;
	WChar c;
	while ((c = *cptr++) != 0)
	{
		if (c == '"')
			isQuote = !isQuote;
		else if (!isQuote && c == ' ')
		{
			break;
		}
		else
		{
			*pptr++ = c;
		}
	}
	*pptr = 0;

	IO::Path::GetFileDirectoryW(buff, progName);
	PROCESS_INFORMATION procInfo;
	STARTUPINFOW startInfo;
	ZeroMemory(&startInfo, sizeof(startInfo));
	startInfo.cb = sizeof(startInfo);
#ifdef _WIN32_WCE
	CreateProcessW(0, (LPWSTR)wptr.Ptr(), 0, 0, false, 0, 0, buff, &startInfo, &procInfo);
#else
	CreateProcessW(0, (LPWSTR)wptr.Ptr(), 0, 0, false, NORMAL_PRIORITY_CLASS, 0, buff, &startInfo, &procInfo);
#endif
	Text::StrDelNew(wptr);
	this->handle = procInfo.hProcess;
	this->procId = procInfo.dwProcessId;
	this->needRelease = true;
	CloseHandle(procInfo.hThread);
}

Manage::Process::Process(const WChar *cmdLine)
{
	WChar buff[MAX_PATH];
	WChar progName[MAX_PATH];
	const WChar *cptr = cmdLine;
	WChar *pptr = progName;
	Bool isQuote = false;
	WChar c;
	while ((c = *cptr++) != 0)
	{
		if (c == '"')
			isQuote = !isQuote;
		else if (!isQuote && c == ' ')
		{
			break;
		}
		else
		{
			*pptr++ = c;
		}
	}
	*pptr = 0;

	IO::Path::GetFileDirectoryW(buff, progName);
	PROCESS_INFORMATION procInfo;
	STARTUPINFOW startInfo;
	ZeroMemory(&startInfo, sizeof(startInfo));
	startInfo.cb = sizeof(startInfo);
#ifdef _WIN32_WCE
	CreateProcessW(0, (LPWSTR)cmdLine, 0, 0, false, 0, 0, buff, &startInfo, &procInfo);
#else
	CreateProcessW(0, (LPWSTR)cmdLine, 0, 0, false, NORMAL_PRIORITY_CLASS, 0, buff, &startInfo, &procInfo);
#endif
	this->handle = procInfo.hProcess;
	this->procId = procInfo.dwProcessId;
	this->needRelease = true;
	CloseHandle(procInfo.hThread);
}

Manage::Process::~Process()
{
	if (needRelease && this->handle)
	{
		CloseHandle((HANDLE)this->handle);
		this->handle = 0;
	}
}

UOSInt Manage::Process::GetCurrProcId()
{
	return GetCurrentProcessId();
}

UOSInt Manage::Process::GetProcId()
{
	return this->procId;
}

Bool Manage::Process::IsRunning() const
{
	if (this->handle)
	{
		UInt32 code;
		if (GetExitCodeProcess(this->handle, (DWORD*)&code))
		{
			return code == STILL_ACTIVE;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

Bool Manage::Process::Kill()
{
//	printf("Process: Kill Process\r\n");
	if (this->handle)
	{
//		printf("Process: Handle Found\r\n");
		if (TerminateProcess(this->handle, 0))
			return true;
//		printf("Process: Kill Error, err = %d\r\n", GetLastError());
	}
	return false;
}

UnsafeArray<WChar> Manage::Process::GetFilename(UnsafeArray<WChar> buff)
{
	if (this->handle)
	{
		UInt32 retSize;
#ifdef _WIN32_WCE
		retSize = GetModuleFileNameW((HMODULE)this->handle, buff, 1024);
#else
		//retSize = GetProcessImageFileNameW(this->handle, buff, 1024);
		retSize = GetModuleFileNameExW(this->handle, 0, buff.Ptr(), 1024);
#endif
		buff[retSize] = 0;
		return &buff[retSize];
	}
	else
	{
		buff[0] = 0;
		return buff;
	}
}

Bool Manage::Process::GetFilename(NN<Text::StringBuilderUTF8> sb)
{
	if (this->handle)
	{
		UInt32 retSize;
		WChar *buff = MemAlloc(WChar, 1024);
#ifdef _WIN32_WCE
		retSize = GetModuleFileNameW((HMODULE)this->handle, buff, 1024);
#else
		//retSize = GetProcessImageFileNameW(this->handle, buff, 1024);
		retSize = GetModuleFileNameExW(this->handle, 0, buff, 1024);
#endif
		buff[retSize] = 0;
		sb->AppendW(buff);
		MemFree(buff);
		return true;
	}
	else
	{
		return false;
	}
}

typedef NTSTATUS (NTAPI* NtQueryInformationProcessFunc)(
	IN HANDLE ProcessHandle,
	IN PROCESSINFOCLASS ProcessInformationClass,
	OUT PVOID ProcessInformation,
	IN ULONG ProcessInformationLength,
	OUT PULONG ReturnLength OPTIONAL
);

Bool Manage::Process::GetCommandLine(NN<Text::StringBuilderUTF8> sb)
{
	if (this->handle == 0)
		return false;

	IO::Library lib((const UTF8Char*)"Ntdll.dll");
	NtQueryInformationProcessFunc qip = (NtQueryInformationProcessFunc)lib.GetFunc("NtQueryInformationProcess");
	if (qip == 0)
		return false;
	PROCESS_BASIC_INFORMATION pinfo;
	LONG status = qip((HANDLE)this->handle, ProcessBasicInformation, &pinfo, sizeof(pinfo), NULL);
	if (status != 0)
	{
		return false;
	}
	PPEB ppeb = pinfo.PebBaseAddress;
	PEB pebCopy;
	if (!ReadProcessMemory((HANDLE)this->handle, ppeb, &pebCopy, sizeof(PEB), NULL))
	{
		return false;
	}

	RTL_USER_PROCESS_PARAMETERS rtlProcParamCopy;
	if (!ReadProcessMemory((HANDLE)this->handle, pebCopy.ProcessParameters, &rtlProcParamCopy, sizeof(RTL_USER_PROCESS_PARAMETERS), NULL))
	{
		return false;
	}
	PWSTR wBuffer = rtlProcParamCopy.CommandLine.Buffer;
	USHORT len = rtlProcParamCopy.CommandLine.Length;
	WChar *cmdLine = MemAlloc(WChar, (len >> 1) + 1);
	if (!ReadProcessMemory((HANDLE)this->handle, wBuffer, cmdLine, len, NULL))
	{
		MemFree(cmdLine);
		return false;
	}
	cmdLine[len >> 1] = 0;
	sb->AppendW(cmdLine);
	MemFree(cmdLine);
	return true;
}

Bool Manage::Process::GetWorkingDir(NN<Text::StringBuilderUTF8> sb)
{
	if (this->handle)
	{
		UOSInt retSize;
		WChar* buff = MemAlloc(WChar, 1024);
#ifdef _WIN32_WCE
		retSize = GetModuleFileNameW((HMODULE)this->handle, buff, 1024);
#else
		//retSize = GetProcessImageFileNameW(this->handle, buff, 1024);
		retSize = GetModuleFileNameExW(this->handle, 0, buff, 1024);
#endif
		buff[retSize] = 0;
		retSize = Text::StrLastIndexOfCharW(buff, '\\');
		if (retSize != INVALID_INDEX)
		{
			buff[retSize] = 0;
			sb->AppendW(buff);
		}
		else
		{
			sb->AppendW(buff);
		}
		MemFree(buff);
		return true;
	}
	else
	{
		return false;
	}

}

Bool Manage::Process::GetTrueProgramPath(NN<Text::StringBuilderUTF8> sb)
{
	return this->GetFilename(sb);
}

UOSInt Manage::Process::GetMemorySize()
{
#ifdef _WIN32_WCE
	return 0;
#else
	PROCESS_MEMORY_COUNTERS cnt;
	cnt.cb = sizeof(cnt);
	if (GetProcessMemoryInfo((HANDLE)this->handle, &cnt, sizeof(cnt)))
	{
		return cnt.WorkingSetSize;
	}
	else
	{
		return 0;
	}
#endif
}

Bool Manage::Process::SetMemorySize(UOSInt minSize, UOSInt maxSize)
{
#ifdef _WIN32_WCE
	return false;
#else
	BOOL ret = SetProcessWorkingSetSize((HANDLE)this->handle, minSize, maxSize);
//	Int32 lastErr;
//	if (ret == 0)
//	{
//		lastErr = GetLastError();
//	}
	return ret != 0;
#endif
}

UOSInt Manage::Process::GetThreadIds(NN<Data::ArrayList<UInt32>> threadList)
{
	THREADENTRY32 threadEntry;
	UOSInt threadCnt = 0;
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	threadEntry.dwSize = sizeof(threadEntry);
	if (Thread32First(hSnapShot, &threadEntry))
	{
		while (true)
		{
			if (threadEntry.th32OwnerProcessID == (UInt32)this->procId)
			{
				threadList->Add(threadEntry.th32ThreadID);
				threadCnt++;
			}

			if (Thread32Next(hSnapShot, &threadEntry) != TRUE)
			{
				break;
			}
		}
	}
	CloseToolhelp32Snapshot(hSnapShot);
	return threadCnt;
}

void *Manage::Process::GetHandle()
{
	return this->handle;
}

UOSInt Manage::Process::GetModules(NN<Data::ArrayListNN<Manage::ModuleInfo>> modList)
{
#ifdef _WIN32_WCE
	MODULEENTRY32 moduleInfo;
	Manage::ModuleInfo *mod;
	HANDLE hSnapShot;
	UOSInt i;
	hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, 0);
	moduleInfo.dwSize = sizeof(moduleInfo);
	i = 0;
	if (Module32First(hSnapShot, &moduleInfo))
	{
		while (true)
		{
			if (moduleInfo.th32ProcessID == this->procId)
			{
				NEW_CLASS(mod, Manage::ModuleInfo(this->handle, moduleInfo.hModule));
				modList->Add(mod);
				i++;
			}

			if (Module32Next(hSnapShot, &moduleInfo) != TRUE)
			{
				break;
			}
		}
	}
	CloseToolhelp32Snapshot(hSnapShot);
	return i;
#else
	UOSInt i;
	NN<Manage::ModuleInfo> mod;
	HMODULE mods[512];
	UInt32 modCnt;
	if (EnumProcessModules((HANDLE)this->handle, mods, sizeof(mods), (LPDWORD)&modCnt))
	{
		i = 0;
		while (i < (modCnt / sizeof(HMODULE)))
		{
			NEW_CLASSNN(mod, Manage::ModuleInfo(this->handle, mods[i]));
			modList->Add(mod);

			i++;
		}

	}
	return modCnt;
#endif
}

UOSInt Manage::Process::GetThreads(NN<Data::ArrayListNN<Manage::ThreadInfo>> threadList)
{
	NN<Manage::ThreadInfo> tInfo;
	UOSInt threadCnt = 0;
	THREADENTRY32 threadEntry;
	HANDLE hSnapShot;
	hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (hSnapShot == INVALID_HANDLE_VALUE)
	{
//		Int32 ret = GetLastError();
	}
	else
	{
		threadEntry.dwSize = sizeof(threadEntry);
		if (Thread32First(hSnapShot, &threadEntry))
		{
			while (true)
			{
				if (threadEntry.th32OwnerProcessID == this->procId)
				{
					NEW_CLASSNN(tInfo, Manage::ThreadInfo(this->procId, threadEntry.th32ThreadID));
					threadList->Add(tInfo);
					threadCnt++;
				}

				if (Thread32Next(hSnapShot, &threadEntry) != TRUE)
				{
					break;
				}
			}
		}
		CloseToolhelp32Snapshot(hSnapShot);
	}
	return threadCnt;
}

UOSInt Manage::Process::GetHeapLists(NN<Data::ArrayList<UInt32>> heapList)
{
	HEAPLIST32 heapListInfo;
	HANDLE hSnapShot;
	UOSInt i;
	hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPHEAPLIST, (DWORD)this->procId);
	heapListInfo.dwSize = sizeof(heapListInfo);
	i = 0;
	if (Heap32ListFirst(hSnapShot, &heapListInfo))
	{
		while (true)
		{
			if (heapListInfo.th32ProcessID == this->procId)
			{
				heapList->Add((UInt32)heapListInfo.th32HeapID);
				i++;
			}

			if (Heap32ListNext(hSnapShot, &heapListInfo) != TRUE)
			{
				break;
			}
		}
	}
	CloseToolhelp32Snapshot(hSnapShot);
	return i;
}

UOSInt Manage::Process::GetHeaps(NN<Data::ArrayListNN<HeapInfo>> heapList, UInt32 heapListId, UOSInt maxCount)
{
	HEAPENTRY32 ent;
	NN<HeapInfo> heap;
	UOSInt cnt = 0;
	if (maxCount <= 0)
	{
		maxCount = (UOSInt)-1;
		maxCount = (maxCount >> 1);
	}
	ent.dwSize = sizeof(ent);
#ifdef _WIN32_WCE
	HANDLE hand = CreateToolhelp32Snapshot(TH32CS_SNAPHEAPLIST, this->procId);
	if (hand != INVALID_HANDLE_VALUE)
	{
		if (Heap32First(hand, &ent, this->procId, heapListId))
		{
			while (true)
			{
				heap = MemAllocNN(HeapInfo);
				heap->startAddr = ent.dwAddress;
				heap->size = ent.dwBlockSize;
				switch (ent.dwFlags)
				{
				case LF32_FIXED:
					heap->heapType = Manage::Process::HT_FIXED;
					break;
				case LF32_FREE:
					heap->heapType = Manage::Process::HT_FREE;
					break;
				case LF32_MOVEABLE:
					heap->heapType = Manage::Process::HT_MOVABLE;
					break;
				default:
					heap->heapType = Manage::Process::HT_UNKNOWN;
					break;
				}
				heapList->Add(heap);
				cnt++;
				if (cnt >= maxCount || !Heap32Next(hand, &ent))
					break;
			}
		}
		CloseToolhelp32Snapshot(hand);
	}
#else
	if (Heap32First(&ent, (DWORD)this->procId, heapListId))
	{
		while (true)
		{
			heap = MemAllocNN(HeapInfo);
			heap->startAddr = ent.dwAddress;
			heap->size = ent.dwBlockSize;
			switch (ent.dwFlags)
			{
			case LF32_FIXED:
				heap->heapType = Manage::Process::HT_FIXED;
				break;
			case LF32_FREE:
				heap->heapType = Manage::Process::HT_FREE;
				break;
			case LF32_MOVEABLE:
				heap->heapType = Manage::Process::HT_MOVABLE;
				break;
			default:
				heap->heapType = Manage::Process::HT_UNKNOWN;
				break;
			}
			heapList->Add(heap);
			cnt++;
			if (cnt >= maxCount || !Heap32Next(&ent))
				break;
		}
	}
#endif
	return cnt;

}

void Manage::Process::FreeHeaps(NN<Data::ArrayListNN<HeapInfo>> heapList)
{
	heapList->DeleteAll();
}

Data::Timestamp Manage::Process::GetStartTime()
{
	FILETIME ftime;
	FILETIME tmp;
	if (GetProcessTimes(this->handle, &ftime, &tmp, &tmp, &tmp))
	{
		return Data::Timestamp::FromFILETIME(&ftime, Data::DateTimeUtil::GetLocalTzQhr());
	}
	else
	{
		return Data::Timestamp(0);
	}
}

UOSInt Manage::Process::GetHandles(NN<Data::ArrayList<HandleInfo>> handleList)
{
	IO::Library lib((const UTF8Char*)"Ntdll.dll");
	NtQuerySystemInformationFunc qsi = (NtQuerySystemInformationFunc)lib.GetFunc("NtQuerySystemInformation");
	if (qsi == 0)
	{
		return 0;
	}
	ULONG handleInfoSize = 0x10000;
	NTSTATUS status;
	SYSTEM_HANDLE_INFORMATION* handleInfo;
	handleInfo = (SYSTEM_HANDLE_INFORMATION*)MemAlloc(UInt8, handleInfoSize);
	while ((status = qsi(SystemHandleInformation, handleInfo, handleInfoSize, 0)) == STATUS_INFO_LENGTH_MISMATCH)
	{
		MemFree(handleInfo);
		handleInfoSize <<= 1;
		handleInfo = (SYSTEM_HANDLE_INFORMATION*)MemAlloc(UInt8, handleInfoSize);
	}
	if (status < 0)
	{
		MemFree(handleInfo);
		return 0;
	}
	UOSInt ret = 0;
	UOSInt i = 0;
	while (i < handleInfo->Count)
	{
		if (handleInfo->Handle[i].OwnerPid == this->procId)
		{
			handleList->Add(HandleInfo(handleInfo->Handle[i].HandleValue, 0));
			ret++;
		}

		i++;
	}
	MemFree(handleInfo);
	return ret;
}

Bool Manage::Process::GetHandleDetail(Int32 id, OutParam<HandleType> handleType, NN<Text::StringBuilderUTF8> sbDetail)
{
	HANDLE dupHandle;
	HANDLE dupHandle2;
	NTSTATUS status;
	IO::Library lib((const UTF8Char*)"Ntdll.dll");
	NtDuplicateObjectFunc dhand = (NtDuplicateObjectFunc)lib.GetFunc("NtDuplicateObject");
	NtQueryObjectFunc qryObj = (NtQueryObjectFunc)lib.GetFunc("NtQueryObject");
	if (dhand == 0 || qryObj == 0)
	{
		return false;
	}
	Bool needClose = true;
	if (this->procId == GetCurrentProcessId())
	{
		dupHandle = (HANDLE)(OSInt)id;
		needClose = false;
	}
	else
	{
		if ((status = dhand((HANDLE)this->handle, (HANDLE)(OSInt)id, GetCurrentProcess(), &dupHandle, 0, 0, 0)) < 0)
		{
			handleType.Set(HandleType::Unknown);
			sbDetail->AppendC(UTF8STRC("Error in duplicate handle: 0x"));
			sbDetail->AppendHex32((UInt32)status);
			sbDetail->AppendC(UTF8STRC(" ("));
			sbDetail->Append(IO::WindowsError::GetString((UInt32)status));
			sbDetail->AppendUTF8Char(')');
			return true;
		}
	}
	UInt8 buff[4096];
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	PMyPUBLIC_OBJECT_TYPE_INFORMATION objectTypeInfo = (PMyPUBLIC_OBJECT_TYPE_INFORMATION)buff;
	if ((status = qryObj(dupHandle, ObjectTypeInformation, objectTypeInfo, sizeof(buff), 0)) < 0)
	{
		if (needClose) CloseHandle(dupHandle);
		handleType.Set(HandleType::Unknown);
		sbDetail->AppendC(UTF8STRC("Error in getting handle type: 0x"));
		sbDetail->AppendHex32((UInt32)status);
		sbDetail->AppendC(UTF8STRC(" ("));
		sbDetail->Append(IO::WindowsError::GetString((UInt32)status));
		sbDetail->AppendUTF8Char(')');
		return true;
	}
	sptr = Text::StrUTF16_UTF8C(sbuff, objectTypeInfo->TypeName.Buffer, objectTypeInfo->TypeName.Length >> 1);
	*sptr = 0;
	ProcessNameType nameType = ProcessNameType::Default;
	if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("Event")))
	{
		handleType.Set(HandleType::Event);
		nameType = ProcessNameType::NoName;
	}
	else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("Key")))
	{
		handleType.Set(HandleType::Key);
	}
	else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("WaitCompletionPacket")))
	{
		handleType.Set(HandleType::WaitCompletionPacket);
		nameType = ProcessNameType::NoName;
	}
	else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("IoCompletion")))
	{
		handleType.Set(HandleType::IoCompletion);
		nameType = ProcessNameType::NoName;
	}
	else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("Mutant")))
	{
		handleType.Set(HandleType::Mutant);
		nameType = ProcessNameType::NoName;
	}
	else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("TpWorkerFactory")))
	{
		handleType.Set(HandleType::TpWorkerFactory);
		nameType = ProcessNameType::NoName;
	}
	else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("Section")))
	{
		handleType.Set(HandleType::Section);
		nameType = ProcessNameType::NoName;
 	}
	else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("IRTimer")))
	{
		handleType.Set(HandleType::IRTimer);
		nameType = ProcessNameType::NoName;
	}
	else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("Directory")))
	{
		handleType.Set(HandleType::Directory);
	}
	else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("File")))
	{
		handleType.Set(HandleType::File);
	}
	else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("ALPC Port")))
	{
		handleType.Set(HandleType::ALPC_Port);
		nameType = ProcessNameType::NoName;
	}
	else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("Semaphore")))
	{
		handleType.Set(HandleType::Semaphore);
		nameType = ProcessNameType::NoName;
	}
	else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("Thread")))
	{
		handleType.Set(HandleType::Thread);
		if (dhand((HANDLE)this->handle, (HANDLE)(OSInt)id, GetCurrentProcess(), &dupHandle2, THREAD_QUERY_LIMITED_INFORMATION, 0, 0) >= 0)
		{
			sbDetail->AppendU32(GetThreadId((HANDLE)dupHandle2));
			CloseHandle(dupHandle2);
		}
		nameType = ProcessNameType::NameDone;
	}
	else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("IoCompletionReserve")))
	{
		handleType.Set(HandleType::IoCompletionReserve);
		nameType = ProcessNameType::NoName;
	}
	else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("WindowStation")))
	{
		handleType.Set(HandleType::WindowStation);
	}
	else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("Desktop")))
	{
		handleType.Set(HandleType::Desktop);
	}
	else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("Timer")))
	{
		handleType.Set(HandleType::Timer);
	}
	else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("Token")))
	{
		handleType.Set(HandleType::Token);
	}
	else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("Process")))
	{
		handleType.Set(HandleType::Process);
		if (dhand((HANDLE)this->handle, (HANDLE)(OSInt)id, GetCurrentProcess(), &dupHandle2, PROCESS_QUERY_LIMITED_INFORMATION, 0, 0) >= 0)
		{
			sbDetail->AppendU32(GetProcessId((HANDLE)dupHandle2));
			CloseHandle(dupHandle2);
		}
		nameType = ProcessNameType::NameDone;
	}
	else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("DxgkCompositionObject")))
	{
		handleType.Set(HandleType::DxgkCompositionObject);
	}
	else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("EtwRegistration")))
	{
		handleType.Set(HandleType::EtwRegistration);
		nameType = ProcessNameType::NoName;
	}
	else
	{
		handleType.Set(HandleType::Unknown);
		sbDetail->AppendC(UTF8STRC("Unknown Type name: "));
		sbDetail->AppendP(sbuff, sptr);
		nameType = ProcessNameType::NameDone;
	}
	if (nameType == ProcessNameType::Default)
	{
		UNICODE_STRING* objectTypeInfo = (UNICODE_STRING*)buff;
		ULONG returnLength;
		if ((status = qryObj(dupHandle, ObjectNameInformation, objectTypeInfo, sizeof(buff) - 2, &returnLength)) >= 0)
		{
			sbDetail->AppendW(objectTypeInfo->Buffer, objectTypeInfo->Length >> 1);
		}
		else if (status == STATUS_INFO_LENGTH_MISMATCH)
		{
			UInt8* tmpBuff = MemAlloc(UInt8, returnLength);
			objectTypeInfo = (UNICODE_STRING*)tmpBuff;
			if ((status = qryObj(dupHandle, ObjectNameInformation, objectTypeInfo, returnLength, 0)) >= 0)
			{
				sbDetail->AppendW(objectTypeInfo->Buffer, objectTypeInfo->Length >> 1);
			}
			else
			{
				sbDetail->AppendC(UTF8STRC("Error in getting handle name2: 0x"));
				sbDetail->AppendHex32((UInt32)status);
			}
			MemFree(tmpBuff);
		}
		else
		{
			sbDetail->AppendC(UTF8STRC("Error in getting handle name: 0x"));
			sbDetail->AppendHex32((UInt32)status);
			sbDetail->AppendC(UTF8STRC(" ("));
			sbDetail->Append(IO::WindowsError::GetString((UInt32)status));
			sbDetail->AppendUTF8Char(')');
		}
	}
	else if (nameType == ProcessNameType::NoName)
	{
		sbDetail->AppendUTF8Char('-');
	}
	if (needClose) CloseHandle(dupHandle);
	return true;
}

Bool Manage::Process::GetWorkingSetSize(UOSInt *minSize, UOSInt *maxSize)
{
#ifdef _WIN32_WCE
	return false;
#else
	return GetProcessWorkingSetSize(this->handle, (PSIZE_T)minSize, (PSIZE_T)maxSize) != 0;
#endif

}

Bool Manage::Process::GetMemoryInfo(OptOut<UOSInt> pageFault, OptOut<UOSInt> workingSetSize, OptOut<UOSInt> pagedPoolUsage, OptOut<UOSInt> nonPagedPoolUsage, OptOut<UOSInt> pageFileUsage)
{
#ifdef _WIN32_WCE
	return false;
#else
	PROCESS_MEMORY_COUNTERS memInfo;
	BOOL ret;
	memInfo.cb = sizeof(memInfo);
	ret = GetProcessMemoryInfo(this->handle, &memInfo, sizeof(memInfo));
	if (ret)
	{
		pageFault.Set(memInfo.PageFaultCount);
		workingSetSize.Set(memInfo.WorkingSetSize);
		pagedPoolUsage.Set(memInfo.QuotaPagedPoolUsage);
		nonPagedPoolUsage.Set(memInfo.QuotaNonPagedPoolUsage);
		pageFileUsage.Set(memInfo.PagefileUsage);
		return true;
	}
//	UInt32 err = GetLastError();
	return false;
#endif
}

Bool Manage::Process::GetTimeInfo(OptOut<Data::Timestamp> createTime, OptOut<Data::Timestamp> kernelTime, OptOut<Data::Timestamp> userTime)
{
#ifdef _WIN32_WCE
	FILETIME ttime1 = {0, 0};
	FILETIME ttime2 = {0, 0};
	FILETIME ttime3 = {0, 0};
	FILETIME ttime4 = {0, 0};
	FILETIME time1;
	FILETIME time2;
	FILETIME time3;
	FILETIME time4;
	SYSTEMTIME sysTime;
	Bool found = false;
	THREADENTRY32 threadEntry;
	HANDLE hSnapShot;
	hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (hSnapShot != INVALID_HANDLE_VALUE)
	{
		DWORD oriPerm;
		threadEntry.dwSize = sizeof(threadEntry);
		oriPerm = SetProcPermissions(0xffffffff);
		if (Thread32First(hSnapShot, &threadEntry))
		{
			while (true)
			{
				if (threadEntry.th32OwnerProcessID == this->procId)
				{
					if (GetThreadTimes((HANDLE)threadEntry.th32ThreadID, &time1, &time2, &time3, &time4))
					{
						ttime1 = time1;
						ttime2 = time2;
						ttime3.dwLowDateTime += time3.dwLowDateTime;
						ttime3.dwHighDateTime += time3.dwHighDateTime;
						if (ttime3.dwLowDateTime < time3.dwLowDateTime)
						{
							ttime3.dwHighDateTime += 1;
						}
						ttime4.dwLowDateTime += time4.dwLowDateTime;
						ttime4.dwHighDateTime += time4.dwHighDateTime;
						if (ttime4.dwLowDateTime < time4.dwLowDateTime)
						{
							ttime4.dwHighDateTime += 1;
						}
						found = true;
					}
				}

				if (Thread32Next(hSnapShot, &threadEntry) != TRUE)
				{
					break;
				}
			}
		}
		SetProcPermissions(oriPerm);
		CloseToolhelp32Snapshot(hSnapShot);
	}
	if (found)
	{
		if (createTime.IsNotNull())
		{
			createTime.SetNoCheck(Data::Timestamp::FromFILETIME(&time1, Data::DateTimeUtil::GetLocalTzQhr()));
		}
		if (kernelTime.IsNotNull())
		{
			kernelTime.SetNoCheck(Data::Timestamp::FromFILETIME(&time3, Data::DateTimeUtil::GetLocalTzQhr()));
		}
		if (userTime.IsNotNull())
		{
			userTime.SetNoCheck(Data::Timestamp::FromFILETIME(&time4, Data::DateTimeUtil::GetLocalTzQhr()));
		}
	}
	return found;
#else
	FILETIME time1;
	FILETIME time2;
	FILETIME time3;
	FILETIME time4;
	BOOL ret;
	ret = GetProcessTimes(this->handle, &time1, &time2, &time3, &time4);
	if (ret)
	{
		if (createTime.IsNotNull())
		{
			createTime.SetNoCheck(Data::Timestamp::FromFILETIME(&time1, Data::DateTimeUtil::GetLocalTzQhr()));
		}
		if (kernelTime.IsNotNull())
		{
			kernelTime.SetNoCheck(Data::Timestamp::FromFILETIME(&time3, Data::DateTimeUtil::GetLocalTzQhr()));
		}
		if (userTime.IsNotNull())
		{
			userTime.SetNoCheck(Data::Timestamp::FromFILETIME(&time4, Data::DateTimeUtil::GetLocalTzQhr()));
		}
		return true;
	}
//	UInt32 err = GetLastError();
	return false;
#endif
}


UInt32 Manage::Process::GetGDIObjCount()
{
#ifdef _WIN32_WCE
	return 0;
#else
	return GetGuiResources(this->handle, GR_GDIOBJECTS);
#endif
}

UInt32 Manage::Process::GetUserObjCount()
{
#ifdef _WIN32_WCE
	return 0;
#else
	return GetGuiResources(this->handle, GR_USEROBJECTS);
#endif
}

UInt32 Manage::Process::GetHandleCount()
{
#ifdef _WIN32_WCE
	return 0;
#else
	if (this->handle == 0)
		return 0;
	DWORD handleCount;
	if (GetProcessHandleCount(this->handle, &handleCount))
		return handleCount;
	return 0;
#endif
}

Manage::Process::ProcessPriority Manage::Process::GetPriority()
{
#ifdef _WIN32_WCE
	return PP_UNKNOWN;
#else
	if (this->handle == 0)
		return PP_UNKNOWN;
	return (ProcessPriority)GetPriorityClass(this->handle);
#endif
}

Manage::ThreadContext::ContextType Manage::Process::GetContextType()
{
#if defined(_WIN32_WCE)
#  if defined(ARM)
	return Manage::ThreadContext::ContextType::ARM;
#  elif defined(_ALPHA_)
	return Manage::ThreadContext::ContextType::Unknown;
#  elif defined(_X86_)
	return Manage::ThreadContext::ContextType::X86_32;
#  elif defined(_MIPS_)
	return Manage::ThreadContext::ContextType::MIPS;
#  elif defined(_PPC_)
	return Manage::ThreadContext::ContextType::Unknown;
#  elif defined(_MPPC_)
	return Manage::ThreadContext::ContextType::Unknown;
#  elif defined(_IA64_)
	return Manage::ThreadContext::ContextType::Unknown;
#  elif defined(SHx)
	return Manage::ThreadContext::ContextType::Unknown;
#  endif

#else
	if (this->handle == 0)
		return Manage::ThreadContext::ContextType::Unknown;
	USHORT pProcessMachine;
	USHORT pNativeMachine;
	if (!Process_Inited)
	{
		IO::Library lib((const UTF8Char*)"Kernel32.dll");
		Process_IsWow64Process2 = (IsWow64Process2Func)lib.GetFunc("IsWow64Process2");
		Process_Inited = true;
	}
	if (Process_IsWow64Process2 && Process_IsWow64Process2((HANDLE)this->handle, &pProcessMachine, &pNativeMachine))
	{
		if (pProcessMachine == 0)
			pProcessMachine = pNativeMachine;
		switch (pProcessMachine)
		{
		case IMAGE_FILE_MACHINE_I386: //0x014c
			return Manage::ThreadContext::ContextType::X86_32;
		case IMAGE_FILE_MACHINE_R3000://0x0162
			return Manage::ThreadContext::ContextType::Unknown;
		case IMAGE_FILE_MACHINE_R4000: //0x0166
			return Manage::ThreadContext::ContextType::Unknown;
		case IMAGE_FILE_MACHINE_R10000: //0x0168
			return Manage::ThreadContext::ContextType::Unknown;
		case IMAGE_FILE_MACHINE_WCEMIPSV2: //0x0169
			return Manage::ThreadContext::ContextType::MIPS;
		case IMAGE_FILE_MACHINE_ARM: //0x01c0
			return Manage::ThreadContext::ContextType::ARM;
		case IMAGE_FILE_MACHINE_IA64: //0x0200
			return Manage::ThreadContext::ContextType::X86_64;
		case IMAGE_FILE_MACHINE_ARM64: //0xAA64
			return Manage::ThreadContext::ContextType::ARM64;
		default:
			return Manage::ThreadContext::ContextType::Unknown;
		}
	}
#if defined(CPU_X86_64)
	BOOL isWow64;
	if (!IsWow64Process((HANDLE)this->handle, &isWow64))
		return Manage::ThreadContext::ContextType::X86_32;
	if (isWow64)
	{
		return Manage::ThreadContext::ContextType::X86_32;
	}
	else
	{
		return Manage::ThreadContext::ContextType::X86_64;
	}
#else
	return Manage::ThreadContext::ContextType::X86_32;
#endif
#endif
}

UInt8 Manage::Process::ReadMemUInt8(UInt64 addr)
{
	UInt8 buff;
	SIZE_T size;
	if (ReadProcessMemory(this->handle, (void*)addr, &buff, 1, &size))
	{
		return buff;
	}
	return 0;
}

UInt16 Manage::Process::ReadMemUInt16(UInt64 addr)
{
	UInt8 buff[2];
	SIZE_T size;
	if (ReadProcessMemory(this->handle, (void*)addr, buff, 2, &size))
	{
		if (size == 2)
			return ReadUInt16(&buff[0]);
	}
	return 0;
}

UInt32 Manage::Process::ReadMemUInt32(UInt64 addr)
{
	UInt8 buff[4];
	SIZE_T size;
	if (ReadProcessMemory(this->handle, (void*)addr, buff, 4, &size))
	{
		if (size == 4)
			return ReadUInt32(&buff[0]);
	}
	return 0;
}

UInt64 Manage::Process::ReadMemUInt64(UInt64 addr)
{
	UInt8 buff[8];
	SIZE_T size;
	if (ReadProcessMemory(this->handle, (void*)addr, buff, 8, &size))
	{
		if (size == 8)
			return ReadUInt64(&buff[0]);
	}
	return 0;
}

UOSInt Manage::Process::ReadMemory(UInt64 addr, UnsafeArray<UInt8> buff, UOSInt reqSize)
{
	SIZE_T size;
	if (ReadProcessMemory(this->handle, (void*)addr, buff.Ptr(), reqSize, &size))
	{
		return size;
	}
	return 0;
}

struct Manage::Process::FindProcSess
{
	UnsafeArrayOpt<const WChar> fileName;
	HANDLE hand;
	Bool isFirst;
};

Optional<Manage::Process::FindProcSess> Manage::Process::FindProcess(Text::CString processName)
{
	Manage::Process::FindProcSess *sess;
	HANDLE hand;
	hand = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hand == INVALID_HANDLE_VALUE)
		return 0;
	sess = MemAlloc(Manage::Process::FindProcSess, 1);
	sess->hand = hand;
	Text::CStringNN nnprocessName;
	if (!processName.SetTo(nnprocessName) || nnprocessName.leng == 0)
	{
		sess->fileName = 0;
	}
	else
	{
		sess->fileName = Text::StrToWCharNew(nnprocessName.v);
	}
	sess->isFirst = true;
	return sess;
}

Optional<Manage::Process::FindProcSess> Manage::Process::FindProcessW(UnsafeArrayOpt<const WChar> processName)
{
	Manage::Process::FindProcSess *sess;
	HANDLE hand;
	hand = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hand == INVALID_HANDLE_VALUE)
		return 0;
	sess = MemAlloc(Manage::Process::FindProcSess, 1);
	sess->hand = hand;
	UnsafeArray<const WChar> nnprocessName;
	if (!processName.SetTo(nnprocessName))
	{
		sess->fileName = 0;
	}
	else
	{
		sess->fileName = Text::StrCopyNew(nnprocessName);
	}
	sess->isFirst = true;
	return sess;
}

UnsafeArrayOpt<UTF8Char> Manage::Process::FindProcessNext(UnsafeArray<UTF8Char> processNameBuff, NN<Manage::Process::FindProcSess> pfsess, NN<Manage::Process::ProcessInfo> info)
{
#ifdef _WIN32_WCE
	PROCESSENTRY32 pe32;
	BOOL ret;
	pe32.dwSize = sizeof(PROCESSENTRY32);
	if (pfsess->isFirst)
	{
		ret = Process32First(pfsess->hand, &pe32);
		pfsess->isFirst = false;
	}
	else
	{
		ret = Process32Next(pfsess->hand, &pe32);
	}

	while (ret)
	{
		if (pfsess->fileName)
		{
			if (IO::Path::FileNameCompareW(pfsess->fileName, pe32.szExeFile) == 0)
			{
				info->parentId = pe32.th32ParentProcessID;
				info->processId = pe32.th32ProcessID;
				info->threadCnt = pe32.cntThreads;
				return Text::StrWChar_UTF8(processNameBuff, pe32.szExeFile, -1);
			}
		}
		else
		{
			info->parentId = pe32.th32ParentProcessID;
			info->processId = pe32.th32ProcessID;
			info->threadCnt = pe32.cntThreads;
			return Text::StrWChar_UTF8(processNameBuff, pe32.szExeFile, -1);
		}
		ret = Process32Next(pfsess->hand, &pe32);
	}
	return 0;
#else
	PROCESSENTRY32W pe32;
	UnsafeArray<const WChar> fileName;
	BOOL ret;
	pe32.dwSize = sizeof(PROCESSENTRY32W);
	if (pfsess->isFirst)
	{
		ret = Process32FirstW(pfsess->hand, &pe32);
		pfsess->isFirst = false;
	}
	else
	{
		ret = Process32NextW(pfsess->hand, &pe32);
	}

	while (ret)
	{
		if (pfsess->fileName.SetTo(fileName))
		{
			if (IO::Path::FileNameCompareW(fileName, pe32.szExeFile) == 0)
			{
				info->parentId = pe32.th32ParentProcessID;
				info->processId = pe32.th32ProcessID;
				info->threadCnt = pe32.cntThreads;
				return Text::StrWChar_UTF8(processNameBuff, pe32.szExeFile);
			}
		}
		else
		{
			info->parentId = pe32.th32ParentProcessID;
			info->processId = pe32.th32ProcessID;
			info->threadCnt = pe32.cntThreads;
			return Text::StrWChar_UTF8(processNameBuff, pe32.szExeFile);
		}
		ret = Process32NextW(pfsess->hand, &pe32);
	}
	return 0;
#endif
}

UnsafeArrayOpt<WChar> Manage::Process::FindProcessNextW(UnsafeArray<WChar> processNameBuff, NN<Manage::Process::FindProcSess> pfsess, NN<Manage::Process::ProcessInfo> info)
{
#ifdef _WIN32_WCE
	PROCESSENTRY32 pe32;
	BOOL ret;
	pe32.dwSize = sizeof(PROCESSENTRY32);
	if (pfsess->isFirst)
	{
		ret = Process32First(pfsess->hand, &pe32);
		pfsess->isFirst = false;
	}
	else
	{
		ret = Process32Next(pfsess->hand, &pe32);
	}

	while (ret)
	{
		if (pfsess->fileName)
		{
			if (IO::Path::FileNameCompareW(pfsess->fileName, pe32.szExeFile) == 0)
			{
				info->parentId = pe32.th32ParentProcessID;
				info->processId = pe32.th32ProcessID;
				info->threadCnt = pe32.cntThreads;
				return Text::StrConcat(processNameBuff, pe32.szExeFile);
			}
		}
		else
		{
			info->parentId = pe32.th32ParentProcessID;
			info->processId = pe32.th32ProcessID;
			info->threadCnt = pe32.cntThreads;
			return Text::StrConcat(processNameBuff, pe32.szExeFile);
		}
		ret = Process32Next(pfsess->hand, &pe32);
	}
	return 0;
#else
	PROCESSENTRY32W pe32;
	BOOL ret;
	UnsafeArray<const WChar> fileName;
	pe32.dwSize = sizeof(PROCESSENTRY32W);
	if (pfsess->isFirst)
	{
		ret = Process32FirstW(pfsess->hand, &pe32);
		pfsess->isFirst = false;
	}
	else
	{
		ret = Process32NextW(pfsess->hand, &pe32);
	}

	while (ret)
	{
		if (pfsess->fileName.SetTo(fileName))
		{
			if (IO::Path::FileNameCompareW(fileName, pe32.szExeFile) == 0)
			{
				info->parentId = pe32.th32ParentProcessID;
				info->processId = pe32.th32ProcessID;
				info->threadCnt = pe32.cntThreads;
				return Text::StrConcat(processNameBuff, pe32.szExeFile);
			}
		}
		else
		{
			info->parentId = pe32.th32ParentProcessID;
			info->processId = pe32.th32ProcessID;
			info->threadCnt = pe32.cntThreads;
			return Text::StrConcat(processNameBuff, pe32.szExeFile);
		}
		ret = Process32NextW(pfsess->hand, &pe32);
	}
	return 0;
#endif
}

void Manage::Process::FindProcessClose(NN<Manage::Process::FindProcSess> pfsess)
{
	UnsafeArray<const WChar> fileName;
	if (pfsess->fileName.SetTo(fileName))
	{
		Text::StrDelNew(fileName);
	}
	CloseToolhelp32Snapshot(pfsess->hand);
	MemFreeNN(pfsess);
}

Int32 Manage::Process::ExecuteProcess(Text::CStringNN cmd, NN<Text::StringBuilderUTF8> result)
{
	if (cmd.leng > 32767)
	{
		return -1;
	}
	UnsafeArray<const WChar> wptr = Text::StrToWCharNew(cmd.v);
	Int32 ret = ExecuteProcessW(wptr.Ptr(), result);
	Text::StrDelNew(wptr);
	return ret;
}

Int32 Manage::Process::ExecuteProcessW(UnsafeArray<const WChar> cmd, NN<Text::StringBuilderUTF8> result)
{
	WChar buff[MAX_PATH];
	WChar progName[MAX_PATH];
	UTF8Char tmpFile[MAX_PATH];
	UOSInt cmdLen = Text::StrCharCnt(cmd);
	WChar *cmdLine = MemAlloc(WChar, cmdLen + 512);
	UnsafeArray<UTF8Char> sptr;
	Text::StrConcat(cmdLine, cmd);

	WChar *cptr = cmdLine;
	WChar *pptr = progName;
	Bool isQuote = false;
	WChar c;
	while ((c = *cptr++) != 0)
	{
		if (c == '"')
			isQuote = !isQuote;
		else if (!isQuote && c == ' ')
		{
			break;
		}
		else
		{
			*pptr++ = c;
		}
	}
	*pptr = 0;

	IO::Path::GetFileDirectoryW(buff, progName);
	PROCESS_INFORMATION procInfo;
	STARTUPINFOW startInfo;
	BOOL createRet;
	sptr = IO::Path::GetTempFile(tmpFile, UTF8STRC("ProcessTmp"));
	sptr = Text::StrUInt32(sptr, (UInt32)GetCurrProcId());
	sptr = Text::StrConcatC(sptr, UTF8STRC("_"));
	sptr = Text::StrInt32(sptr, Sync::Interlocked::IncrementI32(Process_ExecFileId));
	sptr = Text::StrConcatC(sptr, UTF8STRC(".dat"));

	SECURITY_ATTRIBUTES sa;
	sa.nLength= sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;

	ZeroMemory(&startInfo, sizeof(startInfo));
#ifdef _WIN32_WCE
	cptr = &cmdLine[Text::StrCharCnt(cmdLine)];
	cptr = Text::StrConcat(cptr, L" > \"");
	cptr = Text::StrUTF8_WChar(cptr, tmpFile, -1, 0);
	cptr = Text::StrConcat(cptr, L"\"");
	createRet = CreateProcessW(0, cmdLine, 0, 0, true, 0, 0, buff, 0, &procInfo);
#else
	startInfo.cb = sizeof(startInfo);
	startInfo.dwFlags = STARTF_USESTDHANDLES;
	UnsafeArray<const WChar> wptr = Text::StrToWCharNew(tmpFile);
	startInfo.hStdOutput = CreateFileW(wptr.Ptr(), GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, &sa, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	Text::StrDelNew(wptr);
	startInfo.hStdError = startInfo.hStdOutput;
	startInfo.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
	createRet = CreateProcessW(0, cmdLine, 0, 0, true, NORMAL_PRIORITY_CLASS, 0, buff, &startInfo, &procInfo);
#endif
	if(createRet)
	{
		UInt32 exitCode;
		WaitForSingleObject(procInfo.hProcess, INFINITE);
		GetExitCodeProcess(procInfo.hProcess, (LPDWORD)&exitCode);
		CloseHandle(procInfo.hProcess);
		CloseHandle(procInfo.hThread);
		CloseHandle(startInfo.hStdOutput);
		//CloseHandle(startInfo.hStdError);

		NN<IO::FileStream> fs;
		UTF8Char lineBuff[128];
		UnsafeArray<UTF8Char> linePtr;
		UOSInt retryCnt = 20;
		while (true)
		{
			NEW_CLASSNN(fs, IO::FileStream(CSTRP(tmpFile, sptr), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Sequential));
			if (!fs->IsError() || retryCnt-- <= 0)
			{
				break;
			}
			fs.Delete();
			Sync::SimpleThread::Sleep(100);
		}
		{
			IO::StreamReader reader(fs);
			while (reader.ReadLine(lineBuff, 124).SetTo(linePtr))
			{
				linePtr = reader.GetLastLineBreak(linePtr);
				result->AppendP(lineBuff, linePtr);
			}
		}
		fs.Delete();
		IO::FileUtil::DeleteFile(CSTRP(tmpFile, sptr), false);
		MemFree(cmdLine);
		return (Int32)exitCode;
	}
	else
	{
		CloseHandle(startInfo.hStdOutput);
		IO::FileUtil::DeleteFile(CSTRP(tmpFile, sptr), false);
		//UInt32 ret = GetLastError();
		MemFree(cmdLine);
		return -1;
	}

}

Bool Manage::Process::IsAlreadyStarted()
{
	WChar wbuff[512];
	Optional<Manage::Process::FindProcSess> sess;
	NN<Manage::Process::FindProcSess> nnsess;
	Bool found = false;
	UOSInt procId = GetCurrProcId();
	UOSInt i;
	IO::Path::GetProcessFileNameW(wbuff);
	Manage::Process::ProcessInfo info;

	i = Text::StrLastIndexOfCharW(wbuff, IO::Path::PATH_SEPERATOR);
	if (i != INVALID_INDEX)
	{
		sess = Manage::Process::FindProcessW(&wbuff[i + 1]);
	}
	else
	{
		sess = Manage::Process::FindProcessW(wbuff);
	}
	if (sess.SetTo(nnsess))
	{
		while (Manage::Process::FindProcessNextW(wbuff, nnsess, info).NotNull())
		{
			if (info.processId != procId)
			{
				found = true;
				break;
			}
		}
		Manage::Process::FindProcessClose(nnsess);
	}
	return found;
}

Bool Manage::Process::OpenPath(Text::CStringNN path)
{
#ifdef _WIN32_WCE
	return false;
#else
	UOSInt strLen = Text::StrUTF8_WCharCnt(path.v);
	WChar *s = MemAlloc(WChar, strLen + 1);
	Text::StrUTF8_WChar(s, path.v, 0);
	Bool succ = 32 < (OSInt)ShellExecuteW(0, L"open", s, 0, 0, SW_SHOW);
	MemFree(s);
	return succ;
#endif
}

Bool Manage::Process::OpenPathW(UnsafeArray<const WChar> path)
{
#ifdef _WIN32_WCE
	return false;
#else
	Bool succ = 32 < (OSInt)ShellExecuteW(0, L"open", path.Ptr(), 0, 0, SW_SHOW);
	return succ;
#endif
}

Text::CStringNN Manage::Process::GetPriorityName(ProcessPriority priority)
{
	switch (priority)
	{
	case PP_REALTIME:
		return CSTR("Realtime");
	case PP_HIGH:
		return CSTR("High");
	case PP_ABOVE_NORMAL:
		return CSTR("Above Normal");
	case PP_NORMAL:
		return CSTR("Normal");
	case PP_BELOW_NORMAL:
		return CSTR("Below Normal");
	case PP_IDLE:
		return CSTR("Idle");
	case PP_UNKNOWN:
	default:
		return CSTR("Unknown");
	}
}
