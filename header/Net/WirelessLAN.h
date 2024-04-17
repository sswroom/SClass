#ifndef _SM_NET_WIRELESSLAN
#define _SM_NET_WIRELESSLAN
#include "Data/ArrayListNN.h"
#include "IO/Library.h"
#include "Net/WirelessLANIE.h"
#include "Text/String.h"

#define WLAN_OUI_CNT 3

namespace Net
{
	class 	WirelessLAN
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
			NotNullPtr<Text::String> ssid;
			Double rssi;
		public:
			Network(NotNullPtr<Text::String> ssid, Double rssi);
			Network(Text::CString ssid, Double rssi);
			~Network();
			Double GetRSSI() const;
			NotNullPtr<Text::String> GetSSID() const;
		};

		class BSSInfo
		{
		private:
			NotNullPtr<Text::String> ssid;
			UInt32 phyId;
			UInt8 mac[6];
			BSSType bssType;
			Int32 phyType;
			Double rssi;
			UInt32 linkQuality;
			Double freq; //Hz
			Text::String *devManuf;
			Text::String *devModel;
			Text::String *devSN;
			UTF8Char devCountry[3];
			UInt8 chipsetOUIs[WLAN_OUI_CNT][3];
			Data::ArrayListNN<Net::WirelessLANIE> ieList;
		public:
			BSSInfo(Text::CString ssid, const void *bssEntry);
			~BSSInfo();
			NotNullPtr<Text::String> GetSSID() const;
			UInt32 GetPHYId();
			const UInt8 *GetMAC();
			BSSType GetBSSType();
			Int32 GetPHYType();
			Double GetRSSI();
			UInt32 GetLinkQuality();
			Double GetFreq();
			Text::String *GetManuf();
			Text::String *GetModel();
			Text::String *GetSN();
			const UTF8Char *GetCountry();
			const UInt8 *GetChipsetOUI(OSInt index);
			UOSInt GetIECount();
			Optional<Net::WirelessLANIE> GetIE(UOSInt index);
		};

		class Interface
		{
		protected:
			NotNullPtr<Text::String> name;

		public:
			Interface();
			virtual ~Interface();

			NotNullPtr<Text::String> GetName() const;
			virtual Bool Scan() = 0;
			virtual UOSInt GetNetworks(NotNullPtr<Data::ArrayListNN<Net::WirelessLAN::Network>> networkList) = 0;
			virtual UOSInt GetBSSList(NotNullPtr<Data::ArrayListNN<Net::WirelessLAN::BSSInfo>> bssList) = 0;
		};

	public:
		WirelessLAN();
		~WirelessLAN();

		Bool IsError();
		UOSInt GetInterfaces(NotNullPtr<Data::ArrayListNN<Net::WirelessLAN::Interface>> outArr);
	};
}
#endif
