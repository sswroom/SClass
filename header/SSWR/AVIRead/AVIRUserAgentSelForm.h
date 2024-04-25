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
			NN<SSWR::AVIRead::AVIRCore> core;
			const UTF8Char *currUserAgent;
			UOSInt currUserAgentLen;
			Data::ArrayList<OSItem*> *osList;

			NN<UI::GUIPanel> pnlFilter;
			NN<UI::GUILabel> lblFilterOS;
			NN<UI::GUIComboBox> cboFilterOS;
			NN<UI::GUILabel> lblFilterBrowser;
			NN<UI::GUIComboBox> cboFilterBrowser;
			NN<UI::GUIPanel> pnlControl;
			NN<UI::GUIButton> btnOk;
			NN<UI::GUIListView> lvUserAgent;

			static void __stdcall OnOkClicked(AnyType userObj);
			static void __stdcall OnFilterChg(AnyType userObj);
			static void __stdcall OnUserAgentSelChg(AnyType userObj);
			static void __stdcall OnUserAgentDblClk(AnyType userObj, UOSInt itemIndex);
			void UpdateUAList(Manage::OSInfo::OSType os, Text::CString osVer, Net::BrowserInfo::BrowserType browser);
		public:
			AVIRUserAgentSelForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, Text::CString currUserAgent);
			virtual ~AVIRUserAgentSelForm();

			virtual void OnMonitorChanged();

			Text::CString GetUserAgent();
		};
	}
}
#endif
