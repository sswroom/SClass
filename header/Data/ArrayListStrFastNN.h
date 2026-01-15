#ifndef _SM_DATA_ARRAYLISTSTRFASTNN
#define _SM_DATA_ARRAYLISTSTRFASTNN
#include "Data/SortableArrayListNN.hpp"
#include "Text/String.h"

namespace Data
{
	class ArrayListStrFastNN : public Data::SortableArrayListNN<Text::String>
	{
	public:
		ArrayListStrFastNN();
		ArrayListStrFastNN(UIntOS capacity);

		virtual NN<Data::ArrayListNN<Text::String>> Clone() const;
		virtual IntOS Compare(NN<Text::String> obj1, NN<Text::String> obj2) const;
		IntOS SortedIndexOf(Text::CStringNN str) const;

		void FreeAll();
	};
}
#endif
