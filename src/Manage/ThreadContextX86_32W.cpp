#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Manage/DasmX86_32.h"
#include "Manage/ThreadContextX86_32.h"
#include <windows.h>

#if defined(CONTEXT_TYPE)
#undef CONTEXT_TYPE
#endif

#if defined(_WIN32_WCE) && !defined(_X86_)

UInt32 Manage::ThreadContextX86_32::GetEAX()
{
	return 0;
}

UInt32 Manage::ThreadContextX86_32::GetEDX()
{
	return 0;
}

UInt32 Manage::ThreadContextX86_32::GetECX()
{
	return 0;
}

UInt32 Manage::ThreadContextX86_32::GetEBX()
{
	return 0;
}

UInt32 Manage::ThreadContextX86_32::GetESI()
{
	return 0;
}

UInt32 Manage::ThreadContextX86_32::GetEDI()
{
	return 0;
}

#else
#if defined(_WIN64) // && MSC_VER > 0x1400
#define CONTEXT_TYPE WOW64_CONTEXT
#else
#ifdef _WIN64
#define CONTEXT_VX86 1
#endif
#define CONTEXT_TYPE CONTEXT
#endif


Manage::ThreadContextX86_32::ThreadContextX86_32(UOSInt procId, UOSInt threadId, void *context)
{
	this->procId = procId;
	this->threadId = threadId;
	this->context = MemAlloc(CONTEXT_TYPE, 1);
	MemCopyNO(this->context, context, sizeof(CONTEXT_TYPE));
	mmOfst = 0x20;
	xmmOfst = 0xa0;
}

Manage::ThreadContextX86_32::~ThreadContextX86_32()
{
	MemFree(this->context);
}

UOSInt Manage::ThreadContextX86_32::GetRegisterCnt()
{
	UOSInt cnt = 33;
	if (this->mmOfst >= 0)
	{
		cnt += 8;
		if (this->xmmOfst >= 0)
		{
			cnt += 8;
		}
	}
	return cnt;
}

