#ifndef _SM_MANAGE_DASMARM64
#define _SM_MANAGE_DASMARM64
#include "Manage/DasmBase.h"

namespace Manage
{
	class DasmARM64 : public Dasm64
	{
	public:
		struct Registers : public Manage::Dasm::Dasm_Regs
		{
			union 
			{
				struct
				{
					UInt64 X0;
					UInt64 X1;
					UInt64 X2;
					UInt64 X3;
					UInt64 X4;
					UInt64 X5;
					UInt64 X6;
					UInt64 X7;
					UInt64 X8;
					UInt64 X9;
					UInt64 X10;
					UInt64 X11;
					UInt64 X12;
					UInt64 X13;
					UInt64 X14;
					UInt64 X15;
					UInt64 X16;
					UInt64 X17;
					UInt64 X18;
					UInt64 X19;
					UInt64 X20;
					UInt64 X21;
					UInt64 X22;
					UInt64 X23;
					UInt64 X24;
					UInt64 X25;
					UInt64 X26;
					UInt64 X27;
					UInt64 X28;
					UInt64 X29;
					UInt64 LR; //X30
					UInt64 SP; //X31
					UInt64 PC;
				};
				UInt64 indexes[33];
			};
		};

		struct Session;
		typedef Bool (CALLBACKFUNC DasmARM64_Code)(NN<Session> sess);

		struct Session
		{
			Registers regs;
			UnsafeArray<UInt8> code;
			UInt16 codeSegm;
			NN<Data::ArrayListUInt64> callAddrs;
			NN<Data::ArrayListUInt64> jmpAddrs;
			UnsafeArray<UTF8Char> sbuff;
			UInt64 retAddr;
			Int32 thisStatus;
			Int32 endStatus; //0 = not end, 1 = jmp out, 2 = exit program, 3 = func return
			UInt16 endIP;
			EndType endType;
			UnsafeArray<UInt8> codeBuff;
			
			Optional<Manage::AddressResolver> addrResol;
			NN<Manage::MemoryReader> memReader;

			UnsafeArray<DasmARM64_Code> codeHdlrs;
			//prefix 2 1: 00 = no, 01 = 0x66, 02 = f2, 03 = f3
		};

	private:
		UnsafeArray<DasmARM64_Code> codes;
	public:
		DasmARM64();
		virtual ~DasmARM64();

		virtual Text::CStringNN GetHeader(Bool fullRegs) const;
		virtual Bool Disasm64(NN<IO::Writer> writer, Optional<Manage::AddressResolver> addrResol, InOutParam<UInt64> currInst, InOutParam<UInt64> currStack, InOutParam<UInt64> currFrame, NN<Data::ArrayListUInt64> callAddrs, NN<Data::ArrayListUInt64> jmpAddrs, OutParam<UInt64> blockStart, OutParam<UInt64> blockEnd, NN<Manage::Dasm::Dasm_Regs> regs, NN<Manage::MemoryReader> memReader, Bool fullRegs); // true = succ
		virtual NN<Dasm_Regs> CreateRegs() const;
		virtual void FreeRegs(NN<Dasm_Regs> regs) const;

		NN<Session> CreateSess(NN<Registers> regs, UnsafeArray<UInt8> code, UInt16 codeSegm);
		void DeleteSess(NN<Session> sess);

		Bool DasmNext(NN<Session> sess, UnsafeArray<UTF8Char> buff, OutParam<IntOS> outBuffSize); //True = succ
	};
}

#endif
