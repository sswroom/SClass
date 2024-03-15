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
			NotNullPtr<UI::GUILabel> lblFile;
			NotNullPtr<UI::GUITextBox> txtFile;
			NotNullPtr<UI::GUIButton> btnBrowse;
			NotNullPtr<UI::GUIButton> btnParseToText;
			NotNullPtr<UI::GUIButton> btnParseToFile;
			NotNullPtr<UI::GUITextBox> txtOutput;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;

			static void AddFilters(NotNullPtr<IO::FileSelector> selector);
			Bool ParseFile(Text::CStringNN fileName, NotNullPtr<Text::StringBuilderUTF8> output);

			static void __stdcall OnBrowseClicked(AnyType userObj);
			static void __stdcall OnFileDrop(AnyType userObj, Data::DataArray<NotNullPtr<Text::String>> files);
			static void __stdcall OnParseToTextClicked(AnyType userObj);
			static void __stdcall OnParseToFileClicked(AnyType userObj);

		public:
			AVIRWellFormatForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRWellFormatForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
