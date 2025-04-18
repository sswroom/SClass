#include "Stdafx.h"
#include "DB/CSVFile.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/ProjectedCoordinateSystem.h"
#include "Media/ImageList.h"
#include "SSWR/AVIRead/AVIRCoordConvForm.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Writer.h"
#include "UI/Clipboard.h"
#include "UI/GUIFileDialog.h"

void __stdcall SSWR::AVIRead::AVIRCoordConvForm::OnSrcRadChanged(AnyType userObj, Bool newValue)
{
	NN<SSWR::AVIRead::AVIRCoordConvForm> me = userObj.GetNN<SSWR::AVIRead::AVIRCoordConvForm>();
	if (me->inited)
	{
		me->ClearItems(true);
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

void __stdcall SSWR::AVIRead::AVIRCoordConvForm::OnSrcCboChanged(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRCoordConvForm> me = userObj.GetNN<SSWR::AVIRead::AVIRCoordConvForm>();
	me->ClearItems(true);
}

void __stdcall SSWR::AVIRead::AVIRCoordConvForm::OnDestRadChanged(AnyType userObj, Bool newValue)
{
	NN<SSWR::AVIRead::AVIRCoordConvForm> me = userObj.GetNN<SSWR::AVIRead::AVIRCoordConvForm>();
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

void __stdcall SSWR::AVIRead::AVIRCoordConvForm::OnDestCboChanged(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRCoordConvForm> me = userObj.GetNN<SSWR::AVIRead::AVIRCoordConvForm>();
	me->UpdateList();
}

void __stdcall SSWR::AVIRead::AVIRCoordConvForm::OnAddClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRCoordConvForm> me = userObj.GetNN<SSWR::AVIRead::AVIRCoordConvForm>();
	Text::StringBuilderUTF8 sb;
	Double x;
	Double y;
	Double z;
	me->txtEasting->GetText(sb);
	if (!Text::StrToDouble(sb.ToString(), x))
		return;
	sb.ClearStr();
	me->txtNorthing->GetText(sb);
	if (!Text::StrToDouble(sb.ToString(), y))
		return;
	sb.ClearStr();
	me->txtHeight->GetText(sb);
	if (!Text::StrToDouble(sb.ToString(), z))
		return;
	sb.ClearStr();
	me->txtName->GetText(sb);
	if (sb.GetLength() <= 0)
		return;
	me->nameList.Add(Text::String::New(sb.ToCString()));
	me->xList.Add(x);
	me->yList.Add(y);
	me->zList.Add(z);
	me->UpdateList();
}

void __stdcall SSWR::AVIRead::AVIRCoordConvForm::OnClearClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRCoordConvForm> me = userObj.GetNN<SSWR::AVIRead::AVIRCoordConvForm>();
	me->ClearItems(true);
}

void __stdcall SSWR::AVIRead::AVIRCoordConvForm::OnCopyAllClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRCoordConvForm> me = userObj.GetNN<SSWR::AVIRead::AVIRCoordConvForm>();
	UOSInt i = 0;
	UOSInt j = me->lvCoord->GetCount();
	if (j == 0)
	{
		me->txtStatus->SetText(CSTR("No items found"));
	}
	Text::StringBuilderUTF8 sb;
	while (i < j)
	{
		if (i > 0)
		{
			sb.AppendC(UTF8STRC("\r\n"));
		}
		me->lvCoord->GetSubItem(i, 0, sb);
		sb.AppendUTF8Char('\t');
		me->lvCoord->GetSubItem(i, 1, sb);
		sb.AppendUTF8Char('\t');
		me->lvCoord->GetSubItem(i, 2, sb);
		sb.AppendUTF8Char('\t');
		me->lvCoord->GetSubItem(i, 3, sb);
		sb.AppendUTF8Char('\t');
		me->lvCoord->GetSubItem(i, 4, sb);
		sb.AppendUTF8Char('\t');
		me->lvCoord->GetSubItem(i, 5, sb);
		sb.AppendUTF8Char('\t');
		me->lvCoord->GetSubItem(i, 6, sb);
		i++;
	}
	if (UI::Clipboard::SetString(me->GetHandle(), sb.ToCString()))
	{
		me->txtStatus->SetText(CSTR("All items copied"));
	}
	else
	{
		me->txtStatus->SetText(CSTR("Error in copying items"));
	}
}

