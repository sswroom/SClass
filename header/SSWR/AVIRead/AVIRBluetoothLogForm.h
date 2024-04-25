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
			NN<SSWR::AVIRead::AVIRCore> core;

			NN<UI::GUIPanel> pnlControl;
			NN<UI::GUIButton> btnFile;
			NN<UI::GUICheckBox> chkUnkOnly;
			NN<UI::GUIButton> btnStore;
			NN<UI::GUILabel> lblInfo;
			NN<UI::GUIListView> lvContent;

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
			AVIRBluetoothLogForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRBluetoothLogForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
