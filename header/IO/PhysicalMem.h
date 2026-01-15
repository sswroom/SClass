#ifndef _SM_IO_PHYSICALMEM
#define _SM_IO_PHYSICALMEM

namespace IO
{
	class PhysicalMem
	{
	private:
		struct ClassData;
		ClassData *clsData;

	public:
		PhysicalMem(IntOS addr, IntOS size);
		~PhysicalMem();

		UInt8 *GetPointer();
		Bool IsError();
	};
}
#endif
