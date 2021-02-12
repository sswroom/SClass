#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListInt64.h"
#include "IO/FileStream.h"
#include "Manage/ExceptionLogger.h"
#include "Manage/StackTracer.h"
#include "Manage/SymbolResolver.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Writer.h"

#define STACKDUMPSIZE 4096

void Manage::ExceptionLogger::WriteContext(IO::Writer *writer, IO::Stream *stm, Manage::ThreadContext *context, Manage::AddressResolver *addrResol)
{
	Text::StringBuilderUTF8 sb;
	OSInt i;

	context->ToString(&sb);
	writer->WriteLine(sb.ToString());

	Manage::Process proc;
	OSInt size;
	UInt8 buff[STACKDUMPSIZE];
	if ((size = proc.ReadMemory(context->GetInstAddr(), buff, 256)) != 0)
	{
		OSInt currAddr = context->GetInstAddr();
		UInt8 *currPtr = buff;
		writer->WriteLine();
		sb.ClearStr();
		sb.Append((const UTF8Char*)"Memory Dumps (Instructions):");
		while (size > 0)
		{
			sb.Append((const UTF8Char*)"\r\n");
			sb.AppendHexOS(currAddr);
			sb.Append((const UTF8Char*)" ");
			if (size >= 16)
			{
				sb.AppendHex(currPtr, 16, ' ', Text::LBT_NONE);
				size -= 16;
				currAddr += 16;
				currPtr += 16;
			}
			else
			{
				sb.AppendHex(currPtr, size, ' ', Text::LBT_NONE);
				currAddr += (UInt32)size;
				currPtr += size;
				size = 0;
			}
		}
		writer->WriteLine(sb.ToString());
	}
	if ((size = proc.ReadMemory(context->GetStackAddr(), buff, STACKDUMPSIZE)) != 0)
	{
		OSInt currAddr = context->GetStackAddr();
		UInt8 *currPtr = buff;
		writer->WriteLine();
		sb.ClearStr();
		sb.Append((const UTF8Char*)"Stack Dumps:");
		while (size > 0)
		{
			sb.Append((const UTF8Char*)"\r\n");
			sb.AppendHexOS(currAddr);
			sb.Append((const UTF8Char*)" ");
			if (size >= 16)
			{
				sb.AppendHex(currPtr, 16, ' ', Text::LBT_NONE);
				size -= 16;
				currAddr += 16;
				currPtr += 16;
			}
			else
			{
				sb.AppendHex(currPtr, size, ' ', Text::LBT_NONE);
				currAddr += (UInt32)size;
				currPtr += size;
				size = 0;
			}
		}
		writer->WriteLine(sb.ToString());
	}

	Manage::Dasm *dasm = context->CreateDasm();
	if (dasm)
	{
		if (dasm->GetRegBitDepth() == Manage::Dasm::RBD_32)
		{
			UTF8Char sbuff[256];
			Manage::Dasm32 *dasm32 = (Manage::Dasm32*)dasm;
			UInt32 currInst = (UInt32)context->GetInstAddr();
			UInt32 currStack = (UInt32)context->GetStackAddr();
			UInt32 currFrame = (UInt32)context->GetFrameAddr();
			Data::ArrayListInt32 *callAddrs;
			Data::ArrayListInt32 *jmpAddrs;
			UInt32 blockStart;
			UInt32 blockEnd;

			NEW_CLASS(callAddrs, Data::ArrayListInt32());
			NEW_CLASS(jmpAddrs, Data::ArrayListInt32());
			Bool retVal = true;
			writer->WriteLine();
			writer->WriteLine((const UTF8Char*)"Disassembly:");

			Data::ArrayListInt32 *blkStarts;
			Data::ArrayListInt32 *blkEnds;
			NEW_CLASS(blkStarts, Data::ArrayListInt32());
			NEW_CLASS(blkEnds, Data::ArrayListInt32());

			Bool fin = false;

			sb.ClearStr();
			sb.Append((const UTF8Char*)"(");
			if (addrResol->ResolveName(sbuff, currInst))
			{
				sb.Append(sbuff);
				sb.Append((const UTF8Char*)")");
				writer->WriteLine(sb.ToString());
			}

			stm->Flush();
			Manage::Dasm::Dasm_Regs *regs = dasm32->CreateRegs();
			context->GetRegs(regs);
			while (true)
			{
				if ((size = proc.ReadMemory(currInst, buff, 256)) != 0)
				{
					OSInt currAddr = currInst;
					UInt8 *currPtr = buff;
					sb.ClearStr();
					sb.Append((const UTF8Char*)"Memory Dumps (Instructions):");
					while (size > 0)
					{
						sb.Append((const UTF8Char*)"\r\n");
						sb.AppendHexOS(currAddr);
						sb.Append((const UTF8Char*)" ");
						if (size >= 16)
						{
							sb.AppendHex(currPtr, 16, ' ', Text::LBT_NONE);
							size -= 16;
							currAddr += 16;
							currPtr += 16;
						}
						else
						{
							sb.AppendHex(currPtr, size, ' ', Text::LBT_NONE);
							currAddr += (UInt32)size;
							currPtr += size;
							size = 0;
						}
					}
					writer->WriteLine(sb.ToString());
				}

				writer->WriteLine(dasm32->GetHeader(true));
				retVal = dasm32->Disasm32(writer, addrResol, &currInst, &currStack, &currFrame, callAddrs, jmpAddrs, &blockStart, &blockEnd, regs, &proc, true);
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

	//			writer->WriteLine(sb.ToString());
				writer->WriteLine();
				writer->WriteLine((const UTF8Char*)"\r\nCalled by:");

				sb.ClearStr();
				sb.AppendHex32(currInst);
				writer->Write(sb.ToString());

				sb.ClearStr();
				if (addrResol->ResolveName(sbuff, currInst))
				{
					sb.Append((const UTF8Char*)" (");
					sb.Append(sbuff);
					sb.Append((const UTF8Char*)")");
					writer->WriteLine(sb.ToString());
				}
				else
				{
					writer->WriteLine();
				}
				stm->Flush();
				sb.ClearStr();
			}
			dasm32->FreeRegs(regs);
			writer->WriteLine(sb.ToString());
			DEL_CLASS(callAddrs);
			DEL_CLASS(jmpAddrs);
			DEL_CLASS(blkStarts);
			DEL_CLASS(blkEnds);
		}
		else if (dasm->GetRegBitDepth() == Manage::Dasm::RBD_64)
		{
			UTF8Char sbuff[256];
			Dasm64 *dasm64 = (Dasm64*)dasm;
			UInt64 currInst = context->GetInstAddr();
			UInt64 currStack = context->GetStackAddr();
			UInt64 currFrame = context->GetFrameAddr();;
			Data::ArrayListInt64 *callAddrs;
			Data::ArrayListInt64 *jmpAddrs;
			UInt64 blockStart;
			UInt64 blockEnd;

			NEW_CLASS(callAddrs, Data::ArrayListInt64());
			NEW_CLASS(jmpAddrs, Data::ArrayListInt64());
			Bool retVal = true;
			writer->WriteLine();
			writer->WriteLine((const UTF8Char*)"Disassembly:");

			Data::ArrayListInt64 *blkStarts;
			Data::ArrayListInt64 *blkEnds;
			NEW_CLASS(blkStarts, Data::ArrayListInt64());
			NEW_CLASS(blkEnds, Data::ArrayListInt64());

			Bool fin = false;

			sb.ClearStr();
			sb.Append((const UTF8Char*)"(");
			if (addrResol->ResolveName(sbuff, currInst))
			{
				sb.Append(sbuff);
				sb.Append((const UTF8Char*)")");
				writer->WriteLine(sb.ToString());
			}

			stm->Flush();
			Manage::Dasm::Dasm_Regs *regs = dasm64->CreateRegs();
			context->GetRegs(regs);
			while (true)
			{
				if ((size = proc.ReadMemory(currInst, buff, 256)) != 0)
				{
					Int64 currAddr = currInst;
					UInt8 *currPtr = buff;
					sb.ClearStr();
					sb.Append((const UTF8Char*)"Memory Dumps (Instructions):");
					while (size > 0)
					{
						sb.Append((const UTF8Char*)"\r\n");
						sb.AppendHex64(currAddr);
						sb.Append((const UTF8Char*)" ");
						if (size >= 16)
						{
							sb.AppendHex(currPtr, 16, ' ', Text::LBT_NONE);
							size -= 16;
							currAddr += 16;
							currPtr += 16;
						}
						else
						{
							sb.AppendHex(currPtr, size, ' ', Text::LBT_NONE);
							currAddr += (UInt32)size;
							currPtr += size;
							size = 0;
						}
					}
					writer->WriteLine(sb.ToString());
				}

				writer->WriteLine(dasm64->GetHeader(true));
				retVal = dasm64->Disasm64(writer, addrResol, &currInst, &currStack, &currFrame, callAddrs, jmpAddrs, &blockStart, &blockEnd, regs, &proc, true);
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

	//			writer->WriteLine(sb.ToString());
				writer->WriteLine();
				writer->WriteLine((const UTF8Char*)"\r\nCalled by:");

				sb.ClearStr();
				sb.AppendHex64(currInst);
				writer->Write(sb.ToString());

				sb.ClearStr();
				if (addrResol->ResolveName(sbuff, currInst))
				{
					sb.Append((const UTF8Char*)" (");
					sb.Append(sbuff);
					sb.Append((const UTF8Char*)")");
					writer->WriteLine(sb.ToString());
				}
				else
				{
					writer->WriteLine();
				}
				stm->Flush();
				sb.ClearStr();
			}
			dasm64->FreeRegs(regs);
			writer->WriteLine(sb.ToString());
			DEL_CLASS(callAddrs);
			DEL_CLASS(jmpAddrs);
			DEL_CLASS(blkStarts);
			DEL_CLASS(blkEnds);
		}
		DEL_CLASS(dasm);
	}
}

