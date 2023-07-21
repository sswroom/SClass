#ifndef _SM_SSWR_AVIREAD_AVIRAUDIOVIEWERFORM
#define _SM_SSWR_AVIREAD_AVIRAUDIOVIEWERFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIForm.h"
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
		class AVIRAudioViewerForm : public UI::GUIForm
		{
		private:
			UI::GUIMainMenu *mnu;
			UI::GUITabControl *tcMain;
			UI::GUITabPage *tpSample;
			UI::GUIPictureBoxSimple *pbsSample;
			UI::GUITabPage *tpFreq;
			UI::GUIPictureBoxSimple *pbsFreq;

			SSWR::AVIRead::AVIRCore *core;
			Media::IAudioSource *audSrc;
			NotNullPtr<Media::DrawEngine> eng;
			Media::DrawImage *sampleImg;
			Media::DrawImage *fftImg;
			UInt64 currSample;
			UInt64 totalSample;
			Media::AudioFormat *format;

			static void __stdcall OnSizeChanged(void *userObj);
			void UpdateImages();
			void UpdateFreqImage();
		public:
			AVIRAudioViewerForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, SSWR::AVIRead::AVIRCore *core, Media::IAudioSource *audSrc);
			virtual ~AVIRAudioViewerForm();

			virtual void EventMenuClicked(UInt16 cmdId);
			virtual void OnMonitorChanged();
		};
	};
};
#endif
