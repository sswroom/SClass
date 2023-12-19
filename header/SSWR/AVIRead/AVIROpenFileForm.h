#ifndef _SM_SSWR_AVIREAD_AVIROPENFILEFORM
#define _SM_SSWR_AVIREAD_AVIROPENFILEFORM
#include "IO/ParsedObject.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
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
			NotNullPtr<UI::GUILabel> lblName;
			NotNullPtr<UI::GUITextBox> txtName;
			NotNullPtr<UI::GUIButton> btnBrowse;
			NotNullPtr<UI::GUILabel> lblType;
			NotNullPtr<UI::GUIComboBox> cboType;
			NotNullPtr<UI::GUIButton> btnOK;
			NotNullPtr<UI::GUIButton> btnCancel;
			Text::String *fileName;
			IO::ParserType parserType;
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			IO::ParserType t;

			static void __stdcall OnBrowseClicked(void *userObj);
			static void __stdcall OnOKClicked(void *userObj);
			static void __stdcall OnCancelClicked(void *userObj);
			static void __stdcall FileHandler(void *userObj, NotNullPtr<Text::String> *files, UOSInt nFiles);
		public:
			AVIROpenFileForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, IO::ParserType t);
			virtual ~AVIROpenFileForm();

			virtual void OnMonitorChanged();

			NotNullPtr<Text::String> GetFileName() const;
			IO::ParserType GetParserType();
		};
	}
}
#endif