UTF8Char *Manage::ThreadContextX86_32::GetRegister(UOSInt index, UTF8Char *buff, UInt8 *regVal, UInt32 *regBitCount)
{
	switch (index)
	{
	case 0:
#ifdef CONTEXT_VX86
		*(UInt32*)regVal = (UInt32)((CONTEXT_TYPE*)this->context)->Rax;
#else
		*(UInt32*)regVal = ((CONTEXT_TYPE*)this->context)->Eax;
#endif
		*regBitCount = 32;
		return Text::StrConcat(buff, (const UTF8Char*)"EAX");
	case 1:
#ifdef CONTEXT_VX86
		*(UInt32*)regVal = (UInt32)((CONTEXT_TYPE*)this->context)->Rdx;
#else
		*(UInt32*)regVal = ((CONTEXT_TYPE*)this->context)->Edx;
#endif
		*regBitCount = 32;
		return Text::StrConcat(buff, (const UTF8Char*)"EDX");
	case 2:
#ifdef CONTEXT_VX86
		*(UInt32*)regVal = (UInt32)((CONTEXT_TYPE*)this->context)->Rcx;
#else
		*(UInt32*)regVal = ((CONTEXT_TYPE*)this->context)->Ecx;
#endif
		*regBitCount = 32;
		return Text::StrConcat(buff, (const UTF8Char*)"ECX");
	case 3:
#ifdef CONTEXT_VX86
		*(UInt32*)regVal = (UInt32)((CONTEXT_TYPE*)this->context)->Rbx;
#else
		*(UInt32*)regVal = ((CONTEXT_TYPE*)this->context)->Ebx;
#endif
		*regBitCount = 32;
		return Text::StrConcat(buff, (const UTF8Char*)"EBX");
	case 4:
#ifdef CONTEXT_VX86
		*(UInt32*)regVal = (UInt32)((CONTEXT_TYPE*)this->context)->Rsi;
#else
		*(UInt32*)regVal = ((CONTEXT_TYPE*)this->context)->Esi;
#endif
		*regBitCount = 32;
		return Text::StrConcat(buff, (const UTF8Char*)"ESI");
	case 5:
#ifdef CONTEXT_VX86
		*(UInt32*)regVal = (UInt32)((CONTEXT_TYPE*)this->context)->Rdi;
#else
		*(UInt32*)regVal = ((CONTEXT_TYPE*)this->context)->Edi;
#endif
		*regBitCount = 32;
		return Text::StrConcat(buff, (const UTF8Char*)"EDI");
	case 6:
#ifdef CONTEXT_VX86
		*(UInt32*)regVal = (UInt32)((CONTEXT_TYPE*)this->context)->Rsp;
#else
		*(UInt32*)regVal = ((CONTEXT_TYPE*)this->context)->Esp;
#endif
		*regBitCount = 32;
		return Text::StrConcat(buff, (const UTF8Char*)"ESP");
	case 7:
#ifdef CONTEXT_VX86
		*(UInt32*)regVal = (UInt32)((CONTEXT_TYPE*)this->context)->Rbp;
#else
		*(UInt32*)regVal = ((CONTEXT_TYPE*)this->context)->Ebp;
#endif
		*regBitCount = 32;
		return Text::StrConcat(buff, (const UTF8Char*)"EBP");
	case 8:
#ifdef CONTEXT_VX86
		*(UInt32*)regVal = (UInt32)((CONTEXT_TYPE*)this->context)->Rip;
#else
		*(UInt32*)regVal = ((CONTEXT_TYPE*)this->context)->Eip;
#endif
		*regBitCount = 32;
		return Text::StrConcat(buff, (const UTF8Char*)"EIP");
	case 9:
		*(UInt32*)regVal = ((CONTEXT_TYPE*)this->context)->EFlags;
		*regBitCount = 32;
		return Text::StrConcat(buff, (const UTF8Char*)"EFLAGS");
	case 10:
		*(UInt16*)regVal = (UInt16)((CONTEXT_TYPE*)this->context)->SegCs;
		*regBitCount = 16;
		return Text::StrConcat(buff, (const UTF8Char*)"CS");
	case 11:
		*(UInt16*)regVal = (UInt16)((CONTEXT_TYPE*)this->context)->SegSs;
		*regBitCount = 16;
		return Text::StrConcat(buff, (const UTF8Char*)"SS");
	case 12:
		*(UInt16*)regVal = (UInt16)((CONTEXT_TYPE*)this->context)->SegDs;
		*regBitCount = 16;
		return Text::StrConcat(buff, (const UTF8Char*)"DS");
	case 13:
		*(UInt16*)regVal = (UInt16)((CONTEXT_TYPE*)this->context)->SegEs;
		*regBitCount = 16;
		return Text::StrConcat(buff, (const UTF8Char*)"ES");
	case 14:
		*(UInt16*)regVal = (UInt16)((CONTEXT_TYPE*)this->context)->SegFs;
		*regBitCount = 16;
		return Text::StrConcat(buff, (const UTF8Char*)"FS");
	case 15:
		*(UInt16*)regVal = (UInt16)((CONTEXT_TYPE*)this->context)->SegGs;
		*regBitCount = 16;
		return Text::StrConcat(buff, (const UTF8Char*)"GS");
	case 16:
		*(UInt32*)regVal = (UInt32)((CONTEXT_TYPE*)this->context)->Dr0;
		*regBitCount = 32;
		return Text::StrConcat(buff, (const UTF8Char*)"DR0");
	case 17:
		*(UInt32*)regVal = (UInt32)((CONTEXT_TYPE*)this->context)->Dr1;
		*regBitCount = 32;
		return Text::StrConcat(buff, (const UTF8Char*)"DR1");
	case 18:
		*(UInt32*)regVal = (UInt32)((CONTEXT_TYPE*)this->context)->Dr2;
		*regBitCount = 32;
		return Text::StrConcat(buff, (const UTF8Char*)"DR2");
	case 19:
		*(UInt32*)regVal = (UInt32)((CONTEXT_TYPE*)this->context)->Dr3;
		*regBitCount = 32;
		return Text::StrConcat(buff, (const UTF8Char*)"DR3");
	case 20:
		*(UInt32*)regVal = (UInt32)((CONTEXT_TYPE*)this->context)->Dr6;
		*regBitCount = 32;
		return Text::StrConcat(buff, (const UTF8Char*)"DR6");
	case 21:
		*(UInt32*)regVal = (UInt32)((CONTEXT_TYPE*)this->context)->Dr7;
		*regBitCount = 32;
		return Text::StrConcat(buff, (const UTF8Char*)"DR7");
	case 22:
		GetST0(regVal);
		*regBitCount = 80;
		return Text::StrConcat(buff, (const UTF8Char*)"ST0");
	case 23:
		GetST1(regVal);
		*regBitCount = 80;
		return Text::StrConcat(buff, (const UTF8Char*)"ST1");
	case 24:
		GetST2(regVal);
		*regBitCount = 80;
		return Text::StrConcat(buff, (const UTF8Char*)"ST2");
	case 25:
		GetST3(regVal);
		*regBitCount = 80;
		return Text::StrConcat(buff, (const UTF8Char*)"ST3");
	case 26:
		GetST4(regVal);
		*regBitCount = 80;
		return Text::StrConcat(buff, (const UTF8Char*)"ST4");
	case 27:
		GetST5(regVal);
		*regBitCount = 80;
		return Text::StrConcat(buff, (const UTF8Char*)"ST5");
	case 28:
		GetST6(regVal);
		*regBitCount = 80;
		return Text::StrConcat(buff, (const UTF8Char*)"ST6");
	case 29:
		GetST7(regVal);
		*regBitCount = 80;
		return Text::StrConcat(buff, (const UTF8Char*)"ST7");
	case 30:
#ifdef CONTEXT_VX86
		
		*(UInt16*)regVal = (UInt16)((CONTEXT_TYPE*)this->context)->FltSave.ControlWord;
#else
		*(UInt16*)regVal = (UInt16)((CONTEXT_TYPE*)this->context)->FloatSave.ControlWord;
#endif
		*regBitCount = 16;
		return Text::StrConcat(buff, (const UTF8Char*)"CTRL");
	case 31:
#ifdef CONTEXT_VX86
		*(UInt16*)regVal = (UInt16)((CONTEXT_TYPE*)this->context)->FltSave.StatusWord;
#else
		*(UInt16*)regVal = (UInt16)((CONTEXT_TYPE*)this->context)->FloatSave.StatusWord;
#endif
		*regBitCount = 16;
		return Text::StrConcat(buff, (const UTF8Char*)"STAT");
	case 32:
#ifdef CONTEXT_VX86
		*(UInt16*)regVal = (UInt16)((CONTEXT_TYPE*)this->context)->FltSave.TagWord;
#else
		*(UInt16*)regVal = (UInt16)((CONTEXT_TYPE*)this->context)->FloatSave.TagWord;
#endif
		*regBitCount = 16;
		return Text::StrConcat(buff, (const UTF8Char*)"TAG");
	case 33:
		if (this->mmOfst < 0)
			return 0;
#ifdef CONTEXT_VX86
		*(Int64*)regVal = *(Int64*)&((CONTEXT_TYPE*)this->context)->Legacy[0];
#else
		*(Int64*)regVal = *(Int64*)&((CONTEXT_TYPE*)this->context)->ExtendedRegisters[this->mmOfst];
#endif
		*regBitCount = 64;
		return Text::StrConcat(buff, (const UTF8Char*)"MM0");
	case 34:
		if (this->mmOfst < 0)
			return 0;
#ifdef CONTEXT_VX86
		*(Int64*)regVal = *(Int64*)&((CONTEXT_TYPE*)this->context)->Legacy[1];
#else
		*(Int64*)regVal = *(Int64*)&((CONTEXT_TYPE*)this->context)->ExtendedRegisters[this->mmOfst + 16];
#endif
		*regBitCount = 64;
		return Text::StrConcat(buff, (const UTF8Char*)"MM1");
	case 35:
		if (this->mmOfst < 0)
			return 0;
#ifdef CONTEXT_VX86
		*(Int64*)regVal = *(Int64*)&((CONTEXT_TYPE*)this->context)->Legacy[2];
#else
		*(Int64*)regVal = *(Int64*)&((CONTEXT_TYPE*)this->context)->ExtendedRegisters[this->mmOfst + 32];
#endif
		*regBitCount = 64;
		return Text::StrConcat(buff, (const UTF8Char*)"MM2");
	case 36:
		if (this->mmOfst < 0)
			return 0;
#ifdef CONTEXT_VX86
		*(Int64*)regVal = *(Int64*)&((CONTEXT_TYPE*)this->context)->Legacy[3];
#else
		*(Int64*)regVal = *(Int64*)&((CONTEXT_TYPE*)this->context)->ExtendedRegisters[this->mmOfst + 48];
#endif
		*regBitCount = 64;
		return Text::StrConcat(buff, (const UTF8Char*)"MM3");
	case 37:
		if (this->mmOfst < 0)
			return 0;
#ifdef CONTEXT_VX86
		*(Int64*)regVal = *(Int64*)&((CONTEXT_TYPE*)this->context)->Legacy[4];
#else
		*(Int64*)regVal = *(Int64*)&((CONTEXT_TYPE*)this->context)->ExtendedRegisters[this->mmOfst + 64];
#endif
		*regBitCount = 64;
		return Text::StrConcat(buff, (const UTF8Char*)"MM4");
	case 38:
		if (this->mmOfst < 0)
			return 0;
#ifdef CONTEXT_VX86
		*(Int64*)regVal = *(Int64*)&((CONTEXT_TYPE*)this->context)->Legacy[5];
#else
		*(Int64*)regVal = *(Int64*)&((CONTEXT_TYPE*)this->context)->ExtendedRegisters[this->mmOfst + 80];
#endif
		*regBitCount = 64;
		return Text::StrConcat(buff, (const UTF8Char*)"MM5");
	case 39:
		if (this->mmOfst < 0)
			return 0;
#ifdef CONTEXT_VX86
		*(Int64*)regVal = *(Int64*)&((CONTEXT_TYPE*)this->context)->Legacy[6];
#else
		*(Int64*)regVal = *(Int64*)&((CONTEXT_TYPE*)this->context)->ExtendedRegisters[this->mmOfst + 96];
#endif
		*regBitCount = 64;
		return Text::StrConcat(buff, (const UTF8Char*)"MM6");
	case 40:
		if (this->mmOfst < 0)
			return 0;
#ifdef CONTEXT_VX86
		*(Int64*)regVal = *(Int64*)&((CONTEXT_TYPE*)this->context)->Legacy[7];
#else
		*(Int64*)regVal = *(Int64*)&((CONTEXT_TYPE*)this->context)->ExtendedRegisters[this->mmOfst + 112];
#endif
		*regBitCount = 64;
		return Text::StrConcat(buff, (const UTF8Char*)"MM7");
	case 41:
		if (this->xmmOfst < 0)
			return 0;
#ifdef CONTEXT_VX86
		*(M128A*)regVal = ((CONTEXT_TYPE*)this->context)->Xmm0;
#else
		*(Int64*)regVal = *(Int64*)&((CONTEXT_TYPE*)this->context)->ExtendedRegisters[this->xmmOfst];
		*(Int64*)&regVal[8] = *(Int64*)&((CONTEXT_TYPE*)this->context)->ExtendedRegisters[this->xmmOfst + 8];
#endif
		*regBitCount = 128;
		return Text::StrConcat(buff, (const UTF8Char*)"XMM0");
	case 42:
		if (this->xmmOfst < 0)
			return 0;
#ifdef CONTEXT_VX86
		*(M128A*)regVal = ((CONTEXT_TYPE*)this->context)->Xmm1;
#else
		*(Int64*)regVal = *(Int64*)&((CONTEXT_TYPE*)this->context)->ExtendedRegisters[this->xmmOfst + 16];
		*(Int64*)&regVal[8] = *(Int64*)&((CONTEXT_TYPE*)this->context)->ExtendedRegisters[this->xmmOfst + 24];
#endif
		*regBitCount = 128;
		return Text::StrConcat(buff, (const UTF8Char*)"XMM1");
	case 43:
		if (this->xmmOfst < 0)
			return 0;
#ifdef CONTEXT_VX86
		*(M128A*)regVal = ((CONTEXT_TYPE*)this->context)->Xmm2;
#else
		*(Int64*)regVal = *(Int64*)&((CONTEXT_TYPE*)this->context)->ExtendedRegisters[this->xmmOfst + 32];
		*(Int64*)&regVal[8] = *(Int64*)&((CONTEXT_TYPE*)this->context)->ExtendedRegisters[this->xmmOfst + 40];
#endif
		*regBitCount = 128;
		return Text::StrConcat(buff, (const UTF8Char*)"XMM2");
	case 44:
		if (this->xmmOfst < 0)
			return 0;
#ifdef CONTEXT_VX86
		*(M128A*)regVal = ((CONTEXT_TYPE*)this->context)->Xmm3;
#else
		*(Int64*)regVal = *(Int64*)&((CONTEXT_TYPE*)this->context)->ExtendedRegisters[this->xmmOfst + 48];
		*(Int64*)&regVal[8] = *(Int64*)&((CONTEXT_TYPE*)this->context)->ExtendedRegisters[this->xmmOfst + 56];
#endif
		*regBitCount = 128;
		return Text::StrConcat(buff, (const UTF8Char*)"XMM3");
	case 45:
		if (this->xmmOfst < 0)
			return 0;
#ifdef CONTEXT_VX86
		*(M128A*)regVal = ((CONTEXT_TYPE*)this->context)->Xmm4;
#else
		*(Int64*)regVal = *(Int64*)&((CONTEXT_TYPE*)this->context)->ExtendedRegisters[this->xmmOfst + 64];
		*(Int64*)&regVal[8] = *(Int64*)&((CONTEXT_TYPE*)this->context)->ExtendedRegisters[this->xmmOfst + 72];
#endif
		*regBitCount = 128;
		return Text::StrConcat(buff, (const UTF8Char*)"XMM4");
	case 46:
		if (this->xmmOfst < 0)
			return 0;
#ifdef CONTEXT_VX86
		*(M128A*)regVal = ((CONTEXT_TYPE*)this->context)->Xmm5;
#else
		*(Int64*)regVal = *(Int64*)&((CONTEXT_TYPE*)this->context)->ExtendedRegisters[this->xmmOfst + 80];
		*(Int64*)&regVal[8] = *(Int64*)&((CONTEXT_TYPE*)this->context)->ExtendedRegisters[this->xmmOfst + 88];
#endif
		*regBitCount = 128;
		return Text::StrConcat(buff, (const UTF8Char*)"XMM5");
	case 47:
		if (this->xmmOfst < 0)
			return 0;
#ifdef CONTEXT_VX86
		*(M128A*)regVal = ((CONTEXT_TYPE*)this->context)->Xmm6;
#else
		*(Int64*)regVal = *(Int64*)&((CONTEXT_TYPE*)this->context)->ExtendedRegisters[this->xmmOfst + 96];
		*(Int64*)&regVal[8] = *(Int64*)&((CONTEXT_TYPE*)this->context)->ExtendedRegisters[this->xmmOfst + 104];
#endif
		*regBitCount = 128;
		return Text::StrConcat(buff, (const UTF8Char*)"XMM6");
	case 48:
		if (this->xmmOfst < 0)
			return 0;
#ifdef CONTEXT_VX86
		*(M128A*)regVal = ((CONTEXT_TYPE*)this->context)->Xmm7;
#else
		*(Int64*)regVal = *(Int64*)&((CONTEXT_TYPE*)this->context)->ExtendedRegisters[this->xmmOfst + 112];
		*(Int64*)&regVal[8] = *(Int64*)&((CONTEXT_TYPE*)this->context)->ExtendedRegisters[this->xmmOfst + 120];
#endif
		*regBitCount = 128;
		return Text::StrConcat(buff, (const UTF8Char*)"XMM7");
	default:
		return 0;
	}
}

