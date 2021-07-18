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
		typedef void (__stdcall *WaitForExitFunc)(IProgControl *progCtrl);
		typedef UTF8Char **(__stdcall *GetCommandLinesFunc)(IProgControl *progCtrl, UOSInt *cmdCnt);

		WaitForExitFunc WaitForExit;
		GetCommandLinesFunc GetCommandLines;

		static UI::GUICore *CreateGUICore(IProgControl *progCtrl);
	};
	typedef Media::IVideoSource *(__stdcall *DecodeVideoFunc)(Media::IVideoSource *video);
	typedef Media::IAudioSource *(__stdcall *DecodeAudioFunc)(Media::IAudioSource *audio);
	typedef void (__stdcall *SimpleFunc)();

	void CoreStart();
	void CoreEnd();
	void CoreAddVideoDecFunc(DecodeVideoFunc func);
	void CoreAddAudioDecFunc(DecodeAudioFunc func);
	void CoreAddOnExitFunc(SimpleFunc func);
	Media::IVideoSource *DecodeVideo(Media::IVideoSource *video);
	Media::IAudioSource *DecodeAudio(Media::IAudioSource *audio);
}
#endif
