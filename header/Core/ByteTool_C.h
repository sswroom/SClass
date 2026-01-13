#ifndef _SM_CORE_BYTETOOL_C
#define _SM_CORE_BYTETOOL_C
#include "MyMemory.h"

#if MEMORY_UNALIGNED

#define ReadNInt16(uint8Ptr) (*(Int16*)uint8Ptr)
#define ReadNUInt16(uint8Ptr) (*(UInt16*)uint8Ptr)
#define ReadNInt32(uint8Ptr) (*(Int32*)uint8Ptr)
#define ReadNUInt32(uint8Ptr) (*(UInt32*)uint8Ptr)
#define ReadNInt64(uint8Ptr) (*(Int64*)uint8Ptr)
#define ReadNUInt64(uint8Ptr) (*(UInt64*)uint8Ptr)
#define ReadNFloat(uint8Ptr) (*(Single*)uint8Ptr)
#define ReadNDouble(uint8Ptr) (*(Double*)uint8Ptr)
#define WriteNInt16(uint8Ptr, val) *(Int16*)(uint8Ptr) = (val)
#define WriteNUInt16(uint8Ptr, val) *(UInt16*)(uint8Ptr) = (val)
#define WriteNInt32(uint8Ptr, val) *(Int32*)(uint8Ptr) = (val)
#define WriteNUInt32(uint8Ptr, val) *(UInt32*)(uint8Ptr) = (val)
#define WriteNInt64(uint8Ptr, val) *(Int64*)(uint8Ptr) = (val)
#define WriteNUInt64(uint8Ptr, val) *(UInt64*)(uint8Ptr) = (val)
#define WriteNFloat(uint8Ptr, val) *(Single*)(uint8Ptr) = (val)
#define WriteNDouble(uint8Ptr, val) *(Double*)(uint8Ptr) = (val)

#define ReadInt64(uint8Ptr) (*(Int64*)(uint8Ptr))
#define ReadUInt64(uint8Ptr) (*(UInt64*)(uint8Ptr))
#define ReadMInt64(uint8Ptr) (BSWAP64(*(Int64*)(uint8Ptr)))
#define ReadMUInt64(uint8Ptr) (BSWAPU64(*(UInt64*)(uint8Ptr)))
#define WriteInt64(uint8Ptr, val) *(Int64*)(uint8Ptr) = (val)
#define WriteUInt64(uint8Ptr, val) *(UInt64*)(uint8Ptr) = (val)
#define WriteMInt64(uint8Ptr, val) *(Int64*)(uint8Ptr) = BSWAP64(val)
#define WriteMUInt64(uint8Ptr, val) WriteMInt64(uint8Ptr, (Int64)val)

#define ReadInt32(uint8Ptr) (*(Int32*)(uint8Ptr))
#define ReadUInt32(uint8Ptr) (*(UInt32*)(uint8Ptr))
#define ReadMInt32(uint8Ptr) (BSWAP32(ReadInt32(uint8Ptr)))
#define ReadMUInt32(uint8Ptr) ((UInt32)BSWAP32(ReadInt32(uint8Ptr)))
#define WriteInt32(uint8Ptr, val) *(Int32*)(uint8Ptr) = (val)
#define WriteUInt32(uint8Ptr, val) *(UInt32*)(uint8Ptr) = (val)
#define WriteMInt32(uint8Ptr, val) *(Int32*)(uint8Ptr) = BSWAP32(val)

