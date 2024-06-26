#ifndef _SM_MANAGE_DASMX86_64
#define _SM_MANAGE_DASMX86_64
#include "Data/ArrayListUInt64.h"
#include "Manage/AddressResolver.h"
#include "Manage/DasmBase.h"
#include "Manage/IMemoryReader.h"
#include "Text/CString.h"
#include "Text/StringBuilderUTF8.h"

namespace Manage
{
	class DasmX86_64 : public Dasm64
	{
	public:
		typedef enum
		{
			ET_NOT_END,
			ET_FUNC_RET,
			ET_JMP,
			ET_INV_OP,
			ET_EXIT
		} EndType;

		struct DasmX86_64_Regs : public Manage::Dasm::Dasm_Regs
		{
			union
			{
				struct
				{
					UInt64 rax;
					UInt64 rcx;
					UInt64 rdx;
					UInt64 rbx;
					UInt64 rsp;
					UInt64 rbp;
					UInt64 rsi;
					UInt64 rdi;
					UInt64 r8;
					UInt64 r9;
					UInt64 r10;
					UInt64 r11;
					UInt64 r12;
					UInt64 r13;
					UInt64 r14;
					UInt64 r15;
					UInt64 rip;
				};
				struct
				{
					UInt64 regs[17];
				};
			};
			UInt32 EFLAGS;
			union
			{
				struct
				{
					UInt16 cs;
					UInt16 ss;
					UInt16 ds;
					UInt16 es;
					UInt16 fs;
					UInt16 gs;
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
					UInt64 dr0;
					UInt64 dr1;
					UInt64 dr2;
					UInt64 dr3;
					UInt64 dr6;
					UInt64 dr7;
				};
				struct
				{
					UInt64 dregs[6];
				};
			};
		};

		typedef struct
		{
			DasmX86_64_Regs regs;
			Data::ArrayListUInt64 *callAddrs;
			Data::ArrayListUInt64 *jmpAddrs;
			//Text::StringBuilderW *outStr;
			UnsafeArray<UTF8Char> sbuff;
//			Bool isEnded;
			EndType endType;
			Int64 rspOfst;
			UInt64 retAddr;
			Int32 lastStatus;
			Int32 thisStatus; //bit0: prefix1, bit1: ignore esp change, bit2: prefix2, bit8-11: REX(WRXB), bit16: segment mod, bit17: has vex, bit18: vex leng, bit31: ignore esp
			//prefix 2 1: 00 = no, 01 = 0x66, 02 = f2, 03 = f3
			UInt8 vex[3];
			Int32 segmId;
			
			Manage::AddressResolver *addrResol;
			Manage::IMemoryReader *memReader;

			UInt64 stabesp;
			void **codes;
			void **codes0f;
			void **codes0f38;
			void **codes0f3a;
		} DasmX86_64_Sess;

		typedef Bool (CALLBACKFUNC DasmX86_64_Code)(NN<DasmX86_64_Sess> sess);
	private:
		DasmX86_64_Code *codes;
		DasmX86_64_Code *codes0f;
		DasmX86_64_Code *codes0f38;
		DasmX86_64_Code *codes0f3a;

//		AddressNameFunc nameFunc;
//		void *userObj;
	public:
		DasmX86_64();
		virtual ~DasmX86_64();

		virtual Text::CStringNN GetHeader(Bool fullRegs) const;
		virtual Bool Disasm64(NN<IO::Writer> writer, Manage::AddressResolver *addrResol, UInt64 *currRip, UInt64 *currRsp, UInt64 *currRbp, Data::ArrayListUInt64 *callAddrs, Data::ArrayListUInt64 *jmpAddrs, UInt64 *blockStart, UInt64 *blockEnd, NN<Manage::Dasm::Dasm_Regs> regs, Manage::IMemoryReader *memReader, Bool fullRegs); // true = succ
		Bool Disasm64In(NN<Text::StringBuilderUTF8> outStr, Manage::AddressResolver *addrResol, UInt64 *currRip, Data::ArrayListUInt64 *callAddrs, Data::ArrayListUInt64 *jmpAddrs, UInt64 *blockStart, UInt64 *blockEnd, Manage::IMemoryReader *memReader); // true = succ
		virtual NN<Dasm_Regs> CreateRegs() const;
		virtual void FreeRegs(NN<Dasm_Regs> regs) const;

		NN<DasmX86_64_Sess> StartDasm(Manage::AddressResolver *addrResol, void *addr, Manage::IMemoryReader *memReader);
		void EndDasm(NN<DasmX86_64_Sess> sess);
		UnsafeArrayOpt<UTF8Char> DasmNext(NN<DasmX86_64_Sess> sess, UnsafeArray<UTF8Char> buff);
		OSInt SessGetCodeOffset(NN<DasmX86_64_Sess> sess);
		EndType SessGetEndType(NN<DasmX86_64_Sess> sess);
		Bool SessContJmp(NN<DasmX86_64_Sess> sess);
	};
};

#endif
