#ifndef _SM_DATA_RANDOMOS
#define _SM_DATA_RANDOMOS
#include "Data/Random.h"

namespace Data
{
	class RandomOS : public Data::Random
	{
	public:
		RandomOS();
		virtual ~RandomOS();

		virtual Double NextDouble(); // 0 - 1
		virtual Int32 NextInt32();

		UInt32 NextInt15(); // 0 - 32767
		UInt32 NextInt30();
	};
}
#endif
