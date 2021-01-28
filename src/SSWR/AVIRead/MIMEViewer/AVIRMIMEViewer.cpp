#include "Stdafx.h"
#include "SSWR/AVIRead/MIMEViewer/AVIRMailViewer.h"
#include "SSWR/AVIRead/MIMEViewer/AVIRMIMEHTMLViewer.h"
#include "SSWR/AVIRead/MIMEViewer/AVIRMIMEImageViewer.h"
#include "SSWR/AVIRead/MIMEViewer/AVIRMIMEJSONViewer.h"
#include "SSWR/AVIRead/MIMEViewer/AVIRMIMETextViewer.h"
#include "SSWR/AVIRead/MIMEViewer/AVIRMIMEViewer.h"
#include "SSWR/AVIRead/MIMEViewer/AVIRMIMEXMLViewer.h"
#include "SSWR/AVIRead/MIMEViewer/AVIRMultipartViewer.h"
#include "SSWR/AVIRead/MIMEViewer/AVIRUnknownViewer.h"

SSWR::AVIRead::MIMEViewer::AVIRMIMEViewer::AVIRMIMEViewer(SSWR::AVIRead::AVIRCore *core, UI::GUIClientControl *ctrl, Text::IMIMEObj *obj)
{
	this->core = core;
	this->ctrl = ctrl;
}

SSWR::AVIRead::MIMEViewer::AVIRMIMEViewer::~AVIRMIMEViewer()
{
}

SSWR::AVIRead::MIMEViewer::AVIRMIMEViewer *SSWR::AVIRead::MIMEViewer::AVIRMIMEViewer::CreateViewer(SSWR::AVIRead::AVIRCore *core, UI::GUICore *ui, UI::GUIClientControl *ctrl, Media::ColorManagerSess *sess, Text::IMIMEObj *obj)
{
	SSWR::AVIRead::MIMEViewer::AVIRMIMEViewer *viewer;
	if (obj == 0)
	{
		return 0;
	}
	const UTF8Char *clsName = obj->GetClassName();
	if (Text::StrEquals(clsName, (const UTF8Char*)"MailMessage"))
	{
		NEW_CLASS(viewer, SSWR::AVIRead::MIMEViewer::AVIRMailViewer(core, ui, ctrl, sess, (Text::MIMEObj::MailMessage*)obj));
		return viewer;
	}
	else if (Text::StrEquals(clsName, (const UTF8Char*)"TextMIMEObj"))
	{
		NEW_CLASS(viewer, SSWR::AVIRead::MIMEViewer::AVIRMIMETextViewer(core, ui, ctrl, sess, (Text::MIMEObj::TextMIMEObj*)obj));
		return viewer;
	}
	else if (Text::StrEquals(clsName, (const UTF8Char*)"MultipartMIMEObj"))
	{
		NEW_CLASS(viewer, SSWR::AVIRead::MIMEViewer::AVIRMultipartViewer(core, ui, ctrl, sess, (Text::MIMEObj::MultipartMIMEObj*)obj));
		return viewer;
	}
	else
	{
		const UTF8Char *contType = obj->GetContentType();
		if (Text::StrStartsWith(contType, (const UTF8Char*)"image/jpeg") || Text::StrStartsWith(contType, (const UTF8Char*)"image/png") || Text::StrStartsWith(contType, (const UTF8Char*)"image/jpg"))
		{
			NEW_CLASS(viewer, SSWR::AVIRead::MIMEViewer::AVIRMIMEImageViewer(core, ui, ctrl, sess, (Text::MIMEObj::UnknownMIMEObj*)obj));
			return viewer;
		}
		else if (Text::StrStartsWith(contType, (const UTF8Char*)"text/xml"))
		{
			NEW_CLASS(viewer, SSWR::AVIRead::MIMEViewer::AVIRMIMEXMLViewer(core, ui, ctrl, sess, (Text::MIMEObj::UnknownMIMEObj*)obj));
			return viewer;
		}
		else if (Text::StrStartsWith(contType, (const UTF8Char*)"application/json"))
		{
			NEW_CLASS(viewer, SSWR::AVIRead::MIMEViewer::AVIRMIMEJSONViewer(core, ui, ctrl, sess, (Text::MIMEObj::UnknownMIMEObj*)obj));
			return viewer;
		}
		else if (Text::StrStartsWith(contType, (const UTF8Char*)"text/json"))
		{
			NEW_CLASS(viewer, SSWR::AVIRead::MIMEViewer::AVIRMIMEJSONViewer(core, ui, ctrl, sess, (Text::MIMEObj::UnknownMIMEObj*)obj));
			return viewer;
		}
		else if (Text::StrStartsWith(contType, (const UTF8Char*)"text/html"))
		{
			NEW_CLASS(viewer, SSWR::AVIRead::MIMEViewer::AVIRMIMEHTMLViewer(core, ui, ctrl, sess, (Text::MIMEObj::UnknownMIMEObj*)obj));
			return viewer;
		}
		else
		{
			NEW_CLASS(viewer, SSWR::AVIRead::MIMEViewer::AVIRUnknownViewer(core, ui, ctrl, sess, obj));
			return viewer;
		}
	}
}