#define ReadInt16(uint8Ptr) (*(Int16*)(uint8Ptr))
#define ReadUInt16(uint8Ptr) (*(UInt16*)(uint8Ptr))
#define ReadMInt16(uint8Ptr) (Int16)(((uint8Ptr)[0] << 8) | (uint8Ptr)[1])
#define ReadMUInt16(uint8Ptr) (UInt16)(((uint8Ptr)[0] << 8) | (uint8Ptr)[1])
#define WriteInt16(uint8Ptr, val) *(Int16*)(uint8Ptr) = (Int16)((val) & 0xffff)
#define WriteUInt16(uint8Ptr, val) *(UInt16*)(uint8Ptr) = (UInt16)((val) & 0xffff)
#define WriteMInt16(uint8Ptr, val) {(uint8Ptr)[0] = (UInt8)(((val) >> 8) & 0xff); (uint8Ptr)[1] = (UInt8)((val) & 0xff);}
#define WriteMUInt16(uint8Ptr, val) {(uint8Ptr)[0] = (UInt8)(((val) >> 8) & 0xff); (uint8Ptr)[1] = (UInt8)((val) & 0xff);}

#define ReadDouble(uint8Ptr) (*(Double*)(uint8Ptr))
FORCEINLINE Double ReadMDouble(UnsafeArray<const UInt8> dptr)
{
	UInt8 tmpBuff[8];
	WriteInt32(&tmpBuff[4], ReadMInt32(&dptr[0]));
	WriteInt32(&tmpBuff[0], ReadMInt32(&dptr[4]));
	return *(Double*)tmpBuff;
}
#define WriteDouble(uint8Ptr, val) *(Double*)(uint8Ptr) = (val)

#define ReadFloat(uint8Ptr) (*(Single*)(uint8Ptr))
FORCEINLINE Single ReadMFloat(UnsafeArray<const UInt8> dptr)
{
	UInt8 tmpBuff[4];
	WriteInt32(&tmpBuff[0], ReadMInt32(&dptr[0]));
	return *(Single*)tmpBuff;
}
#define WriteFloat(uint8Ptr, val) *(Single*)(uint8Ptr) = (val)
FORCEINLINE void WriteMFloat(UnsafeArray<UInt8> uint8Ptr, Single val)
{
	UInt8 tmpBuff[4];
	WriteFloat(tmpBuff, val);
	uint8Ptr[0] = tmpBuff[3];
	uint8Ptr[1] = tmpBuff[2];
	uint8Ptr[2] = tmpBuff[1];
	uint8Ptr[3] = tmpBuff[0];
}

#else
FORCEINLINE Int16 ReadNInt16(UnsafeArray<const UInt8> dptr)
{
	UInt8 tmpBuff[2];
	tmpBuff[0] = dptr[0];
	tmpBuff[1] = dptr[1];
	return *(Int16*)tmpBuff;
}

FORCEINLINE UInt16 ReadNUInt16(UnsafeArray<const UInt8> dptr)
{
	UInt8 tmpBuff[2];
	tmpBuff[0] = dptr[0];
	tmpBuff[1] = dptr[1];
	return *(UInt16*)tmpBuff;
}

FORCEINLINE Int32 ReadNInt32(UnsafeArray<const UInt8> dptr)
{
	UInt8 tmpBuff[4];
	tmpBuff[0] = dptr[0];
	tmpBuff[1] = dptr[1];
	tmpBuff[2] = dptr[2];
	tmpBuff[3] = dptr[3];
	return *(Int32*)tmpBuff;
}

FORCEINLINE UInt32 ReadNUInt32(UnsafeArray<const UInt8> dptr)
{
	UInt8 tmpBuff[4];
	tmpBuff[0] = dptr[0];
	tmpBuff[1] = dptr[1];
	tmpBuff[2] = dptr[2];
	tmpBuff[3] = dptr[3];
	return *(UInt32*)tmpBuff;
}

FORCEINLINE Int64 ReadNInt64(UnsafeArray<const UInt8> dptr)
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

FORCEINLINE Int64 ReadNUInt64(UnsafeArray<const UInt8> dptr)
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
	return *(UInt64*)tmpBuff;
}

FORCEINLINE void WriteNInt16(UnsafeArray<UInt8> dptr, Int16 val)
{
	UInt8 *vPtr = (UInt8*)&val;
	dptr[0] = vPtr[0];
	dptr[1] = vPtr[1];
}

