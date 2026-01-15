#ifndef _SM_MEDIA_VOBLPCMSTREAMSOURCE
#define _SM_MEDIA_VOBLPCMSTREAMSOURCE
#include "Media/MediaStream.h"
#include "Media/MediaStreamControl.h"
#include "Media/AudioSource.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"

namespace Media
{
	class VOBLPCMStreamSource : public Media::AudioSource, public Media::MediaStream
	{
	private:
		NN<Media::MediaStreamControl> pbc;
		Media::AudioFormat fmt;
		Optional<Sync::Event> pbEvt;

		Sync::Mutex buffMut;
		UInt8 *dataBuff;
		UIntOS buffSize;
		UIntOS buffStart;
		UIntOS buffEnd;
		UInt64 buffSample;


	public:
		VOBLPCMStreamSource(NN<Media::MediaStreamControl> pbc, NN<const Media::AudioFormat> fmt);
		virtual ~VOBLPCMStreamSource();

		virtual UnsafeArrayOpt<UTF8Char> GetSourceName(UnsafeArray<UTF8Char> buff);
		virtual Bool CanSeek();
		virtual Data::Duration GetStreamTime();
		virtual Data::Duration SeekToTime(Data::Duration time);
		virtual Bool TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, OptOut<Int32> syncTime);

		virtual void GetFormat(NN<AudioFormat> format);

		virtual Bool Start(Optional<Sync::Event> evt, UIntOS blkSize);
		virtual void Stop();
		virtual UIntOS ReadBlock(Data::ByteArray blk); //ret actual block size
		virtual UIntOS GetMinBlockSize();
		virtual Data::Duration GetCurrTime();
		virtual Bool IsEnd();

		virtual void DetectStreamInfo(UInt8 *header, UIntOS headerSize);
		virtual void ClearFrameBuff();
		virtual void SetStreamTime(Data::Duration time);
		virtual void WriteFrameStream(UInt8 *buff, UIntOS buffSize);
		virtual Data::Duration GetFrameStreamTime();
		virtual void EndFrameStream();
		virtual UInt64 GetBitRate();
	};
}
#endif
