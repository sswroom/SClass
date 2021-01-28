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

OSInt Manage::ThreadContextX86_64::GetRegisterCnt()
{
	return 0;
}

UTF8Char *Manage::ThreadContextX86_64::GetRegister(OSInt index, UTF8Char *buff, UInt8 *regVal, Int32 *regBitCount)
{
	return 0;
}

void Manage::ThreadContextX86_64::ToString(Text::StringBuilderUTF *sb)
{
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	UInt8 regBuff[16];
	Int32 bitCnt;
	OSInt i = 0;
	OSInt j = this->GetRegisterCnt();
	OSInt k;

	while (i < j)
	{
		if ((sptr = this->GetRegister(i, sbuff, regBuff, &bitCnt)) != 0)
		{
			sptr = Text::StrConcat(sptr, (const UTF8Char*)" = ");
			k = bitCnt >> 3;
			while (k-- > 0)
			{
				sptr = Text::StrHexByte(sptr, regBuff[k]);
			}
			sptr = Text::StrConcat(sptr, (const UTF8Char*)"\r\n");
			sb->Append(sbuff);
		}

		i++;
	}
}

Manage::ThreadContext::ContextType Manage::ThreadContextX86_64::GetType()
{
	return Manage::ThreadContext::CT_X86_64;
}

UOSInt Manage::ThreadContextX86_64::GetThreadId()
{
	return this->threadId;
}

UOSInt Manage::ThreadContextX86_64::GetProcessId()
{
	return this->procId;
}

OSInt Manage::ThreadContextX86_64::GetInstAddr()
{
	return 0;
}

OSInt Manage::ThreadContextX86_64::GetStackAddr()
{
	return 0;
}

OSInt Manage::ThreadContextX86_64::GetFrameAddr()
{
	return 0;
}

void Manage::ThreadContextX86_64::SetInstAddr(OSInt instAddr)
{
}

void Manage::ThreadContextX86_64::SetStackAddr(OSInt stackAddr)
{
}

void Manage::ThreadContextX86_64::SetFrameAddr(OSInt frameAddr)
{
}

Manage::ThreadContext *Manage::ThreadContextX86_64::Clone()
{
	Manage::ThreadContextX86_64 *cont;
	NEW_CLASS(cont, Manage::ThreadContextX86_64(this->procId, this->threadId, this->context));
	return cont;
}

Bool Manage::ThreadContextX86_64::GetRegs(Manage::Dasm::Dasm_Regs *regs)
{
	return false;
}

Manage::Dasm *Manage::ThreadContextX86_64::CreateDasm()
{
	return 0;
}

void *Manage::ThreadContextX86_64::GetContext()
{
	return this->context;
}
#endif