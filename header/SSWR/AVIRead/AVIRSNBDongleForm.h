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
			NN<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::SSLEngine> ssl;
			NN<IO::Stream> stm;
			IO::SNBDongle *snb;
			IO::LogTool log;
			Bool dongleUpdated;

			Sync::RWMutex devMut;
			Data::FastMap<UInt64, DeviceInfo*> devMap;
			Bool devChg;
			Data::FastMap<UInt64, Int32> devHandlerMap;

			NN<UI::GUITabControl> tcMain;

			NN<UI::GUITabPage> tpDevice;
			NN<UI::GUIPanel> pnlDevice;
			NN<UI::GUIButton> btnDongleInfo;
			NN<UI::GUILabel> lblDongleId;
			NN<UI::GUITextBox> txtDongleId;
			NN<UI::GUIButton> btnCheckDongle;
			NN<UI::GUIButton> btnCheckDevices;
			NN<UI::GUIButton> btnResetNetwork;
			NN<UI::GUIButton> btnAddDevice;
			NN<UI::GUILabel> lblRemarks;
			NN<UI::GUITextBox> txtRemarks;
			NN<UI::GUILabel> lblURL;
			NN<UI::GUITextBox> txtURL;
			NN<UI::GUIButton> btnUpload;
			NN<UI::GUIGroupBox> grpDevice;
			NN<UI::GUIPanel> pnlDevCtrl;
			NN<UI::GUIButton> btnCopyDevId;
			NN<UI::GUIButton> btnDevReportTime;
			NN<UI::GUIButton> btnDevSetReportTime;
			NN<UI::GUIButton> btnDevOn;
			NN<UI::GUIButton> btnDevOff;
			NN<UI::GUIButton> btnDevStatus;
			NN<UI::GUIListView> lvDevice;

			NN<UI::GUITabPage> tpLog;
			NN<UI::GUIListBox> lbLog;
			NN<UI::GUITextBox> txtLog;
			NN<UI::ListBoxLogger> logger;

			static void __stdcall OnProtocolReceived(AnyType userObj, UInt8 cmdType, UOSInt cmdSize, UInt8 *cmd);
			static void __stdcall OnDongleInfoClicked(AnyType userObj);
			static void __stdcall OnCheckDongleClicked(AnyType userObj);
			static void __stdcall OnCheckDevicesClicked(AnyType userObj);
			static void __stdcall OnResetNetworkClicked(AnyType userObj);
			static void __stdcall OnAddDeviceClicked(AnyType userObj);
			static void __stdcall OnLogClicked(AnyType userObj);
			static void __stdcall OnTimerTick(AnyType userObj);
			static void __stdcall OnCopyDevIdClicked(AnyType userObj);
			static void __stdcall OnDevReportTimeClicked(AnyType userObj);
			static void __stdcall OnDevSetReportTimeClicked(AnyType userObj);
			static void __stdcall OnDevOnClicked(AnyType userObj);
			static void __stdcall OnDevOffClicked(AnyType userObj);
			static void __stdcall OnDevStatusClicked(AnyType userObj);
			static void __stdcall OnDeviceDblClk(AnyType userObj, UOSInt index);
			static void __stdcall OnUploadClicked(AnyType userObj);

			void LoadFile();
			void SaveFile();
		public:
			AVIRSNBDongleForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<IO::Stream> stm);
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
