#ifndef _SM_DATA_DATASET
#define _SM_DATA_DATASET
#include "Data/ArrayList.h"
#include "Data/FastMapNN.h"
#include "Data/Comparator.h"
#include "Data/SortableArrayListNative.h"
#include "Data/TwinItem.h"
#include "Data/VariItem.h"

namespace Data
{
	class DataSet;
	template <class K> class DataSetGrouper
	{
	protected:
		NN<DataSet> ds;
	public:
		DataSetGrouper(NN<DataSet> ds) { this->ds = ds; }

	protected:
		virtual NN<Data::SortableArrayListNative<K>> CreateKeyIndex() const = 0;
		virtual UOSInt GetKeyIndex(NN<Data::SortableArrayListNative<K>> keyIndex, UOSInt dataIndex) const = 0;
	public:
		void Sum(NN<Data::ArrayList<Data::TwinItem<K, Double>>> result) const;
		void Count(NN<Data::ArrayList<Data::TwinItem<K, UInt32>>> result) const;
	};

	class DataSetMonthGrouper : public DataSetGrouper<Data::Timestamp>
	{
	public:
		DataSetMonthGrouper(NN<DataSet> ds) : DataSetGrouper<Data::Timestamp>(ds){};
	protected:
		virtual NN<Data::SortableArrayListNative<Data::Timestamp>> CreateKeyIndex() const;
		virtual UOSInt GetKeyIndex(NN<Data::SortableArrayListNative<Data::Timestamp>> keyIndex, UOSInt dataIndex) const;
	};

	class DataSet
	{
	private:
		UnsafeArray<VariItem> items;
		UOSInt capacity;
		UOSInt itemCnt;

	public:
		DataSet();
		~DataSet();

		void AddItem(const VariItem& key, const VariItem& value);
		UOSInt GetCount() const { return this->itemCnt; }
		Bool GetKey(UOSInt index, NN<VariItem> key) const { if (index < itemCnt) {key->Set(items[index << 1]); return true;} return false; }
		Bool GetValue(UOSInt index, NN<VariItem> val) const { if (index < itemCnt) {val->Set(items[(index << 1) + 1]); return true;} return false; }
		DataSetMonthGrouper GroupKeyByMonth() { return DataSetMonthGrouper(*this); }
		void ValueCounts(NN<Data::ArrayList<UInt32>> result) const;
		NN<DataSet> ValueCountsAsDS() const;
		void SortByValue(NN<Data::Comparator<NN<VariItem>>> comparator);
		void SortByKey(NN<Data::Comparator<NN<VariItem>>> comparator);
		void SortByValue();
		void SortByValueInv();
		void SortByKey();
		void SortByKeyInv();
	private:
		void Presort(UnsafeArray<VariItem> keyArr, UnsafeArray<VariItem> valArr, OSInt left, OSInt right, NN<Data::Comparator<NN<VariItem>>> comparator);
		void Sort(UnsafeArray<VariItem> keyArr, UnsafeArray<VariItem> valArr, OSInt firstIndex, OSInt lastIndex, NN<Data::Comparator<NN<VariItem>>> comparator);
		void ISortB(UnsafeArray<VariItem> keyArr, UnsafeArray<VariItem> valArr, OSInt firstIndex, OSInt lastIndex, NN<Data::Comparator<NN<VariItem>>> comparator);
	};

	template <class K> void DataSetGrouper<K>::Sum(NN<Data::ArrayList<Data::TwinItem<K, Double>>> result) const
	{
		NN<Data::SortableArrayListNative<K>> indexVal = this->CreateKeyIndex();
		Data::ArrayList<Double> sumVal;
		while (sumVal.GetCount() < indexVal->GetCount())
		{
			sumVal.Add(0);
		}
		VariItem item;
		UOSInt i = 0;
		UOSInt j = this->ds->GetCount();
		UOSInt k;
		while (i < j)
		{
			if (this->ds->GetValue(i, item))
			{
				k = this->GetKeyIndex(indexVal, i);
				sumVal.SetItem(k, sumVal.GetItem(k) + item.GetAsF64());
			}
			else
			{
				printf("Failed to get the value\r\n");
			}
			i++;
		}
		i = 0;
		j = indexVal->GetCount();
		while (i < j)
		{
			result->Add(Data::TwinItem<K, Double>(indexVal->GetItem(i), sumVal.GetItem(i)));
			i++;
		}
		indexVal.Delete();
	}

	template <class K> void DataSetGrouper<K>::Count(NN<Data::ArrayList<Data::TwinItem<K, UInt32>>> result) const
	{
		NN<Data::SortableArrayListNative<K>> indexVal = this->CreateKeyIndex();
		Data::ArrayList<UInt32> countVal;
		while (countVal.GetCount() < indexVal->GetCount())
		{
			countVal.Add(0);
		}
		VariItem item;
		UOSInt i = 0;
		UOSInt j = this->ds->GetCount();
		UOSInt k;
		while (i < j)
		{
			if (this->ds->GetValue(i, item))
			{
				k = this->GetKeyIndex(indexVal, i);
				countVal.SetItem(k, countVal.GetItem(k) + 1);
			}
			else
			{
				printf("Failed to get the value\r\n");
			}
			i++;
		}
		i = 0;
		j = indexVal->GetCount();
		while (i < j)
		{
			result->Add(Data::TwinItem<K, UInt32>(indexVal->GetItem(i), countVal.GetItem(i)));
			i++;
		}
		indexVal.Delete();
	}
}
#endif
