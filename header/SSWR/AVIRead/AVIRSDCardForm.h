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
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Data::ArrayList<IO::SDCardInfo*> sdCardList;

			UI::GUIListBox *lbDevices;
			NotNullPtr<UI::GUIHSplitter> hspDevices;
			NotNullPtr<UI::GUIPanel> pnlDevices;
			NotNullPtr<UI::GUILabel> lblName;
			NotNullPtr<UI::GUITextBox> txtName;
			NotNullPtr<UI::GUILabel> lblCID;
			NotNullPtr<UI::GUITextBox> txtCID;
			NotNullPtr<UI::GUILabel> lblCSD;
			NotNullPtr<UI::GUITextBox> txtCSD;
			NotNullPtr<UI::GUILabel> lblMID;
			NotNullPtr<UI::GUITextBox> txtMID;
			NotNullPtr<UI::GUILabel> lblOID;
			NotNullPtr<UI::GUITextBox> txtOID;
			NotNullPtr<UI::GUILabel> lblProductName;
			NotNullPtr<UI::GUITextBox> txtProductName;
			NotNullPtr<UI::GUILabel> lblProductRevision;
			NotNullPtr<UI::GUITextBox> txtProductRevision;
			NotNullPtr<UI::GUILabel> lblSerialNo;
			NotNullPtr<UI::GUITextBox> txtSerialNo;
			NotNullPtr<UI::GUILabel> lblManuDate;
			NotNullPtr<UI::GUITextBox> txtManuDate;
			NotNullPtr<UI::GUILabel> lblCSDVersion;
			NotNullPtr<UI::GUITextBox> txtCSDVersion;
			NotNullPtr<UI::GUILabel> lblTranRate;
			NotNullPtr<UI::GUITextBox> txtTranRate;
			NotNullPtr<UI::GUILabel> lblCardCmdClass;
			NotNullPtr<UI::GUITextBox> txtCardCmdClass;
			NotNullPtr<UI::GUILabel> lblCapacity;
			NotNullPtr<UI::GUITextBox> txtCapacity;
			
			static void __stdcall OnDevicesSelChg(void *userObj);
			static OSInt __stdcall ItemCompare(IO::SDCardInfo *item1, IO::SDCardInfo *item2);
		public:
			AVIRSDCardForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRSDCardForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
