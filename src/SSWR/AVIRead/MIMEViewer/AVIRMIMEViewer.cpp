#include "Stdafx.h"
#include "SSWR/AVIRead/MIMEViewer/AVIRMailViewer.h"
#include "SSWR/AVIRead/MIMEViewer/AVIRMIMEHTMLViewer.h"
#include "SSWR/AVIRead/MIMEViewer/AVIRMIMEImageViewer.h"
#include "SSWR/AVIRead/MIMEViewer/AVIRMIMEJSONViewer.h"
#include "SSWR/AVIRead/MIMEViewer/AVIRMIMETextViewer.h"
#include "SSWR/AVIRead/MIMEViewer/AVIRMIMEViewer.h"
#include "SSWR/AVIRead/MIMEViewer/AVIRMIMEX509Viewer.h"
#include "SSWR/AVIRead/MIMEViewer/AVIRMIMEXMLViewer.h"
#include "SSWR/AVIRead/MIMEViewer/AVIRMultipartViewer.h"
#include "SSWR/AVIRead/MIMEViewer/AVIRUnknownViewer.h"

SSWR::AVIRead::MIMEViewer::AVIRMIMEViewer::AVIRMIMEViewer(NotNullPtr<SSWR::AVIRead::AVIRCore> core, NotNullPtr<UI::GUIClientControl> ctrl, Text::IMIMEObj *obj)
{
	this->core = core;
	this->ctrl = ctrl;
}

SSWR::AVIRead::MIMEViewer::AVIRMIMEViewer::~AVIRMIMEViewer()
{
}

SSWR::AVIRead::MIMEViewer::AVIRMIMEViewer *SSWR::AVIRead::MIMEViewer::AVIRMIMEViewer::CreateViewer(NotNullPtr<SSWR::AVIRead::AVIRCore> core, NotNullPtr<UI::GUICore> ui, NotNullPtr<UI::GUIClientControl> ctrl, NotNullPtr<Media::ColorManagerSess> sess, Text::IMIMEObj *obj)
{
	SSWR::AVIRead::MIMEViewer::AVIRMIMEViewer *viewer;
	if (obj == 0)
	{
		return 0;
	}
	Text::CString clsName = obj->GetClassName();
	if (clsName.Equals(UTF8STRC("MailMessage")))
	{
		NEW_CLASS(viewer, SSWR::AVIRead::MIMEViewer::AVIRMailViewer(core, ui, ctrl, sess, (Text::MIMEObj::MailMessage*)obj));
		return viewer;
	}
	else if (clsName.Equals(UTF8STRC("TextMIMEObj")))
	{
		NEW_CLASS(viewer, SSWR::AVIRead::MIMEViewer::AVIRMIMETextViewer(core, ui, ctrl, sess, (Text::MIMEObj::TextMIMEObj*)obj));
		return viewer;
	}
	else if (clsName.Equals(UTF8STRC("MultipartMIMEObj")))
	{
		NEW_CLASS(viewer, SSWR::AVIRead::MIMEViewer::AVIRMultipartViewer(core, ui, ctrl, sess, (Text::MIMEObj::MultipartMIMEObj*)obj));
		return viewer;
	}
	else
	{
		Text::CString contType = obj->GetContentType();
		if (contType.StartsWith(UTF8STRC("image/jpeg")) || contType.StartsWith(UTF8STRC("image/png")) || contType.StartsWith(UTF8STRC("image/jpg")))
		{
			NEW_CLASS(viewer, SSWR::AVIRead::MIMEViewer::AVIRMIMEImageViewer(core, ui, ctrl, sess, (Text::MIMEObj::UnknownMIMEObj*)obj));
			return viewer;
		}
		else if (contType.StartsWith(UTF8STRC("text/xml")))
		{
			NEW_CLASS(viewer, SSWR::AVIRead::MIMEViewer::AVIRMIMEXMLViewer(core, ui, ctrl, sess, (Text::MIMEObj::UnknownMIMEObj*)obj));
			return viewer;
		}
		else if (contType.StartsWith(UTF8STRC("application/json")))
		{
			NEW_CLASS(viewer, SSWR::AVIRead::MIMEViewer::AVIRMIMEJSONViewer(core, ui, ctrl, sess, (Text::MIMEObj::UnknownMIMEObj*)obj));
			return viewer;
		}
		else if (contType.StartsWith(UTF8STRC("text/json")))
		{
			NEW_CLASS(viewer, SSWR::AVIRead::MIMEViewer::AVIRMIMEJSONViewer(core, ui, ctrl, sess, (Text::MIMEObj::UnknownMIMEObj*)obj));
			return viewer;
		}
		else if (contType.StartsWith(UTF8STRC("text/html")))
		{
			NEW_CLASS(viewer, SSWR::AVIRead::MIMEViewer::AVIRMIMEHTMLViewer(core, ui, ctrl, sess, (Text::MIMEObj::UnknownMIMEObj*)obj));
			return viewer;
		}
		else if (contType.StartsWith(UTF8STRC("application/pkcs7-signature")))
		{
			NEW_CLASS(viewer, SSWR::AVIRead::MIMEViewer::AVIRMIMEX509Viewer(core, ui, ctrl, sess, (Text::MIMEObj::UnknownMIMEObj*)obj));
			return viewer;
		}
		else
		{
			NEW_CLASS(viewer, SSWR::AVIRead::MIMEViewer::AVIRUnknownViewer(core, ui, ctrl, sess, obj));
			return viewer;
		}
	}
}