void __stdcall SSWR::AVIRead::AVIRCoordConvForm::OnConvFileClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRCoordConvForm> me = userObj.GetNN<SSWR::AVIRead::AVIRCoordConvForm>();
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> strBuff;
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> sptr2;
	NN<Parser::ParserList> parsers = me->core->GetParserList();
	Optional<Math::CoordinateSystem> srcCoord;
	Optional<Math::CoordinateSystem> destCoord;
	NN<Math::CoordinateSystem> srcCsys;
	NN<Math::CoordinateSystem> destCsys;
	UOSInt i;

	i = me->cboSrc->GetSelectedIndex();
	if (i == INVALID_INDEX)
	{
		me->ui->ShowMsgOK(CSTR("Please select source coordinate system"), CSTR("Error"), me);
		return;
	}

	i = me->cboDest->GetSelectedIndex();
	if (i == INVALID_INDEX)
	{
		me->ui->ShowMsgOK(CSTR("Please select dest coordinate system"), CSTR("Error"), me);
		return;
	}

	UOSInt xCol = (UOSInt)-1;
	UOSInt yCol = (UOSInt)-1;
	UOSInt colCnt;
	Optional<DB::ReadingDB> db = 0;
	NN<DB::ReadingDB> nndb;
	NN<UI::GUIFileDialog> dlg = me->ui->NewFileDialog(L"SSWR", L"AVIRead", L"CoordConvFile", false);
	parsers->PrepareSelector(dlg, IO::ParserType::ReadingDB);
	if (!dlg->ShowDialog(me->GetHandle()))
	{
		return;
	}
	if (dlg->GetFileName()->EndsWithICase(UTF8STRC(".CSV")))
	{
		DB::CSVFile *csv;
		NEW_CLASS(csv, DB::CSVFile(dlg->GetFileName(), 0));
		db = csv;
	}
	if (!db.SetTo(nndb))
	{
		IO::StmData::FileData fd(dlg->GetFileName(), false);
		db = Optional<DB::ReadingDB>::ConvertFrom(parsers->ParseFileType(fd, IO::ParserType::ReadingDB));
		if (!db.SetTo(nndb))
		{
			me->ui->ShowMsgOK(CSTR("File is not a database file"), CSTR("Error"), me);
			dlg.Delete();
			return;
		}
	}
	NN<DB::DBReader> reader;
	if (!nndb->QueryTableData(CSTR_NULL, CSTR(""), 0, 0, 0, CSTR_NULL, 0).SetTo(reader))
	{
		me->ui->ShowMsgOK(CSTR("Unsupported database format"), CSTR("Error"), me);
		nndb.Delete();
		dlg.Delete();
		return;
	}
	i = reader->ColCount();
	colCnt = i;
	while (i-- > 0)
	{
		if (reader->GetName(i, sbuff).SetTo(sptr))
		{
			if (Text::StrEqualsICaseC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("MAPX")))
			{
				xCol = i;
			}
			else if (Text::StrEqualsICaseC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("MAPY")))
			{
				yCol = i;
			}
		}
	}
	nndb->CloseReader(reader);
	if (xCol == (UOSInt)-1 || yCol == (UOSInt)-1)
	{
		nndb.Delete();
		me->ui->ShowMsgOK(CSTR("XY Database column not found"), CSTR("Error"), me);
		dlg.Delete();
		return;
	}
	dlg.Delete();

	dlg = me->ui->NewFileDialog(L"SSWR", L"AVIRead", L"CoordConvSave", true);
	dlg->AddFilter(CSTR("*.csv"), CSTR("CSV File"));
	if (!dlg->ShowDialog(me->GetHandle()))
	{
		nndb.Delete();
		dlg.Delete();
		return;
	}
	IO::FileStream fs(dlg->GetFileName(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (fs.IsError())
	{
		nndb.Delete();
		me->ui->ShowMsgOK(CSTR("Error in creating output file"), CSTR("Error"), me);
		dlg.Delete();
		return;
	}
	dlg.Delete();
	Text::StringBuilderUTF8 sb;
	UnsafeArray<UnsafeArrayOpt<const UTF8Char>> sarr;
	sarr = MemAllocArr(UnsafeArrayOpt<const UTF8Char>, colCnt + 2);
	i = 0;
	if (!nndb->QueryTableData(CSTR_NULL, CSTR(""), 0, 0, 0, CSTR_NULL, 0).SetTo(reader))
	{
		MemFreeArr(sarr);
		nndb.Delete();
		me->ui->ShowMsgOK(CSTR("Error in reading source file"), CSTR("Error"), me);
		return;
	}

	if (me->radSrcGeo->IsSelected())
	{
		Math::CoordinateSystemManager::GeoCoordSysType gcst = (Math::CoordinateSystemManager::GeoCoordSysType)me->cboSrc->GetItem(i).GetOSInt();
		srcCoord = Math::CoordinateSystemManager::CreateGeogCoordinateSystemDefName(gcst);
	}
	else
	{
		Math::CoordinateSystemManager::ProjCoordSysType pcst = (Math::CoordinateSystemManager::ProjCoordSysType)me->cboSrc->GetItem(i).GetOSInt();
		srcCoord = Math::CoordinateSystemManager::CreateProjCoordinateSystemDefName(pcst);
	}

	if (me->radDestGeo->IsSelected())
	{
		Math::CoordinateSystemManager::GeoCoordSysType gcst = (Math::CoordinateSystemManager::GeoCoordSysType)me->cboDest->GetItem(i).GetOSInt();
		destCoord = Math::CoordinateSystemManager::CreateGeogCoordinateSystemDefName(gcst);
	}
	else
	{
		Math::CoordinateSystemManager::ProjCoordSysType pcst = (Math::CoordinateSystemManager::ProjCoordSysType)me->cboDest->GetItem(i).GetOSInt();
		destCoord = Math::CoordinateSystemManager::CreateProjCoordinateSystemDefName(pcst);
	}

	strBuff = MemAlloc(UTF8Char, 16384);
	Text::UTF8Writer writer(fs);
	sptr = strBuff;
	i = 0;
	while (i < colCnt)
	{
		sarr[i] = UnsafeArray<const UTF8Char>(sptr);
		if (reader->GetName(i, sptr).SetTo(sptr2))
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
	writer.WriteLine(sb.ToCString());

	Double inX;
	Double inY;
	Math::Coord2DDbl outPos;
	while (reader->ReadNext())
	{
		sptr = strBuff;
		i = 0;
		while (i < colCnt)
		{
			sarr[i] = UnsafeArray<const UTF8Char>(sptr);
			if (reader->GetStr(i, sptr, 16384 - (UOSInt)(sptr - strBuff)).SetTo(sptr2))
			{
				sptr = sptr2 + 1;
			}
			else
			{
				sarr[i] = (const UTF8Char*)"";
			}
			i++;
		}

		if (srcCoord.SetTo(srcCsys) && destCoord.SetTo(destCsys) && Text::StrToDouble(sarr[xCol].Ptr(), inX) && Text::StrToDouble(sarr[yCol].Ptr(), inY))
		{
			outPos = Math::CoordinateSystem::Convert(srcCsys, destCsys, Math::Coord2DDbl(inX, inY));
			sarr[colCnt] = UnsafeArray<const UTF8Char>(sptr);
			sptr = Text::StrDouble(sptr, outPos.x) + 1;
			sarr[colCnt + 1] = UnsafeArray<const UTF8Char>(sptr);
			sptr = Text::StrDouble(sptr, outPos.y) + 1;
		}
		else
		{
			sarr[colCnt] = (const UTF8Char*)"";
			sarr[colCnt + 1] = (const UTF8Char*)"";
		}
		sb.ClearStr();
		sb.AppendCSV(sarr, colCnt + 2);
		writer.WriteLine(sb.ToCString());
	}
	nndb->CloseReader(reader);

	MemFreeArr(strBuff);
	MemFreeArr(sarr);
	srcCoord.Delete();
	destCoord.Delete();
	nndb.Delete();
}

void __stdcall SSWR::AVIRead::AVIRCoordConvForm::OnCoordDblClk(AnyType userObj, UOSInt itemIndex)
{
	NN<SSWR::AVIRead::AVIRCoordConvForm> me = userObj.GetNN<SSWR::AVIRead::AVIRCoordConvForm>();
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sb2;
	me->lvCoord->GetSubItem(itemIndex, 0, sb);
	sb2.Append(sb);
	sb.AppendUTF8Char('\t');
	me->lvCoord->GetSubItem(itemIndex, 1, sb);
	sb.AppendUTF8Char('\t');
	me->lvCoord->GetSubItem(itemIndex, 2, sb);
	sb.AppendUTF8Char('\t');
	me->lvCoord->GetSubItem(itemIndex, 3, sb);
	sb.AppendUTF8Char('\t');
	me->lvCoord->GetSubItem(itemIndex, 4, sb);
	sb.AppendUTF8Char('\t');
	me->lvCoord->GetSubItem(itemIndex, 5, sb);
	sb.AppendUTF8Char('\t');
	me->lvCoord->GetSubItem(itemIndex, 6, sb);
	sb2.AppendC(UTF8STRC(" copied"));
	if (UI::Clipboard::SetString(me->GetHandle(), sb.ToCString()))
	{
		me->txtStatus->SetText(sb2.ToCString());
	}
}

void __stdcall SSWR::AVIRead::AVIRCoordConvForm::OnFileDrop(AnyType userObj, Data::DataArray<NN<Text::String>> files)
{
	NN<SSWR::AVIRead::AVIRCoordConvForm> me = userObj.GetNN<SSWR::AVIRead::AVIRCoordConvForm>();
	NN<Parser::ParserList> parsers = me->core->GetParserList();
	Bool listUpdated = false;
	Text::StringBuilderUTF8 sb;
	Double lat;
	Double lon;
	Double altitude;
	Int64 gpsTimeTick;
	UOSInt i = 0;
	UOSInt nFiles = files.GetCount();
	UOSInt j;
	while (i < nFiles)
	{
		IO::StmData::FileData fd(files[i], false);
		NN<Media::ImageList> imgList;
		if (Optional<Media::ImageList>::ConvertFrom(parsers->ParseFileType(fd, IO::ParserType::ImageList)).SetTo(imgList))
		{
			NN<Media::RasterImage> img;
			NN<Media::EXIFData> exif;
			if (imgList->GetImage(0, 0).SetTo(img) && img->exif.SetTo(exif) && exif->GetPhotoLocation(lat, lon, altitude, gpsTimeTick))
			{
				sb.ClearStr();
				sb.Append(imgList->GetSourceNameObj());
				j = sb.LastIndexOf(IO::Path::PATH_SEPERATOR);
				if (j == INVALID_INDEX)
				{
					me->nameList.Add(imgList->GetSourceNameObj()->Clone());
				}
				else
				{
					me->nameList.Add(Text::String::New(sb.ToCString().Substring(j + 1)));
				}
				me->xList.Add(lon);
				me->yList.Add(lat);
				me->zList.Add(altitude);
				listUpdated = true;
			}
			imgList.Delete();
		}
		i++;
	}
	if (listUpdated)
	{
		me->UpdateList();
	}
}

void SSWR::AVIRead::AVIRCoordConvForm::ClearItems(Bool updateList)
{
	this->nameList.FreeAll();
	this->xList.Clear();
	this->yList.Clear();
	this->zList.Clear();
	if (updateList) this->UpdateList();
}

void SSWR::AVIRead::AVIRCoordConvForm::UpdateList()
{
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	Double x;
	Double y;
	Double z;
	Math::Vector3 destPos;
	Optional<Math::CoordinateSystem> srcCoord;
	Optional<Math::CoordinateSystem> destCoord;
	NN<Math::CoordinateSystem> srcCsys;
	NN<Math::CoordinateSystem> destCsys;

	i = this->cboSrc->GetSelectedIndex();
	if (i == INVALID_INDEX)
	{
		srcCoord = 0;
	}
	else
	{
		if (this->radSrcGeo->IsSelected())
		{
			Math::CoordinateSystemManager::GeoCoordSysType gcst = (Math::CoordinateSystemManager::GeoCoordSysType)this->cboSrc->GetItem(i).GetOSInt();
			srcCoord = Math::CoordinateSystemManager::CreateGeogCoordinateSystemDefName(gcst);
		}
		else
		{
			Math::CoordinateSystemManager::ProjCoordSysType pcst = (Math::CoordinateSystemManager::ProjCoordSysType)this->cboSrc->GetItem(i).GetOSInt();
			srcCoord = Math::CoordinateSystemManager::CreateProjCoordinateSystemDefName(pcst);
		}
	}

	i = this->cboDest->GetSelectedIndex();
	if (i == INVALID_INDEX)
	{
		destCoord = 0;
	}
	else
	{
		if (this->radDestGeo->IsSelected())
		{
			Math::CoordinateSystemManager::GeoCoordSysType gcst = (Math::CoordinateSystemManager::GeoCoordSysType)this->cboDest->GetItem(i).GetOSInt();
			destCoord = Math::CoordinateSystemManager::CreateGeogCoordinateSystemDefName(gcst);
		}
		else
		{
			Math::CoordinateSystemManager::ProjCoordSysType pcst = (Math::CoordinateSystemManager::ProjCoordSysType)this->cboDest->GetItem(i).GetOSInt();
			destCoord = Math::CoordinateSystemManager::CreateProjCoordinateSystemDefName(pcst);
		}
	}

	this->lvCoord->ClearItems();
	i = 0;
	j = this->nameList.GetCount();
	while (i < j)
	{
		k = this->lvCoord->AddItem(Text::String::OrEmpty(this->nameList.GetItem(i))->ToCString(), 0);
		x = this->xList.GetItem(i);
		y = this->yList.GetItem(i);
		z = this->zList.GetItem(i);
		sptr = Text::StrDouble(sbuff, x);
		this->lvCoord->SetSubItem(k, 1, CSTRP(sbuff, sptr));
		sptr = Text::StrDouble(sbuff, y);
		this->lvCoord->SetSubItem(k, 2, CSTRP(sbuff, sptr));
		sptr = Text::StrDouble(sbuff, z);
		this->lvCoord->SetSubItem(k, 3, CSTRP(sbuff, sptr));
		if (srcCoord.SetTo(srcCsys) && destCoord.SetTo(destCsys))
		{
			destPos = Math::CoordinateSystem::Convert3D(srcCsys, destCsys, Math::Vector3(x, y, z));
			sptr = Text::StrDouble(sbuff, destPos.GetX());
			this->lvCoord->SetSubItem(k, 4, CSTRP(sbuff, sptr));
			sptr = Text::StrDouble(sbuff, destPos.GetY());
			this->lvCoord->SetSubItem(k, 5, CSTRP(sbuff, sptr));
			sptr = Text::StrDouble(sbuff, destPos.GetZ());
			this->lvCoord->SetSubItem(k, 6, CSTRP(sbuff, sptr));
		}
		i++;
	}
	srcCoord.Delete();
	destCoord.Delete();
}

void SSWR::AVIRead::AVIRCoordConvForm::FillCoordGeo(NN<UI::GUIComboBox> cbo)
{
	cbo->ClearItems();
	Math::CoordinateSystemManager::GeoCoordSysType gcst = Math::CoordinateSystemManager::GCST_FIRST;
	while (gcst <= Math::CoordinateSystemManager::GCST_LAST)
	{
		cbo->AddItem(Math::CoordinateSystemManager::GeoCoordSysTypeGetName(gcst), (void*)(OSInt)gcst);
		gcst = (Math::CoordinateSystemManager::GeoCoordSysType)(gcst + 1);
	}
}

void SSWR::AVIRead::AVIRCoordConvForm::FillCoordProj(NN<UI::GUIComboBox> cbo)
{
	cbo->ClearItems();
	Math::CoordinateSystemManager::ProjCoordSysType pcst = Math::CoordinateSystemManager::PCST_FIRST;
	while (pcst <= Math::CoordinateSystemManager::PCST_LAST)
	{
		cbo->AddItem(Math::CoordinateSystemManager::ProjCoordSysTypeGetName(pcst).OrEmpty(), (void*)(OSInt)pcst);
		pcst = (Math::CoordinateSystemManager::ProjCoordSysType)(pcst + 1);
	}
}

SSWR::AVIRead::AVIRCoordConvForm::AVIRCoordConvForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("Coordinate Converter"));
	this->SetFont(0, 0, 8.25, false);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->inited = false;

	this->pnlSrc = ui->NewPanel(*this);
	this->pnlSrc->SetRect(0, 0, 100, 24, false);
	this->pnlSrc->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblSrc = ui->NewLabel(this->pnlSrc, CSTR("Source Type"));
	this->lblSrc->SetRect(0, 0, 100, 23, false);
	this->radSrcGeo = ui->NewRadioButton(this->pnlSrc, CSTR("Geographic"), false);
	this->radSrcGeo->SetRect(100, 0, 100, 23, false);
	this->radSrcGeo->HandleSelectedChange(OnSrcRadChanged, this);
	this->radSrcProj = ui->NewRadioButton(this->pnlSrc, CSTR("Projected"), true);
	this->radSrcProj->SetRect(200, 0, 100, 23, false);
	this->cboSrc = ui->NewComboBox(this->pnlSrc, false);
	this->cboSrc->SetRect(300, 0, 200, 23, false);
	this->cboSrc->HandleSelectionChange(OnSrcCboChanged, this);
	this->pnlDest = ui->NewPanel(*this);
	this->pnlDest->SetRect(0, 0, 100, 24, false);
	this->pnlDest->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblDest = ui->NewLabel(this->pnlDest, CSTR("Dest Type"));
	this->lblDest->SetRect(0, 0, 100, 23, false);
	this->radDestGeo = ui->NewRadioButton(this->pnlDest, CSTR("Geographic"), true);
	this->radDestGeo->SetRect(100, 0, 100, 23, false);
	this->radDestGeo->HandleSelectedChange(OnDestRadChanged, this);
	this->radDestProj = ui->NewRadioButton(this->pnlDest, CSTR("Projected"), false);
	this->radDestProj->SetRect(200, 0, 100, 23, false);
	this->cboDest = ui->NewComboBox(this->pnlDest, false);
	this->cboDest->SetRect(300, 0, 200, 23, false);
	this->cboDest->HandleSelectionChange(OnDestCboChanged, this);
	this->btnConvFile = ui->NewButton(this->pnlDest, CSTR("Conv File"));
	this->btnConvFile->SetRect(500, 0, 75, 23, false);
	this->btnConvFile->HandleButtonClick(OnConvFileClicked, this);
	this->pnlCoord = ui->NewPanel(*this);
	this->pnlCoord->SetRect(0, 0, 100, 24, false);
	this->pnlCoord->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblName = ui->NewLabel(this->pnlCoord, CSTR("Name"));
	this->lblName->SetRect(0, 0, 100, 23, false);
	this->txtName = ui->NewTextBox(this->pnlCoord, CSTR(""));
	this->txtName->SetRect(100, 0, 100, 23, false);
	this->lblEasting = ui->NewLabel(this->pnlCoord, CSTR("Easting"));
	this->lblEasting->SetRect(200, 0, 75, 23, false);
	this->txtEasting = ui->NewTextBox(this->pnlCoord, CSTR(""));
	this->txtEasting->SetRect(275, 0, 100, 23, false);
	this->lblNorthing = ui->NewLabel(this->pnlCoord, CSTR("Northing"));
	this->lblNorthing->SetRect(375, 0, 75, 23, false);
	this->txtNorthing = ui->NewTextBox(this->pnlCoord, CSTR(""));
	this->txtNorthing->SetRect(450, 0, 100, 23, false);
	this->lblHeight = ui->NewLabel(this->pnlCoord, CSTR("Height"));
	this->lblHeight->SetRect(550, 0, 75, 23, false);
	this->txtHeight = ui->NewTextBox(this->pnlCoord, CSTR(""));
	this->txtHeight->SetRect(625, 0, 75, 23, false);
	this->btnAdd = ui->NewButton(this->pnlCoord, CSTR("Add"));
	this->btnAdd->SetRect(700, 0, 75, 23, false);
	this->btnAdd->HandleButtonClick(OnAddClicked, this);
	this->btnClear = ui->NewButton(this->pnlCoord, CSTR("Clear"));
	this->btnClear->SetRect(780, 0, 75, 23, false);
	this->btnClear->HandleButtonClick(OnClearClicked, this);
	this->btnCopyAll = ui->NewButton(this->pnlCoord, CSTR("Copy All"));
	this->btnCopyAll->SetRect(860, 0, 75, 23, false);
	this->btnCopyAll->HandleButtonClick(OnCopyAllClicked, this);
	this->txtStatus = ui->NewTextBox(*this, CSTR(""));
	this->txtStatus->SetRect(0, 0, 100, 23, false);
	this->txtStatus->SetReadOnly(true);
	this->txtStatus->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->lvCoord = ui->NewListView(*this, UI::ListViewStyle::Table, 7);
	this->lvCoord->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvCoord->AddColumn(CSTR("Name"), 100);
	this->lvCoord->AddColumn(CSTR("Easting"), 100);
	this->lvCoord->AddColumn(CSTR("Northing"), 100);
	this->lvCoord->AddColumn(CSTR("Height"), 100);
	this->lvCoord->AddColumn(CSTR("DestX"), 100);
	this->lvCoord->AddColumn(CSTR("DestY"), 100);
	this->lvCoord->AddColumn(CSTR("DestZ"), 100);
	this->lvCoord->SetShowGrid(true);
	this->lvCoord->SetFullRowSelect(true);
	this->lvCoord->HandleDblClk(OnCoordDblClk, this);
	this->HandleDropFiles(OnFileDrop, this);

	this->inited = true;
	this->FillCoordProj(this->cboSrc);
	this->FillCoordGeo(this->cboDest);
}

SSWR::AVIRead::AVIRCoordConvForm::~AVIRCoordConvForm()
{
	this->ClearItems(false);
}

void SSWR::AVIRead::AVIRCoordConvForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
