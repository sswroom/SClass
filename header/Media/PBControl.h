#ifndef _SM_MEDIA_PBCONTROL
#define _SM_MEDIA_PBCONTROL

namespace Media
{
	class PBControl
	{
	public:
		virtual ~PBControl(){};

/*		virtual WChar *GetMediaName(WChar *buff) = 0;
		virtual Data::Duration GetStreamTime() = 0;
		virtual Bool StartAudio() = 0;
		virtual Bool StopAudio() = 0;
		virtual Bool StartVideo() = 0;
		virtual Bool StopVideo() = 0;
		virtual Bool IsRunning() = 0;
		virtual Data::Duration SeekToTime(Data::Duration mediaTime) = 0;
		virtual Bool IsRealTimeSrc() = 0;
		virtual Bool CanSeek() = 0;*/

		virtual Bool IsPlaying() = 0;
		virtual Bool StartPlayback() = 0;
		virtual Bool StopPlayback() = 0;
		virtual Bool PrevChapter() = 0;
		virtual Bool NextChapter() = 0;
		virtual Data::Duration GetCurrTime() = 0;
	};
}
#endif
