#ifndef _SM_MEDIA_VCDMPGFILE
#define _SM_MEDIA_VCDMPGFILE
#include "Data/ByteBuffer.h"
#include "Data/FastMap.h"
#include "IO/ISectorData.h"
#include "Media/IAudioSource.h"
#include "Media/M2VStreamSource.h"
#include "Media/MediaFile.h"

namespace Media
{
	class VCDMPGFile : public Media::MediaFile, public Media::IStreamControl
	{
	private:
		NotNullPtr<IO::ISectorData> data;
		Data::ByteBuffer readBuff;
		UInt64 readOfst;
		UInt64 fleng;
		Data::Duration startTime;
		Media::M2VStreamSource *vstm;
		Data::FastMap<Int32, Media::IMediaStream*> dataStms;
		Data::ArrayList<Media::IAudioSource*> audStms;

		Int32 playing;
		Bool playStarted;
		Bool playToStop;

		static UInt32 __stdcall PlayThread(void *userData);

		Bool StartPlay();
		Bool StopPlay();
	public:
		VCDMPGFile(NotNullPtr<IO::ISectorData> data, UInt64 startSector, UInt64 endSector);
		virtual ~VCDMPGFile();
		
		virtual UOSInt AddSource(Media::IMediaSource *src, Int32 syncTime);
		virtual Media::IMediaSource *GetStream(UOSInt index, Int32 *syncTime);
		virtual void KeepStream(UOSInt index, Bool toKeep);

		virtual UTF8Char *GetMediaName(UTF8Char *buff);
		virtual Data::Duration GetStreamTime();
		virtual Bool StartAudio();
		virtual Bool StopAudio();
		virtual Bool StartVideo();
		virtual Bool StopVideo();
		virtual Bool IsRunning();
		virtual Data::Duration SeekToTime(Data::Duration mediaTime);
		virtual Bool IsRealTimeSrc();
		virtual Bool CanSeek();
		virtual UOSInt GetDataSeekCount();
	};
}
#endif
