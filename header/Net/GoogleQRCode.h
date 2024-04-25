#ifndef _SM_NET_GOOGLEQRCODE
#define _SM_NET_GOOGLEQRCODE
#include "Text/String.h"
#include "Text/StringBuilderUTF8.h"

namespace Net
{
	class GoogleQRCode
	{
	private:
		NN<Text::String> dataStr;
		UOSInt width;
		UOSInt height;		
	public:
		GoogleQRCode(UOSInt width, UOSInt height, Text::CString dataStr);
		~GoogleQRCode();

		void GetImageURL(NN<Text::StringBuilderUTF8> sb);
	};
}
#endif
