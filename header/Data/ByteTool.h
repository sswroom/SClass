#ifndef _SM_DATA_BYTETOOL
#define _SM_DATA_BYTETOOL
#include "MyMemory.h"

#if defined(CPU_X86_32) || defined(CPU_X86_64) || defined(CPU_ARM64) || (defined(CPU_ARM) && defined(__ARM_FEATURE_UNALIGNED)) || (defined(_MSC_VER) && !defined(WIN32_WCE))

#define ReadNInt16(uint8Ptr) (*(Int16*)uint8Ptr)
#define ReadNUInt16(uint8Ptr) (*(UInt16*)uint8Ptr)
#define ReadNInt32(uint8Ptr) (*(Int32*)uint8Ptr)
#define ReadNUInt32(uint8Ptr) (*(UInt32*)uint8Ptr)
#define ReadNInt64(uint8Ptr) (*(Int64*)uint8Ptr)
#define ReadNUInt64(uint8Ptr) (*(UInt64*)uint8Ptr)
#define WriteNInt16(uint8Ptr, val) *(Int16*)(uint8Ptr) = (val)
#define WriteNUInt16(uint8Ptr, val) *(UInt16*)(uint8Ptr) = (val)
#define WriteNInt32(uint8Ptr, val) *(Int32*)(uint8Ptr) = (val)
#define WriteNUInt32(uint8Ptr, val) *(UInt32*)(uint8Ptr) = (val)
#define WriteNInt64(uint8Ptr, val) *(Int64*)(uint8Ptr) = (val)
#define WriteNUInt64(uint8Ptr, val) *(UInt64*)(uint8Ptr) = (val)

#define ReadInt64(uint8Ptr) (*(Int64*)(uint8Ptr))
#define ReadUInt64(uint8Ptr) (*(UInt64*)(uint8Ptr))
#define ReadMInt64(uint8Ptr) ((((Int64)ReadMInt32(uint8Ptr)) << 32) | ReadMUInt32(&(uint8Ptr)[4]))
#define ReadMUInt64(uint8Ptr) ((((UInt64)ReadMUInt32(uint8Ptr)) << 32) | ReadMUInt32(&(uint8Ptr)[4]))
#define WriteInt64(uint8Ptr, val) *(Int64*)(uint8Ptr) = (val)
#define WriteUInt64(uint8Ptr, val) *(UInt64*)(uint8Ptr) = (val)

FORCEINLINE void WriteMInt64(UInt8 *addr, Int64 val)
{
#ifdef HAS_ASM32
	_asm
	{
		mov eax,dword ptr val
		mov edx,dword ptr val[4]
		mov ecx,addr
		bswap eax
		bswap edx
		mov dword ptr [ecx],edx
		mov dword ptr [ecx + 4],eax
	}
#else
	addr[0] = (UInt8)((val >> 56) & 0xff);
	addr[1] = (UInt8)((val >> 48) & 0xff);
	addr[2] = (UInt8)((val >> 40) & 0xff);
	addr[3] = (UInt8)((val >> 32) & 0xff);
	addr[4] = (UInt8)((val >> 24) & 0xff);
	addr[5] = (UInt8)((val >> 16) & 0xff);
	addr[6] = (UInt8)((val >> 8) & 0xff);
	addr[7] = (UInt8)(val & 0xff);
#endif
}

#define WriteMUInt64(uint8Ptr, val) WriteMInt64(uint8Ptr, (Int64)val)

