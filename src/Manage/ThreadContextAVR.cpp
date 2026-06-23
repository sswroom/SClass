#include "Stdafx.h"
#include "MyMemory.h"
#include "Manage/ThreadContextAVR.h"
#include "Text/MyString.h"

Manage::ThreadContextAVR::ThreadContextAVR(UIntOS procId, UIntOS threadId, void *context)
{
	this->procId = procId;
	this->threadId = threadId;
	this->context = 0;
}

Manage::ThreadContextAVR::~ThreadContextAVR()
{
}

UIntOS Manage::ThreadContextAVR::GetRegisterCnt() const
{
	return 0;
}

UnsafeArrayOpt<UTF8Char> Manage::ThreadContextAVR::GetRegister(UIntOS index, UnsafeArray<UTF8Char> buff, UnsafeArray<UInt8> regVal, OutParam<UInt32> regBitCount) const
{
	return nullptr;
}

void Manage::ThreadContextAVR::ToString(NN<Text::StringBuilderUTF8> sb) const
{
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	UInt8 regBuff[16];
	UInt32 bitCnt;
	UIntOS i = 0;
	UIntOS j = this->GetRegisterCnt();
	UIntOS k;

	while (i < j)
	{
		if (this->GetRegister(i, sbuff, regBuff, bitCnt).SetTo(sptr))
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

Manage::ThreadContext::ContextType Manage::ThreadContextAVR::GetType() const
{
	return Manage::ThreadContext::ContextType::AVR;
}

UIntOS Manage::ThreadContextAVR::GetThreadId() const
{
	return this->threadId;
}

UIntOS Manage::ThreadContextAVR::GetProcessId() const
{
	return this->procId;
}

void *Manage::ThreadContextAVR::GetContext() const
{
	return this->context;
}

UIntOS Manage::ThreadContextAVR::GetInstAddr() const
{
	return 0;
}

UIntOS Manage::ThreadContextAVR::GetStackAddr() const
{
	return 0;
}

UIntOS Manage::ThreadContextAVR::GetFrameAddr() const
{
	return 0;
}

void Manage::ThreadContextAVR::SetInstAddr(UIntOS instAddr)
{
}

void Manage::ThreadContextAVR::SetStackAddr(UIntOS stackAddr)
{
}

void Manage::ThreadContextAVR::SetFrameAddr(UIntOS frameAddr)
{
}

NN<Manage::ThreadContext> Manage::ThreadContextAVR::Clone() const
{
	NN<Manage::ThreadContextAVR> cont;
	NEW_CLASSNN(cont, Manage::ThreadContextAVR(this->procId, this->threadId, this->context));
	return cont;
}

Bool Manage::ThreadContextAVR::GetRegs(NN<Manage::Dasm::Dasm_Regs> regs) const
{
	return false;
}

Optional<Manage::Dasm> Manage::ThreadContextAVR::CreateDasm() const
{
	return nullptr;
}

