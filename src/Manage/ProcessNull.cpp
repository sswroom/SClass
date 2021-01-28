#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Data/Integer32Map.h"
#include "IO/FileStream.h"
#include "IO/FileUtil.h"
#include "IO/Path.h"
#include "Manage/Process.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/StringBuilderW.h"
#include "Text/UTF8Reader.h"

Manage::Process::Process(UOSInt procId, Bool controlRight)
{
	this->procId = procId;
	this->needRelease = true;
}

Manage::Process::Process()
{
	this->procId = 0;
	this->needRelease = false;
}
Manage::Process::Process(const WChar *cmdLine)
{
	this->procId = 0;
	this->needRelease = true;
}

Manage::Process::~Process()
{
}

UOSInt Manage::Process::GetCurrProcId()
{
	return 0;
}

UOSInt Manage::Process::GetProcId()
{
	return this->procId;
}

Bool Manage::Process::IsRunning()
{
	return true;
}

Bool Manage::Process::Kill()
{
	return false;
}

WChar *Manage::Process::GetFilename(WChar *buff)
{
	return buff;
}

Bool Manage::Process::GetFilename(Text::StringBuilderUTF *sb)
{
	return false;
}

OSInt Manage::Process::GetMemorySize()
{
	return 0;
}

Bool Manage::Process::SetMemorySize(OSInt minSize, OSInt maxSize)
{
	return false;
}

OSInt Manage::Process::GetThreadIds(Data::ArrayList<UInt32> *threadList)
{
	return 0;
}

void *Manage::Process::GetHandle()
{
	return 0;
}


OSInt Manage::Process::GetModules(Data::ArrayList<Manage::ModuleInfo *> *modList)
{
	return 0;
}

OSInt Manage::Process::GetThreads(Data::ArrayList<Manage::ThreadInfo *> *threadList)
{
	return 0;
}

OSInt Manage::Process::GetHeapLists(Data::ArrayList<Int32> *heapList)
{
	return 0;
}

OSInt Manage::Process::GetHeaps(Data::ArrayList<HeapInfo*> *heapList, Int32 heapListId, OSInt maxCount)
{
	return 0;

}

void Manage::Process::FreeHeaps(Data::ArrayList<HeapInfo*> *heapList)
{
}

Bool Manage::Process::GetWorkingSetSize(UOSInt *minSize, UOSInt *maxSize)
{
	return false;
}

Bool Manage::Process::GetMemoryInfo(OSInt *pageFault, OSInt *workingSetSize, OSInt *pagedPoolUsage, OSInt *nonPagedPoolUsage, OSInt *pageFileUsage)
{
	return false;
}

Bool Manage::Process::GetTimeInfo(Data::DateTime *createTime, Data::DateTime *kernelTime, Data::DateTime *userTime)
{
	return false;
}

UInt32 Manage::Process::GetGDIObjCount()
{
	return 0;
}

UInt32 Manage::Process::GetUserObjCount()
{
	return 0;
}

UInt32 Manage::Process::GetHandleCount()
{
	return 0;
}

Manage::Process::ProcessPriority Manage::Process::GetPriority()
{
	return PP_NORMAL;
}

Manage::ThreadContext::ContextType Manage::Process::GetContextType()
{
#if defined(CPU_X86_32)
	return Manage::ThreadContext::CT_X86_32;
#elif defined(CPU_X86_64)
	return Manage::ThreadContext::CT_X86_64;
#elif defined(CPU_ARM)
	return Manage::ThreadContext::CT_ARM;
#elif defined(CPU_MIPS)
	return Manage::ThreadContext::CT_MIPS;
#elif defined(CPU_AVR)
	return Manage::ThreadContext::CT_AVR;
#else
	return Manage::ThreadContext::CT_X86_32;
#endif
}
UInt8 Manage::Process::ReadMemUInt8(UInt64 addr)
{
	UInt8 buff[1];
	OSInt size = this->ReadMemory(addr, buff, 1);
	if (size == 1)
		return buff[0];
	return 0;
}

UInt16 Manage::Process::ReadMemUInt16(UInt64 addr)
{
	UInt8 buff[2];
	OSInt size = this->ReadMemory(addr, buff, 2);
	if (size == 2)
		return ReadUInt16(&buff[0]);
	return 0;
}

UInt32 Manage::Process::ReadMemUInt32(UInt64 addr)
{
	UInt8 buff[4];
	OSInt size = this->ReadMemory(addr, buff, 4);
	if (size == 4)
		return ReadUInt32(&buff[0]);
	return 0;
}

UInt64 Manage::Process::ReadMemUInt64(UInt64 addr)
{
	UInt8 buff[8];
	OSInt size = this->ReadMemory(addr, buff, 8);
	if (size == 8)
		return ReadUInt64(&buff[0]);
	return 0;
}

OSInt Manage::Process::ReadMemory(UInt64 addr, UInt8 *buff, OSInt reqSize)
{
	const UInt8 *srcPtr = (const UInt8*)(UOSInt)addr;
	MemCopyNO(buff, srcPtr, reqSize);
	return reqSize;
}

void *Manage::Process::FindProcess(const WChar *processName)
{
	return 0;
}

WChar *Manage::Process::FindProcessNext(WChar *processNameBuff, void *sess, Manage::Process::ProcessInfo *info)
{
	return 0;
}

void Manage::Process::FindProcessClose(void *sess)
{
}

Int32 Manage::Process::ExecuteProcess(const WChar *cmd, Text::StringBuilderUTF *result)
{
	return -1;
}

Bool Manage::Process::IsAlreadyStarted()
{
	return false;
}

Bool Manage::Process::OpenPath(const WChar *path)
{
	return false;
}

const UTF8Char *Manage::Process::GetPriorityName(ProcessPriority priority)
{
	switch (priority)
	{
	case PP_REALTIME:
		return (const UTF8Char*)"Realtime";
	case PP_HIGH:
		return (const UTF8Char*)"High";
	case PP_ABOVE_NORMAL:
		return (const UTF8Char*)"Above Normal";
	case PP_NORMAL:
		return (const UTF8Char*)"Normal";
	case PP_BELOW_NORMAL:
		return (const UTF8Char*)"Below Normal";
	case PP_IDLE:
		return (const UTF8Char*)"Idle";
	default:
		return (const UTF8Char*)"Unknown";
	}
}

