#ifndef _SM_SSWR_AVIREAD_AVIRCPPENUMFORM
#define _SM_SSWR_AVIREAD_AVIRCPPENUMFORM
#include "SSWR/AVIRead/AVIRCore.h"
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
		class AVIRCppEnumForm : public UI::GUIForm
		{
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;

			UI::GUIPanel *pnlCtrl;
			UI::GUILabel *lblType;
			UI::GUIComboBox *cboType;
			UI::GUIButton *btnConv;
			UI::GUIButton *btnConv2;
			UI::GUILabel *lblPrefix;
			UI::GUITextBox *txtPrefix;
			UI::GUITextBox *txtSource;
			UI::GUIHSplitter *hspMain;
			UI::GUITextBox *txtDest;

			static void __stdcall OnConvClicked(void *userObj);
			static void __stdcall OnConv2Clicked(void *userObj);
			void ConvEnum();
		public:
			AVIRCppEnumForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRCppEnumForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
