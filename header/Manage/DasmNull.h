#ifndef _SM_MANAGE_DASMNULL
#define _SM_MANAGE_DASMNULL
#include "Data/ArrayListInt32.h"
#include "Manage/DasmBase.h"

namespace Manage
{
	class DasmNull : public Dasm32
	{
	public:
		enum class EndType
		{
			NotEnd,
			FuncRet,
			Jmp,
			InvalidOp,
			Exit
		};

	public:
		DasmNull();
		virtual ~DasmNull();

		virtual Text::CStringNN GetHeader(Bool fullRegs);
		virtual Bool Disasm32(NN<IO::Writer> writer, NN<Manage::AddressResolver> addrResol, InOutParam<UInt32> currInst, InOutParam<UInt32> currStack, InOutParam<UInt32> currFrame, NN<Data::ArrayListUInt32> callAddrs, NN<Data::ArrayListUInt32> jmpAddrs, OutParam<UInt32> blockStart, OutParam<UInt32> blockEnd, NN<Manage::Dasm::Dasm_Regs> regs, NN<Manage::MemoryReader> memReader, Bool fullRegs); // true = succ
		virtual NN<Dasm_Regs> CreateRegs();
		virtual void FreeRegs(NN<Dasm_Regs> regs);
	};
}
#endif
