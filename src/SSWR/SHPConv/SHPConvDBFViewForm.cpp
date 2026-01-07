#include "Stdafx.h"
#include "SSWR/SHPConv/SHPConvDBFViewForm.h"
#include "Text/HKSCSFix.h"

SSWR::SHPConv::SHPConvDBFViewForm::SHPConvDBFViewForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, DB::DBFFile *dbf, SSWR::SHPConv::SHPConvEng *eng, Text::CStringNN lbl) : UI::GUIForm(parent, 424, 300, ui)
{
	this->SetText(CSTR("DBFView"));
	this->SetFont(nullptr, 8.25, false);
	NEW_CLASS(this->monMgr, Media::MonitorMgr());
	this->SetDPI(this->monMgr->GetMonitorHDPI(this->GetHMonitor()), this->monMgr->GetMonitorDDPI(this->GetHMonitor()));
	
	this->lvDBF = ui->NewListView(*this, UI::ListViewStyle::Table, dbf->GetColCount() + 1);
	this->lvDBF->SetDockType(UI::GUIControl::DOCK_FILL);

	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	WChar wbuff[256];
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;
	UOSInt m;
	Data::ArrayList<const UTF8Char*> dbCols;
	Data::ArrayList<UInt32> dbCols2;

	eng->ParseLabelStr(lbl, &dbCols, &dbCols2);
	this->lvDBF->AddColumn(CSTR("Output"), 120);
	i = 0;
	j = dbf->GetColCount();
	while (i < j)
	{
		sbuff[0] = 0;
		sptr = dbf->GetColumnName(i, sbuff).Or(sbuff);
		this->lvDBF->AddColumn(CSTRP(sbuff, sptr), 60);
		i += 1;
	}

	i = 0;
	j = dbf->GetRowCnt();
	if (j > 300)
	{
		j = 300;
	}
	Text::HKSCSFix hkscs;
	NN<Text::String> s;
	while (i < j)
	{
		s = eng->GetNewDBFName(dbf, &dbCols, i, dbCols2);
		m = this->lvDBF->AddItem(s, 0);
		s->Release();
		k = 0;
		l = dbf->GetColCount();
		while (k < l)
		{
			dbf->GetRecord(wbuff, i, k);
			hkscs.FixString(wbuff);
			this->lvDBF->SetSubItem(m, k + 1, wbuff);
			k++;
		}
		i++;
	}
}

SSWR::SHPConv::SHPConvDBFViewForm::~SHPConvDBFViewForm()
{
	DEL_CLASS(this->monMgr);
}

void SSWR::SHPConv::SHPConvDBFViewForm::OnMonitorChanged()
{
	this->SetDPI(this->monMgr->GetMonitorHDPI(this->GetHMonitor()), this->monMgr->GetMonitorDDPI(this->GetHMonitor()));
}

