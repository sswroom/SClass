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
			NN<SSWR::AVIRead::AVIRCore> core;

			NN<UI::GUIPanel> pnlControl;
			NN<UI::GUIButton> btnFile;
			NN<UI::GUICheckBox> chkUnkOnly;
			NN<UI::GUICheckBox> chkLocal;
			NN<UI::GUITextBox> txtFilter;
			NN<UI::GUIButton> btnFilter;
			NN<UI::GUIButton> btnStore;
			NN<UI::GUILabel> lblInfo;
			NN<UI::GUILabel> lblDblClk;
			NN<UI::GUIComboBox> cboDblClk;
			NN<UI::GUITextBox> txtFileIE;
			NN<UI::GUIVSplitter> vspFile;
			NN<UI::GUIListView> lvContent;

			NN<Net::WiFiLogFile> wifiLogFile;
			NN<Net::MACInfoList> macList;
			Optional<Text::String> filterText;

			static void __stdcall OnFileClicked(AnyType userObj);
			static void __stdcall OnStoreClicked(AnyType userObj);
			static void __stdcall OnContentDblClicked(AnyType userObj, UIntOS index);
			static void __stdcall OnContentSelChg(AnyType userObj);
			static void __stdcall OnUnkOnlyChkChg(AnyType userObj, Bool checked);
			static void __stdcall OnFilterClicked(AnyType userObj);
			Bool LogFileStore();
			void LogUIUpdate();
			void EntryUpdated(NN<const Net::MACInfo::MACEntry> entry);

			void UpdateStatus();
		public:
			AVIRWiFiLogManagerForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRWiFiLogManagerForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
