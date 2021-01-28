#ifndef _SM_MEDIA_IMEDIASTREAM
#define _SM_MEDIA_IMEDIASTREAM

namespace Media
{
	class IMediaStream
	{
	public:
		virtual ~IMediaStream(){};

		virtual void DetectStreamInfo(UInt8 *header, UOSInt headerSize) = 0;
		virtual void ClearFrameBuff() = 0;
		virtual void SetStreamTime(Int32 time) = 0;
		virtual void WriteFrameStream(UInt8 *buff, UOSInt buffSize) = 0;
		virtual Int32 GetFrameStreamTime() = 0;
		virtual void EndFrameStream() = 0;
		virtual Int64 GetBitRate() = 0;
	};
}
#endif
