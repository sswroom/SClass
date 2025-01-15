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
		Optional<Sync::Event> evt;

		Sync::Event *dataEvt;
		Bool started;

		void OpenAudio();
		static void __stdcall AudioBlock(void *hwi, UInt32 uMsg, UInt32 *dwInstance, UInt32 *dwParam1, UInt32 *dwParam2);
	public:
		static UInt32 GetDeviceCount();
		static UnsafeArrayOpt<UTF8Char> GetDeviceName(UnsafeArray<UTF8Char> sbuff, UInt32 devNo);

		WaveInSource(UnsafeArray<const UTF8Char> devName, UInt32 freq, UInt16 nbits, UInt16 nChannels);
		WaveInSource(UInt32 devId, UInt32 freq, UInt16 nbits, UInt16 nChannels);
		virtual ~WaveInSource();

		Bool IsError();

		virtual UnsafeArrayOpt<UTF8Char> GetSourceName(UnsafeArray<UTF8Char> buff);
		virtual Bool CanSeek();
		virtual Data::Duration GetStreamTime();
		virtual Data::Duration SeekToTime(Data::Duration time);
		virtual Bool TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, OptOut<Int32> syncTime);

		virtual void GetFormat(NN<AudioFormat> format);

		virtual Bool Start(Optional<Sync::Event> evt, UOSInt blkSize);
		virtual void Stop();
		virtual UOSInt ReadBlock(Data::ByteArray blk); //ret actual block size
		virtual UOSInt GetMinBlockSize();
		virtual Data::Duration GetCurrTime();
		virtual Bool IsEnd();
	};
}
#endif