#define ReadInt32(uint8Ptr) (*(Int32*)(uint8Ptr))
#define ReadUInt32(uint8Ptr) (*(UInt32*)(uint8Ptr))
#define ReadMInt32(uint8Ptr) (BSWAP32(ReadInt32(uint8Ptr)))
#define ReadMUInt32(uint8Ptr) ((UInt32)BSWAP32(ReadInt32(uint8Ptr)))
#define WriteInt32(uint8Ptr, val) *(Int32*)(uint8Ptr) = (val)
#define WriteUInt32(uint8Ptr, val) *(UInt32*)(uint8Ptr) = (val)
FORCEINLINE void WriteMInt32(UInt8 *addr, Int32 val)
{
#ifdef HAS_ASM32
	_asm
	{
		mov eax,val
		mov edx,addr
		bswap eax
		mov dword ptr [edx],eax
	}
#else
	addr[0] = (UInt8)((val >> 24) & 0xff);
	addr[1] = (UInt8)((val >> 16) & 0xff);
	addr[2] = (UInt8)((val >> 8) & 0xff);
	addr[3] = (UInt8)(val & 0xff);
#endif
}

#define ReadInt16(uint8Ptr) (*(Int16*)(uint8Ptr))
#define ReadUInt16(uint8Ptr) (*(UInt16*)(uint8Ptr))
#define ReadMInt16(uint8Ptr) (Int16)(((uint8Ptr)[0] << 8) | (uint8Ptr)[1])
#define ReadMUInt16(uint8Ptr) (UInt16)(((uint8Ptr)[0] << 8) | (uint8Ptr)[1])
#define WriteInt16(uint8Ptr, val) *(Int16*)(uint8Ptr) = (Int16)((val) & 0xffff)
#define WriteUInt16(uint8Ptr, val) *(UInt16*)(uint8Ptr) = (UInt16)((val) & 0xffff)
#define WriteMInt16(uint8Ptr, val) {(uint8Ptr)[0] = (UInt8)(((val) >> 8) & 0xff); (uint8Ptr)[1] = (UInt8)((val) & 0xff);}
#define WriteMUInt16(uint8Ptr, val) {(uint8Ptr)[0] = (UInt8)(((val) >> 8) & 0xff); (uint8Ptr)[1] = (UInt8)((val) & 0xff);}

#define ReadDouble(uint8Ptr) (*(Double*)(uint8Ptr))
FORCEINLINE Double ReadMDouble(const UInt8 *dptr)
{
	UInt8 tmpBuff[8];
	WriteInt32(&tmpBuff[4], ReadMInt32(dptr));
	WriteInt32(&tmpBuff[0], ReadMInt32(&dptr[4]));
	return *(Double*)tmpBuff;
}
#define WriteDouble(uint8Ptr, val) *(Double*)(uint8Ptr) = (val)

#define ReadFloat(uint8Ptr) (*(Single*)(uint8Ptr))
FORCEINLINE Single ReadMFloat(const UInt8 *dptr)
{
	UInt8 tmpBuff[4];
	WriteInt32(&tmpBuff[0], ReadMInt32(dptr));
	return *(Single*)tmpBuff;
}
#define WriteFloat(uint8Ptr, val) *(Single*)(uint8Ptr) = (val)
FORCEINLINE void WriteMFloat(UInt8 *uint8Ptr, Single val)
{
	UInt8 tmpBuff[4];
	WriteFloat(tmpBuff, val);
	uint8Ptr[0] = tmpBuff[3];
	uint8Ptr[1] = tmpBuff[2];
	uint8Ptr[2] = tmpBuff[1];
	uint8Ptr[3] = tmpBuff[0];
}

#elif defined(CPU_ARM) || defined(__MIPSEL__) || defined(CPU_AVR)
FORCEINLINE Int16 ReadNInt16(const UInt8 *dptr)
{
	UInt8 tmpBuff[2];
	tmpBuff[0] = dptr[0];
	tmpBuff[1] = dptr[1];
	return *(Int16*)tmpBuff;
}

FORCEINLINE UInt16 ReadNUInt16(const UInt8 *dptr)
{
	UInt8 tmpBuff[2];
	tmpBuff[0] = dptr[0];
	tmpBuff[1] = dptr[1];
	return *(UInt16*)tmpBuff;
}

