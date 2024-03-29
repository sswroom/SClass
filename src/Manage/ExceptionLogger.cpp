#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListInt64.h"
#include "IO/FileStream.h"
#include "Manage/ExceptionLogger.h"
#include "Manage/StackTracer.h"
#include "Manage/SymbolResolver.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Writer.h"

#define STACKDUMPSIZE 4096

void Manage::ExceptionLogger::WriteContext(NotNullPtr<IO::Writer> writer, NotNullPtr<IO::Stream> stm, NotNullPtr<Manage::ThreadContext> context, NotNullPtr<Manage::AddressResolver> addrResol)
{
	Text::StringBuilderUTF8 sb;
	UOSInt i;

	context->ToString(sb);
	writer->WriteLineC(sb.ToString(), sb.GetLength());

	Manage::Process proc;
	UOSInt size;
	UInt8 buff[STACKDUMPSIZE];
	if ((size = proc.ReadMemory(context->GetInstAddr(), buff, 256)) != 0)
	{
		UOSInt currAddr = context->GetInstAddr();
		UInt8 *currPtr = buff;
		writer->WriteLine();
		sb.ClearStr();
		sb.AppendC(UTF8STRC("Memory Dumps (Instructions):"));
		while (size > 0)
		{
			sb.AppendC(UTF8STRC("\r\n"));
			sb.AppendHexOS(currAddr);
			sb.AppendC(UTF8STRC(" "));
			if (size >= 16)
			{
				sb.AppendHexBuff(currPtr, 16, ' ', Text::LineBreakType::None);
				size -= 16;
				currAddr += 16;
				currPtr += 16;
			}
			else
			{
				sb.AppendHexBuff(currPtr, size, ' ', Text::LineBreakType::None);
				currAddr += (UInt32)size;
				currPtr += size;
				size = 0;
			}
		}
		writer->WriteLineC(sb.ToString(), sb.GetLength());
	}
	if ((size = proc.ReadMemory(context->GetStackAddr(), buff, STACKDUMPSIZE)) != 0)
	{
		UOSInt currAddr = context->GetStackAddr();
		UInt8 *currPtr = buff;
		writer->WriteLine();
		sb.ClearStr();
		sb.AppendC(UTF8STRC("Stack Dumps:"));
		while (size > 0)
		{
			sb.AppendC(UTF8STRC("\r\n"));
			sb.AppendHexOS(currAddr);
			sb.AppendC(UTF8STRC(" "));
			if (size >= 16)
			{
				sb.AppendHexBuff(currPtr, 16, ' ', Text::LineBreakType::None);
				size -= 16;
				currAddr += 16;
				currPtr += 16;
			}
			else
			{
				sb.AppendHexBuff(currPtr, size, ' ', Text::LineBreakType::None);
				currAddr += (UInt32)size;
				currPtr += size;
				size = 0;
			}
		}
		writer->WriteLineC(sb.ToString(), sb.GetLength());
	}

	Manage::Dasm *dasm = context->CreateDasm();
	if (dasm)
	{
		if (dasm->GetRegBitDepth() == Manage::Dasm::RBD_32)
		{
			UTF8Char sbuff[256];
			UTF8Char *sptr;
			Manage::Dasm32 *dasm32 = (Manage::Dasm32*)dasm;
			UInt32 currInst = (UInt32)context->GetInstAddr();
			UInt32 currStack = (UInt32)context->GetStackAddr();
			UInt32 currFrame = (UInt32)context->GetFrameAddr();
			Data::ArrayListUInt32 *callAddrs;
			Data::ArrayListUInt32 *jmpAddrs;
			UInt32 blockStart;
			UInt32 blockEnd;

			NEW_CLASS(callAddrs, Data::ArrayListUInt32());
			NEW_CLASS(jmpAddrs, Data::ArrayListUInt32());
			Bool retVal = true;
			writer->WriteLine();
			writer->WriteLineC(UTF8STRC("Disassembly:"));

			Data::ArrayListUInt32 *blkStarts;
			Data::ArrayListUInt32 *blkEnds;
			NEW_CLASS(blkStarts, Data::ArrayListUInt32());
			NEW_CLASS(blkEnds, Data::ArrayListUInt32());

			Bool fin = false;

			sb.ClearStr();
			sb.AppendC(UTF8STRC("("));
			if ((sptr = addrResol->ResolveName(sbuff, currInst)) != 0)
			{
				sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
				sb.AppendC(UTF8STRC(")"));
				writer->WriteLineC(sb.ToString(), sb.GetLength());
			}

			stm->Flush();
			Manage::Dasm::Dasm_Regs *regs = dasm32->CreateRegs();
			context->GetRegs(regs);
			while (true)
			{
				if ((size = proc.ReadMemory(currInst, buff, 256)) != 0)
				{
					UOSInt currAddr = currInst;
					UInt8 *currPtr = buff;
					sb.ClearStr();
					sb.AppendC(UTF8STRC("Memory Dumps (Instructions):"));
					while (size > 0)
					{
						sb.AppendC(UTF8STRC("\r\n"));
						sb.AppendHexOS(currAddr);
						sb.AppendC(UTF8STRC(" "));
						if (size >= 16)
						{
							sb.AppendHexBuff(currPtr, 16, ' ', Text::LineBreakType::None);
							size -= 16;
							currAddr += 16;
							currPtr += 16;
						}
						else
						{
							sb.AppendHexBuff(currPtr, size, ' ', Text::LineBreakType::None);
							currAddr += (UInt32)size;
							currPtr += size;
							size = 0;
						}
					}
					writer->WriteLineC(sb.ToString(), sb.GetLength());
				}

				writer->WriteLineCStr(dasm32->GetHeader(true));
				retVal = dasm32->Disasm32(writer, addrResol.Ptr(), &currInst, &currStack, &currFrame, callAddrs, jmpAddrs, &blockStart, &blockEnd, regs, &proc, true);
				if (!retVal)
				{
					break;
				}
				blkStarts->Add(blockStart);
				blkEnds->Add(blockEnd);

				i = blkStarts->GetCount();
				while (i-- > 0)
				{
					if (currInst >= (UInt32)blkStarts->GetItem(i) && currInst < (UInt32)blkEnds->GetItem(i))
					{
						fin = true;
						break;
					}
				}
				if (fin)
					break;
				callAddrs->Clear();
				jmpAddrs->Clear();

	//			writer->WriteLineC(sb.ToString(), sb.GetLength());
				writer->WriteLine();
				writer->WriteLineC(UTF8STRC("\r\nCalled by:"));

				sb.ClearStr();
				sb.AppendHex32(currInst);
				writer->WriteStrC(sb.ToString(), sb.GetLength());

				sb.ClearStr();
				if ((sptr = addrResol->ResolveName(sbuff, currInst)) != 0)
				{
					sb.AppendC(UTF8STRC(" ("));
					sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
					sb.AppendC(UTF8STRC(")"));
					writer->WriteLineC(sb.ToString(), sb.GetLength());
				}
				else
				{
					writer->WriteLine();
				}
				stm->Flush();
				sb.ClearStr();
			}
			dasm32->FreeRegs(regs);
			writer->WriteLineC(sb.ToString(), sb.GetLength());
			DEL_CLASS(callAddrs);
			DEL_CLASS(jmpAddrs);
			DEL_CLASS(blkStarts);
			DEL_CLASS(blkEnds);
		}
		else if (dasm->GetRegBitDepth() == Manage::Dasm::RBD_64)
		{
			UTF8Char sbuff[256];
			UTF8Char *sptr;
			Dasm64 *dasm64 = (Dasm64*)dasm;
			UInt64 currInst = context->GetInstAddr();
			UInt64 currStack = context->GetStackAddr();
			UInt64 currFrame = context->GetFrameAddr();;
			Data::ArrayListUInt64 *callAddrs;
			Data::ArrayListUInt64 *jmpAddrs;
			UInt64 blockStart;
			UInt64 blockEnd;

			NEW_CLASS(callAddrs, Data::ArrayListUInt64());
			NEW_CLASS(jmpAddrs, Data::ArrayListUInt64());
			Bool retVal = true;
			writer->WriteLine();
			writer->WriteLineC(UTF8STRC("Disassembly:"));

			Data::ArrayListUInt64 *blkStarts;
			Data::ArrayListUInt64 *blkEnds;
			NEW_CLASS(blkStarts, Data::ArrayListUInt64());
			NEW_CLASS(blkEnds, Data::ArrayListUInt64());

			Bool fin = false;

			sb.ClearStr();
			sb.AppendC(UTF8STRC("("));
			if ((sptr = addrResol->ResolveName(sbuff, currInst)) != 0)
			{
				sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
				sb.AppendC(UTF8STRC(")"));
				writer->WriteLineC(sb.ToString(), sb.GetLength());
			}

			stm->Flush();
			Manage::Dasm::Dasm_Regs *regs = dasm64->CreateRegs();
			context->GetRegs(regs);
			while (true)
			{
				if ((size = proc.ReadMemory(currInst, buff, 256)) != 0)
				{
					UInt64 currAddr = currInst;
					UInt8 *currPtr = buff;
					sb.ClearStr();
					sb.AppendC(UTF8STRC("Memory Dumps (Instructions):"));
					while (size > 0)
					{
						sb.AppendC(UTF8STRC("\r\n"));
						sb.AppendHex64(currAddr);
						sb.AppendC(UTF8STRC(" "));
						if (size >= 16)
						{
							sb.AppendHexBuff(currPtr, 16, ' ', Text::LineBreakType::None);
							size -= 16;
							currAddr += 16;
							currPtr += 16;
						}
						else
						{
							sb.AppendHexBuff(currPtr, size, ' ', Text::LineBreakType::None);
							currAddr += (UInt32)size;
							currPtr += size;
							size = 0;
						}
					}
					writer->WriteLineC(sb.ToString(), sb.GetLength());
				}

				writer->WriteLineCStr(dasm64->GetHeader(true));
				retVal = dasm64->Disasm64(writer, addrResol.Ptr(), &currInst, &currStack, &currFrame, callAddrs, jmpAddrs, &blockStart, &blockEnd, regs, &proc, true);
				if (!retVal)
				{
					break;
				}
				blkStarts->Add(blockStart);
				blkEnds->Add(blockEnd);

				i = blkStarts->GetCount();
				while (i-- > 0)
				{
					if (currInst >= (UInt64)blkStarts->GetItem(i) && currInst < (UInt64)blkEnds->GetItem(i))
					{
						fin = true;
						break;
					}
				}
				if (fin)
					break;
				callAddrs->Clear();
				jmpAddrs->Clear();

	//			writer->WriteLineC(sb.ToString(), sb.GetLength());
				writer->WriteLine();
				writer->WriteLineC(UTF8STRC("\r\nCalled by:"));

				sb.ClearStr();
				sb.AppendHex64(currInst);
				writer->WriteStrC(sb.ToString(), sb.GetLength());

				sb.ClearStr();
				if ((sptr = addrResol->ResolveName(sbuff, currInst)) != 0)
				{
					sb.AppendC(UTF8STRC(" ("));
					sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
					sb.AppendC(UTF8STRC(")"));
					writer->WriteLineC(sb.ToString(), sb.GetLength());
				}
				else
				{
					writer->WriteLine();
				}
				stm->Flush();
				sb.ClearStr();
			}
			dasm64->FreeRegs(regs);
			writer->WriteLineC(sb.ToString(), sb.GetLength());
			DEL_CLASS(callAddrs);
			DEL_CLASS(jmpAddrs);
			DEL_CLASS(blkStarts);
			DEL_CLASS(blkEnds);
		}
		DEL_CLASS(dasm);
	}
}

