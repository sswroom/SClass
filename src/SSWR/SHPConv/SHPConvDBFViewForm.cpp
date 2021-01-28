#include "Stdafx.h"
#include "SSWR/SHPConv/SHPConvDBFViewForm.h"
#include "Text/HKSCSFix.h"

SSWR::SHPConv::SHPConvDBFViewForm::SHPConvDBFViewForm(UI::GUIClientControl *parent, UI::GUICore *ui, DB::DBFFile *dbf, SSWR::SHPConv::IMapEng *eng, const UTF8Char *lbl) : UI::GUIForm(parent, 424, 300, ui)
{
	this->SetText((const UTF8Char*)"DBFView");
	this->SetFont(0, 8.25, false);
	NEW_CLASS(this->monMgr, Media::MonitorMgr());
	this->SetDPI(this->monMgr->GetMonitorHDPI(this->GetHMonitor()), this->monMgr->GetMonitorDDPI(this->GetHMonitor()));
	
	NEW_CLASS(this->lvDBF, UI::GUIListView(ui, this, UI::GUIListView::LVSTYLE_TABLE, dbf->GetColCount() + 1));
	this->lvDBF->SetDockType(UI::GUIControl::DOCK_FILL);

	UTF8Char sbuff[256];
	WChar wbuff[256];
	OSInt i;
	OSInt j;
	OSInt k;
	OSInt l;
	OSInt m;
	Data::ArrayList<const UTF8Char*> dbCols;
	Data::ArrayList<Int32> dbCols2;

	eng->ParseLabelStr(lbl, &dbCols, &dbCols2);
	this->lvDBF->AddColumn((const UTF8Char*)"Output", 120);
	i = 0;
	j = dbf->GetColCount();
	while (i < j)
	{
		dbf->GetColumnName(i, sbuff);
		this->lvDBF->AddColumn(sbuff, 60);
		i += 1;
	}

	i = 0;
	j = dbf->GetRowCnt();
	if (j > 300)
	{
		j = 300;
	}
	Text::HKSCSFix hkscs;
	const UTF8Char *csptr;
	while (i < j)
	{
		csptr = eng->GetDBFName(dbf, &dbCols, i, &dbCols2);
		m = this->lvDBF->AddItem(csptr, 0);
		Text::StrDelNew(csptr);
		k = 0;
		l = dbf->GetColCount();
		while (k < l)
		{
			dbf->GetRecord(wbuff, k, i);
			hkscs.FixString(wbuff);
			this->lvDBF->SetSubItem(m, k, wbuff);
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