void Manage::ThreadContextX86_32::ToString(Text::StringBuilderUTF *sb)
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
			sptr = Text::StrConcat(sptr, (const UTF8Char*)" = ");
			k = bitCnt >> 3;
			while (k-- > 0)
			{
				sptr = Text::StrHexByte(sptr, regBuff[k]);
			}
			switch (i)
			{
			case 3:
			case 8:
			case 9:
			case 15:
			case 21:
			case 25:
			case 29:
			case 32:
			case 36:
			case 40:
			case 44:
				sptr = Text::StrConcat(sptr, (const UTF8Char*)"\r\n");
				break;
			case 48:
				break;
			default:
				sptr = Text::StrConcat(sptr, (const UTF8Char*)", ");
			}

			sb->Append(sbuff);
		}

		i++;
	}
}

Manage::ThreadContext::ContextType Manage::ThreadContextX86_32::GetType()
{
	return Manage::ThreadContext::CT_X86_32;
}

UOSInt Manage::ThreadContextX86_32::GetThreadId()
{
	return this->threadId;
}

UOSInt Manage::ThreadContextX86_32::GetProcessId()
{
	return this->procId;
}

UOSInt Manage::ThreadContextX86_32::GetInstAddr()
{
	return this->GetEIP();
}

UOSInt Manage::ThreadContextX86_32::GetStackAddr()
{
	return this->GetESP();
}

