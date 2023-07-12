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
			SSWR::AVIRead::AVIRCore *core;
			UI::GUIPanel *pnlCtrl;
			UI::GUIButton *btnSave;
			UI::GUIButton *btnHex;
			UI::GUITextBox *txtEDID;
			UInt8 *edid;
			UOSInt edidSize;
			
			void UpdateEDIDDisp();
			static void __stdcall OnSaveClicked(void *userObj);
			static void __stdcall OnHexClicked(void *userObj);
			static void __stdcall OnFileDrop(void *userObj, Text::String **fileNames, UOSInt fileCnt);
		public:
			AVIREDIDViewerForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIREDIDViewerForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
