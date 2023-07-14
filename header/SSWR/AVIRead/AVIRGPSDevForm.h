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
			UI::GUIPanel *pnlConn;
			UI::GUILabel *lblHost;
			UI::GUITextBox *txtHost;
			UI::GUILabel *lblPort;
			UI::GUITextBox *txtPort;
			UI::GUIButton *btnConn;
			UI::GUITabControl *tcMain;

			UI::GUITabPage *tpStatus;
			UI::GUILabel *lblStatusConn;
			UI::GUITextBox *txtStatusConn;

			UI::GUITabPage *tpDevice;
			UI::GUIPanel *pnlDeviceC;
			UI::GUIButton *btnDeviceR;
			UI::GUIListBox *lbDevice;
			UI::GUIHSplitter *hspDevice;
			UI::GUITabControl *tcDevice;
			UI::GUITabPage *tpDevInfo;
			UI::GUIListView *lvDevInfo;
			UI::GUITabPage *tpDevAlert;
			UI::GUIListView *lvDevAlert;
			UI::GUITabPage *tpDevGuard;
			UI::GUIListView *lvDevGuard;

			UI::GUITabPage *tpUser;
			UI::GUIPanel *pnlUserC;
			UI::GUIButton *btnUserR;
			UI::GUIListBox *lbUser;
			UI::GUIHSplitter *hspUser;
			UI::GUIPanel *pnlUser;
			UI::GUIListBox *lbUserDev;
			UI::GUIListView *lvUser;

			UI::GUITabPage *tpAlert;
			UI::GUIPanel *pnlAlertC;
			UI::GUIButton *btnAlertR;
			UI::GUIListBox *lbAlert;
			UI::GUIHSplitter *hspAlert;
			UI::GUIPanel *pnlAlert;
			UI::GUIListBox *lbAlertDev;
			UI::GUIListView *lvAlert;

			SSWR::AVIRead::AVIRCore *core;
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
			Data::ArrayListNN<Text::String> devConts;
			UOSInt devContACnt;
			DevAlert *devContAlerts;
			UOSInt devContGCnt;
			DevGuard *devContGuards;

			Bool alertContUpd;
			Sync::Mutex alertContMut;
			Data::ArrayListNN<Text::String> alertConts;
			Data::ArrayList<Int64> alertContDevs;

			Bool userContUpd;
			Sync::Mutex userContMut;
			Data::ArrayListNN<Text::String> userConts;
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
			AVIRGPSDevForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRGPSDevForm();

			virtual void OnMonitorChanged();

			virtual void DataParsed(NotNullPtr<IO::Stream> stm, void *stmObj, Int32 cmdType, Int32 seqId, const UInt8 *cmd, UOSInt cmdSize);
			virtual void DataSkipped(NotNullPtr<IO::Stream> stm, void *stmObj, const UInt8 *buff, UOSInt buffSize);
		};
	}
}
#endif
