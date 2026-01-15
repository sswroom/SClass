#ifndef _SM_NET_ARPINFO
#define _SM_NET_ARPINFO
#include "Data/ArrayListNN.hpp"
//requires iphlpapi.lib

namespace Net
{
	class ARPInfo
	{
	public:
		typedef enum
		{
			ARPT_OTHER = 1,
			ARPT_INVALID = 2,
			ARPT_DYNAMIC = 3,
			ARPT_STATIC = 4
		} ARPType;
	private:
		UInt32 ifIndex;
		UInt8 *phyAddr;
		UIntOS phyAddrLen;
		UInt32 ipAddr;
		ARPType arpType;

		ARPInfo(void *info);
	public:
		~ARPInfo();
		UInt32 GetAdaptorIndex();
		UIntOS GetPhysicalAddr(UInt8 *buff);
		UInt32 GetIPAddress();
		ARPType GetARPType();

		static UIntOS GetARPInfoList(NN<Data::ArrayListNN<Net::ARPInfo>> arpInfoList);
	};
}
#endif