UOSInt Manage::ThreadContextX86_32::GetFrameAddr()
{
	return this->GetEBP();
}

void Manage::ThreadContextX86_32::SetInstAddr(UOSInt instAddr)
{
#ifdef CONTEXT_VX86
	((CONTEXT_TYPE*)this->context)->Rip = instAddr;
#else
	((CONTEXT_TYPE*)this->context)->Eip = (UInt32)instAddr;
#endif
}

void Manage::ThreadContextX86_32::SetStackAddr(UOSInt stackAddr)
{
#ifdef CONTEXT_VX86
	((CONTEXT_TYPE*)this->context)->Rsp = stackAddr;
#else
	((CONTEXT_TYPE*)this->context)->Esp = (UInt32)stackAddr;
#endif
}

void Manage::ThreadContextX86_32::SetFrameAddr(UOSInt frameAddr)
{
#ifdef CONTEXT_VX86
	((CONTEXT_TYPE*)this->context)->Rbp = frameAddr;
#else
	((CONTEXT_TYPE*)this->context)->Ebp = (UInt32)frameAddr;
#endif
}

Manage::ThreadContext *Manage::ThreadContextX86_32::Clone()
{
	Manage::ThreadContextX86_32 *ret;
	NEW_CLASS(ret, Manage::ThreadContextX86_32(this->procId, this->threadId, this->context));
	return ret;
}

