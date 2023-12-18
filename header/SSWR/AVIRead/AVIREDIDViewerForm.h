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
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			NotNullPtr<UI::GUIPanel> pnlCtrl;
			NotNullPtr<UI::GUIButton> btnSave;
			NotNullPtr<UI::GUIButton> btnHex;
			UI::GUITextBox *txtEDID;
			UInt8 *edid;
			UOSInt edidSize;
			
			void UpdateEDIDDisp();
			static void __stdcall OnSaveClicked(void *userObj);
			static void __stdcall OnHexClicked(void *userObj);
			static void __stdcall OnFileDrop(void *userObj, NotNullPtr<Text::String> *fileNames, UOSInt fileCnt);
		public:
			AVIREDIDViewerForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIREDIDViewerForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
