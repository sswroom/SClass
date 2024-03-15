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
			NotNullPtr<UI::GUIButton> btnFile;
			NotNullPtr<UI::GUICheckBox> chkUnkOnly;
			NotNullPtr<UI::GUIButton> btnStore;
			NotNullPtr<UI::GUILabel> lblInfo;
			NotNullPtr<UI::GUIListView> lvContent;

			IO::BTDevLog btLog;
			Net::MACInfoList macList;

			static void __stdcall OnFileClicked(AnyType userObj);
			static void __stdcall OnStoreClicked(AnyType userObj);
			static void __stdcall OnContentDblClicked(AnyType userObj, UOSInt index);
			static void __stdcall OnContentSelChg(AnyType userObj);
			static void __stdcall OnUnkOnlyChkChg(AnyType userObj, Bool checked);
			Bool LogFileStore();
			void LogUIUpdate();

			void UpdateStatus();
		public:
			AVIRBluetoothLogForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRBluetoothLogForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
