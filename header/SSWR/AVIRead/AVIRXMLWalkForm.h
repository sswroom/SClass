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

			NotNullPtr<UI::GUIPanel> pnlFile;
			NotNullPtr<UI::GUILabel> lblFile;
			NotNullPtr<UI::GUITextBox> txtFile;
			NotNullPtr<UI::GUIButton> btnBrowse;
			NotNullPtr<UI::GUIListView> lvXML;

			static void __stdcall OnBrowseClick(AnyType userObj);
			static void __stdcall OnFileDrop(AnyType userObj, Data::DataArray<NotNullPtr<Text::String>> fileNames);

			void LoadFile(Text::CStringNN fileName);
		public:
			AVIRXMLWalkForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRXMLWalkForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
