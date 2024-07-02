#ifndef _SM_DATA_ARRAYLISTDATE
#define _SM_DATA_ARRAYLISTDATE
#include "Data/DateTime.h"

namespace Data
{
	class ArrayListDate
	{
	private:
		UnsafeArray<Int64> arr;
		UOSInt objCnt;
		UOSInt capacity;

		void Init(UOSInt capacity);
	public:
		ArrayListDate();
		ArrayListDate(UOSInt capacity);
		~ArrayListDate();

		UOSInt Add(NN<Data::DateTime> val);
		void AddAll(NN<const Data::ArrayListDate> arr);
		void AddRange(UnsafeArray<NN<const Data::DateTime>> arr, UOSInt cnt);
		void RemoveAt(UOSInt index);
		void Insert(UOSInt index, NN<Data::DateTime> val);
		UOSInt SortedInsert(NN<Data::DateTime> val);
		OSInt SortedIndexOf(NN<Data::DateTime> val) const;
		UOSInt IndexOf(NN<Data::DateTime> val) const;
		void Clear();
		NN<ArrayListDate> Clone() const;

		UOSInt GetCount() const;
		UOSInt GetCapacity() const;

		void GetItem(UOSInt index, NN<Data::DateTime> valOut) const;
		void SetItem(UOSInt index, NN<Data::DateTime> val);
		void Sort(Bool decending);
	};
}
#endif
