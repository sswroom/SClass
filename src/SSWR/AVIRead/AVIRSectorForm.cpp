#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRSectorForm.h"

void __stdcall SSWR::AVIRead::AVIRSectorForm::OnParseClicked(void *userObj)
{
	SSWR::AVIRead::AVIRSectorForm *me = (SSWR::AVIRead::AVIRSectorForm *)userObj;
	Parser::ParserList *parsers = me->core->GetParserList();
	NotNullPtr<IO::ParsedObject> pobj;
	if (pobj.Set(parsers->ParseObject(me->data, 0)))
	{
		me->core->OpenObject(pobj);
	}
}

SSWR::AVIRead::AVIRSectorForm::AVIRSectorForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, NotNullPtr<IO::ISectorData> data) : UI::GUIForm(parent, 1024, 768, ui)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	this->data = data;
	this->core = core;
	sptr = data->GetSourceNameObj()->ConcatTo(Text::StrConcatC(sbuff, UTF8STRC("Sector Viewer - ")));
	this->SetText(CSTRP(sbuff, sptr));
	this->SetFont(0, 0, 8.25, false);
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->pnlCtrl, UI::GUIPanel(ui, this));
	this->pnlCtrl->SetRect(0, 0, 100, 72, false);
	this->pnlCtrl->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->btnParse, UI::GUIButton(ui, this->pnlCtrl, CSTR("Parse File")));
	this->btnParse->SetRect(4, 4, 75, 23, false);
	this->btnParse->HandleButtonClick(OnParseClicked, this);

	this->sectorSize = this->data->GetBytesPerSector();
	this->sectorData = MemAlloc(UInt8, this->sectorSize);
}

SSWR::AVIRead::AVIRSectorForm::~AVIRSectorForm()
{
	MemFree(this->sectorData);
	this->data.Delete();
}

void SSWR::AVIRead::AVIRSectorForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
