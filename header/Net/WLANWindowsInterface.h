#ifndef _SM_NET_WLANWINDOWSINTERFACE
#define _SM_NET_WLANWINDOWSINTERFACE
#include "Net/WirelessLAN.h"
#include "Net/WLANWindowsCore.h"

namespace Net
{
	class WLANWindowsInterface : public Net::WirelessLAN::Interface
	{
		Net::WLANWindowsCore *core;
		void *id;
		Net::WirelessLAN::INTERFACE_STATE state;

	public:
		WLANWindowsInterface(Text::String *name, void *guid, Net::WirelessLAN::INTERFACE_STATE state, Net::WLANWindowsCore *core);
		virtual ~WLANWindowsInterface();

		virtual Bool Scan();
		virtual UIntOS GetNetworks(NN<Data::ArrayListNN<Net::WirelessLAN::Network>> networkList);
		virtual UIntOS GetBSSList(NN<Data::ArrayListNN<Net::WirelessLAN::BSSInfo>> bssList);
	};
}
#endif