void Manage::ExceptionLogger::WriteStackTrace(IO::Writer *writer, Manage::StackTracer *tracer, Manage::AddressResolver *addrResol)
{
#ifndef _WIN32_WCE
	if (tracer->IsSupported())
	{
		Text::StringBuilderUTF8 sb;
		writer->WriteLine((const UTF8Char*)"Stack trace (OS):");

		while (tracer->GoToNextLevel())
		{
			sb.ClearStr();
			sb.AppendHex64(tracer->GetCurrentAddr());
			sb.Append((const UTF8Char*)" ");
			addrResol->ResolveNameSB(&sb, tracer->GetCurrentAddr());
			writer->WriteLine(sb.ToString());
		}
	}
#endif
}

Bool Manage::ExceptionLogger::LogToFile(const UTF8Char *fileName, UInt32 exCode, const UTF8Char *exName, OSInt exAddr, Manage::ThreadContext *context)
{
#ifndef _WIN32_WCE
	Manage::SymbolResolver *symResol;
	Manage::Process proc;
	IO::FileStream fs(fileName, IO::FileStream::FILE_MODE_APPEND, IO::FileStream::FILE_SHARE_DENY_ALL, IO::FileStream::BT_NORMAL);
	Text::UTF8Writer writer(&fs);
	Text::StringBuilderUTF8 sb;
	Data::DateTime d;
	OSInt i;
	OSInt j;


	d.SetCurrTime();
	NEW_CLASS(symResol, Manage::SymbolResolver(&proc));
	writer.WriteLine((const UTF8Char*)"----------------------------------");
	sb.Append((const UTF8Char*)"Exception occurs: Code = 0x");
	sb.AppendHex32(exCode);
	sb.Append((const UTF8Char*)" at ");
	sb.AppendHexOS(exAddr);
	sb.Append((const UTF8Char*)", time = ");
	sb.AppendDate(&d);
	sb.Append((const UTF8Char*)", Type = ");
	sb.Append(exName);
	writer.WriteLine(sb.ToString());


	j = symResol->GetModuleCount();
	writer.WriteLine((const UTF8Char*)"\r\nLoaded modules:");
	i = 0;
	while (i < j)
	{
		sb.ClearStr();
		sb.Append(symResol->GetModuleName(i));
		sb.Append((const UTF8Char*)", Addr=");
		sb.AppendHexOS((OSInt)symResol->GetModuleAddr(i));
		sb.Append((const UTF8Char*)",size=");
		sb.AppendHex32((UInt32)symResol->GetModuleSize(i));
		writer.WriteLine(sb.ToString());

		i++;
	}

	{
		Data::ArrayList<Manage::ThreadInfo*> *threadList;
		Manage::ThreadInfo *thread;
		Manage::ThreadContext *tCont;
		Manage::StackTracer *tracer;
		UInt64 startAddr;
		NEW_CLASS(threadList, Data::ArrayList<Manage::ThreadInfo*>());
		proc.GetThreads(threadList);
		i = 0;
		j = threadList->GetCount();
		while (i < j)
		{
			thread = threadList->GetItem(i);

			sb.ClearStr();
			writer.WriteLine();
			sb.Append((const UTF8Char*)"Running threads: (0x");
			sb.AppendHex32((UInt32)thread->GetThreadId());
			sb.Append((const UTF8Char*)")");
			if (thread->IsCurrThread())
			{
				sb.Append((const UTF8Char*)" current thread");
			}
			writer.WriteLine(sb.ToString());

			startAddr = thread->GetStartAddress();
			if (startAddr != 0)
			{
				sb.ClearStr();
				sb.Append((const UTF8Char*)"Start address 0x");
				sb.AppendHex64(startAddr);
				sb.Append((const UTF8Char*)" ");
				symResol->ResolveNameSB(&sb, startAddr);
				writer.WriteLine(sb.ToString());
			}

			if (!thread->IsCurrThread())
			{
				thread->Suspend();
				if ((tCont = thread->GetThreadContext()) != 0)
				{
					sb.ClearStr();
					sb.Append((const UTF8Char*)"Curr address 0x");
					sb.AppendHexOS(tCont->GetInstAddr());
					sb.Append((const UTF8Char*)" ");
					symResol->ResolveNameSB(&sb, tCont->GetInstAddr());
					writer.WriteLine(sb.ToString());

					NEW_CLASS(tracer, Manage::StackTracer(tCont));
					WriteStackTrace(&writer, tracer, symResol);
					DEL_CLASS(tracer);
					DEL_CLASS(tCont);
				}
				thread->Resume();
			}
			else
			{
				writer.WriteLine((const UTF8Char*)"Current Thread");
			}

			i++;
		}
		DEL_CLASS(threadList);
	}

	{
		Manage::StackTracer *tracer;
		writer.WriteLine();
		writer.WriteLine((const UTF8Char*)"Exception Thread:");
		NEW_CLASS(tracer, Manage::StackTracer(context));
		WriteStackTrace(&writer, tracer, symResol);
		DEL_CLASS(tracer);
		writer.WriteLine();

/*		if (exAction == EA_RESTART)
		{
			Sync::Thread::Sleep(2000);
			Manage::Process proc(GetCommandLineW());
			exAction = EA_CLOSE;
		}*/

		WriteContext(&writer, &fs, context, symResol);
	}

	{
		Data::ArrayList<Manage::ThreadInfo*> *threadList;
		Manage::ThreadInfo *thread;
		Manage::ThreadContext *tCont;

		NEW_CLASS(threadList, Data::ArrayList<Manage::ThreadInfo*>());
		proc.GetThreads(threadList);
		i = 0;
		j = threadList->GetCount();
		while (i < j)
		{
			thread = threadList->GetItem(i);

			if (!thread->IsCurrThread())
			{
				sb.ClearStr();
				writer.WriteLine();
				sb.Append((const UTF8Char*)"Running threads: (0x");
				sb.AppendHex32((UInt32)thread->GetThreadId());
				sb.Append((const UTF8Char*)")");
				writer.WriteLine(sb.ToString());

				tCont = thread->GetThreadContext();
				if (tCont)
				{
					WriteContext(&writer, &fs, tCont, symResol);
					DEL_CLASS(tCont);
				}
			}
			DEL_CLASS(thread);
			i++;
		}
		DEL_CLASS(threadList);
	}

	DEL_CLASS(symResol);
	return true;
#else
	return false;
#endif
}
