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
				const Char *osVer;
			} OSItem;
			
		private:
			SSWR::AVIRead::AVIRCore *core;
			const UTF8Char *currUserAgent;
			Data::ArrayList<OSItem*> *osList;

			UI::GUIPanel *pnlFilter;
			UI::GUILabel *lblFilterOS;
			UI::GUIComboBox *cboFilterOS;
			UI::GUILabel *lblFilterBrowser;
			UI::GUIComboBox *cboFilterBrowser;
			UI::GUIPanel *pnlControl;
			UI::GUIButton *btnOk;
			UI::GUIListView *lvUserAgent;

			static void __stdcall OnOkClicked(void *userObj);
			static void __stdcall OnFilterChg(void *userObj);
			static void __stdcall OnUserAgentSelChg(void *userObj);
			static void __stdcall OnUserAgentDblClk(void *userObj, OSInt itemIndex);
			void UpdateUAList(Manage::OSInfo::OSType os, const Char *osVer, Net::BrowserInfo::BrowserType browser);
		public:
			AVIRUserAgentSelForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, const UTF8Char *currUserAgent);
			virtual ~AVIRUserAgentSelForm();

			virtual void OnMonitorChanged();

			const UTF8Char *GetUserAgent();
		};
	}
}
#endif
