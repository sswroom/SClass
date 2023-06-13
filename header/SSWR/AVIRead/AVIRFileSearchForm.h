#ifndef _SM_SSWR_AVIREAD_AVIRFILESEARCHFORM
#define _SM_SSWR_AVIREAD_AVIRFILESEARCHFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "Text/TextBinEnc/TextBinEncList.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRFileSearchForm : public UI::GUIForm
		{
		private:
			SSWR::AVIRead::AVIRCore *core;
			Text::TextBinEnc::TextBinEncList encList;
			Data::ArrayList<Text::String*> fileList;

			UI::GUIPanel *pnlControl;
			UI::GUILabel *lblDir;
			UI::GUITextBox *txtDir;
			UI::GUILabel *lblEncoding;
			UI::GUIComboBox *cboEncoding;
			UI::GUILabel *lblText;
			UI::GUITextBox *txtText;
			UI::GUIButton *btnSearch;
			UI::GUIListView *lvFiles;

			static void __stdcall OnSearchClicked(void *userObj);
			static void __stdcall OnDirectoryDrop(void *userObj, Text::String **files, UOSInt nFiles);
			static void __stdcall OnFilesDblClk(void *userObj, UOSInt itemIndex);

			void ClearFiles();
			void FindDir(UTF8Char *dir, UTF8Char *dirEnd, const UInt8 *searchBuff, UOSInt searchLen);
		public:
			AVIRFileSearchForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRFileSearchForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
