#ifndef _SM_SSWR_AVIREAD_AVIRSNBDONGLEFORM
#define _SM_SSWR_AVIREAD_AVIRSNBDONGLEFORM
#include "Data/FastMap.h"
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
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::SSLEngine> ssl;
			NotNullPtr<IO::Stream> stm;
			IO::SNBDongle *snb;
			IO::LogTool log;
			Bool dongleUpdated;

			Sync::RWMutex devMut;
			Data::FastMap<UInt64, DeviceInfo*> devMap;
			Bool devChg;
			Data::FastMap<UInt64, Int32> devHandlerMap;

			NotNullPtr<UI::GUITabControl> tcMain;

			NotNullPtr<UI::GUITabPage> tpDevice;
			NotNullPtr<UI::GUIPanel> pnlDevice;
			NotNullPtr<UI::GUIButton> btnDongleInfo;
			NotNullPtr<UI::GUILabel> lblDongleId;
			NotNullPtr<UI::GUITextBox> txtDongleId;
			NotNullPtr<UI::GUIButton> btnCheckDongle;
			NotNullPtr<UI::GUIButton> btnCheckDevices;
			NotNullPtr<UI::GUIButton> btnResetNetwork;
			NotNullPtr<UI::GUIButton> btnAddDevice;
			NotNullPtr<UI::GUILabel> lblRemarks;
			NotNullPtr<UI::GUITextBox> txtRemarks;
			NotNullPtr<UI::GUILabel> lblURL;
			NotNullPtr<UI::GUITextBox> txtURL;
			NotNullPtr<UI::GUIButton> btnUpload;
			NotNullPtr<UI::GUIGroupBox> grpDevice;
			NotNullPtr<UI::GUIPanel> pnlDevCtrl;
			NotNullPtr<UI::GUIButton> btnCopyDevId;
			NotNullPtr<UI::GUIButton> btnDevReportTime;
			NotNullPtr<UI::GUIButton> btnDevSetReportTime;
			NotNullPtr<UI::GUIButton> btnDevOn;
			NotNullPtr<UI::GUIButton> btnDevOff;
			NotNullPtr<UI::GUIButton> btnDevStatus;
			UI::GUIListView *lvDevice;

			NotNullPtr<UI::GUITabPage> tpLog;
			NotNullPtr<UI::GUIListBox> lbLog;
			NotNullPtr<UI::GUITextBox> txtLog;
			NotNullPtr<UI::ListBoxLogger> logger;

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
			AVIRSNBDongleForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, NotNullPtr<IO::Stream> stm);
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
