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
		typedef void (__stdcall *WaitForExitFunc)(NotNullPtr<IProgControl> progCtrl);
		typedef UTF8Char **(__stdcall *GetCommandLinesFunc)(NotNullPtr<IProgControl> progCtrl, OutParam<UOSInt> cmdCnt);

		WaitForExitFunc WaitForExit;
		WaitForExitFunc SignalExit;
		WaitForExitFunc SignalRestart;
		GetCommandLinesFunc GetCommandLines;

		static Optional<UI::GUICore> CreateGUICore(NotNullPtr<IProgControl> progCtrl);
	};
	typedef Media::IVideoSource *(__stdcall *DecodeVideoFunc)(NotNullPtr<Media::IVideoSource> video);
	typedef Media::IAudioSource *(__stdcall *DecodeAudioFunc)(NotNullPtr<Media::IAudioSource> audio);
	typedef void (__stdcall *SimpleFunc)();

	void CoreStart();
	void CoreEnd();
	void CoreAddVideoDecFunc(DecodeVideoFunc func);
	void CoreAddAudioDecFunc(DecodeAudioFunc func);
	void CoreAddOnExitFunc(SimpleFunc func);
	Media::IVideoSource *DecodeVideo(NotNullPtr<Media::IVideoSource> video);
	Media::IAudioSource *DecodeAudio(NotNullPtr<Media::IAudioSource> audio);
}
#endif
