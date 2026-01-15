#ifndef _SM_DATA_ARRAYLISTDATE
#define _SM_DATA_ARRAYLISTDATE
#include "Data/DateTime.h"

namespace Data
{
	class ArrayListDate
	{
	private:
		UnsafeArray<Int64> arr;
		UIntOS objCnt;
		UIntOS capacity;

		void Init(UIntOS capacity);
	public:
		ArrayListDate();
		ArrayListDate(UIntOS capacity);
		~ArrayListDate();

		UIntOS Add(NN<Data::DateTime> val);
		void AddAll(NN<const Data::ArrayListDate> arr);
		void AddRange(UnsafeArray<NN<const Data::DateTime>> arr, UIntOS cnt);
		void RemoveAt(UIntOS index);
		void Insert(UIntOS index, NN<Data::DateTime> val);
		UIntOS SortedInsert(NN<Data::DateTime> val);
		IntOS SortedIndexOf(NN<Data::DateTime> val) const;
		UIntOS IndexOf(NN<Data::DateTime> val) const;
		void Clear();
		NN<ArrayListDate> Clone() const;

		UIntOS GetCount() const;
		UIntOS GetCapacity() const;

		void GetItem(UIntOS index, NN<Data::DateTime> valOut) const;
		void SetItem(UIntOS index, NN<Data::DateTime> val);
		void Sort(Bool decending);
	};
}
#endif
