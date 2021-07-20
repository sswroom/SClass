#ifndef _SM_SSWR_AVIREAD_AVIRBLUETOOTHCTLFORM
#define _SM_SSWR_AVIREAD_AVIRBLUETOOTHCTLFORM
#include "Data/UInt64Map.h"
#include "IO/ProgCtrl/BluetoothCtlProgCtrl.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRBluetoothCtlForm : public UI::GUIForm
		{
		private:
			SSWR::AVIRead::AVIRCore *core;
			Sync::Mutex *devMut;
			Data::UInt64Map<UInt32> *devMap;
			IO::ProgCtrl::BluetoothCtlProgCtrl *bt;

			UI::GUIPanel *pnlControl;
			UI::GUIButton *btnStart;
			UI::GUIButton *btnStoreList;
			UI::GUIListView *lvDevices;

			static void __stdcall OnStartClicked(void *userObj);
			static void __stdcall OnStoreListClicked(void *userObj);
			static void __stdcall OnDevicesDblClick(void *userObj, UOSInt index);
			static void __stdcall OnTimerTick(void *userObj);
			static void __stdcall OnDeviceUpdated(IO::ProgCtrl::BluetoothCtlProgCtrl::DeviceInfo *dev, IO::ProgCtrl::BluetoothCtlProgCtrl::UpdateType updateType, void *userObj);
		public:
			AVIRBluetoothCtlForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRBluetoothCtlForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
