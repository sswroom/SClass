#ifndef _SM_MEDIA_VOBAC3STREAMSOURCE
#define _SM_MEDIA_VOBAC3STREAMSOURCE
#include "Media/IMediaStream.h"
#include "Media/IStreamControl.h"
#include "Media/IAudioSource.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"

namespace Media
{
	class VOBAC3StreamSource : public Media::IAudioSource, public Media::IMediaStream
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


	public:
		VOBAC3StreamSource(NN<Media::IStreamControl> pbc);
		virtual ~VOBAC3StreamSource();

		Bool ParseHeader(UnsafeArray<UInt8> buff, UOSInt buffSize);
		Bool IsReady();

		virtual UnsafeArrayOpt<UTF8Char> GetSourceName(UnsafeArray<UTF8Char> buff);
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
