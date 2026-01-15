#ifndef _SM_MANAGE_THREADCONTEXT
#define _SM_MANAGE_THREADCONTEXT
#include "Manage/DasmBase.h"
#include "Text/StringBuilderUTF8.h"

namespace Manage
{
	class ThreadContext
	{
	public:
		enum class ContextType
		{
			Unknown,
			X86_32,
			X86_64,
			ARM,
			MIPS,
			ARM64,
			MIPS64,
			AVR,
			ARM64EC
		};

		virtual ~ThreadContext() {};

		virtual UIntOS GetRegisterCnt() const = 0;
		virtual UnsafeArrayOpt<UTF8Char> GetRegister(UIntOS index, UnsafeArray<UTF8Char> buff, UnsafeArray<UInt8> regVal, OutParam<UInt32> regBitCount) const = 0;
		virtual void ToString(NN<Text::StringBuilderUTF8> sb) const = 0;
		virtual ContextType GetType() const = 0;
		virtual UIntOS GetThreadId() const = 0;
		virtual UIntOS GetProcessId() const = 0;
		virtual UIntOS GetInstAddr() const = 0;
		virtual UIntOS GetStackAddr() const = 0;
		virtual UIntOS GetFrameAddr() const = 0;
		virtual void SetInstAddr(UIntOS instAddr) = 0;
		virtual void SetStackAddr(UIntOS stackAddr) = 0;
		virtual void SetFrameAddr(UIntOS frameAddr) = 0;
		virtual NN<ThreadContext> Clone() const = 0;
		virtual Bool GetRegs(NN<Manage::Dasm::Dasm_Regs> regs) const = 0;

		virtual Optional<Manage::Dasm> CreateDasm() const = 0;

		static Text::CStringNN ContextTypeGetName(ContextType contextType);
	};
}
#endif
