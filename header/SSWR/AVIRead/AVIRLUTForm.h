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
			UI::GUITabControl *tcMain;

			UI::GUITabPage *tpInfo;
			UI::GUIPanel *pnlInfo;
			UI::GUILabel *lblFileName;
			UI::GUITextBox *txtFileName;
			UI::GUITextBox *txtRemark;

			UI::GUITabPage *tpValues;
			UI::GUIPanel *pnlValues;
			UI::GUIComboBox *cboChannels;
			UI::GUIListView *lvValues;

			SSWR::AVIRead::AVIRCore *core;
			Media::LUT *lut;

			static void __stdcall OnChannelChg(void *userObj);
			void UpdateValues();
		public:
			AVIRLUTForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, SSWR::AVIRead::AVIRCore *core, Media::LUT *lut);
			virtual ~AVIRLUTForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
