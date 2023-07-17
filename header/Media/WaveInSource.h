#ifndef _SM_MEDIA_WAVEINSOURCE
#define _SM_MEDIA_WAVEINSOURCE
#include "Media/IAudioSource.h"
//need Winmm.lib

namespace Media
{
	class WaveInSource : public IAudioSource
	{
	private:
		void *hWaveIn;
		UInt32 devId;
		UInt32 freq;
		UInt16 nbits;
		UInt16 nChannels;

		UInt32 nextId;
		void *hdrs;
		UInt32 hdrsCnt;
		Sync::Event *evt;

		Sync::Event *dataEvt;
		Bool started;

		void OpenAudio();
		static void __stdcall AudioBlock(void *hwi, UInt32 uMsg, UInt32 *dwInstance, UInt32 *dwParam1, UInt32 *dwParam2);
	public:
		static UInt32 GetDeviceCount();
		static UTF8Char *GetDeviceName(UTF8Char *sbuff, UInt32 devNo);

		WaveInSource(const UTF8Char *devName, UInt32 freq, UInt16 nbits, UInt16 nChannels);
		WaveInSource(UInt32 devId, UInt32 freq, UInt16 nbits, UInt16 nChannels);
		virtual ~WaveInSource();

		Bool IsError();

		virtual UTF8Char *GetSourceName(UTF8Char *buff);
		virtual Bool CanSeek();
		virtual Int32 GetStreamTime(); //ms
		virtual UInt32 SeekToTime(UInt32 time); //ms, ret actual time
		virtual Bool TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, Int32 *syncTime);

		virtual void GetFormat(AudioFormat *format);

		virtual Bool Start(Sync::Event *evt, UOSInt blkSize);
		virtual void Stop();
		virtual UOSInt ReadBlock(Data::ByteArray blk); //ret actual block size
		virtual UOSInt GetMinBlockSize();
		virtual UInt32 GetCurrTime();
		virtual Bool IsEnd();
	};
}
#endif
