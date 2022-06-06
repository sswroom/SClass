#ifndef _SM_SSWR_AVIREAD_AVIRENCRYPTFORM
#define _SM_SSWR_AVIREAD_AVIRENCRYPTFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "Text/TextBinEnc/TextBinEncList.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUILabel.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIREncryptForm : public UI::GUIForm
		{
		private:
			UI::GUIPanel *pnlSrc;
			UI::GUIPanel *pnlSrcCtrl;
			UI::GUIPanel *pnlDest;
			UI::GUIPanel *pnlDestCtrl;
			UI::GUIHSplitter *hspMain;
			UI::GUILabel *lblSrc;
			UI::GUIComboBox *cboSrc;
			UI::GUITextBox *txtSrc;
			UI::GUILabel *lblDest;
			UI::GUIComboBox *cboDest;
			UI::GUITextBox *txtDest;
			UI::GUIButton *btnConvert;
			UOSInt fileIndex;

			SSWR::AVIRead::AVIRCore *core;
			Text::TextBinEnc::TextBinEncList encList;
			static void __stdcall OnConvertClicked(void *userObj);
		public:
			AVIREncryptForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIREncryptForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
