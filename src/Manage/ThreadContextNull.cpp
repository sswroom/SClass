#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"

#if defined(CPU_X86_64)
#include "Manage/ThreadContextX86_64.h"
Manage::ThreadContextX86_64::ThreadContextX86_64(UOSInt procId, UOSInt threadId, void *context)
{
	this->procId = procId;
	this->threadId = threadId;
	this->context = context;
}

Manage::ThreadContextX86_64::~ThreadContextX86_64()
{
}

UOSInt Manage::ThreadContextX86_64::GetRegisterCnt() const
{
	return 0;
}

UnsafeArrayOpt<UTF8Char> Manage::ThreadContextX86_64::GetRegister(UOSInt index, UnsafeArray<UTF8Char> buff, UnsafeArray<UInt8> regVal, OutParam<UInt32> regBitCount) const
{
	return 0;
}

void Manage::ThreadContextX86_64::ToString(NN<Text::StringBuilderUTF8> sb) const
{
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	UInt8 regBuff[16];
	UInt32 bitCnt;
	UOSInt i = 0;
	UOSInt j = this->GetRegisterCnt();
	UOSInt k;

	while (i < j)
	{
		if (this->GetRegister(i, sbuff, regBuff, OutParam<UInt32>(bitCnt)).SetTo(sptr))
		{
			sptr = Text::StrConcatC(sptr, UTF8STRC(" = "));
			k = bitCnt >> 3;
			while (k-- > 0)
			{
				sptr = Text::StrHexByte(sptr, regBuff[k]);
			}
			sptr = Text::StrConcatC(sptr, UTF8STRC("\r\n"));
			sb->AppendP(sbuff, sptr);
		}

		i++;
	}
}

Manage::ThreadContext::ContextType Manage::ThreadContextX86_64::GetType() const
{
	return Manage::ThreadContext::ContextType::X86_64;
}

UOSInt Manage::ThreadContextX86_64::GetThreadId() const
{
	return this->threadId;
}

UOSInt Manage::ThreadContextX86_64::GetProcessId() const
{
	return this->procId;
}

UOSInt Manage::ThreadContextX86_64::GetInstAddr() const
{
	return 0;
}

UOSInt Manage::ThreadContextX86_64::GetStackAddr() const
{
	return 0;
}

UOSInt Manage::ThreadContextX86_64::GetFrameAddr() const
{
	return 0;
}

void Manage::ThreadContextX86_64::SetInstAddr(UOSInt instAddr)
{
}

void Manage::ThreadContextX86_64::SetStackAddr(UOSInt stackAddr)
{
}

void Manage::ThreadContextX86_64::SetFrameAddr(UOSInt frameAddr)
{
}

NN<Manage::ThreadContext> Manage::ThreadContextX86_64::Clone() const
{
	NN<Manage::ThreadContextX86_64> cont;
	NEW_CLASSNN(cont, Manage::ThreadContextX86_64(this->procId, this->threadId, this->context));
	return cont;
}

Bool Manage::ThreadContextX86_64::GetRegs(NN<Manage::Dasm::Dasm_Regs> regs) const
{
	return false;
}

NN<Manage::Dasm> Manage::ThreadContextX86_64::CreateDasm() const
{
	return 0;
}

void *Manage::ThreadContextX86_64::GetContext() const
{
	return this->context;
}
#endif