Bool Manage::ThreadContextX86_32::GetRegs(Manage::Dasm::Dasm_Regs *regs)
{
	Manage::DasmX86_32::DasmX86_32_Regs *r = (Manage::DasmX86_32::DasmX86_32_Regs *)regs;
#ifdef CONTEXT_VX86
	r->EAX = (UInt32)((CONTEXT_TYPE*)this->context)->Rax;
	r->EDX = (UInt32)((CONTEXT_TYPE*)this->context)->Rdx;
	r->ECX = (UInt32)((CONTEXT_TYPE*)this->context)->Rcx;
	r->EBX = (UInt32)((CONTEXT_TYPE*)this->context)->Rbx;
	r->ESI = (UInt32)((CONTEXT_TYPE*)this->context)->Rsi;
	r->EDI = (UInt32)((CONTEXT_TYPE*)this->context)->Rdi;
	r->ESP = (UInt32)((CONTEXT_TYPE*)this->context)->Rsp;
	r->EBP = (UInt32)((CONTEXT_TYPE*)this->context)->Rbp;
	r->EIP = (UInt32)((CONTEXT_TYPE*)this->context)->Rip;
#else
	r->EAX = ((CONTEXT_TYPE*)this->context)->Eax;
	r->EDX = ((CONTEXT_TYPE*)this->context)->Edx;
	r->ECX = ((CONTEXT_TYPE*)this->context)->Ecx;
	r->EBX = ((CONTEXT_TYPE*)this->context)->Ebx;
	r->ESI = ((CONTEXT_TYPE*)this->context)->Esi;
	r->EDI = ((CONTEXT_TYPE*)this->context)->Edi;
	r->ESP = ((CONTEXT_TYPE*)this->context)->Esp;
	r->EBP = ((CONTEXT_TYPE*)this->context)->Ebp;
	r->EIP = ((CONTEXT_TYPE*)this->context)->Eip;
#endif
	r->EFLAGS = ((CONTEXT_TYPE*)this->context)->EFlags;
	r->CS = (UInt16)((CONTEXT_TYPE*)this->context)->SegCs;
	r->SS = (UInt16)((CONTEXT_TYPE*)this->context)->SegSs;
	r->DS = (UInt16)((CONTEXT_TYPE*)this->context)->SegDs;
	r->ES = (UInt16)((CONTEXT_TYPE*)this->context)->SegEs;
	r->FS = (UInt16)((CONTEXT_TYPE*)this->context)->SegFs;
	r->GS = (UInt16)((CONTEXT_TYPE*)this->context)->SegGs;

#ifdef CONTEXT_VX86
	r->DR0 = (UInt32)((CONTEXT_TYPE*)this->context)->Dr0;
	r->DR1 = (UInt32)((CONTEXT_TYPE*)this->context)->Dr1;
	r->DR2 = (UInt32)((CONTEXT_TYPE*)this->context)->Dr2;
	r->DR3 = (UInt32)((CONTEXT_TYPE*)this->context)->Dr3;
	r->DR6 = (UInt32)((CONTEXT_TYPE*)this->context)->Dr6;
	r->DR7 = (UInt32)((CONTEXT_TYPE*)this->context)->Dr7;
#else
	r->DR0 = ((CONTEXT_TYPE*)this->context)->Dr0;
	r->DR1 = ((CONTEXT_TYPE*)this->context)->Dr1;
	r->DR2 = ((CONTEXT_TYPE*)this->context)->Dr2;
	r->DR3 = ((CONTEXT_TYPE*)this->context)->Dr3;
	r->DR6 = ((CONTEXT_TYPE*)this->context)->Dr6;
	r->DR7 = ((CONTEXT_TYPE*)this->context)->Dr7;
#endif

#ifdef CONTEXT_VX86
	OSInt i = 8;
	while (i-- > 0)
	{
		MemCopyNO(&r->floatBuff[i * 10], &((CONTEXT_TYPE*)this->context)->FltSave.FloatRegisters[i], 10);
	}
	r->CTRL = (UInt16)((CONTEXT_TYPE*)this->context)->FltSave.ControlWord;
	r->STAT = (UInt16)((CONTEXT_TYPE*)this->context)->FltSave.StatusWord;
	r->TAG = (UInt16)((CONTEXT_TYPE*)this->context)->FltSave.TagWord;
#else
	MemCopyNO(r->floatBuff, ((CONTEXT_TYPE*)this->context)->FloatSave.RegisterArea, 80);
	r->CTRL = (UInt16)((CONTEXT_TYPE*)this->context)->FloatSave.ControlWord;
	r->STAT = (UInt16)((CONTEXT_TYPE*)this->context)->FloatSave.StatusWord;
	r->TAG = (UInt16)((CONTEXT_TYPE*)this->context)->FloatSave.TagWord;
#endif
	return true;
}

