#ifndef _SM_SSWR_AVIREAD_AVIRMODBUSMASTERFORM
#define _SM_SSWR_AVIREAD_AVIRMODBUSMASTERFORM
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
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUIRadioButton.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"

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

				DT_FIRST = DT_SDM120,
				DT_LAST = DT_AMGU4241
			} DeviceType;

			typedef struct
			{
				const UTF8Char *name;
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
			SSWR::AVIRead::AVIRCore *core;
			IO::Stream *stm;
			IO::MODBUSMaster *modbus;
			IO::MODBUSController *modbusCtrl;
			Data::ArrayList<MODBUSEntry*> *entryList;

			UI::GUIGroupBox *grpStream;
			UI::GUILabel *lblStream;
			UI::GUITextBox *txtStream;
			UI::GUIRadioButton *radMODBUSRTU;
			UI::GUIRadioButton *radMODBUSTCP;
			UI::GUIButton *btnStream;
			UI::GUITabControl *tcMain;

			UI::GUITabPage *tpGetValue;
			UI::GUILabel *lblDevAddr;
			UI::GUILabel *lblRegAddr;
			UI::GUILabel *lblValue;
			UI::GUILabel *lblI32Name;
			UI::GUITextBox *txtI32DevAddr;
			UI::GUITextBox *txtI32RegAddr;
			UI::GUIButton *btnI32Get;
			UI::GUITextBox *txtI32Value;
			UI::GUILabel *lblF32Name;
			UI::GUITextBox *txtF32DevAddr;
			UI::GUITextBox *txtF32RegAddr;
			UI::GUIButton *btnF32Get;
			UI::GUITextBox *txtF32Value;

			UI::GUITabPage *tpDevice;
			UI::GUIPanel *pnlDevice;
			UI::GUILabel *lblDeviceAddr;
			UI::GUITextBox *txtDeviceAddr;
			UI::GUILabel *lblDevice;
			UI::GUIComboBox *cboDevice;
			UI::GUIButton *btnDeviceAdd;
			UI::GUIListView *lvDevice;

		private:
			static void __stdcall OnStreamClicked(void *userObj);
			static void __stdcall OnI32GetClicked(void *userObj);
			static void __stdcall OnF32GetClicked(void *userObj);
			static void __stdcall OnDeviceAddClicked(void *userObj);
			static void __stdcall OnTimerTick(void *userObj);

			static void __stdcall OnMODBUSEntry(void *userObj, const UTF8Char *name, UInt8 devAddr, UInt32 regAddr, IO::MODBUSController::DataType dt, Math::Unit::UnitBase::ValueType vt, Int32 unit, Int32 denorm);
			void StopStream();

		public:
			AVIRMODBUSMasterForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRMODBUSMasterForm();

			virtual void OnMonitorChanged();

			static const UTF8Char *DeviceTypeGetName(DeviceType dt);
		};
	};
};
#endif
