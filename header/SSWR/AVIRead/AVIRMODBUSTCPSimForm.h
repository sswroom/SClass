#ifndef _SM_SSWR_AVIREAD_AVIRMODBUSTCPSIMFORM
#define _SM_SSWR_AVIREAD_AVIRMODBUSTCPSIMFORM
#include "Net/MODBUSTCPListener.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUIGroupBox.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRMODBUSTCPSimForm : public UI::GUIForm
		{
		private:
			enum class DeviceType
			{
				Print,
				ED538,
				ED588,
				ED516,
				ED527,

				First = Print,
				Last = ED527
			};
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Net::MODBUSTCPListener *listener;
			IO::LogTool log;
			IO::MODBUSDevSim *currDev;

			NotNullPtr<UI::GUIPanel> pnlCtrl;
			NotNullPtr<UI::GUILabel> lblPort;
			NotNullPtr<UI::GUITextBox> txtPort;
			NotNullPtr<UI::GUIButton> btnListen;
			NotNullPtr<UI::GUIGroupBox> grpDev;
			NotNullPtr<UI::GUILabel> lblAddr;
			NotNullPtr<UI::GUITextBox> txtAddr;
			NotNullPtr<UI::GUIComboBox> cboDevType;
			NotNullPtr<UI::GUIButton> btnDev;
			NotNullPtr<UI::GUILabel> lblDelay;
			NotNullPtr<UI::GUITextBox> txtDelay;
			NotNullPtr<UI::GUIButton> btnDelay;
			NotNullPtr<UI::GUIListBox> lbDevice;
			NotNullPtr<UI::GUIListView> lvDeviceValues;

			static void __stdcall OnListenClicked(AnyType userObj);
			static void __stdcall OnDevAddClicked(AnyType userObj);
			static void __stdcall OnDelayClicked(AnyType userObj);
			static void __stdcall OnDeviceChanged(AnyType userObj);
			static void __stdcall OnDeviceValuesDblClk(AnyType userObj, UOSInt index);
			static void __stdcall OnTimerTick(AnyType userObj);
			void UpdateDevList();
			void UpdateDevValues();
		public:
			AVIRMODBUSTCPSimForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRMODBUSTCPSimForm();

			virtual void OnMonitorChanged();

			static Text::CStringNN DeviceTypeGetName(DeviceType devType);
		};
	}
}
#endif
