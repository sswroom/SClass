#ifndef _SM_IO_LIBRARY
#define _SM_IO_LIBRARY

namespace IO
{
	class Library
	{
	private:
		void *hModule;

	public:
		Library(const UTF8Char *fileName);
		~Library();
		Bool IsError();
		void *GetFunc(const Char *funcName);
		void *GetFuncNum(OSInt num);
	};
};
#endif