FORCEINLINE void WriteNUInt16(UnsafeArray<UInt8> dptr, UInt16 val)
{
	UInt8 *vPtr = (UInt8*)&val;
	dptr[0] = vPtr[0];
	dptr[1] = vPtr[1];
}

FORCEINLINE void WriteNInt32(UnsafeArray<UInt8> dptr, Int32 val)
{
	UInt8 *vPtr = (UInt8*)&val;
	dptr[0] = vPtr[0];
	dptr[1] = vPtr[1];
	dptr[2] = vPtr[2];
	dptr[3] = vPtr[3];
}

FORCEINLINE void WriteNUInt32(UnsafeArray<UInt8> dptr, UInt32 val)
{
	UInt8 *vPtr = (UInt8*)&val;
	dptr[0] = vPtr[0];
	dptr[1] = vPtr[1];
	dptr[2] = vPtr[2];
	dptr[3] = vPtr[3];
}

FORCEINLINE void WriteNInt64(UnsafeArray<UInt8> dptr, Int64 val)
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

FORCEINLINE void WriteNUInt64(UnsafeArray<UInt8> dptr, UInt64 val)
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

#if IS_BYTEORDER_LE != 0
FORCEINLINE Int64 ReadInt64(UnsafeArray<const UInt8> dptr)
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

FORCEINLINE Int64 ReadMInt64(UnsafeArray<const UInt8> dptr)
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

FORCEINLINE void WriteInt64(UnsafeArray<UInt8> dptr, Int64 val)
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

FORCEINLINE void WriteUInt64(UnsafeArray<UInt8> dptr, UInt64 val)
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

FORCEINLINE void WriteMInt64(UnsafeArray<UInt8> dptr, Int64 val)
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

FORCEINLINE void WriteMUInt64(UnsafeArray<UInt8> dptr, UInt64 val)
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

FORCEINLINE Int32 ReadInt32(UnsafeArray<const UInt8> dptr)
{
	UInt8 tmpBuff[4];
	tmpBuff[0] = dptr[0];
	tmpBuff[1] = dptr[1];
	tmpBuff[2] = dptr[2];
	tmpBuff[3] = dptr[3];
	return *(Int32*)tmpBuff;
}

FORCEINLINE UInt32 ReadUInt32(UnsafeArray<const UInt8> dptr)
{
	UInt8 tmpBuff[4];
	tmpBuff[0] = dptr[0];
	tmpBuff[1] = dptr[1];
	tmpBuff[2] = dptr[2];
	tmpBuff[3] = dptr[3];
	return *(UInt32*)tmpBuff;
}

FORCEINLINE Int32 ReadMInt32(UnsafeArray<const UInt8> dptr)
{
	UInt8 tmpBuff[4];
	tmpBuff[0] = dptr[3];
	tmpBuff[1] = dptr[2];
	tmpBuff[2] = dptr[1];
	tmpBuff[3] = dptr[0];
	return *(Int32*)tmpBuff;
}

FORCEINLINE UInt32 ReadMUInt32(UnsafeArray<const UInt8> dptr)
{
	UInt8 tmpBuff[4];
	tmpBuff[0] = dptr[3];
	tmpBuff[1] = dptr[2];
	tmpBuff[2] = dptr[1];
	tmpBuff[3] = dptr[0];
	return *(UInt32*)tmpBuff;
}

FORCEINLINE void WriteInt32(UnsafeArray<UInt8> dptr, Int32 val)
{
	UInt8 *vPtr = (UInt8*)&val;
	dptr[0] = vPtr[0];
	dptr[1] = vPtr[1];
	dptr[2] = vPtr[2];
	dptr[3] = vPtr[3];
}

FORCEINLINE void WriteUInt32(UnsafeArray<UInt8> dptr, UInt32 val)
{
	UInt8 *vPtr = (UInt8*)&val;
	dptr[0] = vPtr[0];
	dptr[1] = vPtr[1];
	dptr[2] = vPtr[2];
	dptr[3] = vPtr[3];
}

