#ifndef _SM_SSWR_AVIREAD_AVIRFILESEARCHFORM
#define _SM_SSWR_AVIREAD_AVIRFILESEARCHFORM
#include "Data/ArrayListNN.h"
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
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Text::TextBinEnc::TextBinEncList encList;
			Data::ArrayListStringNN fileList;

			NotNullPtr<UI::GUIPanel> pnlControl;
			NotNullPtr<UI::GUILabel> lblDir;
			NotNullPtr<UI::GUITextBox> txtDir;
			NotNullPtr<UI::GUILabel> lblEncoding;
			NotNullPtr<UI::GUIComboBox> cboEncoding;
			NotNullPtr<UI::GUILabel> lblText;
			NotNullPtr<UI::GUITextBox> txtText;
			NotNullPtr<UI::GUIButton> btnSearch;
			NotNullPtr<UI::GUIListView> lvFiles;

			static void __stdcall OnSearchClicked(AnyType userObj);
			static void __stdcall OnDirectoryDrop(AnyType userObj, Data::DataArray<NotNullPtr<Text::String>> files);
			static void __stdcall OnFilesDblClk(AnyType userObj, UOSInt itemIndex);

			void ClearFiles();
			void FindDir(UTF8Char *dir, UTF8Char *dirEnd, const UInt8 *searchBuff, UOSInt searchLen);
		public:
			AVIRFileSearchForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRFileSearchForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