FORCEINLINE Int32 ReadNInt32(const UInt8 *dptr)
{
	UInt8 tmpBuff[4];
	tmpBuff[0] = dptr[0];
	tmpBuff[1] = dptr[1];
	tmpBuff[2] = dptr[2];
	tmpBuff[3] = dptr[3];
	return *(Int32*)tmpBuff;
}

FORCEINLINE UInt32 ReadNUInt32(const UInt8 *dptr)
{
	UInt8 tmpBuff[4];
	tmpBuff[0] = dptr[0];
	tmpBuff[1] = dptr[1];
	tmpBuff[2] = dptr[2];
	tmpBuff[3] = dptr[3];
	return *(UInt32*)tmpBuff;
}

FORCEINLINE Int64 ReadNInt64(const UInt8 *dptr)
{
	UInt8 tmpBuff[8];
	tmpBuff[0] = dptr[0];
	tmpBuff[1] = dptr[1];
	tmpBuff[2] = dptr[2];
	tmpBuff[3] = dptr[3];
	tmpBuff[4] = dptr[4];
	tmpBuff[5] = dptr[5];
	tmpBuff[6] = dptr[6];
	tmpBuff[7] = dptr[7];
	return *(Int64*)tmpBuff;
}

FORCEINLINE void WriteNInt16(UInt8 *dptr, Int16 val)
{
	UInt8 *vPtr = (UInt8*)&val;
	dptr[0] = vPtr[0];
	dptr[1] = vPtr[1];
}

FORCEINLINE void WriteNUInt16(UInt8 *dptr, UInt16 val)
{
	UInt8 *vPtr = (UInt8*)&val;
	dptr[0] = vPtr[0];
	dptr[1] = vPtr[1];
}

FORCEINLINE void WriteNInt32(UInt8 *dptr, Int32 val)
{
	UInt8 *vPtr = (UInt8*)&val;
	dptr[0] = vPtr[0];
	dptr[1] = vPtr[1];
	dptr[2] = vPtr[2];
	dptr[3] = vPtr[3];
}

FORCEINLINE void WriteNUInt32(UInt8 *dptr, UInt32 val)
{
	UInt8 *vPtr = (UInt8*)&val;
	dptr[0] = vPtr[0];
	dptr[1] = vPtr[1];
	dptr[2] = vPtr[2];
	dptr[3] = vPtr[3];
}

FORCEINLINE void WriteNInt64(UInt8 *dptr, Int64 val)
{
	UInt8 *vPtr = (UInt8*)&val;
	dptr[0] = vPtr[0];
	dptr[1] = vPtr[1];
	dptr[2] = vPtr[2];
	dptr[3] = vPtr[3];
	dptr[4] = vPtr[4];
	dptr[5] = vPtr[5];
	dptr[6] = vPtr[6];
	dptr[7] = vPtr[7];
}

FORCEINLINE Int64 ReadInt64(const UInt8 *dptr)
{
	UInt8 tmpBuff[8];
	tmpBuff[0] = dptr[0];
	tmpBuff[1] = dptr[1];
	tmpBuff[2] = dptr[2];
	tmpBuff[3] = dptr[3];
	tmpBuff[4] = dptr[4];
	tmpBuff[5] = dptr[5];
	tmpBuff[6] = dptr[6];
	tmpBuff[7] = dptr[7];
	return *(Int64*)tmpBuff;
}
#define ReadUInt64(uint8Ptr) (UInt64)ReadInt64(uint8Ptr)

FORCEINLINE Int64 ReadMInt64(const UInt8 *dptr)
{
	UInt8 tmpBuff[8];
	tmpBuff[0] = dptr[7];
	tmpBuff[1] = dptr[6];
	tmpBuff[2] = dptr[5];
	tmpBuff[3] = dptr[4];
	tmpBuff[4] = dptr[3];
	tmpBuff[5] = dptr[2];
	tmpBuff[6] = dptr[1];
	tmpBuff[7] = dptr[0];
	return *(Int64*)tmpBuff;
}
#define ReadMUInt64(uint8Ptr) (UInt64)ReadMInt64(uint8Ptr)

