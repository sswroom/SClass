#include "Stdafx.h"
#include "IO/FileStream.h"
#include "IO/ValgrindLog.h"
#include "Text/UTF8Reader.h"

void IO::ValgrindLog::SetPPID(UIntOS ppid)
{
	this->ppid = ppid;
}

void IO::ValgrindLog::SetVersion(Text::CStringNN version)
{
	OPTSTR_DEL(this->version);
	this->version = Text::String::New(version);
}

void IO::ValgrindLog::SetCommandLine(Text::CStringNN commandLine)
{
	OPTSTR_DEL(this->commandLine);
	this->commandLine = Text::String::New(commandLine);
}

void IO::ValgrindLog::BeginLeak(UIntOS threadId, Text::CStringNN message)
{
	NN<LeakInfo> leak;
	NEW_CLASSNN(leak, LeakInfo());
	leak->threadId = threadId;
	leak->message = Text::String::New(message);
	this->leaks.Add(leak);
	this->currLeak = leak;
	this->currException = nullptr;
	this->currStack = leak->stacks;
}

void IO::ValgrindLog::BeginException(UIntOS threadId, Text::CStringNN message)
{
	NN<ExceptionInfo> ex;
	NEW_CLASSNN(ex, ExceptionInfo());
	ex->threadId = threadId;
	ex->message = Text::String::New(message);
	ex->accessAddress = 0;
	ex->addrType = AddressType::NoAddress;
	ex->blockOfst = 0;
	ex->stackSize = 0;
	this->exceptions.Add(ex);
	this->currException = ex;
	this->currLeak = nullptr;
	this->currStack = ex->stacks;
}

void IO::ValgrindLog::NewStack(UInt64 address, Text::CStringNN funcName, Text::CString source)
{
	NN<Data::ArrayListNN<StackEntry>> currStack;
	if (this->currStack.SetTo(currStack))
	{
		NN<StackEntry> stack = MemAllocNN(StackEntry);
		stack->address = address;
		stack->funcName = Text::String::New(funcName);
		stack->source = Text::String::NewOrNull(source);
		currStack->Add(stack);
	}
}

void IO::ValgrindLog::SetExceptionAccessAddress(UInt64 address, AddressType addrType)
{
	NN<ExceptionInfo> ex;
	if (this->currException.SetTo(ex))
	{
		ex->accessAddress = address;
		ex->addrType = addrType;
	}
}

void IO::ValgrindLog::SetExceptionStackSize(UInt32 size)
{
	NN<ExceptionInfo> ex;
	if (this->currException.SetTo(ex))
	{
		ex->stackSize = size;
	}
}

void IO::ValgrindLog::ExceptionBeginAlloc(UInt64 address, UInt32 blockOfst, UInt32 blockSize)
{
	NN<ExceptionInfo> ex;
	if (this->currException.SetTo(ex))
	{
		ex->accessAddress = address;
		ex->addrType = AddressType::Alloc;
		ex->blockOfst = blockOfst;
		ex->stackSize = blockSize;
		this->currStack = ex->allocStacks;
	}
}

void IO::ValgrindLog::SetHeapInUse(UInt64 bytesInUse, UInt32 blocksInUse)
{
	this->bytesInUse = bytesInUse;
	this->blocksInUse = blocksInUse;
}

void IO::ValgrindLog::SetTotalHeaps(UInt32 blocksAllocs, UInt32 blocksFrees, UInt64 bytesAllocs)
{
	this->blocksAllocs = blocksAllocs;
	this->blocksFrees = blocksFrees;
	this->bytesAllocs = bytesAllocs;
}

void __stdcall IO::ValgrindLog::FreeStack(NN<StackEntry> stack)
{
	stack->funcName->Release();
	OPTSTR_DEL(stack->source);
	MemFreeNN(stack);
}

void __stdcall IO::ValgrindLog::FreeLeak(NN<LeakInfo> leak)
{
	leak->message->Release();
	leak->stacks.FreeAll(FreeStack);
	leak.Delete();
}

void __stdcall IO::ValgrindLog::FreeException(NN<ExceptionInfo> ex)
{
	ex->message->Release();
	ex->stacks.FreeAll(FreeStack);
	ex->allocStacks.FreeAll(FreeStack);
	ex.Delete();
}