void Manage::ExceptionLogger::WriteStackTrace(NotNullPtr<IO::Writer> writer, NotNullPtr<Manage::StackTracer> tracer, NotNullPtr<Manage::AddressResolver> addrResol)
{
#ifndef _WIN32_WCE
	if (tracer->IsSupported())
	{
		Text::StringBuilderUTF8 sb;
		writer->WriteLineC(UTF8STRC("Stack trace (OS):"));

		while (tracer->GoToNextLevel())
		{
			sb.ClearStr();
			sb.AppendHex64(tracer->GetCurrentAddr());
			sb.AppendC(UTF8STRC(" "));
			addrResol->ResolveNameSB(sb, tracer->GetCurrentAddr());
			writer->WriteLineC(sb.ToString(), sb.GetLength());
		}
	}
#endif
}

Bool Manage::ExceptionLogger::LogToFile(NotNullPtr<Text::String> fileName, UInt32 exCode, Text::CString exName, UOSInt exAddr, NotNullPtr<Manage::ThreadContext> context)
{
#ifndef _WIN32_WCE
	Manage::Process proc;
	IO::FileStream fs(fileName, IO::FileMode::Append, IO::FileShare::DenyAll, IO::FileStream::BufferType::Normal);
	Text::UTF8Writer writer(fs);
	Text::StringBuilderUTF8 sb;
	Data::Timestamp d = Data::Timestamp::Now();
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	UOSInt i;
	UOSInt j;
	Manage::SymbolResolver symResol(proc);
	writer.WriteLineC(UTF8STRC("----------------------------------"));
	sb.AppendC(UTF8STRC("Exception occurs: Code = 0x"));
	sb.AppendHex32(exCode);
	sb.AppendC(UTF8STRC(" at "));
	sb.AppendHexOS(exAddr);
	sb.AppendC(UTF8STRC(", time = "));
	sb.AppendTSNoZone(d);
	sb.AppendC(UTF8STRC(", Type = "));
	sb.Append(exName);
	writer.WriteLineC(sb.ToString(), sb.GetLength());

	NotNullPtr<Text::String> s;
	j = symResol.GetModuleCount();
	writer.WriteLineC(UTF8STRC("\r\nLoaded modules:"));
	i = 0;
	while (i < j)
	{
		sb.ClearStr();
		if (symResol.GetModuleName(i).SetTo(s))
			sb.Append(s);
		sb.AppendC(UTF8STRC(", Addr="));
		sb.AppendHexOS((UOSInt)symResol.GetModuleAddr(i));
		sb.AppendC(UTF8STRC(",size="));
		sb.AppendHexOS((UOSInt)symResol.GetModuleSize(i));
		writer.WriteLineC(sb.ToString(), sb.GetLength());

		i++;
	}

	{
		Data::ArrayList<Manage::ThreadInfo*> threadList;
		Manage::ThreadInfo *thread;
		NotNullPtr<Manage::ThreadContext> tCont;
		UInt64 startAddr;
		proc.GetThreads(threadList);
		i = 0;
		j = threadList.GetCount();
		while (i < j)
		{
			thread = threadList.GetItem(i);

			sb.ClearStr();
			writer.WriteLine();
			sb.AppendC(UTF8STRC("Running threads: (0x"));
			sb.AppendHex32((UInt32)thread->GetThreadId());
			sb.AppendC(UTF8STRC(")"));
			sptr = thread->GetName(sbuff);
			if (sptr)
			{
				sb.AppendUTF8Char(' ');
				sb.AppendP(sbuff, sptr);
			}
			if (thread->IsCurrThread())
			{
				sb.AppendC(UTF8STRC(" current thread"));
			}
			writer.WriteLineC(sb.ToString(), sb.GetLength());

			startAddr = thread->GetStartAddress();
			if (startAddr != 0)
			{
				sb.ClearStr();
				sb.AppendC(UTF8STRC("Start address 0x"));
				sb.AppendHex64(startAddr);
				sb.AppendC(UTF8STRC(" "));
				symResol.ResolveNameSB(sb, startAddr);
				writer.WriteLineC(sb.ToString(), sb.GetLength());
			}

			if (!thread->IsCurrThread())
			{
				thread->Suspend();
				if (tCont.Set(thread->GetThreadContext()))
				{
					sb.ClearStr();
					sb.AppendC(UTF8STRC("Curr address 0x"));
					sb.AppendHexOS(tCont->GetInstAddr());
					sb.AppendC(UTF8STRC(" "));
					symResol.ResolveNameSB(sb, tCont->GetInstAddr());
					writer.WriteLineC(sb.ToString(), sb.GetLength());

					{
						Manage::StackTracer tracer(tCont);
						WriteStackTrace(writer, tracer, symResol);
					}
					tCont.Delete();
				}
				thread->Resume();
			}
			else
			{
				writer.WriteLineC(UTF8STRC("Current Thread"));
			}

			i++;
		}
	}

	{
		writer.WriteLine();
		writer.WriteLineC(UTF8STRC("Exception Thread:"));
		{
			Manage::StackTracer tracer(context);
			WriteStackTrace(writer, tracer, symResol);
		}
		writer.WriteLine();

/*		if (exAction == EA_RESTART)
		{
			Sync::SimpleThread::Sleep(2000);
			Manage::Process proc(GetCommandLineW());
			exAction = EA_CLOSE;
		}*/

		WriteContext(writer, fs, context, symResol);
	}

	{
		Data::ArrayList<Manage::ThreadInfo*> threadList;
		Manage::ThreadInfo *thread;
		NotNullPtr<Manage::ThreadContext> tCont;
		proc.GetThreads(threadList);
		i = 0;
		j = threadList.GetCount();
		while (i < j)
		{
			thread = threadList.GetItem(i);

			if (!thread->IsCurrThread())
			{
				sb.ClearStr();
				writer.WriteLine();
				sb.AppendC(UTF8STRC("Running threads: (0x"));
				sb.AppendHex32((UInt32)thread->GetThreadId());
				sb.AppendC(UTF8STRC(")"));
				writer.WriteLineC(sb.ToString(), sb.GetLength());

				if (tCont.Set(thread->GetThreadContext()))
				{
					WriteContext(writer, fs, tCont, symResol);
					tCont.Delete();
				}
			}
			DEL_CLASS(thread);
			i++;
		}
	}

	return true;
#else
	return false;
#endif
}
