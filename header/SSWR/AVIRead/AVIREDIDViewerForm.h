#ifndef _SM_SSWR_AVIREAD_AVIREDIDVIEWERFORM
#define _SM_SSWR_AVIREAD_AVIREDIDVIEWERFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIREDIDViewerForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<UI::GUIPanel> pnlCtrl;
			NN<UI::GUIButton> btnSave;
			NN<UI::GUIButton> btnHex;
			NN<UI::GUITextBox> txtEDID;
			UnsafeArrayOpt<UInt8> edid;
			UOSInt edidSize;
			
			void UpdateEDIDDisp();
			static void __stdcall OnSaveClicked(AnyType userObj);
			static void __stdcall OnHexClicked(AnyType userObj);
			static void __stdcall OnFileDrop(AnyType userObj, Data::DataArray<NN<Text::String>> fileNames);
		public:
			AVIREDIDViewerForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIREDIDViewerForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
