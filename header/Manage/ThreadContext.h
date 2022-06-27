#ifndef _SM_MANAGE_THREADCONTEXT
#define _SM_MANAGE_THREADCONTEXT
#include "Manage/DasmBase.h"
#include "Text/StringBuilderUTF8.h"

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

		virtual UOSInt GetRegisterCnt() const = 0;
		virtual UTF8Char *GetRegister(UOSInt index, UTF8Char *buff, UInt8 *regVal, UInt32 *regBitCount) const = 0;
		virtual void ToString(Text::StringBuilderUTF8 *sb) const = 0;
		virtual ContextType GetType() const = 0;
		virtual UOSInt GetThreadId() const = 0;
		virtual UOSInt GetProcessId() const = 0;
		virtual UOSInt GetInstAddr() const = 0;
		virtual UOSInt GetStackAddr() const = 0;
		virtual UOSInt GetFrameAddr() const = 0;
		virtual void SetInstAddr(UOSInt instAddr) = 0;
		virtual void SetStackAddr(UOSInt stackAddr) = 0;
		virtual void SetFrameAddr(UOSInt frameAddr) = 0;
		virtual ThreadContext *Clone() const = 0;
		virtual Bool GetRegs(Manage::Dasm::Dasm_Regs *regs) const = 0;

		virtual Manage::Dasm *CreateDasm() const = 0;
	};
}
#endif
