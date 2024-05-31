#ifndef _SM_MEDIA_M2VFILE
#define _SM_MEDIA_M2VFILE
#include "AnyType.h"
#include "Data/ByteBuffer.h"
#include "IO/StreamData.h"
#include "Media/IStreamControl.h"
#include "Media/MediaFile.h"
namespace Media
{
	class M2VStreamSource;
	class M2VFile : public Media::MediaFile, public Media::IStreamControl
	{
	private:
		NN<IO::StreamData> stmData;
		Data::ByteBuffer readBuff;
		UInt64 readOfst;
		UInt64 bitRate;
		UInt64 fleng;
		Data::Duration startTime;
		Media::M2VStreamSource *stm;

		Bool playing;
		Bool playStarted;
		Bool playToStop;

		static UInt32 __stdcall PlayThread(AnyType userData);
	public:
		M2VFile(NN<IO::StreamData> stmData);
		virtual ~M2VFile();
		
		virtual UOSInt AddSource(Media::IMediaSource *src, Int32 syncTime);
		virtual Media::IMediaSource *GetStream(UOSInt index, Int32 *syncTime);
		virtual void KeepStream(UOSInt index, Bool toKeep);

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
		virtual UOSInt GetDataSeekCount();
	};
}
#endif
