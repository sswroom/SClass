#ifndef _SM_NET_SDPDATA
#define _SM_NET_SDPDATA

namespace Net
{
	class SDPData
	{
	public:
		virtual UInt16 GetSDPDataPort() = 0;
		virtual UnsafeArray<UTF8Char> GetSDPDataType(UnsafeArray<UTF8Char> buff) = 0;
		virtual UInt32 GetSDPDataFreq() = 0;
		virtual UnsafeArray<UTF8Char> GetSDPDataFormat(UnsafeArray<UTF8Char> buff) = 0;
	};
}
#endif
