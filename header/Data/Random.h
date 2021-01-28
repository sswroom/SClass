#ifndef _SM_DATA_RANDOM
#define _SM_DATA_RANDOM
namespace Data
{
	class Random
	{
	public:
		virtual ~Random() {};

		virtual Double NextDouble() = 0; // 0 - 1
		virtual Int32 NextInt32() = 0;
	};
};
#endif
