#ifndef _SM_NET_WIRELESSLANIE
#define _SM_NET_WIRELESSLANIE
#include "Text/String.h"
#include "Text/StringBuilderUTF8.h"

namespace Net
{
	class WirelessLANIE
	{
	private:
		UnsafeArray<UInt8> ieBuff;

	public:
		WirelessLANIE(UnsafeArray<const UInt8> ieBuff);
		~WirelessLANIE();

		UnsafeArray<const UInt8> GetIEBuff();

		static void ToString(UnsafeArray<const UInt8> ieBuff, NN<Text::StringBuilderUTF8> sb);
		static void GetWPSInfo(UnsafeArray<const UInt8> iebuff, UOSInt ieLen, OutParam<Optional<Text::String>> manuf, OutParam<Optional<Text::String>> model, OutParam<Optional<Text::String>> serialNum);
	};
}
#endif
