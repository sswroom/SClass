#ifndef _SM_SSWR_AVIREAD_AVIRBLUETOOTHCTLFORM
#define _SM_SSWR_AVIREAD_AVIRBLUETOOTHCTLFORM
#include "Data/FastMapNative.hpp"
#include "IO/BTScanner.h"
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
			NN<SSWR::AVIRead::AVIRCore> core;
			Sync::Mutex devMut;
			Data::FastMapNative<UInt64, UInt32> randDevMap;
			Data::FastMapNative<UInt64, UInt32> pubDevMap;
			Optional<IO::BTScanner> bt;

			NN<UI::GUIPanel> pnlControl;
			NN<UI::GUIButton> btnStart;
			NN<UI::GUIButton> btnStoreList;
			NN<UI::GUIListView> lvDevices;

			static void __stdcall OnStartClicked(AnyType userObj);
			static void __stdcall OnStoreListClicked(AnyType userObj);
			static void __stdcall OnDevicesDblClick(AnyType userObj, UOSInt index);
			static void __stdcall OnTimerTick(AnyType userObj);
			static void __stdcall OnDeviceUpdated(NN<IO::BTScanLog::ScanRecord3> dev, IO::BTScanner::UpdateType updateType, AnyType userObj);
			UOSInt UpdateList(NN<Data::FastMapNN<UInt64, IO::BTScanLog::ScanRecord3>> devMap, NN<Data::FastMapNative<UInt64, UInt32>> statusMap, UOSInt baseIndex);
		public:
			AVIRBluetoothCtlForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRBluetoothCtlForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
