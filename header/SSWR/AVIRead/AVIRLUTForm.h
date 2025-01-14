#ifndef _SM_SSWR_AVIREAD_AVIRLUTFORM
#define _SM_SSWR_AVIREAD_AVIRLUTFORM
#include "Media/LUT.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"
#include "SSWR/AVIRead/AVIRCore.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRLUTForm : public UI::GUIForm
		{
		private:
			NN<UI::GUITabControl> tcMain;

			NN<UI::GUITabPage> tpInfo;
			NN<UI::GUIPanel> pnlInfo;
			NN<UI::GUILabel> lblFileName;
			NN<UI::GUITextBox> txtFileName;
			NN<UI::GUITextBox> txtRemark;

			NN<UI::GUITabPage> tpValues;
			NN<UI::GUIPanel> pnlValues;
			NN<UI::GUIComboBox> cboChannels;
			NN<UI::GUIListView> lvValues;

			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Media::LUT> lut;

			static void __stdcall OnChannelChg(AnyType userObj);
			void UpdateValues();
		public:
			AVIRLUTForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Media::LUT> lut);
			virtual ~AVIRLUTForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
