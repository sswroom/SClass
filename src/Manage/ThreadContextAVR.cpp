#include "Stdafx.h"
#include "MyMemory.h"
#include "Manage/ThreadContextAVR.h"
#include "Text/MyString.h"

Manage::ThreadContextAVR::ThreadContextAVR(UOSInt procId, UOSInt threadId, void *context)
{
	this->procId = procId;
	this->threadId = threadId;
	this->context = 0;
}

Manage::ThreadContextAVR::~ThreadContextAVR()
{
}

UOSInt Manage::ThreadContextAVR::GetRegisterCnt()
{
	return 0;
}

UTF8Char *Manage::ThreadContextAVR::GetRegister(UOSInt index, UTF8Char *buff, UInt8 *regVal, UInt32 *regBitCount)
{
	return 0;
}

void Manage::ThreadContextAVR::ToString(Text::StringBuilderUTF8 *sb)
{
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	UInt8 regBuff[16];
	UInt32 bitCnt;
	UOSInt i = 0;
	UOSInt j = this->GetRegisterCnt();
	UOSInt k;

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
	return Manage::ThreadContext::CT_AVR;
}

UOSInt Manage::ThreadContextAVR::GetThreadId()
{
	return this->threadId;
}

UOSInt Manage::ThreadContextAVR::GetProcessId()
{
	return this->procId;
}

void *Manage::ThreadContextAVR::GetContext()
{
	return this->context;
}

UOSInt Manage::ThreadContextAVR::GetInstAddr()
{
	return 0;
}

UOSInt Manage::ThreadContextAVR::GetStackAddr()
{
	return 0;
}

UOSInt Manage::ThreadContextAVR::GetFrameAddr()
{
	return 0;
}

void Manage::ThreadContextAVR::SetInstAddr(UOSInt instAddr)
{
}

void Manage::ThreadContextAVR::SetStackAddr(UOSInt stackAddr)
{
}

void Manage::ThreadContextAVR::SetFrameAddr(UOSInt frameAddr)
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

