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
			NN<SSWR::AVIRead::AVIRCore> core;
			Data::ArrayListNN<IO::SDCardInfo> sdCardList;

			NN<UI::GUIListBox> lbDevices;
			NN<UI::GUIHSplitter> hspDevices;
			NN<UI::GUIPanel> pnlDevices;
			NN<UI::GUILabel> lblName;
			NN<UI::GUITextBox> txtName;
			NN<UI::GUILabel> lblCID;
			NN<UI::GUITextBox> txtCID;
			NN<UI::GUILabel> lblCSD;
			NN<UI::GUITextBox> txtCSD;
			NN<UI::GUILabel> lblMID;
			NN<UI::GUITextBox> txtMID;
			NN<UI::GUILabel> lblOID;
			NN<UI::GUITextBox> txtOID;
			NN<UI::GUILabel> lblProductName;
			NN<UI::GUITextBox> txtProductName;
			NN<UI::GUILabel> lblProductRevision;
			NN<UI::GUITextBox> txtProductRevision;
			NN<UI::GUILabel> lblSerialNo;
			NN<UI::GUITextBox> txtSerialNo;
			NN<UI::GUILabel> lblManuDate;
			NN<UI::GUITextBox> txtManuDate;
			NN<UI::GUILabel> lblCSDVersion;
			NN<UI::GUITextBox> txtCSDVersion;
			NN<UI::GUILabel> lblTranRate;
			NN<UI::GUITextBox> txtTranRate;
			NN<UI::GUILabel> lblCardCmdClass;
			NN<UI::GUITextBox> txtCardCmdClass;
			NN<UI::GUILabel> lblCapacity;
			NN<UI::GUITextBox> txtCapacity;
			
			static void __stdcall OnDevicesSelChg(AnyType userObj);
			static OSInt __stdcall ItemCompare(NN<IO::SDCardInfo> item1, NN<IO::SDCardInfo> item2);
		public:
			AVIRSDCardForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRSDCardForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
