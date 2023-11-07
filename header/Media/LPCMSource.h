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
		void SetData(NotNullPtr<IO::StreamData> fd, UInt64 ofst, UInt64 length, NotNullPtr<const Media::AudioFormat> format);
	public:
		LPCMSource(NotNullPtr<IO::StreamData> fd, UInt64 ofst, UInt64 length, NotNullPtr<const Media::AudioFormat> format, NotNullPtr<Text::String> name);
		LPCMSource(NotNullPtr<IO::StreamData> fd, UInt64 ofst, UInt64 length, NotNullPtr<const Media::AudioFormat> format, Text::CStringNN name);
		virtual ~LPCMSource();

		virtual UTF8Char *GetSourceName(UTF8Char *buff);
		virtual Bool CanSeek();
		virtual Data::Duration GetStreamTime();
		virtual Data::Duration SeekToTime(Data::Duration time);
		virtual Bool TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, Int32 *syncTime);

		virtual void GetFormat(NotNullPtr<AudioFormat> format);

		virtual Bool Start(Sync::Event *evt, UOSInt blkSize);
		virtual void Stop();
		virtual UOSInt ReadBlock(Data::ByteArray blk); //ret actual block size
		virtual UOSInt GetMinBlockSize();
		virtual Data::Duration GetCurrTime();
		virtual Bool IsEnd();

		virtual Bool SupportSampleRead();
		virtual UOSInt ReadSample(UInt64 sampleOfst, UOSInt sampleCount, Data::ByteArray buff);
		virtual Int64 GetSampleCount();
	};
}
#endif
