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
			const UTF8Char *ext;
			UOSInt extLen;
			const UTF8Char *mime;
			UOSInt mimeLen;
		} MIMEEntry;

		static MIMEEntry mimeList[];
	public:
		static Text::CStringNN GetMIMEFromExt(Text::CString ext);
		static Text::CStringNN GetMIMEFromFileName(const UTF8Char *fileName, UOSInt nameLen);
	};
}
#endif
