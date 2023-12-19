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
			NotNullPtr<UI::GUITabControl> tcMain;

			NotNullPtr<UI::GUITabPage> tpInfo;
			NotNullPtr<UI::GUIPanel> pnlInfo;
			NotNullPtr<UI::GUILabel> lblFileName;
			NotNullPtr<UI::GUITextBox> txtFileName;
			NotNullPtr<UI::GUITextBox> txtRemark;

			NotNullPtr<UI::GUITabPage> tpValues;
			NotNullPtr<UI::GUIPanel> pnlValues;
			NotNullPtr<UI::GUIComboBox> cboChannels;
			UI::GUIListView *lvValues;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Media::LUT *lut;

			static void __stdcall OnChannelChg(void *userObj);
			void UpdateValues();
		public:
			AVIRLUTForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, Media::LUT *lut);
			virtual ~AVIRLUTForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