FORCEINLINE void WriteMInt32(UnsafeArray<UInt8> dptr, Int32 val)
{
	UInt8 *vPtr = (UInt8*)&val;
	dptr[0] = vPtr[3];
	dptr[1] = vPtr[2];
	dptr[2] = vPtr[1];
	dptr[3] = vPtr[0];
}

FORCEINLINE Int16 ReadInt16(UnsafeArray<const UInt8> dptr)
{
	UInt8 tmpBuff[2];
	tmpBuff[0] = dptr[0];
	tmpBuff[1] = dptr[1];
	return *(Int16*)tmpBuff;
}

FORCEINLINE UInt16 ReadUInt16(UnsafeArray<const UInt8> dptr)
{
	UInt8 tmpBuff[2];
	tmpBuff[0] = dptr[0];
	tmpBuff[1] = dptr[1];
	return *(UInt16*)tmpBuff;
}

FORCEINLINE Int16 ReadMInt16(UnsafeArray<const UInt8> dptr)
{
	UInt8 tmpBuff[2];
	tmpBuff[0] = dptr[1];
	tmpBuff[1] = dptr[0];
	return *(Int16*)tmpBuff;
}

FORCEINLINE UInt16 ReadMUInt16(UnsafeArray<const UInt8> dptr)
{
	UInt8 tmpBuff[2];
	tmpBuff[0] = dptr[1];
	tmpBuff[1] = dptr[0];
	return *(UInt16*)tmpBuff;
}

FORCEINLINE void WriteInt16(UnsafeArray<UInt8> dptr, Int16 val)
{
	UInt8 *vPtr = (UInt8*)&val;
	dptr[0] = vPtr[0];
	dptr[1] = vPtr[1];
}

FORCEINLINE void WriteUInt16(UnsafeArray<UInt8> dptr, UInt16 val)
{
	UInt8 *vPtr = (UInt8*)&val;
	dptr[0] = vPtr[0];
	dptr[1] = vPtr[1];
}

FORCEINLINE void WriteMInt16(UnsafeArray<UInt8> dptr, Int16 val)
{
	UInt8 *vPtr = (UInt8*)&val;
	dptr[0] = vPtr[1];
	dptr[1] = vPtr[0];
}

FORCEINLINE void WriteMUInt16(UnsafeArray<UInt8> dptr, UInt16 val)
{
	UInt8 *vPtr = (UInt8*)&val;
	dptr[0] = vPtr[1];
	dptr[1] = vPtr[0];
}

FORCEINLINE Double ReadDouble(UnsafeArray<const UInt8> dptr)
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

FORCEINLINE Double ReadMDouble(UnsafeArray<const UInt8> dptr)
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

FORCEINLINE void WriteDouble(UnsafeArray<UInt8> dptr, Double val)
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

FORCEINLINE void WriteMDouble(UnsafeArray<UInt8> dptr, Double val)
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

FORCEINLINE Single ReadFloat(UnsafeArray<const UInt8> dptr)
{
	UInt8 tmpBuff[4];
	tmpBuff[0] = dptr[0];
	tmpBuff[1] = dptr[1];
	tmpBuff[2] = dptr[2];
	tmpBuff[3] = dptr[3];
	return *(Single*)tmpBuff;
}

FORCEINLINE Single ReadMFloat(UnsafeArray<const UInt8> dptr)
{
	UInt8 tmpBuff[4];
	tmpBuff[0] = dptr[3];
	tmpBuff[1] = dptr[2];
	tmpBuff[2] = dptr[1];
	tmpBuff[3] = dptr[0];
	return *(Single*)tmpBuff;
}

FORCEINLINE void WriteFloat(UnsafeArray<UInt8> dptr, Single val)
{
	UInt8 *vPtr = (UInt8*)&val;
	dptr[0] = vPtr[0];
	dptr[1] = vPtr[1];
	dptr[2] = vPtr[2];
	dptr[3] = vPtr[3];
}

