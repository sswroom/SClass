#ifndef _SM_DATA_DATAARRAY
#define _SM_DATA_DATAARRAY
#include "Core/ByteTool_C.h"
#include "Data/ReadingList.hpp"

//#define CHECK_RANGE
#if defined(CHECK_RANGE)
#include <stdio.h>
#endif
namespace Data
{
	template <typename T> class DataArray : public Data::ReadingList<T>
	{
	protected:
		UnsafeArray<T> buff;
		UIntOS buffSize;
#if defined(CHECK_RANGE)
		UIntOS prevSize;
#endif

		void CheckError(UIntOS ofst) const
		{
#if defined(CHECK_RANGE)
			if (ofst > buffSize)
				printf("DataArray out of bound: ofst = %d, size = %d\r\n", (UInt32)ofst, (UInt32)this->buffSize);
#endif			
		}

		void CheckErrorPrev(UIntOS ofst) const
		{
#if defined(CHECK_RANGE)
			if (ofst > prevSize)
				printf("DataArray out of bound: prevOfst = %d, prevSize = %d\r\n", (UInt32)ofst, (UInt32)this->prevSize);
#endif			
		}

	public:
		DataArray() = default;
		DataArray(std::nullptr_t)
		{
			this->buffSize = 0;
		}

		DataArray(UnsafeArray<T> buff, UIntOS buffSize)
		{
			this->buff = buff;
			this->buffSize = buffSize;
#if defined(CHECK_RANGE)
			this->prevSize = 0;
#endif			
		}

		T &operator[](UIntOS index) const
		{
			CheckError(index + 1);
			return buff[index];
		}

		T &operator[](IntOS index) const
		{
#if defined(CHECK_RANGE)
			if (index < 0)
				CheckErrorPrev((UIntOS)-index);
			else
				CheckError((UIntOS)index + 1);
#endif
			return buff[index];
		}

#if _OSINT_SIZE == 64
		T &operator[](UInt32 index) const
		{
			CheckError(index + 1);
			return buff[index];
		}

		T &operator[](Int32 index) const
		{
#if defined(CHECK_RANGE)
			if (index < 0)
				CheckErrorPrev((UIntOS)-index);
			else
				CheckError((UIntOS)index + 1);
#endif
			return buff[index];
		}
#endif

		UnsafeArray<T> Arr() const
		{
			return this->buff;
		}

		UnsafeArray<T> ArrEnd() const
		{
			return this->buff + this->buffSize;
		}

		T *Ptr() const
		{
			return this->buff.Ptr();
		}

		UIntOS GetSize() const
		{
			return this->buffSize;
		}

#if defined(CHECK_RANGE)
		UIntOS GetPrevSize() const
		{
			return this->prevSize;
		}
#endif

		virtual UIntOS GetCount() const
		{
			return this->buffSize;
		}

		virtual T GetItem(UIntOS index) const
		{
			CheckError(index + 1);
			return buff[index];
		}

		Data::DataArray<T> SubArray(UIntOS index) const
		{
			CheckError(index);
			return DataArray<T>(&buff[index], this->buffSize - index);
		}

		Bool operator==(const DataArray<T> ptr)
		{
			return this->buff == ptr.buff && this->buffSize == ptr.buffSize;
		}

		static Data::DataArray<T> Alloc(UIntOS cnt)
		{
			return Data::DataArray<T>(MemAllocArr(T, cnt), cnt);
		}
	};
}
#endif