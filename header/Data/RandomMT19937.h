#ifndef _SM_DATA_RANDOMMT19937
#define _SM_DATA_RANDOMMT19937
namespace Data
{
	class RandomMT19937
	{
	private:
		UInt32 *mt;
		OSInt mt_index;

	public:
		RandomMT19937(UInt32 seed);
		virtual ~RandomMT19937();

		virtual Double NextDouble();
		virtual Int32 NextInt32();
	};
}
#endif
