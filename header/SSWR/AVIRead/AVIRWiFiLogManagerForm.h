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
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;

			NotNullPtr<UI::GUIPanel> pnlControl;
			NotNullPtr<UI::GUIButton> btnFile;
			UI::GUICheckBox *chkUnkOnly;
			UI::GUITextBox *txtFilter;
			NotNullPtr<UI::GUIButton> btnFilter;
			NotNullPtr<UI::GUIButton> btnStore;
			NotNullPtr<UI::GUILabel> lblInfo;
			NotNullPtr<UI::GUILabel> lblDblClk;
			NotNullPtr<UI::GUIComboBox> cboDblClk;
			UI::GUITextBox *txtFileIE;
			NotNullPtr<UI::GUIVSplitter> vspFile;
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
			AVIRWiFiLogManagerForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRWiFiLogManagerForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
