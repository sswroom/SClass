#ifndef _SM_MANAGE_THREADCONTEXT
#define _SM_MANAGE_THREADCONTEXT
#include "Manage/DasmBase.h"
#include "Text/StringBuilderUTF.h"

namespace Manage
{
	class ThreadContext
	{
	public:
		typedef enum
		{
			CT_X86_32,
			CT_X86_64,
			CT_ARM,
			CT_MIPS,
			CT_ARM64,
			CT_MIPS64,
			CT_AVR
		} ContextType;

		virtual ~ThreadContext() {};

		virtual OSInt GetRegisterCnt() = 0;
		virtual UTF8Char *GetRegister(OSInt index, UTF8Char *buff, UInt8 *regVal, Int32 *regBitCount) = 0;
		virtual void ToString(Text::StringBuilderUTF *sb) = 0;
		virtual ContextType GetType() = 0;
		virtual UOSInt GetThreadId() = 0;
		virtual UOSInt GetProcessId() = 0;
		virtual OSInt GetInstAddr() = 0;
		virtual OSInt GetStackAddr() = 0;
		virtual OSInt GetFrameAddr() = 0;
		virtual void SetInstAddr(OSInt instAddr) = 0;
		virtual void SetStackAddr(OSInt stackAddr) = 0;
		virtual void SetFrameAddr(OSInt frameAddr) = 0;
		virtual ThreadContext *Clone() = 0;
		virtual Bool GetRegs(Manage::Dasm::Dasm_Regs *regs) = 0;

		virtual Manage::Dasm *CreateDasm() = 0;
	};
}
#endif
