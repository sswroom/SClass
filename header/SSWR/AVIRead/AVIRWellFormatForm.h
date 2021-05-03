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
			UI::GUIPanel *pnlFile;
			UI::GUILabel *lblFile;
			UI::GUITextBox *txtFile;
			UI::GUIButton *btnBrowse;
			UI::GUIButton *btnParseToText;
			UI::GUIButton *btnParseToFile;
			UI::GUITextBox *txtOutput;

			SSWR::AVIRead::AVIRCore *core;

			static void AddFilters(IO::IFileSelector *selector);
			Bool ParseFile(const UTF8Char *fileName, Text::StringBuilderUTF *output);

			static void __stdcall OnBrowseClicked(void *userObj);
			static void __stdcall OnFileDrop(void *userObj, const UTF8Char **files, UOSInt nFile);
			static void __stdcall OnParseToTextClicked(void *userObj);
			static void __stdcall OnParseToFileClicked(void *userObj);

		public:
			AVIRWellFormatForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRWellFormatForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
