#ifndef _SM_DATA_ARRAYLISTSTRINGNN
#define _SM_DATA_ARRAYLISTSTRINGNN
#include "Data/FastStringMapNative.hpp"
#include "Data/SortableArrayListNN.hpp"
#include "Text/String.h"

namespace Data
{
	class ArrayListStringNN : public Data::SortableArrayListNN<Text::String>
	{
	public:
		ArrayListStringNN();
		ArrayListStringNN(UIntOS capacity);

		virtual NN<Data::ArrayListNN<Text::String>> Clone() const;
		virtual IntOS Compare(NN<Text::String> obj1, NN<Text::String> obj2) const;
		virtual IntOS SortedIndexOfC(Text::CStringNN val) const;
		UIntOS IndexOfC(Text::CStringNN val) const;

		NN<Text::String> JoinString() const;
		NN<Text::String> JoinString(Text::CStringNN s) const;
		void FreeAll();
		void ValueCounts(NN<Data::FastStringMapNative<UInt32>> counts) const;
		void RemoveDuplicates(); //Assume sorted
	};
}
#endif
