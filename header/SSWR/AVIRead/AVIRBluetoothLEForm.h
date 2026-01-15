#ifndef _SM_SSWR_AVIREAD_AVIRBLUETOOTHLEFORM
#define _SM_SSWR_AVIREAD_AVIRBLUETOOTHLEFORM
#include "Data/FastMapNN.hpp"
#include "IO/BTManager.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRBluetoothLEForm : public UI::GUIForm
		{
		private:
			typedef struct
			{
				UInt64 mac;
				Optional<Text::String> name;
				Int32 rssi;
				Bool updated;
				Bool shown;
			} BTDevice;
			
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			IO::BTManager btMgr;
			Data::ArrayListNN<IO::BTController> btList;
			Optional<IO::BTController> btCtrl;
			Sync::Mutex devMut;
			Data::FastMapNN<UInt64, BTDevice> devMap;

			NN<UI::GUIPanel> pnlControl;
			NN<UI::GUILabel> lblInterface;
			NN<UI::GUIComboBox> cboInterface;
			NN<UI::GUIButton> btnStart;
			NN<UI::GUIButton> btnStoreList;
			NN<UI::GUIListView> lvDevices;

			static void __stdcall OnStartClicked(AnyType userObj);
			static void __stdcall OnStoreListClicked(AnyType userObj);
			static void __stdcall OnDevicesDblClick(AnyType userObj, UIntOS index);
			static void __stdcall OnTimerTick(AnyType userObj);
			static void __stdcall OnLEScanItem(AnyType userObj, UInt64 mac, Int32 rssi, Text::CString name);
			void ClearDevices();
		public:
			AVIRBluetoothLEForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRBluetoothLEForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
