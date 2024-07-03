#ifndef _SM_IO_LIBRARY
#define _SM_IO_LIBRARY

namespace IO
{
	class Library
	{
	private:
		void *hModule;

	public:
		Library(UnsafeArray<const UTF8Char> fileName);
		~Library();
		Bool IsError();
		void *GetFunc(UnsafeArray<const Char> funcName);
		void *GetFuncNum(OSInt num);
	};
};
#endif
