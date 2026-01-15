#ifndef _SM_DATA_RANDOMBYTESGENERATOR
#define _SM_DATA_RANDOMBYTESGENERATOR
#include "Data/RandomMT19937.h"

namespace Data
{
	class RandomBytesGenerator
	{
	private:
		Optional<Data::RandomMT19937> random;
	public:
		RandomBytesGenerator();
		~RandomBytesGenerator();

		void NextBytes(UnsafeArray<UInt8> buff, UIntOS len);
	};
}
#endif
