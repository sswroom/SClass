#ifndef _SM_DATA_ARRAYLISTICASESTRINGNN
#define _SM_DATA_ARRAYLISTICASESTRINGNN
#include "Data/ArrayListStringNN.h"

namespace Data
{
	class ArrayListICaseStringNN : public Data::ArrayListStringNN
	{
	public:
		ArrayListICaseStringNN();
		ArrayListICaseStringNN(UOSInt capacity);

		virtual NN<Data::ArrayListNN<Text::String>> Clone() const;
		virtual OSInt Compare(NN<Text::String> obj1, NN<Text::String> obj2) const;
		virtual OSInt SortedIndexOfC(Text::CStringNN val) const;
	};
}
#endif
