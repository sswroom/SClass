#ifndef _SM_NET_WLANWINDOWSCORE
#define _SM_NET_WLANWINDOWSCORE

namespace Net
{
	class WLANWindowsCore
	{
	private:
		struct ClassData;
		ClassData *clsData;
	public:
		WLANWindowsCore();
		~WLANWindowsCore();

		Bool IsError();
		
		void FreeMemory(void *pMemory);
		UInt32 Scan(void *pInterfaceGuid, void *pDot11Ssid, void *pIeData, void *pReserved);
		UInt32 EnumInterfaces(void *pReserved, void **ppInterfaceList);
		UInt32 GetAvailableNetworkList(void *pInterfaceGuid, UInt32 dwFlags, void *pReserved, void **ppAvailableNetworkList);
		UInt32 GetNetworkBssList(void *pInterfaceGuid, void *pDot11Ssid, Int32 dot11BssType, Int32 bSecurityEnabled, void *pReserved, void **ppWlanBssList);
	};
}
#endif
