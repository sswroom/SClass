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
		UOSInt buffSize;

		void InitBuff(const UInt8 *buff, UOSInt buffSize);
	public:
		LNKFile(Text::CString fileName);
		LNKFile(const UInt8 *buff, UOSInt buffSize);
		~LNKFile();

		Bool IsError();
		Data::Timestamp GetCreateTime();
		Data::Timestamp GetAccessTime();
		Data::Timestamp GetWriteTime();
		UTF8Char *GetLocalBasePath(UTF8Char *sbuff);
		UTF8Char *GetNameString(UTF8Char *sbuff);
		UTF8Char *GetRelativePath(UTF8Char *sbuff);
		UTF8Char *GetWorkingDirectory(UTF8Char *sbuff);
		UTF8Char *GetCommandLineArguments(UTF8Char *sbuff);
		UTF8Char *GetIconLocation(UTF8Char *sbuff);
	};
}
#endif
