#ifndef _SM_SSWR_AVIREAD_AVIRAUDIOFILTERFORM
#define _SM_SSWR_AVIREAD_AVIRAUDIOFILTERFORM
#include "Math/FFTCalc.h"
#include "Media/WaveInSource.h"
#include "Media/AFilter/AudioAmplifier.h"
#include "Media/AFilter/AudioCaptureFilter.h"
#include "Media/AFilter/AudioLevelMeter.h"
#include "Media/AFilter/AudioSampleRipper.h"
#include "Media/AFilter/AudioSweepFilter.h"
#include "Media/AFilter/DTMFDecoder.h"
#include "Media/AFilter/DTMFGenerator.h"
#include "Media/AFilter/DynamicVolBooster.h"
#include "Media/AFilter/FileMixFilter.h"
#include "Media/AFilter/SoundGenerator.h"
#include "Media/AFilter/ToneGenerator.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIForm.h"
#include "UI/GUIGroupBox.h"
#include "UI/GUILabel.h"
#include "UI/GUIPanel.h"
#include "UI/GUIPictureBoxSimple.h"
#include "UI/GUIRadioButton.h"
#include "UI/GUIRealtimeLineChart.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"
#include "UI/GUITrackBar.h"
#include "UI/GUIVSplitter.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRAudioFilterForm : public UI::GUIForm
		{
		private:
			NN<UI::GUIPanel> pnlInput;
			NN<UI::GUITabControl> tcFilter;
			NN<UI::GUIPanel> pnlAudioSource;
			NN<UI::GUILabel> lblAudioSource;
			NN<UI::GUIRadioButton> radInputWaveIn;
			NN<UI::GUIRadioButton> radInputSilent;
			NN<UI::GUIPanel> pnlAudioOutput;
			NN<UI::GUILabel> lblAudioOutput;
			NN<UI::GUIRadioButton> radOutputDevice;
			NN<UI::GUIRadioButton> radOutputSilent;
			NN<UI::GUILabel> lblBuffSize;
			NN<UI::GUITextBox> txtBuffSize;
			NN<UI::GUILabel> lblFrequency;
			NN<UI::GUITextBox> txtFrequency;
			NN<UI::GUILabel> lblChannel;
			NN<UI::GUITextBox> txtChannel;
			NN<UI::GUILabel> lblBitCount;
			NN<UI::GUITextBox> txtBitCount;
			NN<UI::GUIButton> btnStart;

			NN<UI::GUITabPage> tpVolBoost;
			NN<UI::GUICheckBox> chkVolBoost;
			NN<UI::GUILabel> lblVolBoostBG;
			NN<UI::GUITrackBar> tbVolBoostBG;
			NN<UI::GUILabel> lblVolBoostBGVol;

			NN<UI::GUITabPage> tpDTMF;
			NN<UI::GUIPanel> pnlDTMF;
			NN<UI::GUILabel> lblDTMFInterval;
			NN<UI::GUITextBox> txtDTMFInterval;
			NN<UI::GUILabel> lblDTMFIntervalMS;
			NN<UI::GUILabel> lblDTMFDecode;
			NN<UI::GUITextBox> txtDTMFDecode;
			NN<UI::GUIButton> btnDTMFClear;
			NN<UI::GUITabControl> tcDTMF;
			NN<UI::GUITabPage> tpDTMFGen;
			NN<UI::GUIButton> btnDTMF1;
			NN<UI::GUIButton> btnDTMF2;
			NN<UI::GUIButton> btnDTMF3;
			NN<UI::GUIButton> btnDTMF4;
			NN<UI::GUIButton> btnDTMF5;
			NN<UI::GUIButton> btnDTMF6;
			NN<UI::GUIButton> btnDTMF7;
			NN<UI::GUIButton> btnDTMF8;
			NN<UI::GUIButton> btnDTMF9;
			NN<UI::GUIButton> btnDTMFStar;
			NN<UI::GUIButton> btnDTMF0;
			NN<UI::GUIButton> btnDTMFSharp;
			NN<UI::GUIButton> btnDTMFA;
			NN<UI::GUIButton> btnDTMFB;
			NN<UI::GUIButton> btnDTMFC;
			NN<UI::GUIButton> btnDTMFD;
			NN<UI::GUILabel> lblDTMFVol;
			NN<UI::GUITrackBar> tbDTMFVol;
			NN<UI::GUILabel> lblDTMFVolV;
			NN<UI::GUITabPage> tpDTMFGen2;
			NN<UI::GUILabel> lblDTMFSignalTime;
			NN<UI::GUITextBox> txtDTMFSignalTime;
			NN<UI::GUILabel> lblDTMFSignalTimeMS;
			NN<UI::GUILabel> lblDTMFBreakTime;
			NN<UI::GUITextBox> txtDTMFBreakTime;
			NN<UI::GUILabel> lblDTMFBreakTimeMS;
			NN<UI::GUILabel> lblDTMFTonesVol;
			NN<UI::GUITrackBar> tbDTMFTonesVol;
			NN<UI::GUILabel> lblDTMFTones;
			NN<UI::GUITextBox> txtDTMFTones;
			NN<UI::GUIButton> btnDTMFTones;

			NN<UI::GUITabPage> tpVolLevel;
			NN<UI::GUIRealtimeLineChart> rlcVolLevel;
			NN<UI::GUIVSplitter> vspVolLevel;
			NN<UI::GUIPictureBoxSimple> pbsSample;
			NN<UI::GUIVSplitter> vspSample;
			NN<UI::GUIPictureBoxSimple> pbsFFT;

			NN<UI::GUITabPage> tpFileMix;
			NN<UI::GUILabel> lblFileMix;
			NN<UI::GUITextBox> txtFileMix;
			NN<UI::GUIButton> btnFileMix;
			NN<UI::GUIButton> btnFileMixStart;
			NN<UI::GUIButton> btnFileMixStop;

			NN<UI::GUITabPage> tpCapture;
			NN<UI::GUIButton> btnCaptureStart;
			NN<UI::GUIButton> btnCaptureStop;

			NN<UI::GUITabPage> tpSoundGen;
			NN<UI::GUIButton> btnSoundGenBell;

			NN<UI::GUITabPage> tpSweep;
			NN<UI::GUILabel> lblSweepVol;
			NN<UI::GUITrackBar> tbSweepVol;
			NN<UI::GUILabel> lblSweepVolV;
			NN<UI::GUILabel> lblSweepStartFreq;
			NN<UI::GUITextBox> txtSweepStartFreq;
			NN<UI::GUILabel> lblSweepEndFreq;
			NN<UI::GUITextBox> txtSweepEndFreq;
			NN<UI::GUILabel> lblSweepDur;
			NN<UI::GUITextBox> txtSweepDur;
			NN<UI::GUIButton> btnSweepStart;

			NN<UI::GUITabPage> tpAmplifier;
			NN<UI::GUILabel> lblAmplifierVol;
			NN<UI::GUITrackBar> tbAmplifierVol;
			NN<UI::GUILabel> lblAmplifierVolV;

			NN<SSWR::AVIRead::AVIRCore> core;
			Math::FFTCalc fft;
			Optional<Media::AudioSource> audSrc;
			Int32 audRenderType;
			Optional<Media::AudioRenderer> audRender;
			Media::RefClock clk;
			NN<Media::DrawEngine> eng;
			Optional<Media::DrawImage> sampleImg;
			Optional<Media::DrawImage> fftImg;
			UnsafeArrayOpt<UInt8> sampleBuff;
			UInt16 nChannels;
			UInt16 bitCount;

			Bool dtmfMod;
			Sync::Mutex dtmfMut;
			Text::StringBuilderUTF8 dtmfSb;

			Optional<Media::AFilter::AudioLevelMeter> audioLevel;
			Optional<Media::AFilter::AudioCaptureFilter> audioCapture;
			Optional<Media::AFilter::AudioSampleRipper> audioRipper;
			Optional<Media::AFilter::DynamicVolBooster> volBooster;
			Optional<Media::AFilter::DTMFGenerator> dtmfGen;
			Optional<Media::AFilter::DTMFDecoder> dtmfDec;
			Optional<Media::AFilter::AudioSweepFilter> sweepFilter;
			Optional<Media::AFilter::FileMixFilter> fileMix;
			Optional<Media::AFilter::SoundGenerator> sndGen;
			Optional<Media::AFilter::ToneGenerator> toneGen;
			Optional<Media::AFilter::AudioAmplifier> audioAmp;

			static void __stdcall OnStartClicked(AnyType userObj);
			static void __stdcall OnVolBoostChg(AnyType userObj, Bool newState);
			static void __stdcall OnVolBoostBGChg(AnyType userObj, UOSInt scrollPos);
			static void __stdcall OnDTMFClearClicked(AnyType userObj);
			static void __stdcall OnDTMF1UpDown(AnyType userObj, Bool isDown);
			static void __stdcall OnDTMF2UpDown(AnyType userObj, Bool isDown);
			static void __stdcall OnDTMF3UpDown(AnyType userObj, Bool isDown);
			static void __stdcall OnDTMF4UpDown(AnyType userObj, Bool isDown);
			static void __stdcall OnDTMF5UpDown(AnyType userObj, Bool isDown);
			static void __stdcall OnDTMF6UpDown(AnyType userObj, Bool isDown);
			static void __stdcall OnDTMF7UpDown(AnyType userObj, Bool isDown);
			static void __stdcall OnDTMF8UpDown(AnyType userObj, Bool isDown);
			static void __stdcall OnDTMF9UpDown(AnyType userObj, Bool isDown);
			static void __stdcall OnDTMFStarUpDown(AnyType userObj, Bool isDown);
			static void __stdcall OnDTMF0UpDown(AnyType userObj, Bool isDown);
			static void __stdcall OnDTMFSharpUpDown(AnyType userObj, Bool isDown);
			static void __stdcall OnDTMFAUpDown(AnyType userObj, Bool isDown);
			static void __stdcall OnDTMFBUpDown(AnyType userObj, Bool isDown);
			static void __stdcall OnDTMFCUpDown(AnyType userObj, Bool isDown);
			static void __stdcall OnDTMFDUpDown(AnyType userObj, Bool isDown);
			static void __stdcall OnDTMFVolChg(AnyType userObj, UOSInt scrollPos);
			static void __stdcall OnDTMFTonesClicked(AnyType userObj);
			static void __stdcall OnLevelTimerTick(AnyType userObj);
			static void __stdcall OnDTMFToneChange(AnyType userObj, WChar tone);
			static void __stdcall OnFileMixClicked(AnyType userObj);
			static void __stdcall OnFileMixStartClicked(AnyType userObj);
			static void __stdcall OnFileMixStopClicked(AnyType userObj);
			static void __stdcall OnCaptureStartClicked(AnyType userObj);
			static void __stdcall OnCaptureStopClicked(AnyType userObj);
			static void __stdcall OnSoundGenBellClicked(AnyType userObj);
			static void __stdcall OnSweepVolChg(AnyType userObj, UOSInt scrollPos);
			static void __stdcall OnSweepStartClicked(AnyType userObj);
			static void __stdcall OnAmplifierVolChg(AnyType userObj, UOSInt scrollPos);
			void StopAudio();
		public:
			AVIRAudioFilterForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, Bool showMenu);
			virtual ~AVIRAudioFilterForm();

			virtual void EventMenuClicked(UInt16 cmdId);
			virtual void OnMonitorChanged();
		};
	};
};
#endif
