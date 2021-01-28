#ifndef _SM_SSWR_AVIREAD_MIMEVIEWER_AVIRMULTIPARTVIEWER
#define _SM_SSWR_AVIREAD_MIMEVIEWER_AVIRMULTIPARTVIEWER
#include "SSWR/AVIRead/AVIRCore.h"
#include "SSWR/AVIRead/MIMEViewer/AVIRMIMEViewer.h"
#include "Text/MIMEObj/MultipartMIMEObj.h"
#include "UI/GUITabControl.h"

namespace SSWR
{
	namespace AVIRead
	{
		namespace MIMEViewer
		{
			class AVIRMultipartViewer : public AVIRMIMEViewer
			{
			private:
				UI::GUITabControl *tcParts;
				Data::ArrayList<SSWR::AVIRead::MIMEViewer::AVIRMIMEViewer*> *subViewers;

				Text::MIMEObj::MultipartMIMEObj *obj;

			public:
				AVIRMultipartViewer(SSWR::AVIRead::AVIRCore *core, UI::GUICore *ui, UI::GUIClientControl *ctrl, Media::ColorManagerSess *sess, Text::MIMEObj::MultipartMIMEObj *obj);
				virtual ~AVIRMultipartViewer();
			};
		}
	}
}
#endif