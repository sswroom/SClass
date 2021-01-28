#ifndef _SM_DATA_ARRAYLISTDATE
#define _SM_DATA_ARRAYLISTDATE
#include "Data/DateTime.h"

namespace Data
{
	class ArrayListDate
	{
	private:
		Int64 *arr;
		OSInt objCnt;
		OSInt capacity;

		void Init(OSInt Capacity);
	public:
		ArrayListDate();
		ArrayListDate(OSInt Capacity);
		~ArrayListDate();

		OSInt Add(Data::DateTime *Val);
		void AddRange(Data::ArrayListDate *arr);
		void AddRange(Data::DateTime **arr, OSInt cnt);
		void RemoveAt(OSInt Index);
		void Insert(OSInt Index, Data::DateTime *Val);
		OSInt SortedInsert(Data::DateTime *Val);
		OSInt SortedIndexOf(Data::DateTime *Val);
		OSInt IndexOf(Data::DateTime *Val);
		void Clear();
		ArrayListDate *Clone();

		OSInt GetCount();
		OSInt GetCapacity();

		void GetItem(OSInt Index, Data::DateTime *ValOut);
		void SetItem(OSInt Index, Data::DateTime *Val);
		void Sort(Bool decending);
	};
}
#endif
