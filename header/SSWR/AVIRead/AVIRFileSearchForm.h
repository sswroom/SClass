#ifndef _SM_SSWR_AVIREAD_AVIRFILESEARCHFORM
#define _SM_SSWR_AVIREAD_AVIRFILESEARCHFORM
#include "Data/ArrayListNN.hpp"
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
			NN<SSWR::AVIRead::AVIRCore> core;
			Text::TextBinEnc::TextBinEncList encList;
			Data::ArrayListStringNN fileList;

			NN<UI::GUIPanel> pnlControl;
			NN<UI::GUILabel> lblDir;
			NN<UI::GUITextBox> txtDir;
			NN<UI::GUILabel> lblEncoding;
			NN<UI::GUIComboBox> cboEncoding;
			NN<UI::GUILabel> lblText;
			NN<UI::GUITextBox> txtText;
			NN<UI::GUIButton> btnSearch;
			NN<UI::GUIListView> lvFiles;

			static void __stdcall OnSearchClicked(AnyType userObj);
			static void __stdcall OnDirectoryDrop(AnyType userObj, Data::DataArray<NN<Text::String>> files);
			static void __stdcall OnFilesDblClk(AnyType userObj, UIntOS itemIndex);

			void ClearFiles();
			void FindDir(UnsafeArray<UTF8Char> dir, UnsafeArray<UTF8Char> dirEnd, UnsafeArray<const UInt8> searchBuff, UIntOS searchLen);
		public:
			AVIRFileSearchForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRFileSearchForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
