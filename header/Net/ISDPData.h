#ifndef _SM_NET_ISDPDATA
#define _SM_NET_ISDPDATA

namespace Net
{
	class ISDPData
	{
	public:
		virtual UInt16 GetSDPDataPort() = 0;
		virtual UTF8Char *GetSDPDataType(UTF8Char *buff) = 0;
		virtual UInt32 GetSDPDataFreq() = 0;
		virtual UTF8Char *GetSDPDataFormat(UTF8Char *buff) = 0;
	};
}
#endif
