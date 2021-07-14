#ifndef _SM_NET_GOOGLEQRCODE
#define _SM_NET_GOOGLEQRCODE
#include "Text/StringBuilderUTF.h"

namespace Net
{
	class GoogleQRCode
	{
	private:
		const UTF8Char *dataStr;
		UOSInt width;
		UOSInt height;		
	public:
		GoogleQRCode(UOSInt width, UOSInt height, const UTF8Char *dataStr);
		~GoogleQRCode();

		void GetImageURL(Text::StringBuilderUTF *sb);
	};
}
#endif
