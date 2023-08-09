#ifndef _SM_SSWR_AVIREAD_AVIRXMLWALKFORM
#define _SM_SSWR_AVIREAD_AVIRXMLWALKFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRXMLWalkForm : public UI::GUIForm
		{
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;

			UI::GUIPanel *pnlFile;
			UI::GUILabel *lblFile;
			UI::GUITextBox *txtFile;
			UI::GUIButton *btnBrowse;
			UI::GUIListView *lvXML;

			static void __stdcall OnBrowseClick(void *userObj);
			static void __stdcall OnFileDrop(void *userObj, NotNullPtr<Text::String> *fileNames, UOSInt nFiles);

			void LoadFile(Text::CStringNN fileName);
		public:
			AVIRXMLWalkForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRXMLWalkForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
