#ifndef _SM_SSWR_AVIREAD_AVIRWELLFORMATFORM
#define _SM_SSWR_AVIREAD_AVIRWELLFORMATFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
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
			NN<UI::GUIPanel> pnlFile;
			NN<UI::GUILabel> lblFile;
			NN<UI::GUITextBox> txtFile;
			NN<UI::GUILabel> lblType;
			NN<UI::GUIComboBox> cboType;
			NN<UI::GUIButton> btnBrowse;
			NN<UI::GUIButton> btnParseToText;
			NN<UI::GUIButton> btnParseToFile;
			NN<UI::GUITextBox> txtOutput;

			NN<SSWR::AVIRead::AVIRCore> core;

			static void AddFilters(NN<IO::FileSelector> selector);
			Bool ParseFile(Text::CStringNN fileName, NN<Text::StringBuilderUTF8> output);

			static void __stdcall OnBrowseClicked(AnyType userObj);
			static void __stdcall OnFileDrop(AnyType userObj, Data::DataArray<NN<Text::String>> files);
			static void __stdcall OnParseToTextClicked(AnyType userObj);
			static void __stdcall OnParseToFileClicked(AnyType userObj);

		public:
			AVIRWellFormatForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRWellFormatForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
