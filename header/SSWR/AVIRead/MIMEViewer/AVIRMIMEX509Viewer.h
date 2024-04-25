#ifndef _SM_SSWR_AVIREAD_MIMEVIEWER_AVIRMIMEX509VIEWER
#define _SM_SSWR_AVIREAD_MIMEVIEWER_AVIRMIMEX509VIEWER
#include "SSWR/AVIRead/AVIRCore.h"
#include "SSWR/AVIRead/MIMEViewer/AVIRMIMEViewer.h"
#include "Text/MIMEObj/UnknownMIMEObj.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		namespace MIMEViewer
		{
			class AVIRMIMEX509Viewer : public AVIRMIMEViewer
			{
			private:
				NN<UI::GUITabControl> tcMain;

				NN<UI::GUITabPage> tpDesc;
				NN<UI::GUITextBox> txtDesc;
				NN<UI::GUITabPage> tpASN1;
				NN<UI::GUITextBox> txtASN1;

				Text::MIMEObj::UnknownMIMEObj *obj;

			public:
				AVIRMIMEX509Viewer(NN<SSWR::AVIRead::AVIRCore> core, NN<UI::GUICore> ui, NN<UI::GUIClientControl> ctrl, NN<Media::ColorManagerSess> sess, Text::MIMEObj::UnknownMIMEObj *obj);
				virtual ~AVIRMIMEX509Viewer();
			};
		}
	}
}
#endif