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

			NotNullPtr<UI::GUIPanel> pnlCtrl;
			NotNullPtr<UI::GUILabel> lblType;
			NotNullPtr<UI::GUIComboBox> cboType;
			NotNullPtr<UI::GUIButton> btnConv;
			NotNullPtr<UI::GUIButton> btnConv2;
			NotNullPtr<UI::GUILabel> lblPrefix;
			NotNullPtr<UI::GUITextBox> txtPrefix;
			NotNullPtr<UI::GUITextBox> txtSource;
			NotNullPtr<UI::GUIHSplitter> hspMain;
			NotNullPtr<UI::GUITextBox> txtDest;

			static void __stdcall OnConvClicked(void *userObj);
			static void __stdcall OnConv2Clicked(void *userObj);
			void ConvEnum();
		public:
			AVIRCppEnumForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRCppEnumForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
