#ifndef _SM_NET_WLANLINUXINTERFACE
#define _SM_NET_WLANLINUXINTERFACE
#include "Net/WirelessLAN.h"

namespace Net
{
	class WLANLinuxInterface : public Net::WirelessLAN::Interface
	{
		void *id;
		Net::WirelessLAN::INTERFACE_STATE state;

		void Reopen();
	public:
		WLANLinuxInterface(Text::CStringNN name, void *id, Net::WirelessLAN::INTERFACE_STATE state);
		virtual ~WLANLinuxInterface();

		virtual Bool Scan();
		virtual UOSInt GetNetworks(NN<Data::ArrayListNN<Net::WirelessLAN::Network>> networkList);
		virtual UOSInt GetBSSList(NN<Data::ArrayListNN<Net::WirelessLAN::BSSInfo>> bssList);
	};
}
#endif
