#ifndef _SM_IO_SHAREDMEMORY
#define _SM_IO_SHAREDMEMORY

namespace IO
{
	class SharedMemory
	{
	private:
		void *hand;
		UInt8 *memPtr;
		OSInt memSize;

		SharedMemory(void *hand, UInt8 *memPtr, OSInt memSize);

	public:
		~SharedMemory();
		UInt8 *GetPointer();

		static Optional<IO::SharedMemory> Create(const UTF8Char *name, OSInt size);
		static Optional<IO::SharedMemory> Open(const UTF8Char *name, OSInt size);
	};
}
#endif
