#ifndef _SM_CORE_CORE
#define _SM_CORE_CORE

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
	struct ProgControl
	{
		typedef void (CALLBACKFUNC WaitForExitFunc)(NN<ProgControl> progCtrl);
		typedef UnsafeArray<UnsafeArray<UTF8Char>> (CALLBACKFUNC GetCommandLinesFunc)(NN<ProgControl> progCtrl, OutParam<UIntOS> cmdCnt);

		WaitForExitFunc WaitForExit;
		WaitForExitFunc SignalExit;
		WaitForExitFunc SignalRestart;
		GetCommandLinesFunc GetCommandLines;

		static Optional<UI::GUICore> CreateGUICore(NN<ProgControl> progCtrl);
	};
	typedef Optional<Media::VideoSource> (CALLBACKFUNC DecodeVideoFunc)(NN<Media::VideoSource> video);
	typedef Optional<Media::AudioSource> (CALLBACKFUNC DecodeAudioFunc)(NN<Media::AudioSource> audio);
	typedef void (CALLBACKFUNC SimpleFunc)();

	void CoreStart();
	void CoreEnd();
	void CoreAddVideoDecFunc(DecodeVideoFunc func);
	void CoreAddAudioDecFunc(DecodeAudioFunc func);
	void CoreAddOnExitFunc(SimpleFunc func);
	Optional<Media::VideoSource> DecodeVideo(NN<Media::VideoSource> video);
	Optional<Media::AudioSource> DecodeAudio(NN<Media::AudioSource> audio);
}
#endif
