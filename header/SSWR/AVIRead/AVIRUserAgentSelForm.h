#ifndef _SM_SSWR_AVIREAD_AVIRUSERAGENTSELFORM
#define _SM_SSWR_AVIREAD_AVIRUSERAGENTSELFORM
#include "Manage/OSInfo.h"
#include "Net/BrowserInfo.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRUserAgentSelForm : public UI::GUIForm
		{
		private:
			typedef struct
			{
				Manage::OSInfo::OSType os;
				const UTF8Char *osVer;
				UOSInt osVerLen;
			} OSItem;
			
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			const UTF8Char *currUserAgent;
			UOSInt currUserAgentLen;
			Data::ArrayList<OSItem*> *osList;

			NotNullPtr<UI::GUIPanel> pnlFilter;
			UI::GUILabel *lblFilterOS;
			NotNullPtr<UI::GUIComboBox> cboFilterOS;
			UI::GUILabel *lblFilterBrowser;
			NotNullPtr<UI::GUIComboBox> cboFilterBrowser;
			NotNullPtr<UI::GUIPanel> pnlControl;
			NotNullPtr<UI::GUIButton> btnOk;
			UI::GUIListView *lvUserAgent;

			static void __stdcall OnOkClicked(void *userObj);
			static void __stdcall OnFilterChg(void *userObj);
			static void __stdcall OnUserAgentSelChg(void *userObj);
			static void __stdcall OnUserAgentDblClk(void *userObj, UOSInt itemIndex);
			void UpdateUAList(Manage::OSInfo::OSType os, Text::CString osVer, Net::BrowserInfo::BrowserType browser);
		public:
			AVIRUserAgentSelForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, Text::CString currUserAgent);
			virtual ~AVIRUserAgentSelForm();

			virtual void OnMonitorChanged();

			Text::CString GetUserAgent();
		};
	}
}
#endif
