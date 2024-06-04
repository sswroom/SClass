#ifndef _SM_NET_WLANLINUXMTKINTERFACE
#define _SM_NET_WLANLINUXMTKINTERFACE
#include "Net/WirelessLAN.h"

namespace Net
{
	class WLANLinuxMTKInterface : public Net::WirelessLAN::Interface
	{
		void *id;
		Net::WirelessLAN::INTERFACE_STATE state;
		UInt32 setCmd;
		UInt32 siteSurveyCmd;

		void Reopen();
	public:
		WLANLinuxMTKInterface(Text::CStringNN name, void *id, Net::WirelessLAN::INTERFACE_STATE state, UInt32 setCmd, UInt32 siteSurveyCmd);
		virtual ~WLANLinuxMTKInterface();

		virtual Bool Scan();
		virtual UOSInt GetNetworks(NN<Data::ArrayListNN<Net::WirelessLAN::Network>> networkList);
		virtual UOSInt GetBSSList(NN<Data::ArrayListNN<Net::WirelessLAN::BSSInfo>> bssList);
	};
}
#endif
