#ifndef _SM_IO_OS
#define _SM_IO_OS

namespace IO
{
	class OS
	{
	public:
		static UnsafeArrayOpt<UTF8Char> GetDistro(UnsafeArray<UTF8Char> sbuff);
		static UnsafeArrayOpt<UTF8Char> GetVersion(UnsafeArray<UTF8Char> sbuff);
		static UIntOS GetBuildNumber();
	};
}
#endif