FORCEINLINE void WriteInt64(UInt8 *dptr, Int64 val)
{
	UInt8 *vPtr = (UInt8*)&val;
	dptr[0] = vPtr[0];
	dptr[1] = vPtr[1];
	dptr[2] = vPtr[2];
	dptr[3] = vPtr[3];
	dptr[4] = vPtr[4];
	dptr[5] = vPtr[5];
	dptr[6] = vPtr[6];
	dptr[7] = vPtr[7];
}

FORCEINLINE void WriteUInt64(UInt8 *dptr, UInt64 val)
{
	UInt8 *vPtr = (UInt8*)&val;
	dptr[0] = vPtr[0];
	dptr[1] = vPtr[1];
	dptr[2] = vPtr[2];
	dptr[3] = vPtr[3];
	dptr[4] = vPtr[4];
	dptr[5] = vPtr[5];
	dptr[6] = vPtr[6];
	dptr[7] = vPtr[7];
}

FORCEINLINE void WriteMInt64(UInt8 *dptr, Int64 val)
{
	UInt8 *vPtr = (UInt8*)&val;
	dptr[0] = vPtr[7];
	dptr[1] = vPtr[6];
	dptr[2] = vPtr[5];
	dptr[3] = vPtr[4];
	dptr[4] = vPtr[3];
	dptr[5] = vPtr[2];
	dptr[6] = vPtr[1];
	dptr[7] = vPtr[0];
}

FORCEINLINE void WriteMUInt64(UInt8 *dptr, UInt64 val)
{
	UInt8 *vPtr = (UInt8*)&val;
	dptr[0] = vPtr[7];
	dptr[1] = vPtr[6];
	dptr[2] = vPtr[5];
	dptr[3] = vPtr[4];
	dptr[4] = vPtr[3];
	dptr[5] = vPtr[2];
	dptr[6] = vPtr[1];
	dptr[7] = vPtr[0];
}

FORCEINLINE Int32 ReadInt32(const UInt8 *dptr)
{
	UInt8 tmpBuff[4];
	tmpBuff[0] = dptr[0];
	tmpBuff[1] = dptr[1];
	tmpBuff[2] = dptr[2];
	tmpBuff[3] = dptr[3];
	return *(Int32*)tmpBuff;
}

FORCEINLINE UInt32 ReadUInt32(const UInt8 *dptr)
{
	UInt8 tmpBuff[4];
	tmpBuff[0] = dptr[0];
	tmpBuff[1] = dptr[1];
	tmpBuff[2] = dptr[2];
	tmpBuff[3] = dptr[3];
	return *(UInt32*)tmpBuff;
}

FORCEINLINE Int32 ReadMInt32(const UInt8 *dptr)
{
	UInt8 tmpBuff[4];
	tmpBuff[0] = dptr[3];
	tmpBuff[1] = dptr[2];
	tmpBuff[2] = dptr[1];
	tmpBuff[3] = dptr[0];
	return *(Int32*)tmpBuff;
}

FORCEINLINE UInt32 ReadMUInt32(const UInt8 *dptr)
{
	UInt8 tmpBuff[4];
	tmpBuff[0] = dptr[3];
	tmpBuff[1] = dptr[2];
	tmpBuff[2] = dptr[1];
	tmpBuff[3] = dptr[0];
	return *(UInt32*)tmpBuff;
}

FORCEINLINE void WriteInt32(UInt8 *dptr, Int32 val)
{
	UInt8 *vPtr = (UInt8*)&val;
	dptr[0] = vPtr[0];
	dptr[1] = vPtr[1];
	dptr[2] = vPtr[2];
	dptr[3] = vPtr[3];
}

FORCEINLINE void WriteUInt32(UInt8 *dptr, UInt32 val)
{
	UInt8 *vPtr = (UInt8*)&val;
	dptr[0] = vPtr[0];
	dptr[1] = vPtr[1];
	dptr[2] = vPtr[2];
	dptr[3] = vPtr[3];
}

