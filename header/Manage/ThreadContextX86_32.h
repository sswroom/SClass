#ifndef _SM_MANAGE_THREADCONTEXTX86_32
#define _SM_MANAGE_THREADCONTEXTX86_32
#include "Manage/ThreadContext.h"
#include "Text/StringBuilderUTF8.h"

namespace Manage
{
	class ThreadContextX86_32 : public ThreadContext
	{
	private:
		void *context;
		UIntOS procId;
		UIntOS threadId;
		IntOS mmOfst;
		IntOS xmmOfst;
	public:
		ThreadContextX86_32(UIntOS procId, UIntOS threadId, void *context);
		virtual ~ThreadContextX86_32();

		virtual UIntOS GetRegisterCnt() const;
		virtual UnsafeArrayOpt<UTF8Char> GetRegister(UIntOS index, UnsafeArray<UTF8Char> buff, UnsafeArray<UInt8> regVal, OutParam<UInt32> regBitCount) const;
		virtual void ToString(NN<Text::StringBuilderUTF8> sb) const;
		virtual ContextType GetType() const;
		virtual UIntOS GetThreadId() const;
		virtual UIntOS GetProcessId() const;
		virtual UIntOS GetInstAddr() const;
		virtual UIntOS GetStackAddr() const;
		virtual UIntOS GetFrameAddr() const;
		virtual void SetInstAddr(UIntOS instAddr);
		virtual void SetStackAddr(UIntOS stackAddr);
		virtual void SetFrameAddr(UIntOS frameAddr);
		virtual NN<ThreadContext> Clone() const;
		virtual Bool GetRegs(NN<Manage::Dasm::Dasm_Regs> regs) const;
		virtual Optional<Manage::Dasm> CreateDasm() const;
		void *GetContext() const;

		UInt32 GetEAX() const;
		UInt32 GetEDX() const;
		UInt32 GetECX() const;
		UInt32 GetEBX() const;
		UInt32 GetESI() const;
		UInt32 GetEDI() const;
		UInt32 GetESP() const;
		UInt32 GetEBP() const;
		UInt32 GetEIP() const;
		UInt32 GetEFLAGS() const;
		UInt16 GetCS() const;
		UInt16 GetSS() const;
		UInt16 GetDS() const;
		UInt16 GetES() const;
		UInt16 GetFS() const;
		UInt16 GetGS() const;
		UInt32 GetDR0() const;
		UInt32 GetDR1() const;
		UInt32 GetDR2() const;
		UInt32 GetDR3() const;
		UInt32 GetDR6() const;
		UInt32 GetDR7() const;
		void GetST0(UnsafeArray<UInt8> val) const;
		void GetST1(UnsafeArray<UInt8>val) const;
		void GetST2(UnsafeArray<UInt8>val) const;
		void GetST3(UnsafeArray<UInt8>val) const;
		void GetST4(UnsafeArray<UInt8>val) const;
		void GetST5(UnsafeArray<UInt8>val) const;
		void GetST6(UnsafeArray<UInt8>val) const;
		void GetST7(UnsafeArray<UInt8>val) const;
		UInt16 GetCTRL() const;
		UInt16 GetSTAT() const;
		UInt16 GetTAG() const;
	};
}
#endif
