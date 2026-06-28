#ifndef _SM_IO_PHYSICALMEM
#define _SM_IO_PHYSICALMEM

namespace IO
{
	class PhysicalMem
	{
	private:
		struct ClassData;
		Optional<ClassData> clsData;

	public:
		PhysicalMem(IntOS addr, IntOS size);
		~PhysicalMem();

		UnsafeArrayOpt<UInt8> GetPointer();
		Bool IsError();
	};
}
#endif
