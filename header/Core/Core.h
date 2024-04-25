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
		typedef void (__stdcall *WaitForExitFunc)(NN<IProgControl> progCtrl);
		typedef UTF8Char **(__stdcall *GetCommandLinesFunc)(NN<IProgControl> progCtrl, OutParam<UOSInt> cmdCnt);

		WaitForExitFunc WaitForExit;
		WaitForExitFunc SignalExit;
		WaitForExitFunc SignalRestart;
		GetCommandLinesFunc GetCommandLines;

		static Optional<UI::GUICore> CreateGUICore(NN<IProgControl> progCtrl);
	};
	typedef Media::IVideoSource *(__stdcall *DecodeVideoFunc)(NN<Media::IVideoSource> video);
	typedef Media::IAudioSource *(__stdcall *DecodeAudioFunc)(NN<Media::IAudioSource> audio);
	typedef void (__stdcall *SimpleFunc)();

	void CoreStart();
	void CoreEnd();
	void CoreAddVideoDecFunc(DecodeVideoFunc func);
	void CoreAddAudioDecFunc(DecodeAudioFunc func);
	void CoreAddOnExitFunc(SimpleFunc func);
	Media::IVideoSource *DecodeVideo(NN<Media::IVideoSource> video);
	Media::IAudioSource *DecodeAudio(NN<Media::IAudioSource> audio);
}
#endif
