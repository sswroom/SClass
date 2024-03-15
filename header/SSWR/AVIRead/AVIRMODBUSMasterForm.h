#ifndef _SM_SSWR_AVIREAD_AVIRMODBUSMASTERFORM
#define _SM_SSWR_AVIREAD_AVIRMODBUSMASTERFORM
#include "Data/CircularByteBuff.h"
#include "IO/DataCaptureStream.h"
#include "IO/MemoryStream.h"
#include "IO/MODBUSController.h"
#include "IO/MODBUSDevice.h"
#include "IO/MODBUSMaster.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIGroupBox.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUIRadioButton.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"
#include "UI/ListBoxLogger.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRMODBUSMasterForm : public UI::GUIForm
		{
		public:
			typedef enum
			{
				DT_SDM120,
				DT_AMGU4241,
				DT_ED516,
				DT_ED527,
				DT_ED538,
				DT_ED588,

				DT_FIRST = DT_SDM120,
				DT_LAST = DT_ED588
			} DeviceType;

			typedef struct
			{
				NotNullPtr<Text::String> name;
				UInt8 devAddr;
				UInt32 regAddr;
				IO::MODBUSController::DataType dt;
				Math::Unit::UnitBase::ValueType vt;
				Int32 unit;
				Int32 denorm;
				UOSInt lvIndex;
				Double val;
			} MODBUSEntry;
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			IO::DataCaptureStream *stm;
			IO::Stream *devStm;
			IO::MODBUSMaster *modbus;
			IO::MODBUSController *modbusCtrl;
			Data::ArrayList<MODBUSEntry*> entryList;
			Sync::Mutex recvMut;
			Data::CircularByteBuff recvBuff;
			Bool recvUpdated;
			Sync::Mutex sendMut;
			Data::CircularByteBuff sendBuff;
			Bool sendUpdated;
			IO::LogTool log;
			NotNullPtr<UI::ListBoxLogger> logger;

			NotNullPtr<UI::GUIGroupBox> grpStream;
			NotNullPtr<UI::GUILabel> lblStream;
			NotNullPtr<UI::GUITextBox> txtStream;
			NotNullPtr<UI::GUIButton> btnStream;
			NotNullPtr<UI::GUIRadioButton> radMODBUSRTU;
			NotNullPtr<UI::GUIRadioButton> radMODBUSTCP;
			NotNullPtr<UI::GUILabel> lblTimeout;
			NotNullPtr<UI::GUITextBox> txtTimeout;
			NotNullPtr<UI::GUIButton> btnTimeout;
			NotNullPtr<UI::GUITabControl> tcMain;

			NotNullPtr<UI::GUITabPage> tpGetValue;
			NotNullPtr<UI::GUILabel> lblDevAddr;
			NotNullPtr<UI::GUILabel> lblRegAddr;
			NotNullPtr<UI::GUILabel> lblValue;
			NotNullPtr<UI::GUILabel> lblU8Name;
			NotNullPtr<UI::GUITextBox> txtU8DevAddr;
			NotNullPtr<UI::GUITextBox> txtU8RegAddr;
			NotNullPtr<UI::GUIButton> btnU8Get;
			NotNullPtr<UI::GUITextBox> txtU8Value;
			NotNullPtr<UI::GUILabel> lblU16Name;
			NotNullPtr<UI::GUITextBox> txtU16DevAddr;
			NotNullPtr<UI::GUITextBox> txtU16RegAddr;
			NotNullPtr<UI::GUIButton> btnU16Get;
			NotNullPtr<UI::GUITextBox> txtU16Value;
			NotNullPtr<UI::GUILabel> lblI32Name;
			NotNullPtr<UI::GUITextBox> txtI32DevAddr;
			NotNullPtr<UI::GUITextBox> txtI32RegAddr;
			NotNullPtr<UI::GUIButton> btnI32Get;
			NotNullPtr<UI::GUITextBox> txtI32Value;
			NotNullPtr<UI::GUILabel> lblF32Name;
			NotNullPtr<UI::GUITextBox> txtF32DevAddr;
			NotNullPtr<UI::GUITextBox> txtF32RegAddr;
			NotNullPtr<UI::GUIButton> btnF32Get;
			NotNullPtr<UI::GUITextBox> txtF32Value;

			NotNullPtr<UI::GUITabPage> tpSetValue;
			NotNullPtr<UI::GUILabel> lblSetDevAddr;
			NotNullPtr<UI::GUILabel> lblSetRegAddr;
			NotNullPtr<UI::GUILabel> lblSetValue;
			NotNullPtr<UI::GUILabel> lblSetU8Name;
			NotNullPtr<UI::GUITextBox> txtSetU8DevAddr;
			NotNullPtr<UI::GUITextBox> txtSetU8RegAddr;
			NotNullPtr<UI::GUIButton> btnSetU8Low;
			NotNullPtr<UI::GUIButton> btnSetU8High;
			NotNullPtr<UI::GUITextBox> txtSetU8Value;

			NotNullPtr<UI::GUITabPage> tpDevice;
			NotNullPtr<UI::GUIPanel> pnlDevice;
			NotNullPtr<UI::GUILabel> lblDeviceAddr;
			NotNullPtr<UI::GUITextBox> txtDeviceAddr;
			NotNullPtr<UI::GUILabel> lblDevice;
			NotNullPtr<UI::GUIComboBox> cboDevice;
			NotNullPtr<UI::GUIButton> btnDeviceAdd;
			NotNullPtr<UI::GUIListView> lvDevice;

			NotNullPtr<UI::GUITabPage> tpRAWSend;
			NotNullPtr<UI::GUITextBox> txtRAWSend;

			NotNullPtr<UI::GUITabPage> tpRAWRecv;
			NotNullPtr<UI::GUITextBox> txtRAWRecv;

			NotNullPtr<UI::GUITabPage> tpLog;
			NotNullPtr<UI::GUITextBox> txtLog;
			NotNullPtr<UI::GUIListBox> lbLog;
		private:
			static void __stdcall OnStreamClicked(AnyType userObj);
			static void __stdcall OnU8GetClicked(AnyType userObj);
			static void __stdcall OnU16GetClicked(AnyType userObj);
			static void __stdcall OnI32GetClicked(AnyType userObj);
			static void __stdcall OnF32GetClicked(AnyType userObj);
			static void __stdcall OnSetU8LowClicked(AnyType userObj);
			static void __stdcall OnSetU8HighClicked(AnyType userObj);
			static void __stdcall OnDeviceAddClicked(AnyType userObj);
			static void __stdcall OnTimeoutClicked(AnyType userObj);
			static void __stdcall OnTimerTick(AnyType userObj);
			static void __stdcall OnDataRecv(AnyType userObj, const UInt8 *data, UOSInt dataSize);
			static void __stdcall OnDataSend(AnyType userObj, const UInt8 *data, UOSInt dataSize);

			static void __stdcall OnMODBUSEntry(AnyType userObj, Text::CString name, UInt8 devAddr, UInt32 regAddr, IO::MODBUSController::DataType dt, Math::Unit::UnitBase::ValueType vt, Int32 unit, Int32 denorm);
			void StopStream(Bool clearUI);

		public:
			AVIRMODBUSMasterForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRMODBUSMasterForm();

			virtual void OnMonitorChanged();

			static Text::CStringNN DeviceTypeGetName(DeviceType dt);
		};
	}
}
#endif
