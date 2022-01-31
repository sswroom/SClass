#ifndef _SM_NET_WIRELESSLAN
#define _SM_NET_WIRELESSLAN
#include "Data/ArrayList.h"
#include "IO/Library.h"
#include "Net/WirelessLANIE.h"
#include "Text/String.h"

#define WLAN_OUI_CNT 3

namespace Net
{
	class WirelessLAN
	{

	private:
		void *clsData;
	public:
		typedef enum
		{
			BST_INFRASTRUCTURE = 1,
			BST_INDEPENDENT = 2
		} BSSType;

		typedef enum
		{
			INTERFACE_STATE_NOT_READY,
			INTERFACE_STATE_CONNECTED,
			INTERFACE_STATE_AD_HOC_NETWORK_FORMED,
			INTERFACE_STATE_DISCONNECTING,
			INTERFACE_STATE_DISCONNECTED,
			INTERFACE_STATE_ASSOCIATING,
			INTERFACE_STATE_DISCOVERING,
			INTERFACE_STATE_AUTHENTICATING
		} INTERFACE_STATE;

		class Network
		{
		private:
			const UTF8Char *ssid;
			Double rssi;
		public:
			Network(const UTF8Char *ssid, Double rssi);
			~Network();
			Double GetRSSI();
			const UTF8Char *GetSSID();
		};

		class BSSInfo
		{
		private:
			const UTF8Char *ssid;
			UInt32 phyId;
			UInt8 mac[6];
			BSSType bssType;
			Int32 phyType;
			Double rssi;
			UInt32 linkQuality;
			Double freq; //Hz
			const UTF8Char *devManuf;
			const UTF8Char *devModel;
			const UTF8Char *devSN;
			UTF8Char devCountry[3];
			UInt8 chipsetOUIs[WLAN_OUI_CNT][3];
			Data::ArrayList<Net::WirelessLANIE*> *ieList;
		public:
			BSSInfo(const UTF8Char *ssid, const void *bssEntry);
			~BSSInfo();
			const UTF8Char *GetSSID();
			UInt32 GetPHYId();
			const UInt8 *GetMAC();
			BSSType GetBSSType();
			Int32 GetPHYType();
			Double GetRSSI();
			UInt32 GetLinkQuality();
			Double GetFreq();
			const UTF8Char *GetManuf();
			const UTF8Char *GetModel();
			const UTF8Char *GetSN();
			const UTF8Char *GetCountry();
			const UInt8 *GetChipsetOUI(OSInt index);
			UOSInt GetIECount();
			Net::WirelessLANIE *GetIE(UOSInt index);
		};

		class Interface
		{
		protected:
			Text::String *name;

		public:
			Interface();
			virtual ~Interface();

			Text::String *GetName();
			virtual Bool Scan() = 0;
			virtual UOSInt GetNetworks(Data::ArrayList<Net::WirelessLAN::Network*> *networkList) = 0;
			virtual UOSInt GetBSSList(Data::ArrayList<Net::WirelessLAN::BSSInfo*> *bssList) = 0;
		};

	public:
		WirelessLAN();
		~WirelessLAN();

		Bool IsError();
		UOSInt GetInterfaces(Data::ArrayList<Net::WirelessLAN::Interface*> *outArr);
	};
}
#endif
