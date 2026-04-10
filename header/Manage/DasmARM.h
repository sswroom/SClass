#ifndef _SM_MANAGE_DASMARM
#define _SM_MANAGE_DASMARM
#include "Manage/DasmBase.h"

namespace Manage
{
	class DasmARM : public Dasm32
	{
	public:
		struct Registers : public Manage::Dasm::Dasm_Regs
		{
			union 
			{
				struct
				{
					UInt32 R0;
					UInt32 R1;
					UInt32 R2;
					UInt32 R3;
					UInt32 R4;
					UInt32 R5;
					UInt32 R6;
					UInt32 R7;
					UInt32 R8;
					UInt32 R9;
					UInt32 R10;
					UInt32 FP;
					UInt32 IP;
					UInt32 SP;
					UInt32 PC;
					UInt32 LR;
					UInt32 CPSR;
				};
				Int32 indexes[17];
			};
		};

		struct Session;
		typedef Bool (CALLBACKFUNC DasmARM_Code)(NN<Session> sess);

		struct Session
		{
			Registers regs;
			UnsafeArray<UInt8> code;
			UInt16 codeSegm;
			NN<Data::ArrayListUInt32> callAddrs;
			NN<Data::ArrayListUInt32> jmpAddrs;
			UnsafeArray<UTF8Char> sbuff;
			UInt32 retAddr;
			Int32 thisStatus;
			Int32 endStatus; //0 = not end, 1 = jmp out, 2 = exit program, 3 = func return
			UInt16 endIP;
			EndType endType;
			UnsafeArray<UInt8> codeBuff;
			
			Optional<Manage::AddressResolver> addrResol;
			NN<Manage::MemoryReader> memReader;

			UnsafeArray<DasmARM_Code> codeHdlrs;
			UnsafeArray<DasmARM_Code> codeTHdlrs;
			//prefix 2 1: 00 = no, 01 = 0x66, 02 = f2, 03 = f3
		};

	private:
		UnsafeArray<DasmARM_Code> codes;
		UnsafeArray<DasmARM_Code> codesT;
	public:
		DasmARM();
		virtual ~DasmARM();

		virtual Text::CStringNN GetHeader(Bool fullRegs) const;
		virtual Bool Disasm32(NN<IO::Writer> writer, Optional<Manage::AddressResolver> addrResol, InOutParam<UInt32> currInst, InOutParam<UInt32> currStack, InOutParam<UInt32> currFrame, NN<Data::ArrayListUInt32> callAddrs, NN<Data::ArrayListUInt32> jmpAddrs, OutParam<UInt32> blockStart, OutParam<UInt32> blockEnd, NN<Manage::Dasm::Dasm_Regs> regs, NN<Manage::MemoryReader> memReader, Bool fullRegs); // true = succ
		virtual NN<Dasm_Regs> CreateRegs() const;
		virtual void FreeRegs(NN<Dasm_Regs> regs) const;

		NN<Session> CreateSess(NN<Registers> regs, UnsafeArray<UInt8> code, UInt16 codeSegm);
		void DeleteSess(NN<Session> sess);

		Bool DasmNext(NN<Session> sess, UnsafeArray<UTF8Char> buff, OutParam<UIntOS> outBuffSize); //True = succ
	};
}

#endif
