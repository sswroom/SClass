#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRVectorDocumentForm.h"

void __stdcall SSWR::AVIRead::AVIRVectorDocumentForm::OnPagesSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRVectorDocumentForm> me = userObj.GetNN<SSWR::AVIRead::AVIRVectorDocumentForm>();
	UIntOS pageIndex = me->lbPages->GetSelectedItem().GetUIntOS();
	NN<Media::VectorGraph> page;
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	if (me->vdoc->GetItem(pageIndex).SetTo(page))
	{
		me->lvInfo->ClearItems();
		me->lvInfo->AddItem(CSTR("Width(px)"), nullptr);
		sptr = Text::StrUIntOS(sbuff, page->GetWidth());
		me->lvInfo->SetSubItem(0, 1, CSTRP(sbuff, sptr));
		me->lvInfo->AddItem(CSTR("Height(px)"), nullptr);
		sptr = Text::StrUIntOS(sbuff, page->GetHeight());
		me->lvInfo->SetSubItem(1, 1, CSTRP(sbuff, sptr));
		me->lvInfo->AddItem(CSTR("Width(mm)"), nullptr);
		sptr = Text::StrDouble(sbuff, page->GetVisibleWidthMM());
		me->lvInfo->SetSubItem(2, 1, CSTRP(sbuff, sptr));
		me->lvInfo->AddItem(CSTR("Height(mm)"), nullptr);
		sptr = Text::StrDouble(sbuff, page->GetVisibleHeightMM());
		me->lvInfo->SetSubItem(3, 1, CSTRP(sbuff, sptr));
		me->lvInfo->AddItem(CSTR("H-DPI"), nullptr);
		sptr = Text::StrDouble(sbuff, page->GetHDPI());
		me->lvInfo->SetSubItem(4, 1, CSTRP(sbuff, sptr));
		me->lvInfo->AddItem(CSTR("V-DPI"), nullptr);
		sptr = Text::StrDouble(sbuff, page->GetVDPI());
		me->lvInfo->SetSubItem(5, 1, CSTRP(sbuff, sptr));
		me->lvInfo->AddItem(CSTR("Item Count"), nullptr);
		sptr = Text::StrUIntOS(sbuff, page->GetCount());
		me->lvInfo->SetSubItem(6, 1, CSTRP(sbuff, sptr));
	}
}

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
	this->lbPages->HandleSelectionChange(OnPagesSelChg, this);
	this->hspMain = ui->NewHSplitter(*this, 3, false);
	this->tcMain = ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);
	this->tpPreview = this->tcMain->AddTabPage(CSTR("Preview"));
	this->pbMain = ui->NewPictureBoxSimple(this->tpPreview, this->core->GetDrawEngine(), false);
	this->pbMain->SetDockType(UI::GUIControl::DOCK_FILL);
	this->tpInfo = this->tcMain->AddTabPage(CSTR("Info"));
	this->lvInfo = ui->NewListView(this->tpInfo, UI::ListViewStyle::Table, 2);
	this->lvInfo->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvInfo->AddColumn(CSTR("Name"), 150);
	this->lvInfo->AddColumn(CSTR("Value"), 300);

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
