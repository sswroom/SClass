#ifndef _SM_SSWR_AVIREAD_MIMEVIEWER_AVIRMIMETEXTVIEWER
#define _SM_SSWR_AVIREAD_MIMEVIEWER_AVIRMIMETEXTVIEWER
#include "SSWR/AVIRead/AVIRCore.h"
#include "SSWR/AVIRead/MIMEViewer/AVIRMIMEViewer.h"
#include "Text/MIMEObj/TextMIMEObj.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		namespace MIMEViewer
		{
			class AVIRMIMETextViewer : public AVIRMIMEViewer
			{
			private:
				UI::GUITextBox *txtContent;

				Text::MIMEObj::TextMIMEObj *txt;

			public:
				AVIRMIMETextViewer(SSWR::AVIRead::AVIRCore *core, UI::GUICore *ui, UI::GUIClientControl *ctrl, Media::ColorManagerSess *sess, Text::MIMEObj::TextMIMEObj *txt);
				virtual ~AVIRMIMETextViewer();
			};
		}
	}
}
#endif