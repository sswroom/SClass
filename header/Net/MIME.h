#ifndef _SM_NET_MIME
#define _SM_NET_MIME
#include "Text/CString.h"
namespace Net
{
	class MIME
	{
	private:
		typedef struct
		{
			UnsafeArray<const UTF8Char> ext;
			UOSInt extLen;
			UnsafeArray<const UTF8Char> mime;
			UOSInt mimeLen;
		} MIMEEntry;

		static MIMEEntry mimeList[];
	public:
		static Text::CStringNN GetMIMEFromExt(Text::CStringNN ext);
		static Text::CStringNN GetMIMEFromFileName(UnsafeArray<const UTF8Char> fileName, UOSInt nameLen);
	};
}
#endif
