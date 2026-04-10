#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Data/ArrayListInt32.h"
#include "Data/ArrayListInt64.h"
#include "IO/StreamWriter.h"
#include "IO/FileStream.h"
#include "Manage/DasmX86_32.h"
#include "Manage/SymbolResolver.h"
#include "Sync/ThreadUtil.h"
#include "Text/StringBuilderWriter.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	UIntOS ceip = 0;
	UIntOS cesp = 0;
	UIntOS cebp = 0;

	ceip = (UIntOS)Sync::ThreadUtil::GetCurrAddr().p;
/*#if defined(HAS_ASM32)
	_asm
	{
		mov cesp,esp
		mov cebp,ebp
	}
#elif defined(HAS_GCCASM32)
	asm("movl %esp, cesp\n\t"
		"movl %ebp, cebp");
#elif defined(HAS_GCCASM64)
	asm("movq %rsp, cesp\n\t"
		"movq %rbp, cebp");
#endif*/
	{
		Manage::Process proc;
		Manage::SymbolResolver addrResol(proc);
		IO::FileStream fs(CSTR("Except.txt"), IO::FileMode::Append, IO::FileShare::DenyAll, IO::FileStream::BufferType::Normal);
		Manage::DasmX86_32 dasm;
		Text::Encoding enc(65001);
		IO::StreamWriter writer(fs, enc);
		Text::StringBuilderUTF8 sb;
		UInt32 currEip = (UInt32)ceip;
		UInt32 currEsp = (UInt32)cesp;
		UInt32 currEbp = (UInt32)cebp;
		NN<Data::ArrayListUInt32> callAddrs;
		NN<Data::ArrayListUInt32> jmpAddrs;
		UInt32 blockStart;
		UInt32 blockEnd;
		NEW_CLASSNN(callAddrs, Data::ArrayListUInt32());
		NEW_CLASSNN(jmpAddrs, Data::ArrayListUInt32());
		Bool retVal = true;
		writer.WriteLine(CSTR("\r\nDisassembly:"));

		NN<Data::ArrayListUInt32> blkStarts;
		NN<Data::ArrayListUInt32> blkEnds;
		Text::StringBuilderWriter sbWriter(sb);
		NEW_CLASSNN(blkStarts, Data::ArrayListUInt32());
		NEW_CLASSNN(blkEnds, Data::ArrayListUInt32());

		Bool fin = false;

		sb.ClearStr();
		fs.Flush();
		NN<Manage::Dasm::Dasm_Regs> regs = dasm.CreateRegs();
		while ((retVal = dasm.Disasm32(sbWriter, addrResol, currEip, currEsp, currEbp, callAddrs, jmpAddrs, blockStart, blockEnd, regs, proc, false)) == true)
		{
			blkStarts->Add(blockStart);
			blkEnds->Add(blockEnd);

			UIntOS i = blkStarts->GetCount();
			while (i-- > 0)
			{
				if (currEip >= (UInt32)blkStarts->GetItem(i) && currEip < (UInt32)blkEnds->GetItem(i))
				{
					fin = true;
					break;
				}
			}
			if (fin)
				break;
			callAddrs->Clear();
			jmpAddrs->Clear();

			writer.WriteLine(sb.ToCString());
			writer.WriteLine(CSTR("\r\nCalled by:"));
			fs.Flush();
			sb.ClearStr();
		}
		writer.WriteLine(sb.ToCString());
		dasm.FreeRegs(regs);
		callAddrs.Delete();
		jmpAddrs.Delete();
		blkStarts.Delete();
		blkEnds.Delete();
	}
	return 0;
}
