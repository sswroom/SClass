#ifndef _SM_MANAGE_DASMX86_32
#define _SM_MANAGE_DASMX86_32
#include "Data/ArrayListUInt32.h"
#include "Manage/AddressResolver.h"
#include "Manage/MemoryReader.h"
#include "Manage/ThreadContextX86_32.h"
#include "Text/StringBuilderUTF8.h"

namespace Manage
{
	class DasmX86_32 : public Dasm32
	{
	public:

		struct Registers : public Manage::Dasm::Dasm_Regs
		{
			union
			{
				struct
				{
					UInt32 EAX;
					UInt32 ECX;
					UInt32 EDX;
					UInt32 EBX;
					UInt32 ESP;
					UInt32 EBP;
					UInt32 ESI;
					UInt32 EDI;
					UInt32 EIP;
				};
				struct
				{
					UInt32 regs[9];
				};
			};
			UInt32 EFLAGS;
			union
			{
				struct
				{
					UInt16 CS;
					UInt16 SS;
					UInt16 DS;
					UInt16 ES;
					UInt16 FS;
					UInt16 GS;
				};
				struct
				{
					UInt16 segm[6];
				};
			};

			union
			{
				struct
				{
					UInt32 DR0;
					UInt32 DR1;
					UInt32 DR2;
					UInt32 DR3;
					UInt32 DR6;
					UInt32 DR7;
				};
				struct
				{
					UInt32 dbgreg[6];
				};
			};
			struct
			{
				UInt8 floatBuff[80];
				UInt16 CTRL;
				UInt16 STAT;
				UInt16 TAG;
			};
		};

		struct Session;
		typedef Bool (CALLBACKFUNC DasmX86_32_Code)(NN<Session> sess);

		struct Session
		{
			Registers regs;
			NN<Data::ArrayListUInt32> callAddrs;
			NN<Data::ArrayListUInt32> jmpAddrs;
			//Text::StringBuilderW *outStr;
			UnsafeArray<UTF8Char> sbuff;
//			Bool isEnded;
			EndType endType;
			UInt32 espOfst;
			UInt32 retAddr;
			UInt32 lastStatus;
			UInt32 thisStatus; //bit0: prefix1, bit1: ignore esp change, bit2: prefix2, bit16: segment mod, bit31: ignore esp
			//prefix 2 1: 00 = no, 01 = 0x66, 02 = f2, 03 = f3
			Int32 segmId;
			
			Optional<Manage::AddressResolver> addrResol;
			NN<Manage::MemoryReader> memReader;

			UInt32 stabesp;
			UnsafeArray<DasmX86_32_Code> codes;
			UnsafeArray<DasmX86_32_Code> codes0f;
			UnsafeArray<DasmX86_32_Code> codes0f38;
			UnsafeArray<DasmX86_32_Code> codes0f3a;
		};

	private:
		UnsafeArray<DasmX86_32_Code> codes;
		UnsafeArray<DasmX86_32_Code> codes0f;
		UnsafeArray<DasmX86_32_Code> codes0f38;
		UnsafeArray<DasmX86_32_Code> codes0f3a;

//		AddressNameFunc nameFunc;
//		void *userObj;
	public:
		DasmX86_32();
		virtual ~DasmX86_32();

		virtual Text::CStringNN GetHeader(Bool fullRegs) const;
		virtual Bool Disasm32(NN<IO::Writer> writer, Optional<Manage::AddressResolver> addrResol, InOutParam<UInt32> currEip, InOutParam<UInt32> currEsp, InOutParam<UInt32> currEbp, NN<Data::ArrayListUInt32> callAddrs, NN<Data::ArrayListUInt32> jmpAddrs, OutParam<UInt32> blockStart, OutParam<UInt32> blockEnd, NN<Manage::Dasm::Dasm_Regs> regs, NN<Manage::MemoryReader> memReader, Bool fullRegs); // true = succ
		Bool Disasm32In(NN<Text::StringBuilderUTF8> outStr, Optional<Manage::AddressResolver> addrResol, InOutParam<UInt32> currEip, NN<Data::ArrayListUInt32> callAddrs, NN<Data::ArrayListUInt32> jmpAddrs, OutParam<UInt32> blockStart, OutParam<UInt32> blockEnd, NN<Manage::MemoryReader> memReader); // true = succ
		virtual NN<Dasm_Regs> CreateRegs() const;
		virtual void FreeRegs(NN<Dasm_Regs> regs) const;

		NN<Session> StartDasm(Optional<Manage::AddressResolver> addrResol, UnsafeArray<UInt8> addr, NN<Manage::MemoryReader> memReader);
		void EndDasm(NN<Session> sess);
		UnsafeArrayOpt<UTF8Char> DasmNext(NN<Session> sess, UnsafeArray<UTF8Char> buff);
		IntOS SessGetCodeOffset(NN<Session> sess);
		EndType SessGetEndType(NN<Session> sess);
		Bool SessContJmp(NN<Session> sess);
	};
}

#endif
