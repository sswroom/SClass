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
			NotNullPtr<UI::GUIPanel> pnlSrc;
			NotNullPtr<UI::GUIPanel> pnlSrcCtrl;
			NotNullPtr<UI::GUIPanel> pnlDest;
			NotNullPtr<UI::GUIPanel> pnlDestCtrl;
			NotNullPtr<UI::GUIHSplitter> hspMain;
			NotNullPtr<UI::GUILabel> lblSrc;
			NotNullPtr<UI::GUIComboBox> cboSrc;
			NotNullPtr<UI::GUITextBox> txtSrc;
			NotNullPtr<UI::GUILabel> lblDest;
			NotNullPtr<UI::GUIComboBox> cboDest;
			NotNullPtr<UI::GUITextBox> txtDest;
			NotNullPtr<UI::GUIButton> btnConvert;
			UOSInt fileIndex;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Text::TextBinEnc::TextBinEncList encList;
			static void __stdcall OnConvertClicked(void *userObj);
		public:
			AVIREncryptForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIREncryptForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
