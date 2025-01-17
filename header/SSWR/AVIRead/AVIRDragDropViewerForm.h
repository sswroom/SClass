#ifndef _SM_SSWR_AVIREAD_AVIRDRAGDROPVIEWERFORM
#define _SM_SSWR_AVIREAD_AVIRDRAGDROPVIEWERFORM
#include "Data/FastStringMapNN.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIForm.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUIListBox.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRDragDropViewerForm : public UI::GUIForm, public UI::GUIDropHandler
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			Data::FastStringMapNN<Text::String> dropMap;

			NN<UI::GUIListBox> lbType;
			NN<UI::GUIHSplitter> hspMain;
			NN<UI::GUITextBox> txtMain;

			static void __stdcall OnTypeSelChg(AnyType userObj);
			void ClearDrops();
		public:
			AVIRDragDropViewerForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRDragDropViewerForm();

			virtual void OnMonitorChanged();

			virtual DragEffect DragEnter(NN<UI::GUIDropData> data);
			virtual void DropData(NN<UI::GUIDropData> data, OSInt x, OSInt y);
		};
	}
}
#endif
