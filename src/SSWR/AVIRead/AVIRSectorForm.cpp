#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRSectorForm.h"

void __stdcall SSWR::AVIRead::AVIRSectorForm::OnParseClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRSectorForm> me = userObj.GetNN<SSWR::AVIRead::AVIRSectorForm>();
	NN<Parser::ParserList> parsers = me->core->GetParserList();
	NN<IO::ParsedObject> pobj;
	if (parsers->ParseObject(me->data).SetTo(pobj))
	{
		me->core->OpenObject(pobj);
	}
}

SSWR::AVIRead::AVIRSectorForm::AVIRSectorForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<IO::ISectorData> data) : UI::GUIForm(parent, 1024, 768, ui)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	this->data = data;
	this->core = core;
	sptr = data->GetSourceNameObj()->ConcatTo(Text::StrConcatC(sbuff, UTF8STRC("Sector Viewer - ")));
	this->SetText(CSTRP(sbuff, sptr));
	this->SetFont(0, 0, 8.25, false);
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->pnlCtrl = ui->NewPanel(*this);
	this->pnlCtrl->SetRect(0, 0, 100, 72, false);
	this->pnlCtrl->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->btnParse = ui->NewButton(this->pnlCtrl, CSTR("Parse File"));
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
