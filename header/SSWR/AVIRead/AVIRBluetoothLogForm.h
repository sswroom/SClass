#ifndef _SM_SSWR_AVIREAD_AVIRBLUETOOTHLOGFORM
#define _SM_SSWR_AVIREAD_AVIRBLUETOOTHLOGFORM
#include "IO/BTDevLog.h"
#include "Net/MACInfoList.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRBluetoothLogForm : public UI::GUIForm
		{
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;

			NotNullPtr<UI::GUIPanel> pnlControl;
			UI::GUIButton *btnFile;
			UI::GUICheckBox *chkUnkOnly;
			UI::GUIButton *btnStore;
			UI::GUILabel *lblInfo;
			UI::GUIListView *lvContent;

			IO::BTDevLog btLog;
			Net::MACInfoList macList;

			static void __stdcall OnFileClicked(void *userObj);
			static void __stdcall OnStoreClicked(void *userObj);
			static void __stdcall OnContentDblClicked(void *userObj, UOSInt index);
			static void __stdcall OnContentSelChg(void *userObj);
			static void __stdcall OnUnkOnlyChkChg(void *userObj, Bool checked);
			Bool LogFileStore();
			void LogUIUpdate();

			void UpdateStatus();
		public:
			AVIRBluetoothLogForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRBluetoothLogForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
