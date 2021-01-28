#ifndef _SM_SSWR_AVIREAD_MIMEVIEWER_AVIRMIMEJSONVIEWER
#define _SM_SSWR_AVIREAD_MIMEVIEWER_AVIRMIMEJSONVIEWER
#include "SSWR/AVIRead/AVIRCore.h"
#include "SSWR/AVIRead/MIMEViewer/AVIRMIMEViewer.h"
#include "Text/MIMEObj/UnknownMIMEObj.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		namespace MIMEViewer
		{
			class AVIRMIMEJSONViewer : public AVIRMIMEViewer
			{
			private:
				UI::GUITextBox *txtJSON;

				Text::MIMEObj::UnknownMIMEObj *obj;

			public:
				AVIRMIMEJSONViewer(SSWR::AVIRead::AVIRCore *core, UI::GUICore *ui, UI::GUIClientControl *ctrl, Media::ColorManagerSess *sess, Text::MIMEObj::UnknownMIMEObj *obj);
				virtual ~AVIRMIMEJSONViewer();
			};
		}
	}
}
#endif