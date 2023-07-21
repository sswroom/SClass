#ifndef _SM_SSWR_AVIREAD_AVIRWIFILOGMANAGERFORM
#define _SM_SSWR_AVIREAD_AVIRWIFILOGMANAGERFORM
#include "Net/MACInfoList.h"
#include "Net/WiFiLogFile.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"
#include "UI/GUIVSplitter.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRWiFiLogManagerForm : public UI::GUIForm
		{
		private:
			SSWR::AVIRead::AVIRCore *core;

			UI::GUIPanel *pnlControl;
			UI::GUIButton *btnFile;
			UI::GUICheckBox *chkUnkOnly;
			UI::GUITextBox *txtFilter;
			UI::GUIButton *btnFilter;
			UI::GUIButton *btnStore;
			UI::GUILabel *lblInfo;
			UI::GUILabel *lblDblClk;
			UI::GUIComboBox *cboDblClk;
			UI::GUITextBox *txtFileIE;
			UI::GUIVSplitter *vspFile;
			UI::GUIListView *lvContent;

			Net::WiFiLogFile *wifiLogFile;
			Net::MACInfoList *macList;
			Text::String *filterText;

			static void __stdcall OnFileClicked(void *userObj);
			static void __stdcall OnStoreClicked(void *userObj);
			static void __stdcall OnContentDblClicked(void *userObj, UOSInt index);
			static void __stdcall OnContentSelChg(void *userObj);
			static void __stdcall OnUnkOnlyChkChg(void *userObj, Bool checked);
			static void __stdcall OnFilterClicked(void *userObj);
			Bool LogFileStore();
			void LogUIUpdate();
			void EntryUpdated(const Net::MACInfo::MACEntry *entry);

			void UpdateStatus();
		public:
			AVIRWiFiLogManagerForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRWiFiLogManagerForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