FORCEINLINE void WriteMInt32(UInt8 *dptr, Int32 val)
{
	UInt8 *vPtr = (UInt8*)&val;
	dptr[0] = vPtr[3];
	dptr[1] = vPtr[2];
	dptr[2] = vPtr[1];
	dptr[3] = vPtr[0];
}

FORCEINLINE Int16 ReadInt16(const UInt8 *dptr)
{
	UInt8 tmpBuff[2];
	tmpBuff[0] = dptr[0];
	tmpBuff[1] = dptr[1];
	return *(Int16*)tmpBuff;
}

FORCEINLINE UInt16 ReadUInt16(const UInt8 *dptr)
{
	UInt8 tmpBuff[2];
	tmpBuff[0] = dptr[0];
	tmpBuff[1] = dptr[1];
	return *(UInt16*)tmpBuff;
}

FORCEINLINE Int16 ReadMInt16(const UInt8 *dptr)
{
	UInt8 tmpBuff[2];
	tmpBuff[0] = dptr[1];
	tmpBuff[1] = dptr[0];
	return *(Int16*)tmpBuff;
}

FORCEINLINE UInt16 ReadMUInt16(const UInt8 *dptr)
{
	UInt8 tmpBuff[2];
	tmpBuff[0] = dptr[1];
	tmpBuff[1] = dptr[0];
	return *(UInt16*)tmpBuff;
}

FORCEINLINE void WriteInt16(UInt8 *dptr, Int16 val)
{
	UInt8 *vPtr = (UInt8*)&val;
	dptr[0] = vPtr[0];
	dptr[1] = vPtr[1];
}

FORCEINLINE void WriteUInt16(UInt8 *dptr, UInt16 val)
{
	UInt8 *vPtr = (UInt8*)&val;
	dptr[0] = vPtr[0];
	dptr[1] = vPtr[1];
}

FORCEINLINE void WriteMInt16(UInt8 *dptr, Int16 val)
{
	UInt8 *vPtr = (UInt8*)&val;
	dptr[0] = vPtr[1];
	dptr[1] = vPtr[0];
}

FORCEINLINE void WriteMUInt16(UInt8 *dptr, UInt16 val)
{
	UInt8 *vPtr = (UInt8*)&val;
	dptr[0] = vPtr[1];
	dptr[1] = vPtr[0];
}

FORCEINLINE Double ReadDouble(const UInt8 *dptr)
{
	UInt8 tmpBuff[8];
	tmpBuff[0] = dptr[0];
	tmpBuff[1] = dptr[1];
	tmpBuff[2] = dptr[2];
	tmpBuff[3] = dptr[3];
	tmpBuff[4] = dptr[4];
	tmpBuff[5] = dptr[5];
	tmpBuff[6] = dptr[6];
	tmpBuff[7] = dptr[7];
	return *(Double*)tmpBuff;
}

FORCEINLINE Double ReadMDouble(const UInt8 *dptr)
{
	UInt8 tmpBuff[8];
	tmpBuff[0] = dptr[7];
	tmpBuff[1] = dptr[6];
	tmpBuff[2] = dptr[5];
	tmpBuff[3] = dptr[4];
	tmpBuff[4] = dptr[3];
	tmpBuff[5] = dptr[2];
	tmpBuff[6] = dptr[1];
	tmpBuff[7] = dptr[0];
	return *(Double*)tmpBuff;
}

FORCEINLINE void WriteDouble(UInt8 *dptr, Double val)
{
	UInt8 *vPtr = (UInt8*)&val;
	dptr[0] = vPtr[0];
	dptr[1] = vPtr[1];
	dptr[2] = vPtr[2];
	dptr[3] = vPtr[3];
	dptr[4] = vPtr[4];
	dptr[5] = vPtr[5];
	dptr[6] = vPtr[6];
	dptr[7] = vPtr[7];
}