FORCEINLINE void WriteMFloat(UnsafeArray<UInt8> uint8Ptr, Single val)
{
	UInt8 *vPtr = (UInt8*)&val;
	uint8Ptr[0] = vPtr[3];
	uint8Ptr[1] = vPtr[2];
	uint8Ptr[2] = vPtr[1];
	uint8Ptr[3] = vPtr[0];
}

#define ReadNFloat(uint8Ptr) ReadFloat(uint8Ptr)
#define ReadNDouble(uint8Ptr) ReadDouble(uint8Ptr)
#define WriteNFloat(uint8Ptr, val) WriteFloat(uint8Ptr, val)
#define WriteNDouble(uint8Ptr, val) WriteDouble(uint8Ptr, val)

#else
FORCEINLINE Int64 ReadInt64(UnsafeArray<const UInt8> dptr)
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
#define ReadUInt64(uint8Ptr) (UInt64)ReadInt64(uint8Ptr)

FORCEINLINE Int64 ReadMInt64(UnsafeArray<const UInt8> dptr)
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
#define ReadMUInt64(uint8Ptr) (UInt64)ReadMInt64(uint8Ptr)

FORCEINLINE void WriteInt64(UnsafeArray<UInt8> dptr, Int64 val)
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

FORCEINLINE void WriteUInt64(UnsafeArray<UInt8> dptr, UInt64 val)
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

FORCEINLINE void WriteMInt64(UnsafeArray<UInt8> dptr, Int64 val)
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

FORCEINLINE void WriteMUInt64(UnsafeArray<UInt8> dptr, UInt64 val)
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

FORCEINLINE Int32 ReadInt32(UnsafeArray<const UInt8> dptr)
{
	UInt8 tmpBuff[4];
	tmpBuff[0] = dptr[3];
	tmpBuff[1] = dptr[2];
	tmpBuff[2] = dptr[1];
	tmpBuff[3] = dptr[0];
	return *(Int32*)tmpBuff;
}

FORCEINLINE UInt32 ReadUInt32(UnsafeArray<const UInt8> dptr)
{
	UInt8 tmpBuff[4];
	tmpBuff[0] = dptr[3];
	tmpBuff[1] = dptr[2];
	tmpBuff[2] = dptr[1];
	tmpBuff[3] = dptr[0];
	return *(UInt32*)tmpBuff;
}

FORCEINLINE Int32 ReadMInt32(UnsafeArray<const UInt8> dptr)
{
	UInt8 tmpBuff[4];
	tmpBuff[0] = dptr[0];
	tmpBuff[1] = dptr[1];
	tmpBuff[2] = dptr[2];
	tmpBuff[3] = dptr[3];
	return *(Int32*)tmpBuff;
}

FORCEINLINE UInt32 ReadMUInt32(UnsafeArray<const UInt8> dptr)
{
	UInt8 tmpBuff[4];
	tmpBuff[0] = dptr[0];
	tmpBuff[1] = dptr[1];
	tmpBuff[2] = dptr[2];
	tmpBuff[3] = dptr[3];
	return *(UInt32*)tmpBuff;
}

FORCEINLINE void WriteInt32(UnsafeArray<UInt8> dptr, Int32 val)
{
	UInt8 *vPtr = (UInt8*)&val;
	dptr[0] = vPtr[3];
	dptr[1] = vPtr[2];
	dptr[2] = vPtr[1];
	dptr[3] = vPtr[0];
}

FORCEINLINE void WriteUInt32(UnsafeArray<UInt8> dptr, UInt32 val)
{
	UInt8 *vPtr = (UInt8*)&val;
	dptr[0] = vPtr[3];
	dptr[1] = vPtr[2];
	dptr[2] = vPtr[1];
	dptr[3] = vPtr[0];
}

FORCEINLINE void WriteMInt32(UnsafeArray<UInt8> dptr, Int32 val)
{
	UInt8 *vPtr = (UInt8*)&val;
	dptr[0] = vPtr[0];
	dptr[1] = vPtr[1];
	dptr[2] = vPtr[2];
	dptr[3] = vPtr[3];
}

