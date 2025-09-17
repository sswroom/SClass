#ifndef _SM_SSWR_AVIREAD_AVIRPDFOBJECTFORM
#define _SM_SSWR_AVIREAD_AVIRPDFOBJECTFORM
#include "Media/PDFDocument.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIForm.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUIListBox.h"
#include "UI/GUIListView.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRPDFObjectForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Media::PDFDocument> doc;

			NN<UI::GUIListBox> lbObject;
			NN<UI::GUIHSplitter> hspMain;
			NN<UI::GUITabControl> tcMain;
			
			NN<UI::GUITabPage> tpParameter;
			NN<UI::GUIListView> lvParameter;

			NN<UI::GUITabPage> tpText;
			NN<UI::GUITextBox> txtText;

			NN<UI::GUIMainMenu> mnuMain;

			static void __stdcall OnObjectSelChg(AnyType userObj);
			static void __stdcall OnObjectDblClk(AnyType userObj);
		public:
			AVIRPDFObjectForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Media::PDFDocument> doc);
			virtual ~AVIRPDFObjectForm();

			virtual void EventMenuClicked(UInt16 cmdId);
			virtual void OnMonitorChanged();
		};
	}
}
#endif
