#ifndef _SM_DATA_ARRAYLISTDATE
#define _SM_DATA_ARRAYLISTDATE
#include "Data/DateTime.h"

namespace Data
{
	class ArrayListDate
	{
	private:
		Int64 *arr;
		UOSInt objCnt;
		UOSInt capacity;

		void Init(UOSInt Capacity);
	public:
		ArrayListDate();
		ArrayListDate(UOSInt Capacity);
		~ArrayListDate();

		UOSInt Add(Data::DateTime *Val);
		void AddRange(const Data::ArrayListDate *arr);
		void AddRange(const Data::DateTime **arr, UOSInt cnt);
		void RemoveAt(UOSInt Index);
		void Insert(UOSInt Index, Data::DateTime *Val);
		UOSInt SortedInsert(Data::DateTime *Val);
		OSInt SortedIndexOf(Data::DateTime *Val) const;
		UOSInt IndexOf(Data::DateTime *Val) const;
		void Clear();
		ArrayListDate *Clone() const;

		UOSInt GetCount() const;
		UOSInt GetCapacity() const;

		void GetItem(UOSInt Index, Data::DateTime *ValOut) const;
		void SetItem(UOSInt Index, Data::DateTime *Val);
		void Sort(Bool decending);
	};
}
#endif
