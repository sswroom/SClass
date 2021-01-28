#ifndef _SM_MEDIA_M2VFILE
#define _SM_MEDIA_M2VFILE
#include "IO/IStreamData.h"
#include "Media/IStreamControl.h"
#include "Media/MediaFile.h"
namespace Media
{
	class M2VStreamSource;
	class M2VFile : public Media::MediaFile, public Media::IStreamControl
	{
	private:
		IO::IStreamData *stmData;
		UInt8 *readBuff;
		Int64 readOfst;
		Int64 bitRate;
		Int64 fleng;
		Int32 startTime;
		Media::M2VStreamSource *stm;

		Bool playing;
		Bool playStarted;
		Bool playToStop;

		static UInt32 __stdcall PlayThread(void *userData);
	public:
		M2VFile(IO::IStreamData *stmData);
		virtual ~M2VFile();
		
		virtual UOSInt AddSource(Media::IMediaSource *src, Int32 syncTime);
		virtual Media::IMediaSource *GetStream(UOSInt index, Int32 *syncTime);
		virtual void KeepStream(UOSInt index, Bool toKeep);

		virtual UTF8Char *GetMediaName(UTF8Char *buff);
		virtual Int32 GetStreamTime();
		virtual Bool StartAudio();
		virtual Bool StopAudio();
		virtual Bool StartVideo();
		virtual Bool StopVideo();
		virtual Bool IsRunning();
		virtual Int32 SeekToTime(Int32 mediaTime);
		virtual Bool IsRealTimeSrc();
		virtual Bool CanSeek();
		virtual OSInt GetDataSeekCount();
	};
}
#endif
