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

const UTF8Char *Manage::DasmNull::GetHeader(Bool fullRegs)
{
	return (const UTF8Char*)"Code";
}

Bool Manage::DasmNull::Disasm32(IO::Writer *writer, Manage::AddressResolver *addrResol, UInt32 *currInst, UInt32 *currStack, UInt32 *currFrame, Data::ArrayListInt32 *callAddrs, Data::ArrayListInt32 *jmpAddrs, UInt32 *blockStart, UInt32 *blockEnd, Manage::Dasm::Dasm_Regs *regs, Manage::IMemoryReader *memReader, Bool fullRegs)
{
	Text::StringBuilderUTF8 sb;
	sb.Append((const UTF8Char*)"Unknown opcode ");
	sb.AppendHexBuff((const UInt8*)(OSInt)*currInst, 16, ' ', Text::LBT_NONE);
	sb.Append((const UTF8Char*)"\r\n");
	writer->Write(sb.ToString());
	return false;
}

Manage::Dasm::Dasm_Regs *Manage::DasmNull::CreateRegs()
{
	return 0;
}

void Manage::DasmNull::FreeRegs(Dasm_Regs *regs)
{
}
