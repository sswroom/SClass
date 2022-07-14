#ifndef _SM_SSWR_AVIREAD_AVIRSNBDONGLEFORM
#define _SM_SSWR_AVIREAD_AVIRSNBDONGLEFORM
#include "Data/Int64Map.h"
#include "IO/LogTool.h"
#include "IO/SNBDongle.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "Sync/RWMutex.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUIGroupBox.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"
#include "UI/ListBoxLogger.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRSNBDongleForm : public UI::GUIForm, public IO::SNBDongle::SNBHandler
		{
		private:
			struct DeviceInfo
			{
				UInt64 devId;
				UInt16 shortAddr;
				IO::SNBDongle::HandleType handType;
				Sync::RWMutex mut;
				Bool readingChg;
				Bool shortAddrChg;
				IO::SNBDongle::SensorType sensorType;
				Int64 readingTime;
				UOSInt nReading;
				Double readings[16];
				IO::SNBDongle::ReadingType readingTypes[16];
			};
		private:
			SSWR::AVIRead::AVIRCore *core;
			Net::SSLEngine *ssl;
			IO::Stream *stm;
			IO::SNBDongle *snb;
			IO::LogTool log;
			Bool dongleUpdated;

			Sync::RWMutex devMut;
			Data::UInt64Map<DeviceInfo*> devMap;
			Bool devChg;
			Data::UInt64Map<Int32> devHandlerMap;

			UI::GUITabControl *tcMain;

			UI::GUITabPage *tpDevice;
			UI::GUIPanel *pnlDevice;
			UI::GUIButton *btnDongleInfo;
			UI::GUILabel *lblDongleId;
			UI::GUITextBox *txtDongleId;
			UI::GUIButton *btnCheckDongle;
			UI::GUIButton *btnCheckDevices;
			UI::GUIButton *btnResetNetwork;
			UI::GUIButton *btnAddDevice;
			UI::GUILabel *lblRemarks;
			UI::GUITextBox *txtRemarks;
			UI::GUILabel *lblURL;
			UI::GUITextBox *txtURL;
			UI::GUIButton *btnUpload;
			UI::GUIGroupBox *grpDevice;
			UI::GUIPanel *pnlDevCtrl;
			UI::GUIButton *btnCopyDevId;
			UI::GUIButton *btnDevReportTime;
			UI::GUIButton *btnDevSetReportTime;
			UI::GUIButton *btnDevOn;
			UI::GUIButton *btnDevOff;
			UI::GUIButton *btnDevStatus;
			UI::GUIListView *lvDevice;

			UI::GUITabPage *tpLog;
			UI::GUIListBox *lbLog;
			UI::GUITextBox *txtLog;
			UI::ListBoxLogger *logger;

			static void __stdcall OnProtocolReceived(void *userObj, UInt8 cmdType, UOSInt cmdSize, UInt8 *cmd);
			static void __stdcall OnDongleInfoClicked(void *userObj);
			static void __stdcall OnCheckDongleClicked(void *userObj);
			static void __stdcall OnCheckDevicesClicked(void *userObj);
			static void __stdcall OnResetNetworkClicked(void *userObj);
			static void __stdcall OnAddDeviceClicked(void *userObj);
			static void __stdcall OnLogClicked(void *userObj);
			static void __stdcall OnTimerTick(void *userObj);
			static void __stdcall OnCopyDevIdClicked(void *userObj);
			static void __stdcall OnDevReportTimeClicked(void *userObj);
			static void __stdcall OnDevSetReportTimeClicked(void *userObj);
			static void __stdcall OnDevOnClicked(void *userObj);
			static void __stdcall OnDevOffClicked(void *userObj);
			static void __stdcall OnDevStatusClicked(void *userObj);
			static void __stdcall OnDeviceDblClk(void *userObj, UOSInt index);
			static void __stdcall OnUploadClicked(void *userObj);

			void LoadFile();
			void SaveFile();
		public:
			AVIRSNBDongleForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, IO::Stream *stm);
			virtual ~AVIRSNBDongleForm();

			virtual void OnMonitorChanged();

			virtual void DeviceAdded(UInt64 devId);
			virtual void DeviceSensor(UInt64 devId, IO::SNBDongle::SensorType sensorType, UOSInt nReading, IO::SNBDongle::ReadingType *readingTypes, Double *readingVals);
			virtual void DeviceUpdated(UInt64 devId, UInt16 shortAddr);
			virtual void DongleInfoUpdated();
		};
	}
}
#endif
