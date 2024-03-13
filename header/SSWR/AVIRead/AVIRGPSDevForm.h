#ifndef _SM_SSWR_AVIREAD_AVIRGPSDEVFORM
#define _SM_SSWR_AVIREAD_AVIRGPSDEVFORM
#include "Data/ArrayListNN.h"
#include "IO/ProtoHdlr/ProtoGPSDevInfoHandler.h"
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

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRGPSDevForm : public UI::GUIForm, public IO::IProtocolHandler::DataListener
		{
		private:
			typedef struct
			{
				Int32 alertId;
				Bool isAlerting;
				Bool isAlerted;
				Bool isFirst;
				Int64 beginTimeTick;
				Int64 lastTimeTick;
			} DevAlert;

			typedef struct
			{
				Int32 userId;
				Int32 alarmType;
				Double alarmLat;
				Double alarmLon;
				Int64 alarmStartTicks;
				Bool found;
				Int32 alarmStatus;
				Int32 alarmStatus2;
				Int32 alarmStatus3;
				Int32 alarmStatus4;
				Int32 alarmStatus5;
			} DevGuard;
		private:
			NotNullPtr<UI::GUIPanel> pnlConn;
			NotNullPtr<UI::GUILabel> lblHost;
			NotNullPtr<UI::GUITextBox> txtHost;
			NotNullPtr<UI::GUILabel> lblPort;
			NotNullPtr<UI::GUITextBox> txtPort;
			NotNullPtr<UI::GUIButton> btnConn;
			NotNullPtr<UI::GUITabControl> tcMain;

			NotNullPtr<UI::GUITabPage> tpStatus;
			NotNullPtr<UI::GUILabel> lblStatusConn;
			NotNullPtr<UI::GUITextBox> txtStatusConn;

			NotNullPtr<UI::GUITabPage> tpDevice;
			NotNullPtr<UI::GUIPanel> pnlDeviceC;
			NotNullPtr<UI::GUIButton> btnDeviceR;
			NotNullPtr<UI::GUIListBox> lbDevice;
			NotNullPtr<UI::GUIHSplitter> hspDevice;
			NotNullPtr<UI::GUITabControl> tcDevice;
			NotNullPtr<UI::GUITabPage> tpDevInfo;
			NotNullPtr<UI::GUIListView> lvDevInfo;
			NotNullPtr<UI::GUITabPage> tpDevAlert;
			NotNullPtr<UI::GUIListView> lvDevAlert;
			NotNullPtr<UI::GUITabPage> tpDevGuard;
			NotNullPtr<UI::GUIListView> lvDevGuard;

			NotNullPtr<UI::GUITabPage> tpUser;
			NotNullPtr<UI::GUIPanel> pnlUserC;
			NotNullPtr<UI::GUIButton> btnUserR;
			NotNullPtr<UI::GUIListBox> lbUser;
			NotNullPtr<UI::GUIHSplitter> hspUser;
			NotNullPtr<UI::GUIPanel> pnlUser;
			NotNullPtr<UI::GUIListBox> lbUserDev;
			NotNullPtr<UI::GUIListView> lvUser;

			NotNullPtr<UI::GUITabPage> tpAlert;
			NotNullPtr<UI::GUIPanel> pnlAlertC;
			NotNullPtr<UI::GUIButton> btnAlertR;
			NotNullPtr<UI::GUIListBox> lbAlert;
			NotNullPtr<UI::GUIHSplitter> hspAlert;
			NotNullPtr<UI::GUIPanel> pnlAlert;
			NotNullPtr<UI::GUIListBox> lbAlertDev;
			NotNullPtr<UI::GUIListView> lvAlert;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Sync::Mutex cliMut;
			Net::TCPClient *cli;
			IO::ProtoHdlr::ProtoGPSDevInfoHandler protoHdlr;
			Bool threadRunning;
			Bool threadToStop;
			Sync::Event *threadEvt;
			Bool dispConn;

			Sync::Mutex alertMut;
			Data::ArrayList<Int32> alertList;
			Bool alertUpd;

			Sync::Mutex deviceMut;
			Data::ArrayList<Int64> deviceList;
			Bool deviceUpd;

			Sync::Mutex userMut;
			Data::ArrayList<Int32> userList;
			Bool userUpd;

			Bool devContUpd;
			Sync::Mutex devContMut;
			Data::ArrayListStringNN devConts;
			UOSInt devContACnt;
			DevAlert *devContAlerts;
			UOSInt devContGCnt;
			DevGuard *devContGuards;

			Bool alertContUpd;
			Sync::Mutex alertContMut;
			Data::ArrayListStringNN alertConts;
			Data::ArrayList<Int64> alertContDevs;

			Bool userContUpd;
			Sync::Mutex userContMut;
			Data::ArrayListStringNN userConts;
			Data::ArrayList<Int64> userContDevs;

			static UInt32 __stdcall ClientThread(void *userObj);
			static void __stdcall OnConnClicked(void *userObj);
			static void __stdcall OnDeviceRClicked(void *userObj);
			static void __stdcall OnUserRClicked(void *userObj);
			static void __stdcall OnAlertRClicked(void *userObj);
			static void __stdcall OnDeviceSelChg(void *userObj);
			static void __stdcall OnAlertSelChg(void *userObj);
			static void __stdcall OnUserSelChg(void *userObj);
			static void __stdcall OnTimerTick(void *userObj);

			void ToStop();
			void ClearDevConts();
			void ClearAlertConts();
			void ClearUserConts();

			void SendGetAlerts();
			void SendGetDevices();
			void SendGetUsers();
			void SendGetDevice(Int64 devId);
			void SendGetAlert(Int32 alertId);
			void SendGetUser(Int32 userId);
		public:
			AVIRGPSDevForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRGPSDevForm();

			virtual void OnMonitorChanged();

			virtual void DataParsed(NotNullPtr<IO::Stream> stm, void *stmObj, Int32 cmdType, Int32 seqId, const UInt8 *cmd, UOSInt cmdSize);
			virtual void DataSkipped(NotNullPtr<IO::Stream> stm, void *stmObj, const UInt8 *buff, UOSInt buffSize);
		};
	}
}
#endif
