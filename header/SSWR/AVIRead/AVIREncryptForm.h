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
			NN<UI::GUIPanel> pnlSrc;
			NN<UI::GUIPanel> pnlSrcCtrl;
			NN<UI::GUIPanel> pnlDest;
			NN<UI::GUIPanel> pnlDestCtrl;
			NN<UI::GUIHSplitter> hspMain;
			NN<UI::GUILabel> lblSrc;
			NN<UI::GUIComboBox> cboSrc;
			NN<UI::GUITextBox> txtSrc;
			NN<UI::GUILabel> lblDest;
			NN<UI::GUIComboBox> cboDest;
			NN<UI::GUITextBox> txtDest;
			NN<UI::GUIButton> btnConvert;
			NN<UI::GUIButton> btnHex;
			UOSInt fileIndex;

			NN<SSWR::AVIRead::AVIRCore> core;
			Text::TextBinEnc::TextBinEncList encList;
			static void __stdcall OnConvertClicked(AnyType userObj);
			static void __stdcall OnHexClicked(AnyType userObj);
		public:
			AVIREncryptForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIREncryptForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
