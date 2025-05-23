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
			NN<UI::GUIMainMenu> mnu;
			NN<UI::GUITabControl> tcMain;
			NN<UI::GUITabPage> tpSample;
			NN<UI::GUIPictureBoxSimple> pbsSample;
			NN<UI::GUITabPage> tpFreq;
			NN<UI::GUIPictureBoxSimple> pbsFreq;

			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Media::AudioSource> audSrc;
			NN<Media::DrawEngine> eng;
			Optional<Media::DrawImage> sampleImg;
			Optional<Media::DrawImage> fftImg;
			UInt64 currSample;
			UInt64 totalSample;
			Media::AudioFormat format;

			static void __stdcall OnSizeUpdated(AnyType userObj);
			void UpdateImages();
			void UpdateFreqImage();
		public:
			AVIRAudioViewerForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Media::AudioSource> audSrc);
			virtual ~AVIRAudioViewerForm();

			virtual void EventMenuClicked(UInt16 cmdId);
			virtual void OnMonitorChanged();
		};
	}
}
#endif
