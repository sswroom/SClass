#ifndef _SM_NET_ISDPDATA
#define _SM_NET_ISDPDATA

namespace Net
{
	class ISDPData
	{
	public:
		virtual Int32 GetSDPDataPort() = 0;
		virtual UTF8Char *GetSDPDataType(UTF8Char *buff) = 0;
		virtual Int32 GetSDPDataFreq() = 0;
		virtual UTF8Char *GetSDPDataFormat(UTF8Char *buff) = 0;
	};
};
#endif
