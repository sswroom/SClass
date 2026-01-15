#ifndef _SM_MEDIA_VCDMPGFILE
#define _SM_MEDIA_VCDMPGFILE
#include "AnyType.h"
#include "Data/ByteBuffer.h"
#include "Data/FastMapNN.hpp"
#include "IO/SectorData.h"
#include "Media/AudioSource.h"
#include "Media/M2VStreamSource.h"
#include "Media/MediaFile.h"

namespace Media
{
	class VCDMPGFile : public Media::MediaFile, public Media::MediaStreamControl
	{
	private:
		NN<IO::SectorData> data;
		Data::ByteBuffer readBuff;
		UInt64 readOfst;
		UInt64 fleng;
		Data::Duration startTime;
		Optional<Media::M2VStreamSource> vstm;
		Data::FastMapNN<Int32, Media::MediaStream> dataStms;
		Data::ArrayListNN<Media::AudioSource> audStms;

		Int32 playing;
		Bool playStarted;
		Bool playToStop;

		static UInt32 __stdcall PlayThread(AnyType userData);

		Bool StartPlay();
		Bool StopPlay();
	public:
		VCDMPGFile(NN<IO::SectorData> data, UInt64 startSector, UInt64 endSector);
		virtual ~VCDMPGFile();
		
		virtual UIntOS AddSource(NN<Media::MediaSource> src, Int32 syncTime);
		virtual Optional<Media::MediaSource> GetStream(UIntOS index, OptOut<Int32> syncTime);
		virtual void KeepStream(UIntOS index, Bool toKeep);

		virtual UnsafeArrayOpt<UTF8Char> GetMediaName(UnsafeArray<UTF8Char> buff);
		virtual Data::Duration GetStreamTime();
		virtual Bool StartAudio();
		virtual Bool StopAudio();
		virtual Bool StartVideo();
		virtual Bool StopVideo();
		virtual Bool IsRunning();
		virtual Data::Duration SeekToTime(Data::Duration mediaTime);
		virtual Bool IsRealTimeSrc();
		virtual Bool CanSeek();
		virtual UIntOS GetDataSeekCount();
	};
}
#endif
