#ifndef _SM_MANAGE_DASM
#define _SM_MANAGE_DASM
#include "Data/ArrayListUInt32.h"
#include "Data/ArrayListUInt64.h"
#include "IO/Writer.h"
#include "Manage/MemoryReader.h"
#include "Text/CString.h"

namespace Manage
{
	class AddressResolver;
	class ThreadContext;

	class Dasm
	{
	public:
		typedef enum
		{
			RBD_16,
			RBD_32,
			RBD_64
		} RegBitDepth;
		
		enum class EndType
		{
			NotEnd,
			FuncRet,
			Jmp,
			InvalidOp,
			Exit
		};

		struct Dasm_Regs
		{
		};

	public:
		virtual ~Dasm();

		virtual RegBitDepth GetRegBitDepth() const = 0;
		virtual Text::CStringNN GetHeader(Bool fullRegs) const = 0;
		virtual NN<Dasm_Regs> CreateRegs() const = 0;
		virtual void FreeRegs(NN<Dasm_Regs> regs) const = 0;
	};

	class Dasm16 : public Dasm
	{
	public:
		virtual RegBitDepth GetRegBitDepth() const;
		virtual Bool Disasm16(NN<IO::Writer> writer, Optional<Manage::AddressResolver> addrResol, InOutParam<UInt16> currInst, InOutParam<UInt16> currStack, InOutParam<UInt16> currFrame, NN<Data::ArrayListUInt32> callAddrs, NN<Data::ArrayListUInt32> jmpAddrs, OutParam<UInt16> blockStart, OutParam<UInt16> blockEnd, NN<Manage::Dasm::Dasm_Regs> regs, NN<Manage::MemoryReader> memReader, Bool fullRegs) = 0; // true = succ
	};

	class Dasm32 : public Dasm
	{
	public:
		virtual RegBitDepth GetRegBitDepth() const;
		virtual Bool Disasm32(NN<IO::Writer> writer, Optional<Manage::AddressResolver> addrResol, InOutParam<UInt32> currInst, InOutParam<UInt32> currStack, InOutParam<UInt32> currFrame, NN<Data::ArrayListUInt32> callAddrs, NN<Data::ArrayListUInt32> jmpAddrs, OutParam<UInt32> blockStart, OutParam<UInt32> blockEnd, NN<Manage::Dasm::Dasm_Regs> regs, NN<Manage::MemoryReader> memReader, Bool fullRegs) = 0; // true = succ
	};

	class Dasm64 : public Dasm
	{
	public:
		virtual RegBitDepth GetRegBitDepth() const;
		virtual Bool Disasm64(NN<IO::Writer> writer, Optional<Manage::AddressResolver> addrResol, InOutParam<UInt64> currInst, InOutParam<UInt64> currStack, InOutParam<UInt64> currFrame, NN<Data::ArrayListUInt64> callAddrs, NN<Data::ArrayListUInt64> jmpAddrs, OutParam<UInt64> blockStart, OutParam<UInt64> blockEnd, NN<Manage::Dasm::Dasm_Regs> regs, NN<Manage::MemoryReader> memReader, Bool fullRegs) = 0; // true = succ
	};
}

#endif
