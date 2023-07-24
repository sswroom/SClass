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
			UI::GUIPanel *pnlInput;
			UI::GUITabControl *tcFilter;
			UI::GUIPanel *pnlAudioSource;
			UI::GUILabel *lblAudioSource;
			UI::GUIRadioButton *radInputWaveIn;
			UI::GUIRadioButton *radInputSilent;
			UI::GUIPanel *pnlAudioOutput;
			UI::GUILabel *lblAudioOutput;
			UI::GUIRadioButton *radOutputDevice;
			UI::GUIRadioButton *radOutputSilent;
			UI::GUILabel *lblBuffSize;
			UI::GUITextBox *txtBuffSize;
			UI::GUILabel *lblFrequency;
			UI::GUITextBox *txtFrequency;
			UI::GUILabel *lblChannel;
			UI::GUITextBox *txtChannel;
			UI::GUILabel *lblBitCount;
			UI::GUITextBox *txtBitCount;
			UI::GUIButton *btnStart;

			UI::GUITabPage *tpVolBoost;
			UI::GUICheckBox *chkVolBoost;
			UI::GUILabel *lblVolBoostBG;
			UI::GUITrackBar *tbVolBoostBG;
			UI::GUILabel *lblVolBoostBGVol;

			UI::GUITabPage *tpDTMF;
			UI::GUIPanel *pnlDTMF;
			UI::GUILabel *lblDTMFInterval;
			UI::GUITextBox *txtDTMFInterval;
			UI::GUILabel *lblDTMFIntervalMS;
			UI::GUILabel *lblDTMFDecode;
			UI::GUITextBox *txtDTMFDecode;
			UI::GUIButton *btnDTMFClear;
			UI::GUITabControl *tcDTMF;
			UI::GUITabPage *tpDTMFGen;
			UI::GUIButton *btnDTMF1;
			UI::GUIButton *btnDTMF2;
			UI::GUIButton *btnDTMF3;
			UI::GUIButton *btnDTMF4;
			UI::GUIButton *btnDTMF5;
			UI::GUIButton *btnDTMF6;
			UI::GUIButton *btnDTMF7;
			UI::GUIButton *btnDTMF8;
			UI::GUIButton *btnDTMF9;
			UI::GUIButton *btnDTMFStar;
			UI::GUIButton *btnDTMF0;
			UI::GUIButton *btnDTMFSharp;
			UI::GUIButton *btnDTMFA;
			UI::GUIButton *btnDTMFB;
			UI::GUIButton *btnDTMFC;
			UI::GUIButton *btnDTMFD;
			UI::GUILabel *lblDTMFVol;
			UI::GUITrackBar *tbDTMFVol;
			UI::GUILabel *lblDTMFVolV;
			UI::GUITabPage *tpDTMFGen2;
			UI::GUILabel *lblDTMFSignalTime;
			UI::GUITextBox *txtDTMFSignalTime;
			UI::GUILabel *lblDTMFSignalTimeMS;
			UI::GUILabel *lblDTMFBreakTime;
			UI::GUITextBox *txtDTMFBreakTime;
			UI::GUILabel *lblDTMFBreakTimeMS;
			UI::GUILabel *lblDTMFTonesVol;
			UI::GUITrackBar *tbDTMFTonesVol;
			UI::GUILabel *lblDTMFTones;
			UI::GUITextBox *txtDTMFTones;
			UI::GUIButton *btnDTMFTones;

			UI::GUITabPage *tpVolLevel;
			UI::GUIRealtimeLineChart *rlcVolLevel;
			UI::GUIVSplitter *vspVolLevel;
			UI::GUIPictureBoxSimple *pbsSample;
			UI::GUIVSplitter *vspSample;
			UI::GUIPictureBoxSimple *pbsFFT;

			UI::GUITabPage *tpFileMix;
			UI::GUILabel *lblFileMix;
			UI::GUITextBox *txtFileMix;
			UI::GUIButton *btnFileMix;
			UI::GUIButton *btnFileMixStart;
			UI::GUIButton *btnFileMixStop;

			UI::GUITabPage *tpCapture;
			UI::GUIButton *btnCaptureStart;
			UI::GUIButton *btnCaptureStop;

			UI::GUITabPage *tpSoundGen;
			UI::GUIButton *btnSoundGenBell;

			UI::GUITabPage *tpSweep;
			UI::GUILabel *lblSweepVol;
			UI::GUITrackBar *tbSweepVol;
			UI::GUILabel *lblSweepVolV;
			UI::GUILabel *lblSweepStartFreq;
			UI::GUITextBox *txtSweepStartFreq;
			UI::GUILabel *lblSweepEndFreq;
			UI::GUITextBox *txtSweepEndFreq;
			UI::GUILabel *lblSweepDur;
			UI::GUITextBox *txtSweepDur;
			UI::GUIButton *btnSweepStart;

			UI::GUITabPage *tpAmplifier;
			UI::GUILabel *lblAmplifierVol;
			UI::GUITrackBar *tbAmplifierVol;
			UI::GUILabel *lblAmplifierVolV;

			SSWR::AVIRead::AVIRCore *core;
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
			AVIRAudioFilterForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, SSWR::AVIRead::AVIRCore *core, Bool showMenu);
			virtual ~AVIRAudioFilterForm();

			virtual void EventMenuClicked(UInt16 cmdId);
			virtual void OnMonitorChanged();
		};
	};
};
#endif
