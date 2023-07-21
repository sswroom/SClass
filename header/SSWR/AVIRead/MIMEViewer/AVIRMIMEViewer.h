#ifndef _SM_SSWR_AVIREAD_MIMEVIEWER_AVIRMIMEVIEWER
#define _SM_SSWR_AVIREAD_MIMEVIEWER_AVIRMIMEVIEWER
#include "SSWR/AVIRead/AVIRCore.h"
#include "Text/IMIMEObj.h"
#include "UI/GUIClientControl.h"

namespace SSWR
{
	namespace AVIRead
	{
		namespace MIMEViewer
		{
			class AVIRMIMEViewer
			{
			protected:
				SSWR::AVIRead::AVIRCore *core;
				UI::GUIClientControl *ctrl;

			public:
				AVIRMIMEViewer(SSWR::AVIRead::AVIRCore *core, UI::GUIClientControl *ctrl, Text::IMIMEObj *obj);
				virtual ~AVIRMIMEViewer();

				static AVIRMIMEViewer *CreateViewer(SSWR::AVIRead::AVIRCore *core, NotNullPtr<UI::GUICore> ui, UI::GUIClientControl *ctrl, Media::ColorManagerSess *sess, Text::IMIMEObj *obj);
			};
		}
	}
}
#endif