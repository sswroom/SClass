#ifndef _SM_DATA_RANDOMBYTESGENERATOR
#define _SM_DATA_RANDOMBYTESGENERATOR
#include "Data/RandomMT19937.h"

namespace Data
{
	class RandomBytesGenerator
	{
	private:
		Data::RandomMT19937 *random;
	public:
		RandomBytesGenerator();
		~RandomBytesGenerator();

		void NextBytes(UInt8 *buff, UOSInt len);
	};
}
#endif
