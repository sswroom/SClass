#ifndef _SM_SSWR_AVIREAD_AVIRCPPENUMFORM
#define _SM_SSWR_AVIREAD_AVIRCPPENUMFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUILabel.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRCppEnumForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;

			NN<UI::GUIPanel> pnlCtrl;
			NN<UI::GUILabel> lblType;
			NN<UI::GUIComboBox> cboType;
			NN<UI::GUIButton> btnConv;
			NN<UI::GUIButton> btnConv2;
			NN<UI::GUILabel> lblPrefix;
			NN<UI::GUITextBox> txtPrefix;
			NN<UI::GUITextBox> txtSource;
			NN<UI::GUIHSplitter> hspMain;
			NN<UI::GUITextBox> txtDest;

			static void __stdcall OnConvClicked(AnyType userObj);
			static void __stdcall OnConv2Clicked(AnyType userObj);
			void ConvEnum();
		public:
			AVIRCppEnumForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRCppEnumForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
