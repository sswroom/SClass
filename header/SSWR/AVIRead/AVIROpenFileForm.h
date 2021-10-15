#ifndef _SM_SSWR_AVIREAD_AVIROPENFILEFORM
#define _SM_SSWR_AVIREAD_AVIROPENFILEFORM
#include "IO/ParsedObject.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIROpenFileForm : public UI::GUIForm
		{
		private:
			UI::GUILabel *lblName;
			UI::GUITextBox *txtName;
			UI::GUIButton *btnBrowse;
			UI::GUIButton *btnOK;
			UI::GUIButton *btnCancel;
			const UTF8Char *fileName;
			SSWR::AVIRead::AVIRCore *core;
			IO::ParserType t;

			static void __stdcall OnBrowseClicked(void *userObj);
			static void __stdcall OnOKClicked(void *userObj);
			static void __stdcall OnCancelClicked(void *userObj);
		public:
			AVIROpenFileForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, IO::ParserType t);
			virtual ~AVIROpenFileForm();

			virtual void OnMonitorChanged();

			const UTF8Char *GetFileName();
		};
	};
};
#endif
