#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/ByteTool_C.h"
#include "Data/FastMap.hpp"
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

Bool Manage::Process::IsRunning() const
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

Bool Manage::Process::GetFilename(NN<Text::StringBuilderUTF8> sb)
{
	return false;
}

UOSInt Manage::Process::GetMemorySize()
{
	return 0;
}

Bool Manage::Process::SetMemorySize(UOSInt minSize, UOSInt maxSize)
{
	return false;
}

UOSInt Manage::Process::GetThreadIds(Data::ArrayList<UInt32> *threadList)
{
	return 0;
}

void *Manage::Process::GetHandle()
{
	return 0;
}


UOSInt Manage::Process::GetModules(Data::ArrayList<Manage::ModuleInfo *> *modList)
{
	return 0;
}

UOSInt Manage::Process::GetThreads(NN<Data::ArrayList<Manage::ThreadInfo *>> threadList)
{
	return 0;
}

UOSInt Manage::Process::GetHeapLists(Data::ArrayList<UInt32> *heapList)
{
	return 0;
}

UOSInt Manage::Process::GetHeaps(Data::ArrayList<HeapInfo*> *heapList, UInt32 heapListId, UOSInt maxCount)
{
	return 0;

}

void Manage::Process::FreeHeaps(Data::ArrayList<HeapInfo*> *heapList)
{
}

Data::Timestamp Manage::Process::GetStartTime()
{
	return Data::Timestamp(0);
}

Bool Manage::Process::GetWorkingSetSize(UOSInt *minSize, UOSInt *maxSize)
{
	return false;
}

Bool Manage::Process::GetMemoryInfo(UOSInt *pageFault, UOSInt *workingSetSize, UOSInt *pagedPoolUsage, UOSInt *nonPagedPoolUsage, UOSInt *pageFileUsage)
{
	return false;
}

Bool Manage::Process::GetTimeInfo(Data::Timestamp *createTime, Data::Timestamp *kernelTime, Data::Timestamp *userTime)
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
	return Manage::ThreadContext::ContextType::X86_32;
#elif defined(CPU_X86_64)
	return Manage::ThreadContext::ContextType::X86_64;
#elif defined(CPU_ARM)
	return Manage::ThreadContext::ContextType::ARM;
#elif defined(CPU_MIPS)
	return Manage::ThreadContext::ContextType::MIPS;
#elif defined(CPU_AVR)
	return Manage::ThreadContext::ContextType::AVR;
#else
	return Manage::ThreadContext::ContextType::X86_32;
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

UOSInt Manage::Process::ReadMemory(UInt64 addr, UInt8 *buff, UOSInt reqSize)
{
	const UInt8 *srcPtr = (const UInt8*)(UOSInt)addr;
	MemCopyNO(buff, srcPtr, reqSize);
	return reqSize;
}

Manage::Process::FindProcSess *Manage::Process::FindProcess(Text::CString processName)
{
	return 0;
}

Manage::Process::FindProcSess *Manage::Process::FindProcessW(const WChar *processName)
{
	return 0;
}

UTF8Char *Manage::Process::FindProcessNext(UTF8Char *processNameBuff, FindProcSess *sess, Manage::Process::ProcessInfo *info)
{
	return 0;
}

WChar *Manage::Process::FindProcessNextW(WChar *processNameBuff, FindProcSess *sess, Manage::Process::ProcessInfo *info)
{
	return 0;
}

void Manage::Process::FindProcessClose(FindProcSess *sess)
{
}

Int32 Manage::Process::ExecuteProcess(Text::CString cmd, NN<Text::StringBuilderUTF8> result)
{
	return -1;
}

Int32 Manage::Process::ExecuteProcessW(const WChar *cmd, NN<Text::StringBuilderUTF8> result)
{
	return -1;
}

Bool Manage::Process::IsAlreadyStarted()
{
	return false;
}

Bool Manage::Process::OpenPath(Text::CString path)
{
	return false;
}

Text::CString Manage::Process::GetPriorityName(ProcessPriority priority)
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
	default:
		return CSTR("Unknown");
	}
}