IO::ValgrindLog::ValgrindLog(UIntOS mainPID)
{
	this->ppid = 0;
	this->mainPID = mainPID;
	this->version = nullptr;
	this->commandLine = nullptr;
	this->bytesInUse = 0;
	this->blocksInUse = 0;
	this->blocksAllocs = 0;
	this->blocksFrees = 0;
	this->bytesAllocs = 0;
	this->currException = nullptr;
	this->currLeak = nullptr;
	this->currStack = nullptr;
}

IO::ValgrindLog::~ValgrindLog()
{
	OPTSTR_DEL(this->version);
	OPTSTR_DEL(this->commandLine);
	this->leaks.FreeAll(FreeLeak);
	this->exceptions.FreeAll(FreeException);
}

UIntOS IO::ValgrindLog::GetMainPID() const
{
	return this->mainPID;
}

UIntOS IO::ValgrindLog::GetPPID() const
{
	return this->ppid;
}

Optional<Text::String> IO::ValgrindLog::GetVersion() const
{
	return this->version;
}

Optional<Text::String> IO::ValgrindLog::GetCommandLine() const
{
	return this->commandLine;
}

UInt64 IO::ValgrindLog::GetBytesInUse() const
{
	return this->bytesInUse;
}

UInt32 IO::ValgrindLog::GetBlocksInUse() const
{
	return this->blocksInUse;
}

UInt32 IO::ValgrindLog::GetBlocksAllocs() const
{
	return this->blocksAllocs;
}

UInt32 IO::ValgrindLog::GetBlocksFrees() const
{
	return this->blocksFrees;
}

UInt64 IO::ValgrindLog::GetBytesAllocs() const
{
	return this->bytesAllocs;
}

NN<const Data::ArrayListNN<IO::ValgrindLog::ExceptionInfo>> IO::ValgrindLog::GetErrorList() const
{
	return this->exceptions;
}

NN<const Data::ArrayListNN<IO::ValgrindLog::LeakInfo>> IO::ValgrindLog::GetLeakList() const
{
	return this->leaks;
}

