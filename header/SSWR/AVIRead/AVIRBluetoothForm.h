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
				NN<IO::BTController> bt;
				NN<Data::ArrayListNN<IO::BTController::BTDevice>> devList;
			} BTStatus;
		private:
			NN<UI::GUIListBox> lbCtrl;
			NN<UI::GUIHSplitter> hspMain;
			NN<UI::GUITabControl> tcMain;

			NN<UI::GUITabPage> tpInfo;
			NN<UI::GUILabel> lblRadioName;
			NN<UI::GUITextBox> txtRadioName;
			NN<UI::GUILabel> lblAddr;
			NN<UI::GUITextBox> txtAddr;
			NN<UI::GUILabel> lblManu;
			NN<UI::GUITextBox> txtManu;
			NN<UI::GUILabel> lblSubver;
			NN<UI::GUITextBox> txtSubver;
			NN<UI::GUILabel> lblClass;
			NN<UI::GUITextBox> txtClass;

			NN<UI::GUITabPage> tpDevice;
			NN<UI::GUIPanel> pnlDevice;
			NN<UI::GUIButton> btnDeviceSrch;
			NN<UI::GUIButton> btnDeviceUpdate;
			NN<UI::GUIPanel> pnlDevDetail;
			NN<UI::GUIVSplitter> vspDevice;
			NN<UI::GUIPanel> pnlDevCtrl;
			NN<UI::GUIButton> btnDevAuthen;
			NN<UI::GUIButton> btnDevUnauthen;
			NN<UI::GUIListBox> lbDevServices;
			NN<UI::GUIListView> lvDevice;

			Data::ArrayListNN<BTStatus> btList;
			Optional<IO::BTController::BTDevice> currDev;
			Data::ArrayList<void *> guidList;

			NN<SSWR::AVIRead::AVIRCore> core;

			static void __stdcall OnCtrlChanged(AnyType userObj);
			static void __stdcall OnDeviceSrchClicked(AnyType userObj);
			static void __stdcall OnDeviceUpdateClicked(AnyType userObj);
			static void __stdcall OnDeviceSelChg(AnyType userObj);
			static void __stdcall OnDevAuthenClicked(AnyType userObj);
			static void __stdcall OnDevUnauthenClicked(AnyType userObj);

			void ClearGUIDs();
			void UpdateDevList(NN<BTStatus> btStatus);
		public:
			AVIRBluetoothForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRBluetoothForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
