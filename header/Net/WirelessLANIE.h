#ifndef _SM_NET_WIRELESSLANIE
#define _SM_NET_WIRELESSLANIE
#include "Text/String.h"
#include "Text/StringBuilderUTF8.h"

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

		static void ToString(const UInt8 *ieBuff, NN<Text::StringBuilderUTF8> sb);
		static void GetWPSInfo(const UInt8 *iebuff, UOSInt ieLen, Text::String **manuf, Text::String **model, Text::String **serialNum);
	};
}
#endif
