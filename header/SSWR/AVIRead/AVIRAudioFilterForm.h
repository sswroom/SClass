#ifndef _SM_SSWR_AVIREAD_AVIRAUDIOFILTERFORM
#define _SM_SSWR_AVIREAD_AVIRAUDIOFILTERFORM
#include "Math/FFTCalc.h"
#include "Media/WaveInSource.h"
#include "Media/AudioFilter/AudioAmplifier.h"
#include "Media/AudioFilter/AudioCaptureFilter.h"
#include "Media/AudioFilter/AudioLevelMeter.h"
#include "Media/AudioFilter/AudioSampleRipper.h"
#include "Media/AudioFilter/AudioSweepFilter.h"
#include "Media/AudioFilter/DTMFDecoder.h"
#include "Media/AudioFilter/DTMFGenerator.h"
#include "Media/AudioFilter/DynamicVolBooster.h"
#include "Media/AudioFilter/FileMixFilter.h"
#include "Media/AudioFilter/SoundGenerator.h"
#include "Media/AudioFilter/ToneGenerator.h"
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
			NotNullPtr<UI::GUIPanel> pnlInput;
			NotNullPtr<UI::GUITabControl> tcFilter;
			NotNullPtr<UI::GUIPanel> pnlAudioSource;
			NotNullPtr<UI::GUILabel> lblAudioSource;
			UI::GUIRadioButton *radInputWaveIn;
			UI::GUIRadioButton *radInputSilent;
			NotNullPtr<UI::GUIPanel> pnlAudioOutput;
			NotNullPtr<UI::GUILabel> lblAudioOutput;
			UI::GUIRadioButton *radOutputDevice;
			UI::GUIRadioButton *radOutputSilent;
			NotNullPtr<UI::GUILabel> lblBuffSize;
			NotNullPtr<UI::GUITextBox> txtBuffSize;
			NotNullPtr<UI::GUILabel> lblFrequency;
			NotNullPtr<UI::GUITextBox> txtFrequency;
			NotNullPtr<UI::GUILabel> lblChannel;
			NotNullPtr<UI::GUITextBox> txtChannel;
			NotNullPtr<UI::GUILabel> lblBitCount;
			NotNullPtr<UI::GUITextBox> txtBitCount;
			NotNullPtr<UI::GUIButton> btnStart;

			NotNullPtr<UI::GUITabPage> tpVolBoost;
			NotNullPtr<UI::GUICheckBox> chkVolBoost;
			NotNullPtr<UI::GUILabel> lblVolBoostBG;
			UI::GUITrackBar *tbVolBoostBG;
			NotNullPtr<UI::GUILabel> lblVolBoostBGVol;

			NotNullPtr<UI::GUITabPage> tpDTMF;
			NotNullPtr<UI::GUIPanel> pnlDTMF;
			NotNullPtr<UI::GUILabel> lblDTMFInterval;
			NotNullPtr<UI::GUITextBox> txtDTMFInterval;
			NotNullPtr<UI::GUILabel> lblDTMFIntervalMS;
			NotNullPtr<UI::GUILabel> lblDTMFDecode;
			NotNullPtr<UI::GUITextBox> txtDTMFDecode;
			NotNullPtr<UI::GUIButton> btnDTMFClear;
			NotNullPtr<UI::GUITabControl> tcDTMF;
			NotNullPtr<UI::GUITabPage> tpDTMFGen;
			NotNullPtr<UI::GUIButton> btnDTMF1;
			NotNullPtr<UI::GUIButton> btnDTMF2;
			NotNullPtr<UI::GUIButton> btnDTMF3;
			NotNullPtr<UI::GUIButton> btnDTMF4;
			NotNullPtr<UI::GUIButton> btnDTMF5;
			NotNullPtr<UI::GUIButton> btnDTMF6;
			NotNullPtr<UI::GUIButton> btnDTMF7;
			NotNullPtr<UI::GUIButton> btnDTMF8;
			NotNullPtr<UI::GUIButton> btnDTMF9;
			NotNullPtr<UI::GUIButton> btnDTMFStar;
			NotNullPtr<UI::GUIButton> btnDTMF0;
			NotNullPtr<UI::GUIButton> btnDTMFSharp;
			NotNullPtr<UI::GUIButton> btnDTMFA;
			NotNullPtr<UI::GUIButton> btnDTMFB;
			NotNullPtr<UI::GUIButton> btnDTMFC;
			NotNullPtr<UI::GUIButton> btnDTMFD;
			NotNullPtr<UI::GUILabel> lblDTMFVol;
			UI::GUITrackBar *tbDTMFVol;
			NotNullPtr<UI::GUILabel> lblDTMFVolV;
			NotNullPtr<UI::GUITabPage> tpDTMFGen2;
			NotNullPtr<UI::GUILabel> lblDTMFSignalTime;
			NotNullPtr<UI::GUITextBox> txtDTMFSignalTime;
			NotNullPtr<UI::GUILabel> lblDTMFSignalTimeMS;
			NotNullPtr<UI::GUILabel> lblDTMFBreakTime;
			NotNullPtr<UI::GUITextBox> txtDTMFBreakTime;
			NotNullPtr<UI::GUILabel> lblDTMFBreakTimeMS;
			NotNullPtr<UI::GUILabel> lblDTMFTonesVol;
			UI::GUITrackBar *tbDTMFTonesVol;
			NotNullPtr<UI::GUILabel> lblDTMFTones;
			NotNullPtr<UI::GUITextBox> txtDTMFTones;
			NotNullPtr<UI::GUIButton> btnDTMFTones;

			NotNullPtr<UI::GUITabPage> tpVolLevel;
			UI::GUIRealtimeLineChart *rlcVolLevel;
			NotNullPtr<UI::GUIVSplitter> vspVolLevel;
			UI::GUIPictureBoxSimple *pbsSample;
			NotNullPtr<UI::GUIVSplitter> vspSample;
			UI::GUIPictureBoxSimple *pbsFFT;

			NotNullPtr<UI::GUITabPage> tpFileMix;
			NotNullPtr<UI::GUILabel> lblFileMix;
			NotNullPtr<UI::GUITextBox> txtFileMix;
			NotNullPtr<UI::GUIButton> btnFileMix;
			NotNullPtr<UI::GUIButton> btnFileMixStart;
			NotNullPtr<UI::GUIButton> btnFileMixStop;

			NotNullPtr<UI::GUITabPage> tpCapture;
			NotNullPtr<UI::GUIButton> btnCaptureStart;
			NotNullPtr<UI::GUIButton> btnCaptureStop;

			NotNullPtr<UI::GUITabPage> tpSoundGen;
			NotNullPtr<UI::GUIButton> btnSoundGenBell;

			NotNullPtr<UI::GUITabPage> tpSweep;
			NotNullPtr<UI::GUILabel> lblSweepVol;
			UI::GUITrackBar *tbSweepVol;
			NotNullPtr<UI::GUILabel> lblSweepVolV;
			NotNullPtr<UI::GUILabel> lblSweepStartFreq;
			NotNullPtr<UI::GUITextBox> txtSweepStartFreq;
			NotNullPtr<UI::GUILabel> lblSweepEndFreq;
			NotNullPtr<UI::GUITextBox> txtSweepEndFreq;
			NotNullPtr<UI::GUILabel> lblSweepDur;
			NotNullPtr<UI::GUITextBox> txtSweepDur;
			NotNullPtr<UI::GUIButton> btnSweepStart;

			NotNullPtr<UI::GUITabPage> tpAmplifier;
			NotNullPtr<UI::GUILabel> lblAmplifierVol;
			UI::GUITrackBar *tbAmplifierVol;
			NotNullPtr<UI::GUILabel> lblAmplifierVolV;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Math::FFTCalc fft;
			Media::IAudioSource *audSrc;
			Int32 audRenderType;
			Media::IAudioRenderer *audRender;
			Media::RefClock clk;
			NotNullPtr<Media::DrawEngine> eng;
			Media::DrawImage *sampleImg;
			Media::DrawImage *fftImg;
			UInt8 *sampleBuff;
			UInt16 nChannels;
			UInt16 bitCount;

			Bool dtmfMod;
			Sync::Mutex dtmfMut;
			Text::StringBuilderUTF8 dtmfSb;

			Media::AudioFilter::AudioLevelMeter *audioLevel;
			Media::AudioFilter::AudioCaptureFilter *audioCapture;
			Media::AudioFilter::AudioSampleRipper *audioRipper;
			Media::AudioFilter::DynamicVolBooster *volBooster;
			Media::AudioFilter::DTMFGenerator *dtmfGen;
			Media::AudioFilter::DTMFDecoder *dtmfDec;
			Media::AudioFilter::AudioSweepFilter *sweepFilter;
			Media::AudioFilter::FileMixFilter *fileMix;
			Media::AudioFilter::SoundGenerator *sndGen;
			Media::AudioFilter::ToneGenerator *toneGen;
			Media::AudioFilter::AudioAmplifier *audioAmp;

			static void __stdcall OnStartClicked(void *userObj);
			static void __stdcall OnVolBoostChg(void *userObj, Bool newState);
			static void __stdcall OnVolBoostBGChg(void *userObj, UOSInt scrollPos);
			static void __stdcall OnDTMFClearClicked(void *userObj);
			static void __stdcall OnDTMF1UpDown(void *userObj, Bool isDown);
			static void __stdcall OnDTMF2UpDown(void *userObj, Bool isDown);
			static void __stdcall OnDTMF3UpDown(void *userObj, Bool isDown);
			static void __stdcall OnDTMF4UpDown(void *userObj, Bool isDown);
			static void __stdcall OnDTMF5UpDown(void *userObj, Bool isDown);
			static void __stdcall OnDTMF6UpDown(void *userObj, Bool isDown);
			static void __stdcall OnDTMF7UpDown(void *userObj, Bool isDown);
			static void __stdcall OnDTMF8UpDown(void *userObj, Bool isDown);
			static void __stdcall OnDTMF9UpDown(void *userObj, Bool isDown);
			static void __stdcall OnDTMFStarUpDown(void *userObj, Bool isDown);
			static void __stdcall OnDTMF0UpDown(void *userObj, Bool isDown);
			static void __stdcall OnDTMFSharpUpDown(void *userObj, Bool isDown);
			static void __stdcall OnDTMFAUpDown(void *userObj, Bool isDown);
			static void __stdcall OnDTMFBUpDown(void *userObj, Bool isDown);
			static void __stdcall OnDTMFCUpDown(void *userObj, Bool isDown);
			static void __stdcall OnDTMFDUpDown(void *userObj, Bool isDown);
			static void __stdcall OnDTMFVolChg(void *userObj, UOSInt scrollPos);
			static void __stdcall OnDTMFTonesClicked(void *userObj);
			static void __stdcall OnLevelTimerTick(void *userObj);
			static void __stdcall OnDTMFToneChange(void *userObj, WChar tone);
			static void __stdcall OnFileMixClicked(void *userObj);
			static void __stdcall OnFileMixStartClicked(void *userObj);
			static void __stdcall OnFileMixStopClicked(void *userObj);
			static void __stdcall OnCaptureStartClicked(void *userObj);
			static void __stdcall OnCaptureStopClicked(void *userObj);
			static void __stdcall OnSoundGenBellClicked(void *userObj);
			static void __stdcall OnSweepVolChg(void *userObj, UOSInt scrollPos);
			static void __stdcall OnSweepStartClicked(void *userObj);
			static void __stdcall OnAmplifierVolChg(void *userObj, UOSInt scrollPos);
			void StopAudio();
		public:
			AVIRAudioFilterForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, Bool showMenu);
			virtual ~AVIRAudioFilterForm();

			virtual void EventMenuClicked(UInt16 cmdId);
			virtual void OnMonitorChanged();
		};
	};
};
#endif
