#ifndef _SM_CORE_CORE
#define _SM_CORE_CORE
#include "Data/ArrayList.h"

namespace Media
{
	class AudioSource;
	class VideoSource;
};
namespace UI
{
	class GUICore;
}

namespace Core
{
	struct IProgControl
	{
		typedef void (CALLBACKFUNC WaitForExitFunc)(NN<IProgControl> progCtrl);
		typedef UTF8Char **(CALLBACKFUNC GetCommandLinesFunc)(NN<IProgControl> progCtrl, OutParam<UOSInt> cmdCnt);

		WaitForExitFunc WaitForExit;
		WaitForExitFunc SignalExit;
		WaitForExitFunc SignalRestart;
		GetCommandLinesFunc GetCommandLines;

		static Optional<UI::GUICore> CreateGUICore(NN<IProgControl> progCtrl);
	};
	typedef Media::VideoSource *(CALLBACKFUNC DecodeVideoFunc)(NN<Media::VideoSource> video);
	typedef Media::AudioSource *(CALLBACKFUNC DecodeAudioFunc)(NN<Media::AudioSource> audio);
	typedef void (CALLBACKFUNC SimpleFunc)();

	void CoreStart();
	void CoreEnd();
	void CoreAddVideoDecFunc(DecodeVideoFunc func);
	void CoreAddAudioDecFunc(DecodeAudioFunc func);
	void CoreAddOnExitFunc(SimpleFunc func);
	Media::VideoSource *DecodeVideo(NN<Media::VideoSource> video);
	Media::AudioSource *DecodeAudio(NN<Media::AudioSource> audio);
}
#endif
