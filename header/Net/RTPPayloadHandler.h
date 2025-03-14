#ifndef _SM_NET_RTPPAYLOADHANDLER
#define _SM_NET_RTPPAYLOADHANDLER

namespace Net
{
	class RTPPayloadHandler
	{
	public:
		virtual ~RTPPayloadHandler(){};

		virtual void MediaDataReceived(UInt8 *buff, UOSInt dataSize, UInt32 seqNum, UInt32 ts) = 0;
		virtual void SetFormat(UnsafeArray<const UTF8Char> fmtStr) = 0;
		virtual Int32 GetPayloadType() = 0;
	};
}
#endif
