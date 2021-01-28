#ifndef _SM_IO_PHYSICALMEM
#define _SM_IO_PHYSICALMEM

namespace IO
{
	class PhysicalMem
	{
	private:
		void *clsData;

	public:
		PhysicalMem(OSInt addr, OSInt size);
		~PhysicalMem();

		UInt8 *GetPointer();
		Bool IsError();
	};
}
#endif