FORCEINLINE Int16 ReadInt16(UnsafeArray<const UInt8> dptr)
{
	UInt8 tmpBuff[2];
	tmpBuff[0] = dptr[1];
	tmpBuff[1] = dptr[0];
	return *(Int16*)tmpBuff;
}

FORCEINLINE UInt16 ReadUInt16(UnsafeArray<const UInt8> dptr)
{
	UInt8 tmpBuff[2];
	tmpBuff[0] = dptr[1];
	tmpBuff[1] = dptr[0];
	return *(UInt16*)tmpBuff;
}

FORCEINLINE Int16 ReadMInt16(UnsafeArray<const UInt8> dptr)
{
	UInt8 tmpBuff[2];
	tmpBuff[0] = dptr[0];
	tmpBuff[1] = dptr[1];
	return *(Int16*)tmpBuff;
}

FORCEINLINE UInt16 ReadMUInt16(UnsafeArray<const UInt8> dptr)
{
	UInt8 tmpBuff[2];
	tmpBuff[0] = dptr[0];
	tmpBuff[1] = dptr[1];
	return *(UInt16*)tmpBuff;
}

FORCEINLINE void WriteInt16(UnsafeArray<UInt8> dptr, Int16 val)
{
	UInt8 *vPtr = (UInt8*)&val;
	dptr[0] = vPtr[1];
	dptr[1] = vPtr[0];
}

FORCEINLINE void WriteUInt16(UnsafeArray<UInt8> dptr, UInt16 val)
{
	UInt8 *vPtr = (UInt8*)&val;
	dptr[0] = vPtr[1];
	dptr[1] = vPtr[0];
}

FORCEINLINE void WriteMInt16(UnsafeArray<UInt8> dptr, Int16 val)
{
	UInt8 *vPtr = (UInt8*)&val;
	dptr[0] = vPtr[0];
	dptr[1] = vPtr[1];
}

FORCEINLINE void WriteMUInt16(UnsafeArray<UInt8> dptr, UInt16 val)
{
	UInt8 *vPtr = (UInt8*)&val;
	dptr[0] = vPtr[0];
	dptr[1] = vPtr[1];
}

FORCEINLINE Double ReadDouble(UnsafeArray<const UInt8> dptr)
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

FORCEINLINE Double ReadMDouble(UnsafeArray<const UInt8> dptr)
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

FORCEINLINE void WriteDouble(UnsafeArray<UInt8> dptr, Double val)
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

FORCEINLINE void WriteMDouble(UnsafeArray<UInt8> dptr, Double val)
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

FORCEINLINE Single ReadFloat(UnsafeArray<const UInt8> dptr)
{
	UInt8 tmpBuff[4];
	tmpBuff[0] = dptr[3];
	tmpBuff[1] = dptr[2];
	tmpBuff[2] = dptr[1];
	tmpBuff[3] = dptr[0];
	return *(Single*)tmpBuff;
}

FORCEINLINE Single ReadMFloat(UnsafeArray<const UInt8> dptr)
{
	UInt8 tmpBuff[4];
	tmpBuff[0] = dptr[0];
	tmpBuff[1] = dptr[1];
	tmpBuff[2] = dptr[2];
	tmpBuff[3] = dptr[3];
	return *(Single*)tmpBuff;
}

FORCEINLINE void WriteFloat(UnsafeArray<UInt8> dptr, Single val)
{
	UInt8 *vPtr = (UInt8*)&val;
	dptr[0] = vPtr[3];
	dptr[1] = vPtr[2];
	dptr[2] = vPtr[1];
	dptr[3] = vPtr[0];
}

FORCEINLINE void WriteMFloat(UnsafeArray<UInt8> dptr, Single val)
{
	UInt8 *vPtr = (UInt8*)&val;
	dptr[0] = vPtr[0];
	dptr[1] = vPtr[1];
	dptr[2] = vPtr[2];
	dptr[3] = vPtr[3];
}