Manage::Dasm *Manage::ThreadContextX86_32::CreateDasm()
{
	Manage::DasmX86_32 *dasm;
	NEW_CLASS(dasm, Manage::DasmX86_32());
	return dasm;
}

void *Manage::ThreadContextX86_32::GetContext()
{
	return this->context;
}

UInt32 Manage::ThreadContextX86_32::GetEAX()
{
#ifdef CONTEXT_VX86
	return (UInt32)((CONTEXT_TYPE*)this->context)->Rax;
#else
	return ((CONTEXT_TYPE*)this->context)->Eax;
#endif
}

UInt32 Manage::ThreadContextX86_32::GetEDX()
{
#ifdef CONTEXT_VX86
	return (UInt32)((CONTEXT_TYPE*)this->context)->Rdx;
#else
	return ((CONTEXT_TYPE*)this->context)->Edx;
#endif
}

UInt32 Manage::ThreadContextX86_32::GetECX()
{
#ifdef CONTEXT_VX86
	return (UInt32)((CONTEXT_TYPE*)this->context)->Rcx;
#else
	return ((CONTEXT_TYPE*)this->context)->Ecx;
#endif
}

UInt32 Manage::ThreadContextX86_32::GetEBX()
{
#ifdef CONTEXT_VX86
	return (UInt32)((CONTEXT_TYPE*)this->context)->Rbx;
#else
	return ((CONTEXT_TYPE*)this->context)->Ebx;
#endif
}

UInt32 Manage::ThreadContextX86_32::GetESI()
{
#ifdef CONTEXT_VX86
	return (UInt32)((CONTEXT_TYPE*)this->context)->Rsi;
#else
	return ((CONTEXT_TYPE*)this->context)->Esi;
#endif
}

