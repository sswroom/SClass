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
				NN<UI::GUIListView> lvXML;

				Text::MIMEObj::UnknownMIMEObj *obj;

			public:
				AVIRMIMEXMLViewer(NN<SSWR::AVIRead::AVIRCore> core, NN<UI::GUICore> ui, NN<UI::GUIClientControl> ctrl, NN<Media::ColorManagerSess> sess, Text::MIMEObj::UnknownMIMEObj *obj);
				virtual ~AVIRMIMEXMLViewer();
			};
		}
	}
}
#endif