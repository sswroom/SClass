#ifndef _SM_DATA_DATAARRAY
#define _SM_DATA_DATAARRAY
#include "Data/ByteTool.h"
#include "Data/ReadingList.h"

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
		UOSInt buffSize;
#if defined(CHECK_RANGE)
		UOSInt prevSize;
#endif

		void CheckError(UOSInt ofst) const
		{
#if defined(CHECK_RANGE)
			if (ofst > buffSize)
				printf("DataArray out of bound: ofst = %d, size = %d\r\n", (UInt32)ofst, (UInt32)this->buffSize);
#endif			
		}

		void CheckErrorPrev(UOSInt ofst) const
		{
#if defined(CHECK_RANGE)
			if (ofst > prevSize)
				printf("DataArray out of bound: prevOfst = %d, prevSize = %d\r\n", (UInt32)ofst, (UInt32)this->prevSize);
#endif			
		}

	public:
		DataArray() = default;
		DataArray(UnsafeArray<T> buff, UOSInt buffSize)
		{
			this->buff = buff;
			this->buffSize = buffSize;
#if defined(CHECK_RANGE)
			this->prevSize = 0;
#endif			
		}

		T &operator[](UOSInt index) const
		{
			CheckError(index + 1);
			return buff[index];
		}

		T &operator[](OSInt index) const
		{
#if defined(CHECK_RANGE)
			if (index < 0)
				CheckErrorPrev((UOSInt)-index);
			else
				CheckError((UOSInt)index + 1);
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
				CheckErrorPrev((UOSInt)-index);
			else
				CheckError((UOSInt)index + 1);
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

		UOSInt GetSize() const
		{
			return this->buffSize;
		}

#if defined(CHECK_RANGE)
		UOSInt GetPrevSize() const
		{
			return this->prevSize;
		}
#endif

		virtual UOSInt GetCount() const
		{
			return this->buffSize;
		}

		virtual T GetItem(UOSInt index) const
		{
			CheckError(index + 1);
			return buff[index];
		}

		Data::DataArray<T> SubArray(UOSInt index) const
		{
			CheckError(index);
			return DataArray<T>(&buff[index], this->buffSize - index);
		}
	};
}
#endif