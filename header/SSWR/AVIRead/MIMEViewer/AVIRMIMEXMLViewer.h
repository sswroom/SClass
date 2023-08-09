#ifndef _SM_SSWR_AVIREAD_MIMEVIEWER_AVIRMIMEXMLVIEWER
#define _SM_SSWR_AVIREAD_MIMEVIEWER_AVIRMIMEXMLVIEWER
#include "SSWR/AVIRead/AVIRCore.h"
#include "SSWR/AVIRead/MIMEViewer/AVIRMIMEViewer.h"
#include "Text/MIMEObj/UnknownMIMEObj.h"
#include "UI/GUIListView.h"

namespace SSWR
{
	namespace AVIRead
	{
		namespace MIMEViewer
		{
			class AVIRMIMEXMLViewer : public AVIRMIMEViewer
			{
			private:
				UI::GUIListView *lvXML;

				Text::MIMEObj::UnknownMIMEObj *obj;

			public:
				AVIRMIMEXMLViewer(NotNullPtr<SSWR::AVIRead::AVIRCore> core, NotNullPtr<UI::GUICore> ui, UI::GUIClientControl *ctrl, Media::ColorManagerSess *sess, Text::MIMEObj::UnknownMIMEObj *obj);
				virtual ~AVIRMIMEXMLViewer();
			};
		}
	}
}
#endif