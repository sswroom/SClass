#ifndef _SM_NET_WIFICAPTURER
#define _SM_NET_WIFICAPTURER
#include "Data/ArrayList.h"
#include "Net/WirelessLAN.h"

namespace Net
{
	class WiFiCapturer
	{
	private:
		static UInt32 __stdcall ScanThread(void *userObj);
	public:
		WiFiCapturer();
		~WiFiCapturer();

		void StoreStatus();
	};
}
#endif
