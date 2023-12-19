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
				NotNullPtr<UI::GUITabControl> tcMain;

				NotNullPtr<UI::GUITabPage> tpDesc;
				NotNullPtr<UI::GUITextBox> txtDesc;
				NotNullPtr<UI::GUITabPage> tpASN1;
				NotNullPtr<UI::GUITextBox> txtASN1;

				Text::MIMEObj::UnknownMIMEObj *obj;

			public:
				AVIRMIMEX509Viewer(NotNullPtr<SSWR::AVIRead::AVIRCore> core, NotNullPtr<UI::GUICore> ui, NotNullPtr<UI::GUIClientControl> ctrl, NotNullPtr<Media::ColorManagerSess> sess, Text::MIMEObj::UnknownMIMEObj *obj);
				virtual ~AVIRMIMEX509Viewer();
			};
		}
	}
}
#endif