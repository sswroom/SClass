#ifndef _SM_MANAGE_THREADCONTEXTX86_32
#define _SM_MANAGE_THREADCONTEXTX86_32
#include "Manage/ThreadContext.h"
#include "Text/StringBuilderUTF.h"

namespace Manage
{
	class ThreadContextX86_32 : public ThreadContext
	{
	private:
		void *context;
		UOSInt procId;
		UOSInt threadId;
		OSInt mmOfst;
		OSInt xmmOfst;
	public:
		ThreadContextX86_32(UOSInt procId, UOSInt threadId, void *context);
		virtual ~ThreadContextX86_32();

		virtual UOSInt GetRegisterCnt();
		virtual UTF8Char *GetRegister(UOSInt index, UTF8Char *buff, UInt8 *regVal, UInt32 *regBitCount);
		virtual void ToString(Text::StringBuilderUTF *sb);
		virtual ContextType GetType();
		virtual UOSInt GetThreadId();
		virtual UOSInt GetProcessId();
		virtual UOSInt GetInstAddr();
		virtual UOSInt GetStackAddr();
		virtual UOSInt GetFrameAddr();
		virtual void SetInstAddr(UOSInt instAddr);
		virtual void SetStackAddr(UOSInt stackAddr);
		virtual void SetFrameAddr(UOSInt frameAddr);
		virtual ThreadContext *Clone();
		virtual Bool GetRegs(Manage::Dasm::Dasm_Regs *regs);
		virtual Manage::Dasm *CreateDasm();
		void *GetContext();

		UInt32 GetEAX();
		UInt32 GetEDX();
		UInt32 GetECX();
		UInt32 GetEBX();
		UInt32 GetESI();
		UInt32 GetEDI();
		UInt32 GetESP();
		UInt32 GetEBP();
		UInt32 GetEIP();
		UInt32 GetEFLAGS();
		UInt16 GetCS();
		UInt16 GetSS();
		UInt16 GetDS();
		UInt16 GetES();
		UInt16 GetFS();
		UInt16 GetGS();
		UInt32 GetDR0();
		UInt32 GetDR1();
		UInt32 GetDR2();
		UInt32 GetDR3();
		UInt32 GetDR6();
		UInt32 GetDR7();
		void GetST0(UInt8 *val);
		void GetST1(UInt8 *val);
		void GetST2(UInt8 *val);
		void GetST3(UInt8 *val);
		void GetST4(UInt8 *val);
		void GetST5(UInt8 *val);
		void GetST6(UInt8 *val);
		void GetST7(UInt8 *val);
		UInt16 GetCTRL();
		UInt16 GetSTAT();
		UInt16 GetTAG();
	};
};
#endif
