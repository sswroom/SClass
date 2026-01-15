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

UIntOS Manage::ThreadContextAVR::GetRegisterCnt()
{
	return 0;
}

UTF8Char *Manage::ThreadContextAVR::GetRegister(UIntOS index, UTF8Char *buff, UInt8 *regVal, UInt32 *regBitCount)
{
	return 0;
}

void Manage::ThreadContextAVR::ToString(NN<Text::StringBuilderUTF8> sb)
{
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	UInt8 regBuff[16];
	UInt32 bitCnt;
	UIntOS i = 0;
	UIntOS j = this->GetRegisterCnt();
	UIntOS k;

	while (i < j)
	{
		if ((sptr = this->GetRegister(i, sbuff, regBuff, &bitCnt)) != 0)
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

Manage::ThreadContext::ContextType Manage::ThreadContextAVR::GetType()
{
	return Manage::ThreadContext::ContextType::AVR;
}

UIntOS Manage::ThreadContextAVR::GetThreadId()
{
	return this->threadId;
}

UIntOS Manage::ThreadContextAVR::GetProcessId()
{
	return this->procId;
}

void *Manage::ThreadContextAVR::GetContext()
{
	return this->context;
}

UIntOS Manage::ThreadContextAVR::GetInstAddr()
{
	return 0;
}

UIntOS Manage::ThreadContextAVR::GetStackAddr()
{
	return 0;
}

UIntOS Manage::ThreadContextAVR::GetFrameAddr()
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

Manage::ThreadContext *Manage::ThreadContextAVR::Clone()
{
	Manage::ThreadContextAVR *cont;
	NEW_CLASS(cont, Manage::ThreadContextAVR(this->procId, this->threadId, this->context));
	return cont;
}

Bool Manage::ThreadContextAVR::GetRegs(Manage::Dasm::Dasm_Regs *regs)
{
	return false;
}

Manage::Dasm *Manage::ThreadContextAVR::CreateDasm()
{
	return 0;
}

