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
			NN<UI::GUIPanel> pnlCtrl;
			NN<UI::GUILabel> lblFile;
			NN<UI::GUITextBox> txtFile;
			NN<UI::GUIButton> btnFile;
			NN<UI::GUITabControl> tcMain;
			NN<UI::GUITabPage> tpContent;
			NN<UI::GUITabPage> tpStream;
			NN<UI::GUITabPage> tpTool;

			NN<UI::GUIListBox> lbPackList;
			NN<UI::GUIHSplitter> hspPack;
			NN<UI::GUIListBox> lbPackItems;
			NN<UI::GUIHSplitter> hspPack2;
			NN<UI::GUITextBox> txtPack;

			NN<UI::GUIButton> btnTrimPadding;

			NN<SSWR::AVIRead::AVIRCore> core;
			Optional<IO::FileAnalyse::IFileAnalyse> file;
			UOSInt lastPackCount;

			static void __stdcall OnFileDrop(AnyType userObj, Data::DataArray<NN<Text::String>> files);
			static void __stdcall OnFileClicked(AnyType userObj);
			static void __stdcall OnTrimPaddingClicked(AnyType userObj);
			static void __stdcall OnTimerTick(AnyType userObj);
			static void __stdcall OnPackListChanged(AnyType userObj);
			static void __stdcall OnPackItemChanged(AnyType userObj);
			Bool OpenFile(Text::CStringNN fileName);
		public:
			AVIRFileAnalyseForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRFileAnalyseForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
