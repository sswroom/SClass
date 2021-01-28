#ifndef _SM_NET_WIRELESSLANIE
#define _SM_NET_WIRELESSLANIE
#include "Text/StringBuilderUTF.h"

namespace Net
{
	class WirelessLANIE
	{
	private:
		UInt8 *ieBuff;

	public:
		WirelessLANIE(const UInt8 *ieBuff);
		~WirelessLANIE();

		const UInt8 *GetIEBuff();

		static void ToString(const UInt8 *ieBuff, Text::StringBuilderUTF *sb);
	};
}
#endif