UInt32 Manage::ThreadContextX86_32::GetEDI()
{
#ifdef CONTEXT_VX86
	return (UInt32)((CONTEXT_TYPE*)this->context)->Rdi;
#else
	return ((CONTEXT_TYPE*)this->context)->Edi;
#endif
}

UInt32 Manage::ThreadContextX86_32::GetESP()
{
#ifdef CONTEXT_VX86
	return (UInt32)((CONTEXT_TYPE*)this->context)->Rsp;
#else
	return ((CONTEXT_TYPE*)this->context)->Esp;
#endif
}

UInt32 Manage::ThreadContextX86_32::GetEBP()
{
#ifdef CONTEXT_VX86
	return (UInt32)((CONTEXT_TYPE*)this->context)->Rbp;
#else
	return ((CONTEXT_TYPE*)this->context)->Ebp;
#endif
}

UInt32 Manage::ThreadContextX86_32::GetEIP()
{
#ifdef CONTEXT_VX86
	return (UInt32)((CONTEXT_TYPE*)this->context)->Rip;
#else
	return ((CONTEXT_TYPE*)this->context)->Eip;
#endif
}

UInt32 Manage::ThreadContextX86_32::GetEFLAGS()
{
	return ((CONTEXT_TYPE*)this->context)->EFlags;
}

UInt16 Manage::ThreadContextX86_32::GetCS()
{
	return (UInt16)((CONTEXT_TYPE*)this->context)->SegCs;
}

UInt16 Manage::ThreadContextX86_32::GetSS()
{
	return (UInt16)((CONTEXT_TYPE*)this->context)->SegSs;
}

UInt16 Manage::ThreadContextX86_32::GetDS()
{
	return (UInt16)((CONTEXT_TYPE*)this->context)->SegDs;
}

UInt16 Manage::ThreadContextX86_32::GetES()
{
	return (UInt16)((CONTEXT_TYPE*)this->context)->SegEs;
}

UInt16 Manage::ThreadContextX86_32::GetFS()
{
	return (UInt16)((CONTEXT_TYPE*)this->context)->SegFs;
}

UInt16 Manage::ThreadContextX86_32::GetGS()
{
	return (UInt16)((CONTEXT_TYPE*)this->context)->SegGs;
}

UInt32 Manage::ThreadContextX86_32::GetDR0()
{
#ifdef CONTEXT_VX86
	return (UInt32)((CONTEXT_TYPE*)this->context)->Dr0;
#else
	return ((CONTEXT_TYPE*)this->context)->Dr0;
#endif
}

UInt32 Manage::ThreadContextX86_32::GetDR1()
{
#ifdef CONTEXT_VX86
	return (UInt32)((CONTEXT_TYPE*)this->context)->Dr1;
#else
	return ((CONTEXT_TYPE*)this->context)->Dr1;
#endif
}

UInt32 Manage::ThreadContextX86_32::GetDR2()
{
#ifdef CONTEXT_VX86
	return (UInt32)((CONTEXT_TYPE*)this->context)->Dr2;
#else
	return ((CONTEXT_TYPE*)this->context)->Dr2;
#endif
}

UInt32 Manage::ThreadContextX86_32::GetDR3()
{
#ifdef CONTEXT_VX86
	return (UInt32)((CONTEXT_TYPE*)this->context)->Dr3;
#else
	return ((CONTEXT_TYPE*)this->context)->Dr3;
#endif
}

UInt32 Manage::ThreadContextX86_32::GetDR6()
{
#ifdef CONTEXT_VX86
	return (UInt32)((CONTEXT_TYPE*)this->context)->Dr6;
#else
	return ((CONTEXT_TYPE*)this->context)->Dr6;
#endif
}

UInt32 Manage::ThreadContextX86_32::GetDR7()
{
#ifdef CONTEXT_VX86
	return (UInt32)((CONTEXT_TYPE*)this->context)->Dr7;
#else
	return ((CONTEXT_TYPE*)this->context)->Dr7;
#endif
}

void Manage::ThreadContextX86_32::GetST0(UInt8 *val)
{
#ifdef CONTEXT_VX86
	MemCopyNO(val, &((CONTEXT_TYPE*)this->context)->FltSave.FloatRegisters[(((((CONTEXT_TYPE*)this->context)->FltSave.StatusWord >> 11) + 0) & 7)], 10);
#else
	MemCopyNO(val, &((CONTEXT_TYPE*)this->context)->FloatSave.RegisterArea[10 * (((((CONTEXT_TYPE*)this->context)->FloatSave.StatusWord >> 11) + 0) & 7)], 10);
#endif
}

void Manage::ThreadContextX86_32::GetST1(UInt8 *val)
{
#ifdef CONTEXT_VX86
	MemCopyNO(val, &((CONTEXT_TYPE*)this->context)->FltSave.FloatRegisters[(((((CONTEXT_TYPE*)this->context)->FltSave.StatusWord >> 11) + 1) & 7)], 10);
#else
	MemCopyNO(val, &((CONTEXT_TYPE*)this->context)->FloatSave.RegisterArea[10 * (((((CONTEXT_TYPE*)this->context)->FloatSave.StatusWord >> 11) + 1) & 7)], 10);
#endif
}

