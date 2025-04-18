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

SSWR::AVIRead::MIMEViewer::AVIRMIMEViewer::AVIRMIMEViewer(NN<SSWR::AVIRead::AVIRCore> core, NN<UI::GUIClientControl> ctrl, NN<Text::MIMEObject> obj)
{
	this->core = core;
	this->ctrl = ctrl;
}

SSWR::AVIRead::MIMEViewer::AVIRMIMEViewer::~AVIRMIMEViewer()
{
}

Optional<SSWR::AVIRead::MIMEViewer::AVIRMIMEViewer> SSWR::AVIRead::MIMEViewer::AVIRMIMEViewer::CreateViewer(NN<SSWR::AVIRead::AVIRCore> core, NN<UI::GUICore> ui, NN<UI::GUIClientControl> ctrl, NN<Media::ColorManagerSess> sess, NN<Text::MIMEObject> obj)
{
	NN<SSWR::AVIRead::MIMEViewer::AVIRMIMEViewer> viewer;
	Text::CStringNN clsName = obj->GetClassName();
	if (clsName.Equals(UTF8STRC("MailMessage")))
	{
		NEW_CLASSNN(viewer, SSWR::AVIRead::MIMEViewer::AVIRMailViewer(core, ui, ctrl, sess, NN<Text::MIMEObj::MailMessage>::ConvertFrom(obj)));
		return viewer;
	}
	else if (clsName.Equals(UTF8STRC("TextMIMEObj")))
	{
		NEW_CLASSNN(viewer, SSWR::AVIRead::MIMEViewer::AVIRMIMETextViewer(core, ui, ctrl, sess, NN<Text::MIMEObj::TextMIMEObj>::ConvertFrom(obj)));
		return viewer;
	}
	else if (clsName.Equals(UTF8STRC("MultipartMIMEObj")))
	{
		NEW_CLASSNN(viewer, SSWR::AVIRead::MIMEViewer::AVIRMultipartViewer(core, ui, ctrl, sess, NN<Text::MIMEObj::MultipartMIMEObj>::ConvertFrom(obj)));
		return viewer;
	}
	else
	{
		Text::CStringNN contType = obj->GetContentType();
		if (contType.StartsWith(UTF8STRC("image/jpeg")) || contType.StartsWith(UTF8STRC("image/png")) || contType.StartsWith(UTF8STRC("image/jpg")))
		{
			NEW_CLASSNN(viewer, SSWR::AVIRead::MIMEViewer::AVIRMIMEImageViewer(core, ui, ctrl, sess, NN<Text::MIMEObj::UnknownMIMEObj>::ConvertFrom(obj)));
			return viewer;
		}
		else if (contType.StartsWith(UTF8STRC("text/xml")))
		{
			NEW_CLASSNN(viewer, SSWR::AVIRead::MIMEViewer::AVIRMIMEXMLViewer(core, ui, ctrl, sess, NN<Text::MIMEObj::UnknownMIMEObj>::ConvertFrom(obj)));
			return viewer;
		}
		else if (contType.StartsWith(UTF8STRC("application/json")))
		{
			NEW_CLASSNN(viewer, SSWR::AVIRead::MIMEViewer::AVIRMIMEJSONViewer(core, ui, ctrl, sess, NN<Text::MIMEObj::UnknownMIMEObj>::ConvertFrom(obj)));
			return viewer;
		}
		else if (contType.StartsWith(UTF8STRC("text/json")))
		{
			NEW_CLASSNN(viewer, SSWR::AVIRead::MIMEViewer::AVIRMIMEJSONViewer(core, ui, ctrl, sess, NN<Text::MIMEObj::UnknownMIMEObj>::ConvertFrom(obj)));
			return viewer;
		}
		else if (contType.StartsWith(UTF8STRC("text/html")))
		{
			NEW_CLASSNN(viewer, SSWR::AVIRead::MIMEViewer::AVIRMIMEHTMLViewer(core, ui, ctrl, sess, NN<Text::MIMEObj::UnknownMIMEObj>::ConvertFrom(obj)));
			return viewer;
		}
		else if (contType.StartsWith(UTF8STRC("application/pkcs7-signature")))
		{
			NEW_CLASSNN(viewer, SSWR::AVIRead::MIMEViewer::AVIRMIMEX509Viewer(core, ui, ctrl, sess, NN<Text::MIMEObj::UnknownMIMEObj>::ConvertFrom(obj)));
			return viewer;
		}
		else
		{
			NEW_CLASSNN(viewer, SSWR::AVIRead::MIMEViewer::AVIRUnknownViewer(core, ui, ctrl, sess, obj));
			return viewer;
		}
	}
}
