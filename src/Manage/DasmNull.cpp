#include "Stdafx.h"
#include "MyMemory.h"
#include "Manage/DasmNull.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

Manage::DasmNull::DasmNull()
{
}

Manage::DasmNull::~DasmNull()
{
}

Text::CStringNN Manage::DasmNull::GetHeader(Bool fullRegs)
{
	return CSTR("Code");
}

Bool Manage::DasmNull::Disasm32(NN<IO::Writer> writer, NN<Manage::AddressResolver> addrResol, InOutParam<UInt32> currInst, InOutParam<UInt32> currStack, InOutParam<UInt32> currFrame, NN<Data::ArrayListUInt32> callAddrs, NN<Data::ArrayListUInt32> jmpAddrs, OutParam<UInt32> blockStart, OutParam<UInt32> blockEnd, NN<Manage::Dasm::Dasm_Regs> regs, NN<Manage::MemoryReader> memReader, Bool fullRegs)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("Unknown opcode "));
	sb.AppendHexBuff((const UInt8*)(IntOS)currInst.Get(), 16, ' ', Text::LineBreakType::None);
	sb.AppendC(UTF8STRC("\r\n"));
	writer->Write(sb.ToCString());
	return false;
}

NN<Manage::Dasm::Dasm_Regs> Manage::DasmNull::CreateRegs()
{
	return MemAllocNN(Dasm_Regs);
}

void Manage::DasmNull::FreeRegs(NN<Dasm_Regs> regs)
{
	MemFreeNN(regs);
}
