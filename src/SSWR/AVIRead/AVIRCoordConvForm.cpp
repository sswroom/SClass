#include "Stdafx.h"
#include "DB/CSVFile.h"
#include "IO/FileStream.h"
#include "IO/StreamWriter.h"
#include "IO/StmData/FileData.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/ProjectedCoordinateSystem.h"
#include "SSWR/AVIRead/AVIRCoordConvForm.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/FileDialog.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRCoordConvForm::OnSrcRadChanged(void *userObj, Bool newValue)
{
	SSWR::AVIRead::AVIRCoordConvForm *me = (SSWR::AVIRead::AVIRCoordConvForm *)userObj;
	if (me->inited)
	{
		me->ClearItems();
		if (newValue)
		{
			me->FillCoordGeo(me->cboSrc);
		}
		else
		{
			me->FillCoordProj(me->cboSrc);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRCoordConvForm::OnSrcCboChanged(void *userObj)
{
	SSWR::AVIRead::AVIRCoordConvForm *me = (SSWR::AVIRead::AVIRCoordConvForm *)userObj;
	me->ClearItems();
}

void __stdcall SSWR::AVIRead::AVIRCoordConvForm::OnDestRadChanged(void *userObj, Bool newValue)
{
	SSWR::AVIRead::AVIRCoordConvForm *me = (SSWR::AVIRead::AVIRCoordConvForm *)userObj;
	if (me->inited)
	{
		if (newValue)
		{
			me->FillCoordGeo(me->cboDest);
		}
		else
		{
			me->FillCoordProj(me->cboDest);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRCoordConvForm::OnDestCboChanged(void *userObj)
{
	SSWR::AVIRead::AVIRCoordConvForm *me = (SSWR::AVIRead::AVIRCoordConvForm *)userObj;
	me->UpdateList();
}

void __stdcall SSWR::AVIRead::AVIRCoordConvForm::OnAddClicked(void *userObj)
{
	SSWR::AVIRead::AVIRCoordConvForm *me = (SSWR::AVIRead::AVIRCoordConvForm *)userObj;
	Text::StringBuilderUTF8 sb;
	Double x;
	Double y;
	Double z;
	me->txtEasting->GetText(&sb);
	if (!Text::StrToDouble(sb.ToString(), &x))
		return;
	sb.ClearStr();
	me->txtNorthing->GetText(&sb);
	if (!Text::StrToDouble(sb.ToString(), &y))
		return;
	sb.ClearStr();
	me->txtHeight->GetText(&sb);
	if (!Text::StrToDouble(sb.ToString(), &z))
		return;
	sb.ClearStr();
	me->txtName->GetText(&sb);
	if (sb.GetLength() <= 0)
		return;
	me->nameList->Add(Text::StrCopyNew(sb.ToString()));
	me->xList->Add(x);
	me->yList->Add(y);
	me->zList->Add(z);
	me->UpdateList();
}

void __stdcall SSWR::AVIRead::AVIRCoordConvForm::OnClearClicked(void *userObj)
{
	SSWR::AVIRead::AVIRCoordConvForm *me = (SSWR::AVIRead::AVIRCoordConvForm *)userObj;
	me->ClearItems();
}

void __stdcall SSWR::AVIRead::AVIRCoordConvForm::OnConvFileClicked(void *userObj)
{
	SSWR::AVIRead::AVIRCoordConvForm *me = (SSWR::AVIRead::AVIRCoordConvForm *)userObj;
	UI::FileDialog *dlg;
	UTF8Char sbuff[256];
	UTF8Char *strBuff;
	UTF8Char *sptr;
	UTF8Char *sptr2;
	Parser::ParserList *parsers = me->core->GetParserList();
	Math::CoordinateSystem *srcCoord;
	Math::CoordinateSystem *destCoord;
	OSInt si;

	si = me->cboSrc->GetSelectedIndex();
	if (si == -1)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char *)"Please select source coordinate system", (const UTF8Char *)"Error", me);
		return;
	}

	si = me->cboDest->GetSelectedIndex();
	if (si == -1)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char *)"Please select dest coordinate system", (const UTF8Char *)"Error", me);
		return;
	}

	NEW_CLASS(dlg, UI::FileDialog(L"SSWR", L"AVIRead", L"CoordConvFile", false));
	parsers->PrepareSelector(dlg, IO::ParsedObject::PT_READINGDB_PARSER);
	if (!dlg->ShowDialog(me->GetHandle()))
	{
		DEL_CLASS(dlg);
		return;
	}
	DB::ReadingDB *db = 0;
	if (Text::StrEndsWithICase(dlg->GetFileName(), (const UTF8Char*)".CSV"))
	{
		DB::CSVFile *csv;
		NEW_CLASS(csv, DB::CSVFile(dlg->GetFileName(), 0));
		db = csv;
	}
	if (db == 0)
	{
		IO::StmData::FileData *fd;
		NEW_CLASS(fd, IO::StmData::FileData(dlg->GetFileName(), false));
		DEL_CLASS(dlg);
		db = (DB::ReadingDB*)parsers->ParseFileType(fd, IO::ParsedObject::PT_READINGDB_PARSER);
		DEL_CLASS(fd);
		if (db == 0)
		{
			UI::MessageDialog::ShowDialog((const UTF8Char *)"File is not a database file", (const UTF8Char *)"Error", me);
			return;
		}
	}
	UOSInt xCol = (UOSInt)-1;
	UOSInt yCol = (UOSInt)-1;
	UOSInt colCnt;
	UOSInt i;
	DB::DBReader *reader = db->GetTableData(0, 0, 0, 0);
	if (reader == 0)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char *)"Unsupported database format", (const UTF8Char *)"Error", me);
		DEL_CLASS(db);
		return;
	}
	i = reader->ColCount();
	colCnt = i;
	while (i-- > 0)
	{
		if (reader->GetName(i, sbuff))
		{
			if (Text::StrEqualsICase(sbuff, (const UTF8Char*)"MAPX"))
			{
				xCol = i;
			}
			else if (Text::StrEqualsICase(sbuff, (const UTF8Char*)"MAPY"))
			{
				yCol = i;
			}
		}
	}
	db->CloseReader(reader);
	if (xCol == (UOSInt)-1 || yCol == (UOSInt)-1)
	{
		DEL_CLASS(db);
		UI::MessageDialog::ShowDialog((const UTF8Char *)"XY Database column not found", (const UTF8Char *)"Error", me);
		return;
	}
	

	NEW_CLASS(dlg, UI::FileDialog(L"SSWR", L"AVIRead", L"CoordConvSave", true));
	dlg->AddFilter((const UTF8Char*)"*.csv", (const UTF8Char*)"CSV File");
	if (!dlg->ShowDialog(me->GetHandle()))
	{
		DEL_CLASS(dlg);
		DEL_CLASS(db);
		return;
	}
	IO::FileStream *fs;
	IO::StreamWriter *writer;
	NEW_CLASS(fs, IO::FileStream(dlg->GetFileName(), IO::FileStream::FILE_MODE_CREATE, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
	if (fs->IsError())
	{
		DEL_CLASS(fs);
		DEL_CLASS(dlg);
		DEL_CLASS(db);
		UI::MessageDialog::ShowDialog((const UTF8Char *)"Error in creating output file", (const UTF8Char *)"Error", me);
		return;
	}
	Text::StringBuilderUTF8 sb;
	const UTF8Char **sarr;
	sarr = MemAlloc(const UTF8Char *, colCnt + 2);
	i = 0;
	reader = db->GetTableData(0, 0, 0, 0);
	if (reader == 0)
	{
		MemFree(sarr);
		DEL_CLASS(dlg);
		DEL_CLASS(fs);
		DEL_CLASS(db);
		UI::MessageDialog::ShowDialog((const UTF8Char *)"Error in reading source file", (const UTF8Char *)"Error", me);
		return;
	}

	if (me->radSrcGeo->IsSelected())
	{
		Math::GeographicCoordinateSystem::GeoCoordSysType gcst = (Math::GeographicCoordinateSystem::GeoCoordSysType)(OSInt)me->cboSrc->GetItem(i);
		srcCoord = Math::CoordinateSystemManager::CreateGeogCoordinateSystemDefName(gcst);
	}
	else
	{
		Math::ProjectedCoordinateSystem::ProjCoordSysType pcst = (Math::ProjectedCoordinateSystem::ProjCoordSysType)(OSInt)me->cboSrc->GetItem(i);
		srcCoord = Math::CoordinateSystemManager::CreateProjCoordinateSystemDefName(pcst);
	}

	if (me->radDestGeo->IsSelected())
	{
		Math::GeographicCoordinateSystem::GeoCoordSysType gcst = (Math::GeographicCoordinateSystem::GeoCoordSysType)(OSInt)me->cboDest->GetItem(i);
		destCoord = Math::CoordinateSystemManager::CreateGeogCoordinateSystemDefName(gcst);
	}
	else
	{
		Math::ProjectedCoordinateSystem::ProjCoordSysType pcst = (Math::ProjectedCoordinateSystem::ProjCoordSysType)(OSInt)me->cboDest->GetItem(i);
		destCoord = Math::CoordinateSystemManager::CreateProjCoordinateSystemDefName(pcst);
	}

	strBuff = MemAlloc(UTF8Char, 16384);
	NEW_CLASS(writer, IO::StreamWriter(fs, 65001));
	sptr = strBuff;
	i = 0;
	while (i < colCnt)
	{
		sarr[i] = sptr;
		sptr2 = reader->GetName(i, sptr);
		if (sptr2)
		{
			sptr = sptr2 + 1;
		}
		else
		{
			sarr[i] = (const UTF8Char*)"";
		}
		i++;
	}
	sarr[colCnt] = (const UTF8Char*)"OutX";
	sarr[colCnt + 1] = (const UTF8Char*)"OutY";
	sb.ClearStr();
	sb.AppendCSV(sarr, colCnt + 2);
	writer->WriteLine(sb.ToString());

	Double inX;
	Double inY;
	Double outX;
	Double outY;
	Double outZ;
	while (reader->ReadNext())
	{
		sptr = strBuff;
		i = 0;
		while (i < colCnt)
		{
			sarr[i] = sptr;
			sptr2 = reader->GetStr(i, sptr, 16384 - (UOSInt)(sptr - strBuff));
			if (sptr2)
			{
				sptr = sptr2 + 1;
			}
			else
			{
				sarr[i] = (const UTF8Char*)"";
			}
			i++;
		}

		if (Text::StrToDouble(sarr[xCol], &inX) && Text::StrToDouble(sarr[yCol], &inY))
		{
			Math::CoordinateSystem::ConvertXYZ(srcCoord, destCoord, inX, inY, 0, &outX, &outY, &outZ);
			sarr[colCnt] = sptr;
			sptr = Text::StrDouble(sptr, outX) + 1;
			sarr[colCnt + 1] = sptr;
			sptr = Text::StrDouble(sptr, outY) + 1;
		}
		else
		{
			sarr[colCnt] = (const UTF8Char*)"";
			sarr[colCnt + 1] = (const UTF8Char*)"";
		}
		sb.ClearStr();
		sb.AppendCSV(sarr, colCnt + 2);
		writer->WriteLine(sb.ToString());
	}

	MemFree(strBuff);
	MemFree(sarr);
	DEL_CLASS(srcCoord);
	DEL_CLASS(destCoord);
	DEL_CLASS(dlg);
	DEL_CLASS(writer);
	DEL_CLASS(fs);
	DEL_CLASS(db);
}

void SSWR::AVIRead::AVIRCoordConvForm::ClearItems()
{
	UOSInt i;
	i = this->nameList->GetCount();
	while (i-- > 0)
	{
		Text::StrDelNew(this->nameList->GetItem(i));
	}
	this->nameList->Clear();
	this->xList->Clear();
	this->yList->Clear();
	this->zList->Clear();
	this->UpdateList();
}

void SSWR::AVIRead::AVIRCoordConvForm::UpdateList()
{
	WChar sbuff[32];
	OSInt si;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	Double x;
	Double y;
	Double z;
	Double destX;
	Double destY;
	Double destZ;
	Math::CoordinateSystem *srcCoord;
	Math::CoordinateSystem *destCoord;

	si = this->cboSrc->GetSelectedIndex();
	if (si == -1)
	{
		srcCoord = 0;
	}
	else
	{
		if (this->radSrcGeo->IsSelected())
		{
			Math::GeographicCoordinateSystem::GeoCoordSysType gcst = (Math::GeographicCoordinateSystem::GeoCoordSysType)(OSInt)this->cboSrc->GetItem((UOSInt)si);
			srcCoord = Math::CoordinateSystemManager::CreateGeogCoordinateSystemDefName(gcst);
		}
		else
		{
			Math::ProjectedCoordinateSystem::ProjCoordSysType pcst = (Math::ProjectedCoordinateSystem::ProjCoordSysType)(OSInt)this->cboSrc->GetItem((UOSInt)si);
			srcCoord = Math::CoordinateSystemManager::CreateProjCoordinateSystemDefName(pcst);
		}
	}

	si = this->cboDest->GetSelectedIndex();
	if (si == -1)
	{
		destCoord = 0;
	}
	else
	{
		if (this->radDestGeo->IsSelected())
		{
			Math::GeographicCoordinateSystem::GeoCoordSysType gcst = (Math::GeographicCoordinateSystem::GeoCoordSysType)(OSInt)this->cboDest->GetItem((UOSInt)si);
			destCoord = Math::CoordinateSystemManager::CreateGeogCoordinateSystemDefName(gcst);
		}
		else
		{
			Math::ProjectedCoordinateSystem::ProjCoordSysType pcst = (Math::ProjectedCoordinateSystem::ProjCoordSysType)(OSInt)this->cboDest->GetItem((UOSInt)si);
			destCoord = Math::CoordinateSystemManager::CreateProjCoordinateSystemDefName(pcst);
		}
	}

	this->lvCoord->ClearItems();
	i = 0;
	j = this->nameList->GetCount();
	while (i < j)
	{
		k = this->lvCoord->AddItem(this->nameList->GetItem(i), 0);
		x = this->xList->GetItem(i);
		y = this->yList->GetItem(i);
		z = this->zList->GetItem(i);
		Text::StrDouble(sbuff, x);
		this->lvCoord->SetSubItem(k, 1, sbuff);
		Text::StrDouble(sbuff, y);
		this->lvCoord->SetSubItem(k, 2, sbuff);
		Text::StrDouble(sbuff, z);
		this->lvCoord->SetSubItem(k, 3, sbuff);
		if (srcCoord == 0 || destCoord == 0)
		{
		}
		else
		{
			Math::CoordinateSystem::ConvertXYZ(srcCoord, destCoord, x, y, z, &destX, &destY, &destZ);
			Text::StrDouble(sbuff, destX);
			this->lvCoord->SetSubItem(k, 4, sbuff);
			Text::StrDouble(sbuff, destY);
			this->lvCoord->SetSubItem(k, 5, sbuff);
			Text::StrDouble(sbuff, destZ);
			this->lvCoord->SetSubItem(k, 6, sbuff);
		}
		i++;
	}
	SDEL_CLASS(srcCoord);
	SDEL_CLASS(destCoord);
}

void SSWR::AVIRead::AVIRCoordConvForm::FillCoordGeo(UI::GUIComboBox *cbo)
{
	Text::StringBuilderUTF8 sb;
	cbo->ClearItems();
	Math::GeographicCoordinateSystem::GeoCoordSysType gcst = Math::GeographicCoordinateSystem::GCST_FIRST;
	while (gcst <= Math::GeographicCoordinateSystem::GCST_LAST)
	{
		sb.ClearStr();
		sb.Append(Math::GeographicCoordinateSystem::GetCoordinateSystemName(gcst));
		cbo->AddItem(sb.ToString(), (void*)(OSInt)gcst);
		gcst = (Math::GeographicCoordinateSystem::GeoCoordSysType)(gcst + 1);
	}
}

void SSWR::AVIRead::AVIRCoordConvForm::FillCoordProj(UI::GUIComboBox *cbo)
{
	Text::StringBuilderUTF8 sb;
	cbo->ClearItems();
	Math::ProjectedCoordinateSystem::ProjCoordSysType pcst = Math::ProjectedCoordinateSystem::PCST_FIRST;
	while (pcst <= Math::ProjectedCoordinateSystem::PCST_LAST)
	{
		sb.ClearStr();
		sb.Append(Math::ProjectedCoordinateSystem::GetCoordinateSystemName(pcst));
		cbo->AddItem(sb.ToString(), (void*)(OSInt)pcst);
		pcst = (Math::ProjectedCoordinateSystem::ProjCoordSysType)(pcst + 1);
	}
}

SSWR::AVIRead::AVIRCoordConvForm::AVIRCoordConvForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText((const UTF8Char*)"Coordinate Converter");
	this->SetFont(0, 8.25, false);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	NEW_CLASS(this->nameList, Data::ArrayList<const UTF8Char *>());
	NEW_CLASS(this->xList, Data::ArrayList<Double>());
	NEW_CLASS(this->yList, Data::ArrayList<Double>());
	NEW_CLASS(this->zList, Data::ArrayList<Double>());
	this->inited = false;

	NEW_CLASS(this->pnlSrc, UI::GUIPanel(ui, this));
	this->pnlSrc->SetRect(0, 0, 100, 24, false);
	this->pnlSrc->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblSrc, UI::GUILabel(ui, this->pnlSrc, (const UTF8Char*)"Source Type"));
	this->lblSrc->SetRect(0, 0, 100, 23, false);
	NEW_CLASS(this->radSrcGeo, UI::GUIRadioButton(ui, this->pnlSrc, (const UTF8Char*)"Geographic", false));
	this->radSrcGeo->SetRect(100, 0, 100, 23, false);
	this->radSrcGeo->HandleSelectedChange(OnSrcRadChanged, this);
	NEW_CLASS(this->radSrcProj, UI::GUIRadioButton(ui, this->pnlSrc, (const UTF8Char*)"Projected", true));
	this->radSrcProj->SetRect(200, 0, 100, 23, false);
	NEW_CLASS(this->cboSrc, UI::GUIComboBox(ui, this->pnlSrc, false));
	this->cboSrc->SetRect(300, 0, 200, 23, false);
	this->cboSrc->HandleSelectionChange(OnSrcCboChanged, this);
	NEW_CLASS(this->pnlDest, UI::GUIPanel(ui, this));
	this->pnlDest->SetRect(0, 0, 100, 24, false);
	this->pnlDest->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblDest, UI::GUILabel(ui, this->pnlDest, (const UTF8Char*)"Dest Type"));
	this->lblDest->SetRect(0, 0, 100, 23, false);
	NEW_CLASS(this->radDestGeo, UI::GUIRadioButton(ui, this->pnlDest, (const UTF8Char*)"Geographic", true));
	this->radDestGeo->SetRect(100, 0, 100, 23, false);
	this->radDestGeo->HandleSelectedChange(OnDestRadChanged, this);
	NEW_CLASS(this->radDestProj, UI::GUIRadioButton(ui, this->pnlDest, (const UTF8Char*)"Projected", false));
	this->radDestProj->SetRect(200, 0, 100, 23, false);
	NEW_CLASS(this->cboDest, UI::GUIComboBox(ui, this->pnlDest, false));
	this->cboDest->SetRect(300, 0, 200, 23, false);
	this->cboDest->HandleSelectionChange(OnDestCboChanged, this);
	NEW_CLASS(this->btnConvFile, UI::GUIButton(ui, this->pnlDest, (const UTF8Char*)"Conv File"));
	this->btnConvFile->SetRect(500, 0, 75, 23, false);
	this->btnConvFile->HandleButtonClick(OnConvFileClicked, this);
	NEW_CLASS(this->pnlCoord, UI::GUIPanel(ui, this));
	this->pnlCoord->SetRect(0, 0, 100, 24, false);
	this->pnlCoord->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblName, UI::GUILabel(ui, this->pnlCoord, (const UTF8Char*)"Name"));
	this->lblName->SetRect(0, 0, 100, 23, false);
	NEW_CLASS(this->txtName, UI::GUITextBox(ui, this->pnlCoord, (const UTF8Char*)""));
	this->txtName->SetRect(100, 0, 100, 23, false);
	NEW_CLASS(this->lblEasting, UI::GUILabel(ui, this->pnlCoord, (const UTF8Char*)"Easting"));
	this->lblEasting->SetRect(200, 0, 75, 23, false);
	NEW_CLASS(this->txtEasting, UI::GUITextBox(ui, this->pnlCoord, (const UTF8Char*)""));
	this->txtEasting->SetRect(275, 0, 100, 23, false);
	NEW_CLASS(this->lblNorthing, UI::GUILabel(ui, this->pnlCoord, (const UTF8Char*)"Northing"));
	this->lblNorthing->SetRect(375, 0, 75, 23, false);
	NEW_CLASS(this->txtNorthing, UI::GUITextBox(ui, this->pnlCoord, (const UTF8Char*)""));
	this->txtNorthing->SetRect(450, 0, 100, 23, false);
	NEW_CLASS(this->lblHeight, UI::GUILabel(ui, this->pnlCoord, (const UTF8Char*)"Northing"));
	this->lblHeight->SetRect(550, 0, 75, 23, false);
	NEW_CLASS(this->txtHeight, UI::GUITextBox(ui, this->pnlCoord, (const UTF8Char*)""));
	this->txtHeight->SetRect(625, 0, 75, 23, false);
	NEW_CLASS(this->btnAdd, UI::GUIButton(ui, this->pnlCoord, (const UTF8Char*)"Add"));
	this->btnAdd->SetRect(700, 0, 75, 23, false);
	this->btnAdd->HandleButtonClick(OnAddClicked, this);
	NEW_CLASS(this->btnClear, UI::GUIButton(ui, this->pnlCoord, (const UTF8Char*)"Clear"));
	this->btnClear->SetRect(780, 0, 75, 23, false);
	this->btnClear->HandleButtonClick(OnClearClicked, this);
	NEW_CLASS(this->lvCoord, UI::GUIListView(ui, this, UI::GUIListView::LVSTYLE_TABLE, 7));
	this->lvCoord->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvCoord->AddColumn((const UTF8Char*)"Name", 100);
	this->lvCoord->AddColumn((const UTF8Char*)"Easting", 100);
	this->lvCoord->AddColumn((const UTF8Char*)"Northing", 100);
	this->lvCoord->AddColumn((const UTF8Char*)"Height", 100);
	this->lvCoord->AddColumn((const UTF8Char*)"DestX", 100);
	this->lvCoord->AddColumn((const UTF8Char*)"DestY", 100);
	this->lvCoord->AddColumn((const UTF8Char*)"DestZ", 100);
	this->lvCoord->SetShowGrid(true);
	this->lvCoord->SetFullRowSelect(true);

	this->inited = true;
	this->FillCoordProj(this->cboSrc);
	this->FillCoordGeo(this->cboDest);
}

SSWR::AVIRead::AVIRCoordConvForm::~AVIRCoordConvForm()
{
	this->ClearItems();
	DEL_CLASS(this->nameList);
	DEL_CLASS(this->xList);
	DEL_CLASS(this->yList);
	DEL_CLASS(this->zList);
}

void SSWR::AVIRead::AVIRCoordConvForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
