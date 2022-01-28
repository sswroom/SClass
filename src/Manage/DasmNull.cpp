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

Text::CString Manage::DasmNull::GetHeader(Bool fullRegs)
{
	return CSTR("Code");
}

Bool Manage::DasmNull::Disasm32(IO::Writer *writer, Manage::AddressResolver *addrResol, UInt32 *currInst, UInt32 *currStack, UInt32 *currFrame, Data::ArrayListInt32 *callAddrs, Data::ArrayListInt32 *jmpAddrs, UInt32 *blockStart, UInt32 *blockEnd, Manage::Dasm::Dasm_Regs *regs, Manage::IMemoryReader *memReader, Bool fullRegs)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("Unknown opcode "));
	sb.AppendHexBuff((const UInt8*)(OSInt)*currInst, 16, ' ', Text::LineBreakType::None);
	sb.AppendC(UTF8STRC("\r\n"));
	writer->WriteStrC(sb.ToString(), sb.GetLength());
	return false;
}

Manage::Dasm::Dasm_Regs *Manage::DasmNull::CreateRegs()
{
	return 0;
}

void Manage::DasmNull::FreeRegs(Dasm_Regs *regs)
{
}
