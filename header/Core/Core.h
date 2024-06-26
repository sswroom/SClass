#ifndef _SM_CORE_CORE
#define _SM_CORE_CORE
#include "Data/ArrayList.h"

namespace Media
{
	class IAudioSource;
	class IVideoSource;
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
	typedef Media::IVideoSource *(CALLBACKFUNC DecodeVideoFunc)(NN<Media::IVideoSource> video);
	typedef Media::IAudioSource *(CALLBACKFUNC DecodeAudioFunc)(NN<Media::IAudioSource> audio);
	typedef void (CALLBACKFUNC SimpleFunc)();

	void CoreStart();
	void CoreEnd();
	void CoreAddVideoDecFunc(DecodeVideoFunc func);
	void CoreAddAudioDecFunc(DecodeAudioFunc func);
	void CoreAddOnExitFunc(SimpleFunc func);
	Media::IVideoSource *DecodeVideo(NN<Media::IVideoSource> video);
	Media::IAudioSource *DecodeAudio(NN<Media::IAudioSource> audio);
}
#endif