void Manage::ThreadContextX86_32::GetST2(UInt8 *val)
{
#ifdef CONTEXT_VX86
	MemCopyNO(val, &((CONTEXT_TYPE*)this->context)->FltSave.FloatRegisters[(((((CONTEXT_TYPE*)this->context)->FltSave.StatusWord >> 11) + 2) & 7)], 10);
#else
	MemCopyNO(val, &((CONTEXT_TYPE*)this->context)->FloatSave.RegisterArea[10 * (((((CONTEXT_TYPE*)this->context)->FloatSave.StatusWord >> 11) + 2) & 7)], 10);
#endif
}

void Manage::ThreadContextX86_32::GetST3(UInt8 *val)
{
#ifdef CONTEXT_VX86
	MemCopyNO(val, &((CONTEXT_TYPE*)this->context)->FltSave.FloatRegisters[(((((CONTEXT_TYPE*)this->context)->FltSave.StatusWord >> 11) + 3) & 7)], 10);
#else
	MemCopyNO(val, &((CONTEXT_TYPE*)this->context)->FloatSave.RegisterArea[10 * (((((CONTEXT_TYPE*)this->context)->FloatSave.StatusWord >> 11) + 3) & 7)], 10);
#endif
}

void Manage::ThreadContextX86_32::GetST4(UInt8 *val)
{
#ifdef CONTEXT_VX86
	MemCopyNO(val, &((CONTEXT_TYPE*)this->context)->FltSave.FloatRegisters[(((((CONTEXT_TYPE*)this->context)->FltSave.StatusWord >> 11) + 4) & 7)], 10);
#else
	MemCopyNO(val, &((CONTEXT_TYPE*)this->context)->FloatSave.RegisterArea[10 * (((((CONTEXT_TYPE*)this->context)->FloatSave.StatusWord >> 11) + 4) & 7)], 10);
#endif
}

void Manage::ThreadContextX86_32::GetST5(UInt8 *val)
{
#ifdef CONTEXT_VX86
	MemCopyNO(val, &((CONTEXT_TYPE*)this->context)->FltSave.FloatRegisters[(((((CONTEXT_TYPE*)this->context)->FltSave.StatusWord >> 11) + 5) & 7)], 10);
#else
	MemCopyNO(val, &((CONTEXT_TYPE*)this->context)->FloatSave.RegisterArea[10 * (((((CONTEXT_TYPE*)this->context)->FloatSave.StatusWord >> 11) + 5) & 7)], 10);
#endif
}

void Manage::ThreadContextX86_32::GetST6(UInt8 *val)
{
#ifdef CONTEXT_VX86
	MemCopyNO(val, &((CONTEXT_TYPE*)this->context)->FltSave.FloatRegisters[(((((CONTEXT_TYPE*)this->context)->FltSave.StatusWord >> 11) + 6) & 7)], 10);
#else
	MemCopyNO(val, &((CONTEXT_TYPE*)this->context)->FloatSave.RegisterArea[10 * (((((CONTEXT_TYPE*)this->context)->FloatSave.StatusWord >> 11) + 6) & 7)], 10);
#endif
}

void Manage::ThreadContextX86_32::GetST7(UInt8 *val)
{
#ifdef CONTEXT_VX86
	MemCopyNO(val, &((CONTEXT_TYPE*)this->context)->FltSave.FloatRegisters[(((((CONTEXT_TYPE*)this->context)->FltSave.StatusWord >> 11) + 7) & 7)], 10);
#else
	MemCopyNO(val, &((CONTEXT_TYPE*)this->context)->FloatSave.RegisterArea[10 * (((((CONTEXT_TYPE*)this->context)->FloatSave.StatusWord >> 11) + 7) & 7)], 10);
#endif
}

UInt16 Manage::ThreadContextX86_32::GetCTRL()
{
#ifdef CONTEXT_VX86
	return (UInt16)((CONTEXT_TYPE*)this->context)->FltSave.ControlWord;
#else
	return (UInt16)((CONTEXT_TYPE*)this->context)->FloatSave.ControlWord;
#endif
}

UInt16 Manage::ThreadContextX86_32::GetSTAT()
{
#ifdef CONTEXT_VX86
	return (UInt16)((CONTEXT_TYPE*)this->context)->FltSave.StatusWord;
#else
	return (UInt16)((CONTEXT_TYPE*)this->context)->FloatSave.StatusWord;
#endif
}

UInt16 Manage::ThreadContextX86_32::GetTAG()
{
#ifdef CONTEXT_VX86
	return (UInt16)((CONTEXT_TYPE*)this->context)->FltSave.TagWord;
#else
	return (UInt16)((CONTEXT_TYPE*)this->context)->FloatSave.TagWord;
#endif
}
#endif