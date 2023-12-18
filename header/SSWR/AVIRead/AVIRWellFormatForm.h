#ifndef _SM_SSWR_AVIREAD_AVIRWELLFORMATFORM
#define _SM_SSWR_AVIREAD_AVIRWELLFORMATFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRWellFormatForm : public UI::GUIForm
		{
		private:
			NotNullPtr<UI::GUIPanel> pnlFile;
			UI::GUILabel *lblFile;
			UI::GUITextBox *txtFile;
			NotNullPtr<UI::GUIButton> btnBrowse;
			NotNullPtr<UI::GUIButton> btnParseToText;
			NotNullPtr<UI::GUIButton> btnParseToFile;
			UI::GUITextBox *txtOutput;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;

			static void AddFilters(IO::FileSelector *selector);
			Bool ParseFile(Text::CStringNN fileName, NotNullPtr<Text::StringBuilderUTF8> output);

			static void __stdcall OnBrowseClicked(void *userObj);
			static void __stdcall OnFileDrop(void *userObj, NotNullPtr<Text::String> *files, UOSInt nFile);
			static void __stdcall OnParseToTextClicked(void *userObj);
			static void __stdcall OnParseToFileClicked(void *userObj);

		public:
			AVIRWellFormatForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRWellFormatForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
