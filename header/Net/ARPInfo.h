#ifndef _SM_NET_ARPINFO
#define _SM_NET_ARPINFO
#include "Data/ArrayList.h"
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
		UOSInt phyAddrLen;
		UInt32 ipAddr;
		ARPType arpType;

		ARPInfo(void *info);
	public:
		~ARPInfo();
		UInt32 GetAdaptorIndex();
		UOSInt GetPhysicalAddr(UInt8 *buff);
		UInt32 GetIPAddress();
		ARPType GetARPType();

		static UOSInt GetARPInfoList(Data::ArrayList<Net::ARPInfo*> *arpInfoList);
	};
}
#endif