FORCEINLINE Single ReadFloat(const UInt8 *dptr)
{
	UInt8 tmpBuff[4];
	tmpBuff[0] = dptr[0];
	tmpBuff[1] = dptr[1];
	tmpBuff[2] = dptr[2];
	tmpBuff[3] = dptr[3];
	return *(Single*)tmpBuff;
}

FORCEINLINE Single ReadMFloat(const UInt8 *dptr)
{
	UInt8 tmpBuff[4];
	tmpBuff[0] = dptr[3];
	tmpBuff[1] = dptr[2];
	tmpBuff[2] = dptr[1];
	tmpBuff[3] = dptr[0];
	return *(Single*)tmpBuff;
}

FORCEINLINE void WriteFloat(UInt8 *dptr, Single val)
{
	UInt8 *vPtr = (UInt8*)&val;
	dptr[0] = vPtr[0];
	dptr[1] = vPtr[1];
	dptr[2] = vPtr[2];
	dptr[3] = vPtr[3];
}

FORCEINLINE void WriteMFloat(UInt8 *uint8Ptr, Single val)
{
	UInt8 *vPtr = (UInt8*)&val;
	uint8Ptr[0] = vPtr[3];
	uint8Ptr[1] = vPtr[2];
	uint8Ptr[2] = vPtr[1];
	uint8Ptr[3] = vPtr[0];
}

#else
#define ReadNInt16(uint8Ptr) (*(Int16*)uint8Ptr)
#define ReadNInt32(uint8Ptr) (*(Int32*)uint8Ptr)
#define ReadNInt64(uint8Ptr) (*(Int64*)uint8Ptr)
#define WriteNInt16(uint8Ptr, val) *(Int16*)(uint8Ptr) = (val)
#define WriteNInt32(uint8Ptr, val) *(Int32*)(uint8Ptr) = (val)
#define WriteNInt64(uint8Ptr, val) *(Int64*)(uint8Ptr) = (val)

#define ReadMInt64(uint8Ptr) (*(Int64*)(uint8Ptr))
#define ReadMUInt64(uint8Ptr) (*(UInt64*)(uint8Ptr))
#define ReadInt64(uint8Ptr) (ReadUInt32(uint8Ptr) | ((Int64)ReadInt32(&(uint8Ptr)[4]) << 32))
#define ReadUInt64(uint8Ptr) (ReadUInt32(uint8Ptr) | ((Int64)ReadUInt32(&(uint8Ptr)[4]) << 32))
#define WriteMInt64(uint8Ptr, val) *(Int64*)(uint8Ptr) = (val)
FORCEINLINE void WriteInt64(UInt8 *addr, Int64 val)
{
	addr[0] = (UInt8)(val & 0xff);
	addr[1] = (UInt8)((val >> 8) & 0xff);
	addr[2] = (UInt8)((val >> 16) & 0xff);
	addr[3] = (UInt8)((val >> 24) & 0xff);
	addr[4] = (UInt8)((val >> 32) & 0xff);
	addr[5] = (UInt8)((val >> 40) & 0xff);
	addr[6] = (UInt8)((val >> 48) & 0xff);
	addr[7] = (UInt8)((val >> 56) & 0xff);
}

#define ReadMInt32(uint8Ptr) (*(Int32*)(uint8Ptr))
#define ReadMUInt32(uint8Ptr) (*(UInt32*)(uint8Ptr))
#define ReadInt32(uint8Ptr) (BSWAP32(ReadMInt32(uint8Ptr)))
#define ReadUInt32(uint8Ptr) ((UInt32)BSWAP32(ReadMInt32(uint8Ptr)))
#define WriteMInt32(uint8Ptr, val) *(Int32*)(uint8Ptr) = (val)
#define WriteMUInt32(uint8Ptr, val) *(UInt32*)(uint8Ptr) = (val)
FORCEINLINE void WriteInt32(UInt8 *addr, Int32 val)
{
	addr[0] = val & 0xff;
	addr[1] = (val >> 8) & 0xff;
	addr[2] = (val >> 16) & 0xff;
	addr[3] = (val >> 24) & 0xff;
}
#define WriteUInt32(uint8Ptr, val) WriteInt32(uint8Ptr, (Int32)val)

