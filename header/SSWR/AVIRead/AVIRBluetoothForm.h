#ifndef _SM_SSWR_AVIREAD_AVIRBLUETOOTHFORM
#define _SM_SSWR_AVIREAD_AVIRBLUETOOTHFORM
#include "Data/ArrayListNN.h"
#include "IO/BTManager.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"
#include "UI/GUIVSplitter.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRBluetoothForm : public UI::GUIForm
		{
		private:
			typedef struct
			{
				NotNullPtr<IO::BTController> bt;
				NotNullPtr<Data::ArrayListNN<IO::BTController::BTDevice>> devList;
			} BTStatus;
		private:
			NotNullPtr<UI::GUIListBox> lbCtrl;
			NotNullPtr<UI::GUIHSplitter> hspMain;
			NotNullPtr<UI::GUITabControl> tcMain;

			NotNullPtr<UI::GUITabPage> tpInfo;
			NotNullPtr<UI::GUILabel> lblRadioName;
			NotNullPtr<UI::GUITextBox> txtRadioName;
			NotNullPtr<UI::GUILabel> lblAddr;
			NotNullPtr<UI::GUITextBox> txtAddr;
			NotNullPtr<UI::GUILabel> lblManu;
			NotNullPtr<UI::GUITextBox> txtManu;
			NotNullPtr<UI::GUILabel> lblSubver;
			NotNullPtr<UI::GUITextBox> txtSubver;
			NotNullPtr<UI::GUILabel> lblClass;
			NotNullPtr<UI::GUITextBox> txtClass;

			NotNullPtr<UI::GUITabPage> tpDevice;
			NotNullPtr<UI::GUIPanel> pnlDevice;
			NotNullPtr<UI::GUIButton> btnDeviceSrch;
			NotNullPtr<UI::GUIButton> btnDeviceUpdate;
			NotNullPtr<UI::GUIPanel> pnlDevDetail;
			NotNullPtr<UI::GUIVSplitter> vspDevice;
			NotNullPtr<UI::GUIPanel> pnlDevCtrl;
			NotNullPtr<UI::GUIButton> btnDevAuthen;
			NotNullPtr<UI::GUIButton> btnDevUnauthen;
			NotNullPtr<UI::GUIListBox> lbDevServices;
			NotNullPtr<UI::GUIListView> lvDevice;

			Data::ArrayListNN<BTStatus> btList;
			Optional<IO::BTController::BTDevice> currDev;
			Data::ArrayList<void *> guidList;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;

			static void __stdcall OnCtrlChanged(AnyType userObj);
			static void __stdcall OnDeviceSrchClicked(AnyType userObj);
			static void __stdcall OnDeviceUpdateClicked(AnyType userObj);
			static void __stdcall OnDeviceSelChg(AnyType userObj);
			static void __stdcall OnDevAuthenClicked(AnyType userObj);
			static void __stdcall OnDevUnauthenClicked(AnyType userObj);

			void ClearGUIDs();
			void UpdateDevList(NotNullPtr<BTStatus> btStatus);
		public:
			AVIRBluetoothForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRBluetoothForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