Optional<IO::ValgrindLog> IO::ValgrindLog::LoadFile(Text::CStringNN filePath)
{
	IO::FileStream fs(filePath, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	return LoadStream(fs);
}

Optional<IO::ValgrindLog> IO::ValgrindLog::LoadStream(NN<IO::Stream> stream)
{
	Text::UTF8Reader reader(stream);
	Text::StringBuilderUTF8 sb;
	if (!reader.ReadLine(sb, 1024))
	{
		return nullptr;
	}
	UTF8Char sbuff[128];
	UInt32 lineNum = 1;
	Bool hasError = false;
	Bool beginGroup = false;
	Bool beginLeak = false;
	Bool beginStack = false;
	UIntOS threadId = 0;
	UIntOS pid;
	UIntOS i;
	UIntOS j;
	if (!sb.StartsWith(CSTR("==")))
		return nullptr;
	i = sb.IndexOf(UTF8STRC("== "), 2);
	if (i == INVALID_INDEX)
		return nullptr;
	sb.v[i] = 0;
	if (!Text::StrToUIntOS(&sb.v[2], pid))
		return nullptr;
	if (!sb.ToCString().Substring(i + 3).Equals(CSTR("Memcheck, a memory error detector")))
		return nullptr;
	NN<IO::ValgrindLog> log;
	NEW_CLASSNN(log, IO::ValgrindLog(pid));
	while (true)
	{
		sb.ClearStr();
		if (!reader.ReadLine(sb, 1024))
			break;
		lineNum++;
		if (!sb.StartsWith(CSTR("==")))
		{
			if (!hasError)
			{
				printf("Unsupported line %d: %s\r\n", lineNum, sb.v.Ptr());
				hasError = true;
			}
			continue;
		}
		i = sb.IndexOf(UTF8STRC("== "), 2);
		if (i == INVALID_INDEX)
		{
			if (!hasError)
			{
				printf("Unsupported line %d: %s\r\n", lineNum, sb.v.Ptr());
				hasError = true;
			}
			continue;
		}
		sb.v[i] = 0;
		if (!Text::StrToUIntOS(&sb.v[2], pid))
		{
			sb.v[i] = '=';
			if (!hasError)
			{
				printf("Unsupported line %d: %s\r\n", lineNum, sb.v.Ptr());
				hasError = true;
			}
			continue;
		}
		sb.v[i] = '=';

		Text::CStringNN cont = sb.ToCString().Substring(i + 3);
		if (cont.leng == 0)
		{
			beginGroup = true;
		}
		else
		{
			if (cont.StartsWith(CSTR("Copyright (C)")))
			{
			}
			else if (cont.StartsWith(CSTR("Using Valgrind-")))
			{
				i = cont.IndexOf(' ', 15);
				if (i != INVALID_INDEX)
				{
					log->SetVersion(Text::CStringNN(&cont.v[15], i - 15));
				}
			}
			else if (cont.StartsWith(CSTR("Command: ")))
			{
				log->SetCommandLine(cont.Substring(9));
			}
			else if (cont.StartsWith(CSTR("Parent PID: ")))
			{
				UIntOS ppid;
				if (cont.Substring(12).ToUIntOS(ppid))
				{
					log->SetPPID(ppid);
				}
			}
			else if (cont.StartsWith(CSTR("Warning: ")))
			{
				continue;
			}
			else if (cont.StartsWith(CSTR("More than ")) && cont.EndsWith(CSTR(" errors detected.  Subsequent errors")))
			{
				continue;
			}
			else if (cont.Equals(CSTR("will still be recorded, but in less detail than before.")))
			{
				continue;
			}
			else if (cont.Equals(CSTR("HEAP SUMMARY:")))
			{
				beginLeak = true;
				continue;
			}
			else if (cont.Equals(CSTR("LEAK SUMMARY:")))
			{
				break;
			}
			else if (cont.StartsWith(CSTR("    in use at exit: ")) && cont.EndsWith(CSTR(" blocks")) && (i = cont.IndexOf(UTF8STRC(" bytes in "), 20)) != INVALID_INDEX)
			{
				UInt64 bytesInUse;
				Text::StrConcatC(sbuff, &cont.v[20], i - 20);
				Text::StrRemoveChar(sbuff, ',');
				bytesInUse = Text::StrToUInt64(sbuff);
				Text::StrConcatC(sbuff, &cont.v[i + 10], cont.leng - i - 17);
				Text::StrRemoveChar(sbuff, ',');
				log->SetHeapInUse(bytesInUse, Text::StrToUInt32(sbuff));
			}
			else if (cont.StartsWith(CSTR("  total heap usage: ")) && cont.EndsWith(CSTR(" bytes allocated")) && (i = cont.IndexOf(UTF8STRC(" allocs, "), 20)) != INVALID_INDEX && (j = cont.IndexOf(UTF8STRC(" frees, "), i + 9)) != INVALID_INDEX)
			{
				UInt32 blocksAllocs;
				UInt32 blocksFrees;
				Text::StrConcatC(sbuff, &cont.v[20], i - 20);
				Text::StrRemoveChar(sbuff, ',');
				blocksAllocs = Text::StrToUInt32(sbuff);
				Text::StrConcatC(sbuff, &cont.v[i + 9], j - i - 9);
				Text::StrRemoveChar(sbuff, ',');
				blocksFrees = Text::StrToUInt32(sbuff);
				Text::StrConcatC(sbuff, &cont.v[j + 8], cont.leng - j - 24);
				Text::StrRemoveChar(sbuff, ',');
				log->SetTotalHeaps(blocksAllocs, blocksFrees, Text::StrToUInt64(sbuff));
			}
			else if (cont.StartsWith(CSTR("Thread ")) && cont.EndsWith(':'))
			{
				sb.RemoveChars(1);
				Text::StrToUIntOS(&cont.v[7], threadId);
				continue;
			}
			else if (!beginLeak && cont.StartsWith(CSTR(" Address ")))
			{
				AddressType addrType = AddressType::NoAddress;
				if (cont.EndsWith(CSTR("'s stack")))
				{
					addrType = AddressType::Stack;
					i = cont.IndexOf(' ', 9);
					if (i != INVALID_INDEX)
					{
						Text::StrConcatC(sbuff, &cont.v[9], i - 9);
						log->SetExceptionAccessAddress(Text::StrToUInt64(sbuff), addrType);
					}
				}
				else if (cont.EndsWith(CSTR(" is not stack'd, malloc'd or (recently) free'd")))
				{
					addrType = AddressType::Unknown;
					i = cont.IndexOf(' ', 9);
					if (i != INVALID_INDEX)
					{
						Text::StrConcatC(sbuff, &cont.v[9], i - 9);
						log->SetExceptionAccessAddress(Text::StrToUInt64(sbuff), addrType);
					}
				}
				else if (cont.EndsWith(CSTR(" alloc'd")) && (i = cont.IndexOf(UTF8STRC(" is "), 9)) != INVALID_INDEX && (j = cont.IndexOf(UTF8STRC(" bytes inside a block of size "), i + 4)) != INVALID_INDEX)
				{
					UInt64 addr;
					UInt32 blockOfst;
					UInt32 blockSize;
					Text::StrConcatC(sbuff, &cont.v[9], i - 9);
					addr = Text::StrToUInt64(sbuff);
					Text::StrConcatC(sbuff, &cont.v[i + 4], j - i - 4);
					Text::StrRemoveChar(sbuff, ',');
					blockOfst = Text::StrToUInt32(sbuff);
					Text::StrConcatC(sbuff, &cont.v[j + 30], cont.leng - j - 38);
					Text::StrRemoveChar(sbuff, ',');
					blockSize = Text::StrToUInt32(sbuff);
					log->ExceptionBeginAlloc(addr, blockOfst, blockSize);
					beginStack = true;
				}
				else
				{
					if (!hasError)
					{
						printf("Unsupported line %d: %s\r\n", lineNum, sb.v.Ptr());
						hasError = true;
					}
					continue;
				}
			}
			else if (!beginLeak && cont.StartsWith(' ') && cont.EndsWith(CSTR(" bytes below stack pointer")))
			{
				Text::StrConcatC(sbuff, &cont.v[1], cont.leng - 27);
				log->SetExceptionStackSize(Text::StrToUInt32(sbuff));
			}
			else if (cont.StartsWith(CSTR("   at ")) || cont.StartsWith(CSTR("   by ")))
			{
				if (beginStack)
				{
					if (cont.StartsWith(CSTR("   by ")))
					{
						if (!hasError)
						{
							printf("Unsupported line %d: %s\r\n", lineNum, sb.v.Ptr());
							hasError = true;
						}
					}
				}
				else
				{
					if (cont.StartsWith(CSTR("   at ")))
					{
						if (!hasError)
						{
							printf("Unsupported line %d: %s\r\n", lineNum, sb.v.Ptr());
							hasError = true;
						}
					}
				}
				i = cont.IndexOf(' ', 6);
				if (i != INVALID_INDEX)
				{
					Text::StrConcatC(sbuff, &cont.v[6], i - 7);
					j = cont.IndexOf(UTF8STRC(" ("), i + 1);
					if (j == INVALID_INDEX)
					{
						log->NewStack(Text::StrToUInt64(sbuff), cont.Substring(i + 1), nullptr);
					}
					else
					{
						log->NewStack(Text::StrToUInt64(sbuff), Text::CStringNN(&cont.v[i + 1], j - i - 1), Text::CStringNN(&cont.v[j + 2], cont.leng - j - 3));
					}
					beginStack = false;
				}
				else
				{
					if (!hasError)
					{
						printf("Unsupported line %d: %s\r\n", lineNum, sb.v.Ptr());
						hasError = true;
					}
				}
			}
			else if (beginGroup && !cont.StartsWith(' '))
			{
				if (beginLeak)
				{
					log->BeginLeak(threadId, cont);
				}
				else
				{
					log->BeginException(threadId, cont);
				}
				beginStack = true;
			}
			else
			{
				if (!hasError)
				{
					printf("Unsupported line %d: %s\r\n", lineNum, sb.v.Ptr());
					hasError = true;
				}
			}
			beginGroup = false;
		}
	}
	return log;
}
