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
		UIntOS width;
		UIntOS height;		
	public:
		GoogleQRCode(UIntOS width, UIntOS height, Text::CStringNN dataStr);
		~GoogleQRCode();

		void GetImageURL(NN<Text::StringBuilderUTF8> sb);
	};
}
#endif
