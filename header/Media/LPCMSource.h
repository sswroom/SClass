#ifndef _SM_MEDIA_LPCMSOURCE
#define _SM_MEDIA_LPCMSOURCE
#include "IO/StreamData.h"
#include "Media/IAudioSource.h"

namespace Media
{
	class LPCMSource : public IAudioSource
	{
	protected:
		AudioFormat format;
		IO::StreamData *data;
		NotNullPtr<Text::String> name;

		UInt64 readOfst;
		Sync::Event *readEvt;

	protected:
		LPCMSource(NotNullPtr<Text::String> name);
		LPCMSource(Text::CString name);
		void SetData(IO::StreamData *fd, UInt64 ofst, UInt64 length, Media::AudioFormat *format);
	public:
		LPCMSource(IO::StreamData *fd, UInt64 ofst, UInt64 length, Media::AudioFormat *format, NotNullPtr<Text::String> name);
		LPCMSource(IO::StreamData *fd, UInt64 ofst, UInt64 length, Media::AudioFormat *format, Text::CString name);
		virtual ~LPCMSource();

		virtual UTF8Char *GetSourceName(UTF8Char *buff);
		virtual Bool CanSeek();
		virtual Int32 GetStreamTime(); //ms
		virtual UInt32 SeekToTime(UInt32 time); //ms, ret actual time
		virtual Bool TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, Int32 *syncTime);

		virtual void GetFormat(AudioFormat *format);

		virtual Bool Start(Sync::Event *evt, UOSInt blkSize);
		virtual void Stop();
		virtual UOSInt ReadBlock(UInt8 *buff, UOSInt blkSize); //ret actual block size
		virtual UOSInt GetMinBlockSize();
		virtual UInt32 GetCurrTime();
		virtual Bool IsEnd();

		virtual Bool SupportSampleRead();
		virtual UOSInt ReadSample(UInt64 sampleOfst, UOSInt sampleCount, UInt8 *buff);
		virtual Int64 GetSampleCount();
	};
}
#endif
