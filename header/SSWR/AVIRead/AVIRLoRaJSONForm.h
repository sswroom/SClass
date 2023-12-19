#ifndef _SM_SSWR_AVIREAD_AVIRLORAJSONFORM
#define _SM_SSWR_AVIREAD_AVIRLORAJSONFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/ListBoxLogger.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUIPanel.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRLoRaJSONForm : public UI::GUIForm
		{
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;

			NotNullPtr<UI::GUIPanel> pnlJSON;
			NotNullPtr<UI::GUILabel> lblJSON;
			NotNullPtr<UI::GUITextBox> txtJSON;
			NotNullPtr<UI::GUIPanel> pnlJSONCtrl;
			NotNullPtr<UI::GUIButton> btnJSONParse;
			NotNullPtr<UI::GUILabel> lblInfo;
			NotNullPtr<UI::GUITextBox> txtInfo;

		private:
			static void __stdcall OnJSONParseClick(void *userObj);
			static void PHYPayloadDetail(NotNullPtr<Text::StringBuilderUTF8> sb, const UInt8 *buff, UOSInt buffSize);
			static void MACPayloadDetail(NotNullPtr<Text::StringBuilderUTF8> sb, Bool downLink, const UInt8 *buff, UOSInt buffSize);
		public:
			AVIRLoRaJSONForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRLoRaJSONForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
