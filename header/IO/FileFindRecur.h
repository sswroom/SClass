#ifndef _SM_IO_FILEFINDRECUR
#define _SM_IO_FILEFINDRECUR
#include "IO/Path.h"
#include "Text/CString.h"

namespace IO
{
	class FileFindRecur
	{
	private:
		typedef struct
		{
			IO::Path::FindFileSession *sess;
			UTF8Char *buffPtr;
		} FindRecurPart;

	private:
		UTF8Char **srcStrs;
		FindRecurPart *srchParts;
		UOSInt partCnt;
		UTF8Char *srcBuff;
		UTF8Char currBuff[2048];
		Bool isFirst;
	public:
		FileFindRecur(const UTF8Char *path);
		~FileFindRecur();

		Text::CString NextFile(IO::Path::PathType *pt);
	};
}
#endif
