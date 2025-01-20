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
		class AVIRGPSDevForm : public UI::GUIForm, public IO::ProtocolHandler::DataListener
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
			NN<UI::GUIPanel> pnlConn;
			NN<UI::GUILabel> lblHost;
			NN<UI::GUITextBox> txtHost;
			NN<UI::GUILabel> lblPort;
			NN<UI::GUITextBox> txtPort;
			NN<UI::GUIButton> btnConn;
			NN<UI::GUITabControl> tcMain;

			NN<UI::GUITabPage> tpStatus;
			NN<UI::GUILabel> lblStatusConn;
			NN<UI::GUITextBox> txtStatusConn;

			NN<UI::GUITabPage> tpDevice;
			NN<UI::GUIPanel> pnlDeviceC;
			NN<UI::GUIButton> btnDeviceR;
			NN<UI::GUIListBox> lbDevice;
			NN<UI::GUIHSplitter> hspDevice;
			NN<UI::GUITabControl> tcDevice;
			NN<UI::GUITabPage> tpDevInfo;
			NN<UI::GUIListView> lvDevInfo;
			NN<UI::GUITabPage> tpDevAlert;
			NN<UI::GUIListView> lvDevAlert;
			NN<UI::GUITabPage> tpDevGuard;
			NN<UI::GUIListView> lvDevGuard;

			NN<UI::GUITabPage> tpUser;
			NN<UI::GUIPanel> pnlUserC;
			NN<UI::GUIButton> btnUserR;
			NN<UI::GUIListBox> lbUser;
			NN<UI::GUIHSplitter> hspUser;
			NN<UI::GUIPanel> pnlUser;
			NN<UI::GUIListBox> lbUserDev;
			NN<UI::GUIListView> lvUser;

			NN<UI::GUITabPage> tpAlert;
			NN<UI::GUIPanel> pnlAlertC;
			NN<UI::GUIButton> btnAlertR;
			NN<UI::GUIListBox> lbAlert;
			NN<UI::GUIHSplitter> hspAlert;
			NN<UI::GUIPanel> pnlAlert;
			NN<UI::GUIListBox> lbAlertDev;
			NN<UI::GUIListView> lvAlert;

			NN<SSWR::AVIRead::AVIRCore> core;
			Sync::Mutex cliMut;
			Optional<Net::TCPClient> cli;
			IO::ProtoHdlr::ProtoGPSDevInfoHandler protoHdlr;
			Bool threadRunning;
			Bool threadToStop;
			Sync::Event threadEvt;
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
			UnsafeArrayOpt<DevAlert> devContAlerts;
			UOSInt devContGCnt;
			UnsafeArrayOpt<DevGuard> devContGuards;

			Bool alertContUpd;
			Sync::Mutex alertContMut;
			Data::ArrayListStringNN alertConts;
			Data::ArrayList<Int64> alertContDevs;

			Bool userContUpd;
			Sync::Mutex userContMut;
			Data::ArrayListStringNN userConts;
			Data::ArrayList<Int64> userContDevs;

			static UInt32 __stdcall ClientThread(AnyType userObj);
			static void __stdcall OnConnClicked(AnyType userObj);
			static void __stdcall OnDeviceRClicked(AnyType userObj);
			static void __stdcall OnUserRClicked(AnyType userObj);
			static void __stdcall OnAlertRClicked(AnyType userObj);
			static void __stdcall OnDeviceSelChg(AnyType userObj);
			static void __stdcall OnAlertSelChg(AnyType userObj);
			static void __stdcall OnUserSelChg(AnyType userObj);
			static void __stdcall OnTimerTick(AnyType userObj);

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
			AVIRGPSDevForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRGPSDevForm();

			virtual void OnMonitorChanged();

			virtual void DataParsed(NN<IO::Stream> stm, AnyType stmObj, Int32 cmdType, Int32 seqId, UnsafeArray<const UInt8> cmd, UOSInt cmdSize);
			virtual void DataSkipped(NN<IO::Stream> stm, AnyType stmObj, UnsafeArray<const UInt8> buff, UOSInt buffSize);
		};
	}
}
#endif
