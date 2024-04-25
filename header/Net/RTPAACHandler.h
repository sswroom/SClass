#ifndef _SM_NET_RTPAACHANDLER
#define _SM_NET_RTPAACHANDLER
//RFC3640

#include "Net/RTPAPLHandler.h"
#include "IO/MemoryStream.h"
#include "Sync/Mutex.h"

namespace Net
{
	class RTPAACHandler : public RTPAPLHandler
	{
	private:
		typedef enum
		{
			AACM_UNKNOWN,
			AACM_GENERIC,
			AACM_CELP_CBR,
			AACM_CELP_VBR,
			AACM_AAC_LBR,
			AACM_AAC_HBR
		} AACMode;
	private:
		Int32 payloadType;
		UInt32 freq;
		UInt32 nChannel;
		AACMode aacm;
		Int32 streamType;
		Int32 profileId;
		Int32 config;
		UInt8 *buff;
		UOSInt buffSize;

		Sync::Event *evt;
		Sync::Event *dataEvt;

		Sync::Mutex mut;

		UInt8 GetRateIndex();
	public:
		RTPAACHandler(Int32 payloadType, UInt32 freq, UInt32 nChannel);
		virtual ~RTPAACHandler();

		virtual void MediaDataReceived(UInt8 *buff, UOSInt dataSize, UInt32 seqNum, UInt32 ts);
		virtual void SetFormat(const UTF8Char *fmtStr);
		virtual Int32 GetPayloadType();

		virtual UTF8Char *GetSourceName(UTF8Char *buff);
		virtual Bool CanSeek();
		virtual Data::Duration GetStreamTime();
		virtual Data::Duration SeekToTime(Data::Duration time);
		virtual Bool TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, Int32 *syncTime);

		virtual void GetFormat(NN<Media::AudioFormat> format);

		virtual Bool Start(Sync::Event *evt, UOSInt blkSize);
		virtual void Stop();
		virtual UOSInt ReadBlock(Data::ByteArray blk); //ret actual block size
		virtual UOSInt GetMinBlockSize();
		virtual Data::Duration GetCurrTime();
		virtual Bool IsEnd();
	};
}
#endif
