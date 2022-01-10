#ifndef _SM_NET_MIME
#define _SM_NET_MIME
#include "Text/MyString.h"
namespace Net
{
	class MIME
	{
	private:
		typedef struct
		{
			const Char *ext;
			const UTF8Char *mime;
			UOSInt mimeLen;
		} MIMEEntry;

		static MIMEEntry mimeList[];
	public:
		static Text::CString GetMIMEFromExt(const UTF8Char *ext);
	};
};
#endif
