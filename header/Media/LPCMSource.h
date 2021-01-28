#ifndef _SM_MEDIA_LPCMSOURCE
#define _SM_MEDIA_LPCMSOURCE
#include "IO/IStreamData.h"
#include "Media/IAudioSource.h"

namespace Media
{
	class LPCMSource : public IAudioSource
	{
	protected:
		AudioFormat format;
		IO::IStreamData *data;
		const UTF8Char *name;

		UInt64 readOfst;
		Sync::Event *readEvt;

	protected:
		LPCMSource(const UTF8Char *name);
		void SetData(IO::IStreamData *fd, UInt64 ofst, UInt64 length, Media::AudioFormat *format);
	public:
		LPCMSource(IO::IStreamData *fd, UInt64 ofst, UInt64 length, Media::AudioFormat *format, const UTF8Char *name);
		virtual ~LPCMSource();

		virtual UTF8Char *GetSourceName(UTF8Char *buff);
		virtual Bool CanSeek();
		virtual Int32 GetStreamTime(); //ms
		virtual Int32 SeekToTime(Int32 time); //ms, ret actual time
		virtual Bool TrimStream(Int32 trimTimeStart, Int32 trimTimeEnd, Int32 *syncTime);

		virtual void GetFormat(AudioFormat *format);

		virtual Bool Start(Sync::Event *evt, UOSInt blkSize);
		virtual void Stop();
		virtual UOSInt ReadBlock(UInt8 *buff, UOSInt blkSize); //ret actual block size
		virtual UOSInt GetMinBlockSize();
		virtual Int32 GetCurrTime();
		virtual Bool IsEnd();

		virtual Bool SupportSampleRead();
		virtual UOSInt ReadSample(Int64 sampleOfst, UOSInt sampleCount, UInt8 *buff);
		virtual UInt64 GetSampleCount();
	};
}
#endif
