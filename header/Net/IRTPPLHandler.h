#ifndef _SM_NET_IRTPPLHANDLER
#define _SM_NET_IRTPPLHANDLER

namespace Net
{
	class IRTPPLHandler
	{
	public:
		virtual ~IRTPPLHandler(){};

		virtual void MediaDataReceived(UInt8 *buff, UOSInt dataSize, UInt32 seqNum, UInt32 ts) = 0;
		virtual void SetFormat(const UTF8Char *fmtStr) = 0;
		virtual Int32 GetPayloadType() = 0;
	};
}
#endif
