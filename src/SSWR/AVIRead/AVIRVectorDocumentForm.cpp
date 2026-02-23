#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRVectorDocumentForm.h"

SSWR::AVIRead::AVIRVectorDocumentForm::AVIRVectorDocumentForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Media::VectorDocument> vdoc) : UI::GUIForm(parent, 640, 480, ui), core(core)
{
	this->SetFont(nullptr, 8.25, false);
	this->SetText(CSTR("Vector Document Viewer"));
	
	this->core = core;
	this->vdoc = vdoc;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->lbPages = ui->NewListBox(*this, false);
	this->lbPages->SetRect(0, 0, 100, 100, false);
	this->lbPages->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->hspMain = ui->NewHSplitter(*this, 3, false);
	this->pbMain = ui->NewPictureBoxSimple(*this, this->core->GetDrawEngine(), false);
	this->pbMain->SetDockType(UI::GUIControl::DOCK_FILL);

	UIntOS pageCnt = vdoc->GetCount();
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> sptrEnd;
	UIntOS i;
	sptr = Text::StrConcatC(sbuff, UTF8STRC("Page "));
	i = 0;
	while (i < pageCnt)
	{
		i++;
		sptrEnd = Text::StrUIntOS(sptr, i);
		this->lbPages->AddItem(CSTRP(sbuff, sptrEnd), (void*)(i - 1));
	}
	if (pageCnt > 0)
	{
		this->lbPages->SetSelectedIndex(0);
	}
}

SSWR::AVIRead::AVIRVectorDocumentForm::~AVIRVectorDocumentForm()
{
	this->vdoc.Delete();
}

void SSWR::AVIRead::AVIRVectorDocumentForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
