#ifndef _SM_MEDIA_MPASTREAMSOURCE
#define _SM_MEDIA_MPASTREAMSOURCE
#include "Media/IMediaStream.h"
#include "Media/IStreamControl.h"
#include "Media/IAudioSource.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"

namespace Media
{
	class MPAStreamSource : public Media::IAudioSource, public Media::IMediaStream
	{
	private:
		NN<Media::IStreamControl> pbc;
		Media::AudioFormat fmt;
		Sync::Event *pbEvt;

		Sync::Mutex buffMut;
		UInt8 *dataBuff;
		UInt8 *dataBuff2;
		UOSInt buffSize;
		UOSInt buffStart;
		UOSInt buffEnd;
		UInt64 buffSample;
		UInt32 lastFrameSize;
		Bool streamStarted;


	public:
		MPAStreamSource(NN<Media::IStreamControl> pbc);
		virtual ~MPAStreamSource();

		Bool ParseHeader(UInt8 *buff, UOSInt buffSize);
		Bool IsReady();

		virtual UTF8Char *GetSourceName(UTF8Char *buff);
		virtual Bool CanSeek();
		virtual Data::Duration GetStreamTime();
		virtual Data::Duration SeekToTime(Data::Duration time);
		virtual Bool TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, Int32 *syncTime);

		virtual void GetFormat(NN<AudioFormat> format);

		virtual Bool Start(Sync::Event *evt, UOSInt blkSize);
		virtual void Stop();
		virtual UOSInt ReadBlock(Data::ByteArray blk); //ret actual block size
		virtual UOSInt GetMinBlockSize();
		virtual Data::Duration GetCurrTime();
		virtual Bool IsEnd();

		virtual void DetectStreamInfo(UInt8 *header, UOSInt headerSize);
		virtual void ClearFrameBuff();
		virtual void SetStreamTime(Data::Duration time);
		virtual void WriteFrameStream(UInt8 *buff, UOSInt buffSize);
		virtual Data::Duration GetFrameStreamTime();
		virtual void EndFrameStream();
		virtual UInt64 GetBitRate();
	};
}
#endif
