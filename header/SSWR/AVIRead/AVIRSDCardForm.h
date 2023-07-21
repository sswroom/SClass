#ifndef _SM_SSWR_AVIREAD_AVIRSDCARDFORM
#define _SM_SSWR_AVIREAD_AVIRSDCARDFORM
#include "IO/SDCardMgr.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRSDCardForm : public UI::GUIForm
		{
		private:
			SSWR::AVIRead::AVIRCore *core;
			Data::ArrayList<IO::SDCardInfo*> *sdCardList;

			UI::GUIListBox *lbDevices;
			UI::GUIHSplitter *hspDevices;
			UI::GUIPanel *pnlDevices;
			UI::GUILabel *lblName;
			UI::GUITextBox *txtName;
			UI::GUILabel *lblCID;
			UI::GUITextBox *txtCID;
			UI::GUILabel *lblCSD;
			UI::GUITextBox *txtCSD;
			UI::GUILabel *lblMID;
			UI::GUITextBox *txtMID;
			UI::GUILabel *lblOID;
			UI::GUITextBox *txtOID;
			UI::GUILabel *lblProductName;
			UI::GUITextBox *txtProductName;
			UI::GUILabel *lblProductRevision;
			UI::GUITextBox *txtProductRevision;
			UI::GUILabel *lblSerialNo;
			UI::GUITextBox *txtSerialNo;
			UI::GUILabel *lblManuDate;
			UI::GUITextBox *txtManuDate;
			UI::GUILabel *lblCSDVersion;
			UI::GUITextBox *txtCSDVersion;
			UI::GUILabel *lblTranRate;
			UI::GUITextBox *txtTranRate;
			UI::GUILabel *lblCardCmdClass;
			UI::GUITextBox *txtCardCmdClass;
			UI::GUILabel *lblCapacity;
			UI::GUITextBox *txtCapacity;
			
			static void __stdcall OnDevicesSelChg(void *userObj);
			static OSInt __stdcall ItemCompare(void *item1, void *item2);
		public:
			AVIRSDCardForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRSDCardForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