#define ReadNFloat(uint8Ptr) ReadMFloat(uint8Ptr)
#define ReadNDouble(uint8Ptr) ReadMDouble(uint8Ptr)
#define WriteNFloat(uint8Ptr, val) WriteMFloat(uint8Ptr, val)
#define WriteNDouble(uint8Ptr, val) WriteMDouble(uint8Ptr, val)
#endif
#endif
#define ReadUInt24(uint8Ptr) (UInt32)((ReadUInt16(uint8Ptr + 1) << 8) | (uint8Ptr)[0])
#define ReadInt24(uint8Ptr) ((ReadInt16(uint8Ptr + 1) << 8) | (uint8Ptr)[0])
#define ReadMInt24(uint8Ptr) ((ReadMInt16(uint8Ptr) << 8) | (uint8Ptr)[2])
#define ReadMUInt24(uint8Ptr) ((UInt32)((uint8Ptr)[0] << 16) | ReadMUInt16(uint8Ptr + 1))
#define ReadNUInt24(uint8Ptr) ReadUInt24(uint8Ptr)
#define ReadNInt24(uint8Ptr) ReadInt24(uint8Ptr)
#define WriteInt24(uint8Ptr, val) {(uint8Ptr)[0] = (UInt8)((val) & 0xff); (uint8Ptr)[1] = (UInt8)(((val) >> 8) & 0xff); (uint8Ptr)[2] = (UInt8)(((val) >> 16) & 0xff);}
#define WriteMInt24(uint8Ptr, val) {(uint8Ptr)[0] = (UInt8)(((val) >> 16) & 0xff); (uint8Ptr)[1] = (UInt8)(((val) >> 8) & 0xff); (uint8Ptr)[2] = (UInt8)((val) & 0xff);}

FORCEINLINE UInt8 ByteTool_GetBCD8(UInt8 bcd)
{
	return (UInt8)((bcd >> 4) * 10 + (bcd & 0xf));
}

FORCEINLINE UInt32 ByteTool_GetBCD32(UnsafeArray<const UInt8> bcd)
{
	return (UInt32)((bcd[0] >> 4) * 10000000 + (bcd[0] & 0xf) * 1000000 + (bcd[1] >> 4) * 100000 + (bcd[1] & 0xf) * 10000 + (bcd[2] >> 4) * 1000 + (bcd[2] & 0xf) * 100 + (bcd[3] >> 4) * 10 + (bcd[3] & 0xf));
}

FORCEINLINE UInt8 ByteTool_Int2BCDB(UInt32 val)
{
	UInt32 tmp = val / 10;
	return (UInt8)(((tmp % 10) << 4) + (val % 10));
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

FORCEINLINE void AddEqU8(UnsafeArray<UInt8> dptr, UInt8 val)
{
	*dptr = (UInt8)(*dptr + val);
}

FORCEINLINE void SubEqU16(UInt16 *dptr, UInt16 val)
{
	*dptr = (UInt16)(*dptr - val);
}

FORCEINLINE void SubEqU8(UnsafeArray<UInt8> dptr, UInt8 val)
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

FORCEINLINE void ShlEqU8(UnsafeArray<UInt8> dptr, UInt32 val)
{
	*dptr = (UInt8)(*dptr << val);
}

FORCEINLINE void SarEq8(Int8 *dptr, UInt32 val)
{
	*dptr = (Int8)(*dptr >> val);
}

FORCEINLINE void ShrEq8(UnsafeArray<UInt8> dptr, UInt32 val)
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
#define ReadSingle(dptr) ReadFloat(dptr)
#define ReadMSingle(dptr) ReadMFloat(dptr)
#define ReadNSingle(dptr) ReadNFloat(dptr)

FORCEINLINE Bool BytesEquals(UnsafeArray<const UInt8> buff1, UnsafeArray<const UInt8> buff2, UOSInt buffSize)
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
