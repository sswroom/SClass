#ifndef _SM_DATA_RANDOMMT19937
#define _SM_DATA_RANDOMMT19937
#include "Data/Random.h"
namespace Data
{
	class RandomMT19937 : public Data::Random
	{
	private:
		UnsafeArray<UInt32> mt;
		OSInt mt_index;

	public:
		RandomMT19937(UInt32 seed);
		virtual ~RandomMT19937();

		virtual Double NextDouble();
		virtual Int32 NextInt32();
	};
}
#endif
