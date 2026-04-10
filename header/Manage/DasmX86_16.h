#ifndef _SM_MANAGE_DASMX86_16
#define _SM_MANAGE_DASMX86_16
#include "Data/ArrayListInt32.h"
#include "Manage/DasmBase.h"

namespace Manage
{
	class DasmX86_16 : public Manage::Dasm16
	{
	public:
		struct Registers : public Manage::Dasm::Dasm_Regs
		{
			union
			{
				struct
				{
					union
					{
						Int32 EAX;
						UInt16 AX;
						struct
						{
							UInt8 AL;
							UInt8 AH;
						};
					};
					union
					{
						Int32 ECX;
						UInt16 CX;
						struct
						{
							UInt8 CL;
							UInt8 CH;
						};
					};
					union
					{
						Int32 EDX;
						UInt16 DX;
						struct
						{
							UInt8 DL;
							UInt8 DH;
						};
					};
					union
					{
						Int32 EBX;
						UInt16 BX;
						struct
						{
							UInt8 BL;
							UInt8 BH;
						};
					};
					union
					{
						Int32 ESP;
						UInt16 SP;
					};
					union
					{
						Int32 EBP;
						UInt16 BP;
					};
					union
					{
						Int32 ESI;
						UInt16 SI;
					};
					union
					{
						Int32 EDI;
						UInt16 DI;
					};
					union
					{
						Int32 EFLAGS;
						struct
						{
							int DF : 1;
							int SF : 1;
							int OF : 1;
							int IF : 1;
							int CF : 1;
						};
					};
					UInt16 IP;
					UInt16 CS;
					UInt16 DS;
					UInt16 ES;
					UInt16 SS;
				};
				Int32 indexes[11];
			};
		};

		struct Session;
		typedef Bool (CALLBACKFUNC DasmX86_16_Code)(NN<Session> sess);

		struct Session
		{
			Registers regs;
			UnsafeArray<UInt8> code;
			UInt16 codeSegm;
			Data::ArrayListUInt32 callAddrs;
			Data::ArrayListUInt32 jmpAddrs;
			Int32 thisStatus; //bit0-2: 0 = normal, 1 = cs, 2=ds, 3 = es, 4 = ss, 5 = fs, 6 = gs, bit3: 1 = 32-bit data
			Int32 endStatus; //0 = not end, 1 = jmp out, 2 = exit program, 3 = func return
			UInt16 endIP;
			
			UnsafeArray<DasmX86_16_Code> codeHdlrs;
			UnsafeArray<DasmX86_16_Code> code0fHdlrs;
			UnsafeArray<UTF8Char> outSPtr;
			//prefix 2 1: 00 = no, 01 = 0x66, 02 = f2, 03 = f3
		};

	private:
		UnsafeArray<DasmX86_16_Code> codes;
		UnsafeArray<DasmX86_16_Code> codes0f;
	public:
		DasmX86_16();
		virtual ~DasmX86_16();

		NN<Session> CreateSess(NN<Registers> regs, UnsafeArray<UInt8> code, UInt16 codeSegm);
		void DeleteSess(NN<Session> sess);

		virtual Text::CStringNN GetHeader(Bool fullRegs) const;
		virtual Bool Disasm16(NN<IO::Writer> writer, Optional<Manage::AddressResolver> addrResol, InOutParam<UInt16> currInst, InOutParam<UInt16> currStack, InOutParam<UInt16> currFrame, NN<Data::ArrayListUInt32> callAddrs, NN<Data::ArrayListUInt32> jmpAddrs, OutParam<UInt16> blockStart, OutParam<UInt16> blockEnd, NN<Manage::Dasm::Dasm_Regs> regs, NN<Manage::MemoryReader> memReader, Bool fullRegs); // true = succ
		virtual NN<Dasm_Regs> CreateRegs() const;
		virtual void FreeRegs(NN<Dasm_Regs> regs) const;

		Bool DasmNext(NN<Session> sess, UnsafeArray<UTF8Char> buff, OutParam<UIntOS> outBuffSize); //True = succ
	};
}

#endif
