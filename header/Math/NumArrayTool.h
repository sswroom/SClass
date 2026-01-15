#ifndef _SM_MATH_NUMARRAYTOOL
#define _SM_MATH_NUMARRAYTOOL
#include "Data/ArrayListDbl.h"
#include "Data/Random.h"

namespace Math
{
	class NumArrayTool
	{
	public:
		static void GenerateNormalRandom(NN<Data::ArrayListDbl> out, NN<Data::Random> random, Double average, Double stddev, UOSInt count);
		static void GenerateExponentialRandom(NN<Data::ArrayListDbl> out, NN<Data::Random> random, Double scale, UOSInt count);
		template<class K> static void RandomChoice(NN<Data::ArrayListNative<K>> out, NN<Data::Random> random, NN<Data::ReadingList<K>> srcList, UOSInt count)
		{
			UOSInt c = srcList->GetCount();
			while (count-- > 0)
			{
				out->Add(srcList->GetItem((UInt32)random->NextInt32() % c));
			}
		}
	};
}
#endif
