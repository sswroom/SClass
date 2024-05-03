#ifndef _SM_MANAGE_DASMX86_16
#define _SM_MANAGE_DASMX86_16
#include "Data/ArrayListInt32.h"
#include "Manage/DasmBase.h"

namespace Manage
{
	class DasmX86_16 : public Manage::Dasm16
	{
	public:
		typedef struct
		{
			DasmX86_16_Regs regs;
			UInt8 *code;
			UInt16 codeSegm;
			Data::ArrayListUInt32 callAddrs;
			Data::ArrayListUInt32 jmpAddrs;
			Int32 thisStatus; //bit0-2: 0 = normal, 1 = cs, 2=ds, 3 = es, 4 = ss, 5 = fs, 6 = gs, bit3: 1 = 32-bit data
			Int32 endStatus; //0 = not end, 1 = jmp out, 2 = exit program, 3 = func return
			UInt16 endIP;
			
			void **codeHdlrs;
			void **code0fHdlrs;
			UTF8Char *outSPtr;
			//prefix 2 1: 00 = no, 01 = 0x66, 02 = f2, 03 = f3
		} DasmX86_16_Sess;

		typedef Bool (__stdcall *DasmX86_16_Code)(DasmX86_16_Sess *sess);
	private:
		DasmX86_16_Code *codes;
		DasmX86_16_Code *codes0f;
	public:
		DasmX86_16();
		virtual ~DasmX86_16();

		DasmX86_16_Sess *CreateSess(DasmX86_16_Regs *regs, UInt8 *code, UInt16 codeSegm);
		void DeleteSess(DasmX86_16_Sess *sess);

		virtual Text::CStringNN GetHeader(Bool fullRegs) const;
		virtual Bool Disasm16(NN<IO::Writer> writer, Manage::AddressResolver *addrResol, UInt16 *currInst, UInt16 *currStack, UInt16 *currFrame, Data::ArrayListUInt32 *callAddrs, Data::ArrayListUInt32 *jmpAddrs, UInt16 *blockStart, UInt16 *blockEnd, Manage::Dasm::Dasm_Regs *regs, Manage::IMemoryReader *memReader, Bool fullRegs); // true = succ
		virtual Dasm_Regs *CreateRegs() const;
		virtual void FreeRegs(Dasm_Regs *regs) const;

		Bool DasmNext(DasmX86_16_Sess *sess, UTF8Char *buff, UOSInt *outBuffSize); //True = succ
	};
}

#endif
