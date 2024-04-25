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
			NN<SSWR::AVIRead::AVIRCore> core;

			NN<UI::GUIPanel> pnlFile;
			NN<UI::GUILabel> lblFile;
			NN<UI::GUITextBox> txtFile;
			NN<UI::GUIButton> btnBrowse;
			NN<UI::GUIListView> lvXML;

			static void __stdcall OnBrowseClick(AnyType userObj);
			static void __stdcall OnFileDrop(AnyType userObj, Data::DataArray<NN<Text::String>> fileNames);

			void LoadFile(Text::CStringNN fileName);
		public:
			AVIRXMLWalkForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRXMLWalkForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
