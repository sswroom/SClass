#ifndef _SM_IO_LNKFILE
#define _SM_IO_LNKFILE
#include "Data/Timestamp.h"
#include "Text/CString.h"

namespace IO
{
	class LNKFile
	{
	private:
		UInt8 *buff;
		UIntOS buffSize;

		void InitBuff(const UInt8 *buff, UIntOS buffSize);
	public:
		LNKFile(Text::CStringNN fileName);
		LNKFile(const UInt8 *buff, UIntOS buffSize);
		~LNKFile();

		Bool IsError();
		Data::Timestamp GetCreateTime();
		Data::Timestamp GetAccessTime();
		Data::Timestamp GetWriteTime();
		UnsafeArrayOpt<UTF8Char> GetLocalBasePath(UnsafeArray<UTF8Char> sbuff);
		UnsafeArrayOpt<UTF8Char> GetNameString(UnsafeArray<UTF8Char> sbuff);
		UnsafeArrayOpt<UTF8Char> GetRelativePath(UnsafeArray<UTF8Char> sbuff);
		UnsafeArrayOpt<UTF8Char> GetWorkingDirectory(UnsafeArray<UTF8Char> sbuff);
		UnsafeArrayOpt<UTF8Char> GetCommandLineArguments(UnsafeArray<UTF8Char> sbuff);
		UnsafeArrayOpt<UTF8Char> GetIconLocation(UnsafeArray<UTF8Char> sbuff);
		UnsafeArrayOpt<UTF8Char> GetTarget(UnsafeArray<UTF8Char> sbuff);
	};
}
#endif
