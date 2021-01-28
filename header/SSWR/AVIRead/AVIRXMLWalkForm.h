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
			SSWR::AVIRead::AVIRCore *core;

			UI::GUIPanel *pnlFile;
			UI::GUILabel *lblFile;
			UI::GUITextBox *txtFile;
			UI::GUIButton *btnBrowse;
			UI::GUIListView *lvXML;

			static void __stdcall OnBrowseClick(void *userObj);
			static void __stdcall OnFileDrop(void *userObj, const UTF8Char **fileNames, OSInt nFiles);

			void LoadFile(const UTF8Char *fileName);
		public:
			AVIRXMLWalkForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRXMLWalkForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
