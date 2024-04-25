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
				NN<Text::String> name;
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
			NN<SSWR::AVIRead::AVIRCore> core;
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
			NN<UI::ListBoxLogger> logger;

			NN<UI::GUIGroupBox> grpStream;
			NN<UI::GUILabel> lblStream;
			NN<UI::GUITextBox> txtStream;
			NN<UI::GUIButton> btnStream;
			NN<UI::GUIRadioButton> radMODBUSRTU;
			NN<UI::GUIRadioButton> radMODBUSTCP;
			NN<UI::GUILabel> lblTimeout;
			NN<UI::GUITextBox> txtTimeout;
			NN<UI::GUIButton> btnTimeout;
			NN<UI::GUITabControl> tcMain;

			NN<UI::GUITabPage> tpGetValue;
			NN<UI::GUILabel> lblDevAddr;
			NN<UI::GUILabel> lblRegAddr;
			NN<UI::GUILabel> lblValue;
			NN<UI::GUILabel> lblU8Name;
			NN<UI::GUITextBox> txtU8DevAddr;
			NN<UI::GUITextBox> txtU8RegAddr;
			NN<UI::GUIButton> btnU8Get;
			NN<UI::GUITextBox> txtU8Value;
			NN<UI::GUILabel> lblU16Name;
			NN<UI::GUITextBox> txtU16DevAddr;
			NN<UI::GUITextBox> txtU16RegAddr;
			NN<UI::GUIButton> btnU16Get;
			NN<UI::GUITextBox> txtU16Value;
			NN<UI::GUILabel> lblI32Name;
			NN<UI::GUITextBox> txtI32DevAddr;
			NN<UI::GUITextBox> txtI32RegAddr;
			NN<UI::GUIButton> btnI32Get;
			NN<UI::GUITextBox> txtI32Value;
			NN<UI::GUILabel> lblF32Name;
			NN<UI::GUITextBox> txtF32DevAddr;
			NN<UI::GUITextBox> txtF32RegAddr;
			NN<UI::GUIButton> btnF32Get;
			NN<UI::GUITextBox> txtF32Value;

			NN<UI::GUITabPage> tpSetValue;
			NN<UI::GUILabel> lblSetDevAddr;
			NN<UI::GUILabel> lblSetRegAddr;
			NN<UI::GUILabel> lblSetValue;
			NN<UI::GUILabel> lblSetU8Name;
			NN<UI::GUITextBox> txtSetU8DevAddr;
			NN<UI::GUITextBox> txtSetU8RegAddr;
			NN<UI::GUIButton> btnSetU8Low;
			NN<UI::GUIButton> btnSetU8High;
			NN<UI::GUITextBox> txtSetU8Value;

			NN<UI::GUITabPage> tpDevice;
			NN<UI::GUIPanel> pnlDevice;
			NN<UI::GUILabel> lblDeviceAddr;
			NN<UI::GUITextBox> txtDeviceAddr;
			NN<UI::GUILabel> lblDevice;
			NN<UI::GUIComboBox> cboDevice;
			NN<UI::GUIButton> btnDeviceAdd;
			NN<UI::GUIListView> lvDevice;

			NN<UI::GUITabPage> tpRAWSend;
			NN<UI::GUITextBox> txtRAWSend;

			NN<UI::GUITabPage> tpRAWRecv;
			NN<UI::GUITextBox> txtRAWRecv;

			NN<UI::GUITabPage> tpLog;
			NN<UI::GUITextBox> txtLog;
			NN<UI::GUIListBox> lbLog;
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
			AVIRMODBUSMasterForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRMODBUSMasterForm();

			virtual void OnMonitorChanged();

			static Text::CStringNN DeviceTypeGetName(DeviceType dt);
		};
	}
}
#endif
