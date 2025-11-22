#ifndef _SM_DATA_BYTEARRAYBASE
#define _SM_DATA_BYTEARRAYBASE
#include "MyMemory.h"
#include "Core/ByteTool_C.h"
#include "Data/BinTool.h"
#include "Data/DataArray.hpp"

namespace Data
{
	template <typename T> class ByteArrayBase : public DataArray<T>
	{
	protected:
		ByteArrayBase() = default;
		ByteArrayBase(UnsafeArray<T> buff, UOSInt buffSize) : DataArray<T>(buff, buffSize)
		{
		}

	public:
		T &operator *()
		{
			this->CheckError(1);
			return this->buff[0];
		}

		Bool operator<=(const ByteArrayBase<T> &buff) const
		{
			return this->buff.Ptr() <= buff.buff;
		}

		Bool operator<(const ByteArrayBase<T> &buff) const
		{
			return this->buff.Ptr() < buff.buff;
		}

		Bool operator>=(const ByteArrayBase<T> &buff) const
		{
			return this->buff.Ptr() >= buff.buff;
		}

		Bool operator>(const ByteArrayBase<T> &buff) const
		{
			return this->buff.Ptr() > buff.buff;
		}

		Int16 ReadI16(UOSInt index) const
		{
			this->CheckError(index + 2);
			return ReadInt16(&this->buff[index]);
		}

		UInt16 ReadU16(UOSInt index) const
		{
			this->CheckError(index + 2);
			return ReadUInt16(&this->buff[index]);
		}

		Int16 ReadMI16(UOSInt index) const
		{
			this->CheckError(index + 2);
			return ReadMInt16(&this->buff[index]);
		}

		UInt16 ReadMU16(UOSInt index) const
		{
			this->CheckError(index + 2);
			return ReadMUInt16(&this->buff[index]);
		}

		Int16 ReadNI16(UOSInt index) const
		{
			this->CheckError(index + 2);
			return ReadNInt16(&this->buff[index]);
		}

		UInt16 ReadNU16(UOSInt index) const
		{
			this->CheckError(index + 2);
			return ReadNUInt16(&this->buff[index]);
		}

		Int32 ReadI24(UOSInt index) const
		{
			this->CheckError(index + 3);
			return ReadInt24(&this->buff[index]);
		}

		UInt32 ReadU24(UOSInt index) const
		{
			this->CheckError(index + 3);
			return ReadUInt24(&this->buff[index]);
		}

		Int32 ReadMI24(UOSInt index) const
		{
			this->CheckError(index + 3);
			return ReadMInt24(&this->buff[index]);
		}

		UInt32 ReadMU24(UOSInt index) const
		{
			this->CheckError(index + 3);
			return ReadMUInt24(&this->buff[index]);
		}

		Int32 ReadNI24(UOSInt index) const
		{
			this->CheckError(index + 3);
			return ReadNInt24(&this->buff[index]);
		}

		UInt32 ReadNU24(UOSInt index) const
		{
			this->CheckError(index + 3);
			return ReadNUInt24(&this->buff[index]);
		}

		Int32 ReadI32(UOSInt index) const
		{
			this->CheckError(index + 4);
			return ReadInt32(&this->buff[index]);
		}

		UInt32 ReadU32(UOSInt index) const
		{
			this->CheckError(index + 4);
			return ReadUInt32(&this->buff[index]);
		}

		Int32 ReadMI32(UOSInt index) const
		{
			this->CheckError(index + 4);
			return ReadMInt32(&this->buff[index]);
		}

		UInt32 ReadMU32(UOSInt index) const
		{
			this->CheckError(index + 4);
			return ReadMUInt32(&this->buff[index]);
		}

		Int32 ReadNI32(UOSInt index) const
		{
			this->CheckError(index + 4);
			return ReadNInt32(&this->buff[index]);
		}

		UInt32 ReadNU32(UOSInt index) const
		{
			this->CheckError(index + 4);
			return ReadNUInt32(&this->buff[index]);
		}

#ifdef HAS_INT64
		Int64 ReadI64(UOSInt index) const
		{
			this->CheckError(index + 8);
			return ReadInt64(&this->buff[index]);
		}

		UInt64 ReadU64(UOSInt index) const
		{
			this->CheckError(index + 8);
			return ReadUInt64(&this->buff[index]);
		}

		Int64 ReadMI64(UOSInt index) const
		{
			this->CheckError(index + 8);
			return ReadMInt64(&this->buff[index]);
		}

		UInt64 ReadMU64(UOSInt index) const
		{
			this->CheckError(index + 8);
			return ReadMUInt64(&this->buff[index]);
		}

		Int64 ReadNI64(UOSInt index) const
		{
			this->CheckError(index + 8);
			return ReadNInt64(&this->buff[index]);
		}

		UInt64 ReadNU64(UOSInt index) const
		{
			this->CheckError(index + 8);
			return ReadNUInt64(&this->buff[index]);
		}
#endif

		Bool StartsWith(ByteArrayBase arr)
		{
			return this->buffSize >= arr.buffSize && Data::BinTool::Equals(this->buff, arr.buff, arr.buffSize);
		}
	};
}
#endif