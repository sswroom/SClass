#ifndef _SM_SSWR_AVIREAD_AVIRFILEANALYSEFORM
#define _SM_SSWR_AVIREAD_AVIRFILEANALYSEFORM
#include "IO/FileAnalyse/IFileAnalyse.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUIPanel.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRFileAnalyseForm : public UI::GUIForm
		{
		private:
			NotNullPtr<UI::GUIPanel> pnlCtrl;
			NotNullPtr<UI::GUILabel> lblFile;
			NotNullPtr<UI::GUITextBox> txtFile;
			NotNullPtr<UI::GUIButton> btnFile;
			NotNullPtr<UI::GUITabControl> tcMain;
			NotNullPtr<UI::GUITabPage> tpContent;
			NotNullPtr<UI::GUITabPage> tpStream;
			NotNullPtr<UI::GUITabPage> tpTool;

			NotNullPtr<UI::GUIListBox> lbPackList;
			NotNullPtr<UI::GUIHSplitter> hspPack;
			NotNullPtr<UI::GUIListBox> lbPackItems;
			NotNullPtr<UI::GUIHSplitter> hspPack2;
			NotNullPtr<UI::GUITextBox> txtPack;

			NotNullPtr<UI::GUIButton> btnTrimPadding;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			IO::FileAnalyse::IFileAnalyse *file;
			UOSInt lastPackCount;

			static void __stdcall OnFileDrop(AnyType userObj, Data::DataArray<NotNullPtr<Text::String>> files);
			static void __stdcall OnFileClicked(AnyType userObj);
			static void __stdcall OnTrimPaddingClicked(AnyType userObj);
			static void __stdcall OnTimerTick(AnyType userObj);
			static void __stdcall OnPackListChanged(AnyType userObj);
			static void __stdcall OnPackItemChanged(AnyType userObj);
			Bool OpenFile(Text::CStringNN fileName);
		public:
			AVIRFileAnalyseForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRFileAnalyseForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