#define ReadMInt16(uint8Ptr) (*(Int16*)(uint8Ptr))
#define ReadMUInt16(uint8Ptr) (*(UInt16*)(uint8Ptr))
#define ReadInt16(uint8Ptr) (Int16)(((uint8Ptr)[1] << 8) | (uint8Ptr)[0])
#define ReadUInt16(uint8Ptr) (UInt16)(((uint8Ptr)[1] << 8) | (uint8Ptr)[0])
#define WriteMInt16(uint8Ptr, val) *(Int16*)(uint8Ptr) = ((val) & 0xffff)
#define WriteInt16(uint8Ptr, val) {(uint8Ptr)[1] = (UInt8)(((val) >> 8) & 0xff); (uint8Ptr)[0] = (UInt8)((val) & 0xff);}

#define ReadMDouble(uint8Ptr) (*(Double*)(uint8Ptr))
FORCEINLINE Double ReadDouble(const UInt8 *dptr)
{
	UInt8 tmpBuff[8];
	WriteMInt32(&tmpBuff[4], ReadInt32(dptr));
	WriteMInt32(&tmpBuff[0], ReadInt32(&dptr[4]));
	return *(Double*)tmpBuff;
}

#define WriteMDouble(uint8Ptr, val) *(Double*)(uint8Ptr) = (val)
FORCEINLINE void WriteDouble(UInt8 *dptr, Double val)
{
	UInt8 *tmpPtr = (UInt8*)&val;
	dptr[0] = tmpPtr[7];
	dptr[1] = tmpPtr[6];
	dptr[2] = tmpPtr[5];
	dptr[3] = tmpPtr[4];
	dptr[4] = tmpPtr[3];
	dptr[5] = tmpPtr[2];
	dptr[6] = tmpPtr[1];
	dptr[7] = tmpPtr[0];
}

#define ReadMFloat(uint8Ptr) (*(Single*)(uint8Ptr))
FORCEINLINE Single ReadFloat(const UInt8 *dptr)
{
	UInt8 tmpBuff[4];
	WriteMInt32(&tmpBuff[0], ReadInt32(dptr));
	return *(Single*)tmpBuff;
}
#define WriteMFloat(uint8Ptr, val) *(Single*)(uint8Ptr) = (val)
FORCEINLINE void WriteFloat(UInt8 *dptr, Single val)
{
	UInt8 *tmpPtr = (UInt8*)&val;
	dptr[0] = tmpPtr[3];
	dptr[1] = tmpPtr[2];
	dptr[2] = tmpPtr[1];
	dptr[3] = tmpPtr[0];
}
#endif

#define ReadUInt24(uint8Ptr) (UInt32)((ReadUInt16(uint8Ptr + 1) << 8) | (uint8Ptr)[0])
#define ReadInt24(uint8Ptr) ((ReadInt16(uint8Ptr + 1) << 8) | (uint8Ptr)[0])
#define ReadMInt24(uint8Ptr) ((ReadMInt16(uint8Ptr) << 8) | (uint8Ptr)[2])
#define ReadMUInt24(uint8Ptr) ((UInt32)((uint8Ptr)[0] << 16) | ReadMUInt16(uint8Ptr + 1))
#define WriteInt24(uint8Ptr, val) {(uint8Ptr)[0] = (UInt8)((val) & 0xff); (uint8Ptr)[1] = (UInt8)(((val) >> 8) & 0xff); (uint8Ptr)[2] = (UInt8)(((val) >> 16) & 0xff);}
#define WriteMInt24(uint8Ptr, val) {(uint8Ptr)[0] = (UInt8)(((val) >> 16) & 0xff); (uint8Ptr)[1] = (UInt8)(((val) >> 8) & 0xff); (uint8Ptr)[2] = (UInt8)((val) & 0xff);}

