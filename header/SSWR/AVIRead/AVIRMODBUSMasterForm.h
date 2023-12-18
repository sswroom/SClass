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
			UI::GUILabel *lblStream;
			UI::GUITextBox *txtStream;
			NotNullPtr<UI::GUIButton> btnStream;
			UI::GUIRadioButton *radMODBUSRTU;
			UI::GUIRadioButton *radMODBUSTCP;
			UI::GUILabel *lblTimeout;
			UI::GUITextBox *txtTimeout;
			NotNullPtr<UI::GUIButton> btnTimeout;
			UI::GUITabControl *tcMain;

			NotNullPtr<UI::GUITabPage> tpGetValue;
			UI::GUILabel *lblDevAddr;
			UI::GUILabel *lblRegAddr;
			UI::GUILabel *lblValue;
			UI::GUILabel *lblU8Name;
			UI::GUITextBox *txtU8DevAddr;
			UI::GUITextBox *txtU8RegAddr;
			NotNullPtr<UI::GUIButton> btnU8Get;
			UI::GUITextBox *txtU8Value;
			UI::GUILabel *lblU16Name;
			UI::GUITextBox *txtU16DevAddr;
			UI::GUITextBox *txtU16RegAddr;
			NotNullPtr<UI::GUIButton> btnU16Get;
			UI::GUITextBox *txtU16Value;
			UI::GUILabel *lblI32Name;
			UI::GUITextBox *txtI32DevAddr;
			UI::GUITextBox *txtI32RegAddr;
			NotNullPtr<UI::GUIButton> btnI32Get;
			UI::GUITextBox *txtI32Value;
			UI::GUILabel *lblF32Name;
			UI::GUITextBox *txtF32DevAddr;
			UI::GUITextBox *txtF32RegAddr;
			NotNullPtr<UI::GUIButton> btnF32Get;
			UI::GUITextBox *txtF32Value;

			NotNullPtr<UI::GUITabPage> tpSetValue;
			UI::GUILabel *lblSetDevAddr;
			UI::GUILabel *lblSetRegAddr;
			UI::GUILabel *lblSetValue;
			UI::GUILabel *lblSetU8Name;
			UI::GUITextBox *txtSetU8DevAddr;
			UI::GUITextBox *txtSetU8RegAddr;
			NotNullPtr<UI::GUIButton> btnSetU8Low;
			NotNullPtr<UI::GUIButton> btnSetU8High;
			UI::GUITextBox *txtSetU8Value;

			NotNullPtr<UI::GUITabPage> tpDevice;
			NotNullPtr<UI::GUIPanel> pnlDevice;
			UI::GUILabel *lblDeviceAddr;
			UI::GUITextBox *txtDeviceAddr;
			UI::GUILabel *lblDevice;
			UI::GUIComboBox *cboDevice;
			NotNullPtr<UI::GUIButton> btnDeviceAdd;
			UI::GUIListView *lvDevice;

			NotNullPtr<UI::GUITabPage> tpRAWSend;
			UI::GUITextBox *txtRAWSend;

			NotNullPtr<UI::GUITabPage> tpRAWRecv;
			UI::GUITextBox *txtRAWRecv;

			NotNullPtr<UI::GUITabPage> tpLog;
			UI::GUITextBox *txtLog;
			NotNullPtr<UI::GUIListBox> lbLog;
		private:
			static void __stdcall OnStreamClicked(void *userObj);
			static void __stdcall OnU8GetClicked(void *userObj);
			static void __stdcall OnU16GetClicked(void *userObj);
			static void __stdcall OnI32GetClicked(void *userObj);
			static void __stdcall OnF32GetClicked(void *userObj);
			static void __stdcall OnSetU8LowClicked(void *userObj);
			static void __stdcall OnSetU8HighClicked(void *userObj);
			static void __stdcall OnDeviceAddClicked(void *userObj);
			static void __stdcall OnTimeoutClicked(void *userObj);
			static void __stdcall OnTimerTick(void *userObj);
			static void __stdcall OnDataRecv(void *userObj, const UInt8 *data, UOSInt dataSize);
			static void __stdcall OnDataSend(void *userObj, const UInt8 *data, UOSInt dataSize);

			static void __stdcall OnMODBUSEntry(void *userObj, Text::CString name, UInt8 devAddr, UInt32 regAddr, IO::MODBUSController::DataType dt, Math::Unit::UnitBase::ValueType vt, Int32 unit, Int32 denorm);
			void StopStream(Bool clearUI);

		public:
			AVIRMODBUSMasterForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRMODBUSMasterForm();

			virtual void OnMonitorChanged();

			static Text::CStringNN DeviceTypeGetName(DeviceType dt);
		};
	}
}
#endif