namespace Data
{
	namespace ByteTool
	{
		UInt8 GetBCD8(UInt8 bcd);
		UInt32 GetBCD32(const UInt8 *bcd);
		UInt8 Int2BCDB(UInt32 val);
	}
}

#define WriteMUInt32(uint8Ptr, val) WriteMInt32(uint8Ptr, (Int32)(val))

FORCEINLINE UInt16 AddU16(UInt16 val1, UInt16 val2)
{
	return (UInt16)(val1 + val2);
}

FORCEINLINE void AddEqU16(UInt16 *dptr, UInt16 val)
{
	*dptr = (UInt16)(*dptr + val);
}

FORCEINLINE void AddEqU8(UInt8 *dptr, UInt8 val)
{
	*dptr = (UInt8)(*dptr + val);
}

FORCEINLINE void SubEqU16(UInt16 *dptr, UInt16 val)
{
	*dptr = (UInt16)(*dptr - val);
}

FORCEINLINE void SubEqU8(UInt8 *dptr, UInt8 val)
{
	*dptr = (UInt8)(*dptr - val);
}

FORCEINLINE void OrEqU16(UInt16 *dptr, UInt16 val)
{
	*dptr = (UInt16)(*dptr | val);
}

FORCEINLINE void ShlEqU16(UInt16 *dptr, UInt32 val)
{
	*dptr = (UInt16)(*dptr << val);
}

FORCEINLINE void ShrEq16(UInt16 *dptr, UInt32 val)
{
	*dptr = (UInt16)(*dptr >> val);
}

FORCEINLINE void SarEq16(Int16 *dptr, UInt32 val)
{
	*dptr = (Int16)(*dptr >> val);
}

FORCEINLINE void ShlEqU8(UInt8 *dptr, UInt32 val)
{
	*dptr = (UInt8)(*dptr << val);
}

FORCEINLINE void SarEq8(Int8 *dptr, UInt32 val)
{
	*dptr = (Int8)(*dptr >> val);
}

FORCEINLINE void ShrEq8(UInt8 *dptr, UInt32 val)
{
	*dptr = (UInt8)(*dptr >> val);
}

FORCEINLINE Double UInt16_Float16(UInt16 val)
{
	UInt16 e = (val >> 10) & 0x1F;
	if (e == 0)
	{
		if ((val & 0x3ff) == 0)
		{

		}
		else
		{
			Double d = (val & 0x3ff) / 1024.0 * 0.000061035156;
			if (val & 0x8000)
			{
				return -d;
			}
			else
			{
				return d;
			}
		}
	}
	else if (e == 0x1F)
	{
		e = 0x7ff;
	}
	else
	{
		e = (UInt16)(e - 15 + 1023);
	}
	UInt8 buff[8];
	buff[0] = 0;
	buff[1] = 0;
	buff[2] = 0;
	buff[3] = 0;
	buff[4] = 0;
	buff[5] = (UInt8)((val << 2) & 0xff);
	buff[6] = (UInt8)(((val >> 6) & 0xf) | ((e << 4) & 0xf0));
	buff[7] = (UInt8)(((val >> 8) & 0x80) | (e >> 4));
	return ReadDouble(buff);
}

#define ReadFloat16(dptr) UInt16_Float16(ReadUInt16(dptr))
#define ReadMFloat16(dptr) UInt16_Float16(ReadMUInt16(dptr))
#define ReadNFloat16(dptr) UInt16_Float16(ReadNUInt16(dptr))

FORCEINLINE Bool BytesEquals(const UInt8 *buff1, const UInt8 *buff2, UOSInt buffSize)
{
	UOSInt i = 0;
	while (i < buffSize)
	{
		if (buff1[i] != buff2[i])
			return false;
		i++;
	}
	return true;
}
#endif
