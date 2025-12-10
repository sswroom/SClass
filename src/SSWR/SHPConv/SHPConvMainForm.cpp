#include "Stdafx.h"
#include "Data/ArrayListStrUTF8.h"
#include "Core/ByteTool_C.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "DB/DBFFile.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Math/Math_C.h"
#include "SSWR/SHPConv/SHPConvCurrFilterForm.h"
#include "SSWR/SHPConv/SHPConvDBFViewForm.h"
#include "SSWR/SHPConv/SHPConvGroupForm.h"
#include "SSWR/SHPConv/SHPConvMainForm.h"
#include "SSWR/SHPConv/ValueFilter.h"
#include "Text/EncodingFactory.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/StringBuilderUTF16.h"
#include "UI/GUIFileDialog.h"
#define LATSCALE 200000

Text::CStringNN SSWR::SHPConv::SHPConvMainForm::typeName[] = {
	CSTR("Null Shape"),
	CSTR("Point"),
	CSTR(""),
	CSTR("PolyLine"),
	CSTR(""),
	CSTR("Polygon"),
	CSTR(""),
	CSTR(""),
	CSTR("MultiPoint"),
	CSTR(""),
	CSTR(""),
	CSTR("PointZ"),
	CSTR(""),
	CSTR("PolyLineZ"),
	CSTR(""),
	CSTR("PolygonZ"),
	CSTR(""),
	CSTR(""),
	CSTR("MultiPointZ"),
	CSTR(""),
	CSTR(""),
	CSTR("PointM"),
	CSTR(""),
	CSTR("PolyLineM"),
	CSTR(""),
	CSTR("PolygonM"),
	CSTR(""),
	CSTR(""),
	CSTR("MultiPointM"),
	CSTR(""),
	CSTR(""),
	CSTR("MultiPatch")};

void __stdcall SSWR::SHPConv::SHPConvMainForm::OnDirectoryClicked(AnyType userObj)
{
//	NN<SSWR::SHPConv::SHPConvMainForm> me = userObj.GetNN<SSWR::SHPConv::SHPConvMainForm>();
/*
        If Me.txtOutput.Text <> "" Then
            Dim frm As New frmDirectory(Me.txtOutput.Text, Me)
            frm.ShowDialog(Me)
        End If*/
}

void __stdcall SSWR::SHPConv::SHPConvMainForm::OnSBrowseClicked(AnyType userObj)
{
	NN<SSWR::SHPConv::SHPConvMainForm> me = userObj.GetNN<SSWR::SHPConv::SHPConvMainForm>();
	NN<UI::GUIFileDialog> ofd = me->ui->NewFileDialog(L"SSWR", L"SHPConv", L"Source", false);
	ofd->AddFilter(CSTR("*.shp"), CSTR("Shape File"));
	if (ofd->ShowDialog(me->GetHandle()))
	{
		UTF8Char sbuff[16];
		UnsafeArray<UTF8Char> sptr;
		sptr = Text::StrInt32(sbuff, me->LoadShape(ofd->GetFileName()->ToCString(), true));
		me->txtBlkScale->SetText(CSTRP(sbuff, sptr));
	}
	ofd.Delete();
}

void __stdcall SSWR::SHPConv::SHPConvMainForm::OnLangSelChg(AnyType userObj)
{
	NN<SSWR::SHPConv::SHPConvMainForm> me = userObj.GetNN<SSWR::SHPConv::SHPConvMainForm>();
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	sptr = Text::StrOSInt(sbuff, (OSInt)me->lstLang->GetSelectedItem().p);
	me->txtCodePage->SetText(CSTRP(sbuff, sptr));
}

void __stdcall SSWR::SHPConv::SHPConvMainForm::OnRecordsSelChg(AnyType userObj)
{
	NN<SSWR::SHPConv::SHPConvMainForm> me = userObj.GetNN<SSWR::SHPConv::SHPConvMainForm>();
	Data::ArrayList<UInt32> indices;
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	me->lstRecords->GetSelectedIndices(indices);
	UOSInt i;
	UOSInt j;
	if (indices.GetCount() <= 0)
	{
		me->txtLabel->SetText(CSTR(""));
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		me->cboSeperator->GetText(sb);

		sptr = Text::StrConcatC(sbuff, UTF8STRC("<%="));
		sptr = me->lstRecords->GetItemText(sptr, indices.GetItem(0)).Or(sptr);
		sptr = Text::StrConcatC(sptr, UTF8STRC("%>"));
		i = 1;
		j = indices.GetCount();
		while (i < j)
		{
			sptr = Text::StrConcatC(sptr, sb.ToString(), sb.GetLength());
			sptr = Text::StrConcatC(sptr, UTF8STRC("<%="));
			sptr = me->lstRecords->GetItemText(sptr, indices.GetItem(i)).Or(sptr);
			sptr = Text::StrConcatC(sptr, UTF8STRC("%>"));
			i++;
		}
		me->txtLabel->SetText(CSTRP(sbuff, sptr));
	}
}

void __stdcall SSWR::SHPConv::SHPConvMainForm::OnGroupClicked(AnyType userObj)
{
	NN<SSWR::SHPConv::SHPConvMainForm> me = userObj.GetNN<SSWR::SHPConv::SHPConvMainForm>();
	NN<Text::String> s;
	UOSInt i;
	UOSInt j;
	SSWR::SHPConv::SHPConvGroupForm frm(0, me->ui);
	frm.AddGroup(CSTR("-- None --"));
	i = 0;
	j = me->lstRecords->GetCount();
	while (i < j)
	{
		if (me->lstRecords->GetItemTextNew(i).SetTo(s))
		{
			frm.AddGroup(s->ToCString());
			s->Release();
		}
		i++;
	}
	frm.SetCurrGroup(me->currGroup);
	if (frm.ShowDialog(me) == UI::GUIForm::DR_OK)
	{
		me->currGroup = frm.GetCurrGroup();
	}
}

void __stdcall SSWR::SHPConv::SHPConvMainForm::OnFilterClicked(AnyType userObj)
{
	NN<SSWR::SHPConv::SHPConvMainForm> me = userObj.GetNN<SSWR::SHPConv::SHPConvMainForm>();
	Text::StringBuilderUTF8 sb;
	me->txtSource->GetText(sb);
	if (sb.GetLength() <= 0)
	{
		return;
	}
	IO::StmData::FileData fd(sb.ToCString(), false);
	DB::DBFFile dbf(fd, (UInt32)(UOSInt)me->lstLang->GetSelectedItem().p);
	if (!dbf.IsError())
	{
		SSWR::SHPConv::SHPConvCurrFilterForm frm(0, me->ui, dbf, me->globalFilters, me->deng);
		frm.ShowDialog(me);
	}
}

void __stdcall SSWR::SHPConv::SHPConvMainForm::OnPreviewClicked(AnyType userObj)
{
	NN<SSWR::SHPConv::SHPConvMainForm> me = userObj.GetNN<SSWR::SHPConv::SHPConvMainForm>();
	Text::StringBuilderUTF8 sb;
	me->txtSource->GetText(sb);
	UOSInt i = sb.LastIndexOf('.');
	sb.TrimToLength(i + 1);
	sb.AppendC(UTF8STRC("dbf"));
	IO::StmData::FileData fd(sb.ToCString(), false);
	if (fd.GetDataSize() > 0)
	{
		DB::DBFFile dbf(fd, (UInt32)(UOSInt)me->lstLang->GetSelectedItem().p);
		if (!dbf.IsError())
		{
			sb.ClearStr();
			me->txtLabel->GetText(sb);
			SSWR::SHPConv::SHPConvDBFViewForm frm(0, me->ui, &dbf, me.Ptr(), sb.ToCString());
			frm.ShowDialog(me);
		}
	}
}

void __stdcall SSWR::SHPConv::SHPConvMainForm::OnConvertClicked(AnyType userObj)
{
	NN<SSWR::SHPConv::SHPConvMainForm> me = userObj.GetNN<SSWR::SHPConv::SHPConvMainForm>();
	Text::StringBuilderUTF8 sb;
	Int32 blkScale;
	me->txtBlkScale->GetText(sb);
	if (!sb.ToInt32(blkScale))
	{
		me->ui->ShowMsgOK(CSTR("Block scale is not integer"), CSTR("Error"), me);
		return;
	}

	me->btnConvert->SetEnabled(false);
	me->btnDirectory->SetEnabled(false);
	me->btnFilter->SetEnabled(false);
	me->btnGroup->SetEnabled(false);
	me->btnPreview->SetEnabled(false);
	me->btnSBrowse->SetEnabled(false);

	Data::ArrayList<const UTF8Char*> dbCols;
	Data::ArrayList<UInt32> dbCols2;
	sb.ClearStr();
	me->txtLabel->GetText(sb);
	me->ParseLabelStr(sb.ToCString(), &dbCols, &dbCols2);
	sb.ClearStr();
	me->txtSource->GetText(sb);
	NN<Text::String> srcFile = Text::String::New(sb.ToString(), sb.GetLength());
	sb.RemoveChars(4);
	if (me->currGroup == (UOSInt)-1)
	{
		me->ConvertShp(srcFile->ToCString(), sb.ToCString(), &dbCols, blkScale, me->globalFilters, me.Ptr(), dbCols2);
	}
	else
	{
		me->GroupConvert(srcFile->ToCString(), sb.ToCString(), &dbCols, blkScale, me->globalFilters, me.Ptr(), me->currGroup, 0, dbCols2);
	}
	srcFile->Release();
	me->FreeLabelStr(&dbCols, &dbCols2);

	me->btnConvert->SetEnabled(true);
	me->btnDirectory->SetEnabled(true);
	me->btnFilter->SetEnabled(true);
	me->btnGroup->SetEnabled(true);
	me->btnPreview->SetEnabled(true);
	me->btnSBrowse->SetEnabled(true);
}

void __stdcall SSWR::SHPConv::SHPConvMainForm::OnFile(AnyType userObj, Data::DataArray<NN<Text::String>> files)
{
	NN<SSWR::SHPConv::SHPConvMainForm> me = userObj.GetNN<SSWR::SHPConv::SHPConvMainForm>();
	UTF8Char sbuff[16];
	UnsafeArray<UTF8Char> sptr;
	sptr = Text::StrInt32(sbuff, me->LoadShape(files[0]->ToCString(), true));
	me->txtBlkScale->SetText(CSTRP(sbuff, sptr));
}

Int32 SSWR::SHPConv::SHPConvMainForm::GroupConvert(Text::CStringNN sourceFile, Text::CString outFilePrefix, Data::ArrayList<const UTF8Char*> *dbCols, Int32 blkScale, NN<Data::ArrayListNN<MapFilter>> filters, IO::ProgressHandler *progress, UOSInt groupCol, Data::ArrayList<const UTF8Char*> *outNames, NN<Data::ArrayList<UInt32>> dbCols2)
{
	UOSInt i;
	OSInt si;
	Text::StringBuilderUTF8 sb;
	Data::ArrayListStringNN names;
	Int32 shpType = 0;
	NN<DB::DBReader> r;
	NN<Text::String> s;

	sb.Append(sourceFile);
	i = sb.LastIndexOf('.');
	sb.RemoveChars(sb.GetLength() - i - 1);
	sb.AppendC(UTF8STRC("dbf"));
	{
		IO::StmData::FileData fd(sb.ToCString(), false);
		DB::DBFFile dbf(fd, (UInt32)(UOSInt)this->lstLang->GetSelectedItem().p);
		if (dbf.QueryTableData(nullptr, CSTR(""), 0, 0, 0, nullptr, 0).SetTo(r))
		{
			while (r->ReadNext())
			{
				sb.ClearStr();
				r->GetStr(groupCol, sb);
				si = names.SortedIndexOfC(sb.ToCString());
				if (si < 0)
				{
					names.Insert((UOSInt)~si, Text::String::New(sb.ToCString()));
				}
			}
			dbf.CloseReader(r);
		}
	}

	Text::StringBuilderUTF8 sb2;
	Data::ArrayListNN<MapFilter> newFilters;
	newFilters.AddAll(filters);
	i = names.GetCount();
	while (i-- > 0)
	{
		s = Text::String::OrEmpty(names.GetItem(i));

		sb.ClearStr();
		sb.Append(s);
		sb.Replace('/', ' ');
		sb.Replace('&', ' ');
		sb.Replace('-', ' ');
		sb.ReplaceStr(UTF8STRC("  "), UTF8STRC(" "));
		sb.Replace(' ', '_');

		{
			SSWR::SHPConv::ValueFilter filter(groupCol, s->ToCString(), 3);
			newFilters.Add(filter);
			sb2.ClearStr();
			sb2.AppendOpt(outFilePrefix);
			sb2.AppendUTF8Char('_');
			sb2.AppendC(sb.ToString(), sb.GetLength());
			shpType = this->ConvertShp(sourceFile, sb2.ToCString(), dbCols, blkScale, newFilters, progress, dbCols2);
			newFilters.RemoveAt(newFilters.GetCount() - 1);
		}
		if (outNames)
		{
			outNames->Add(Text::StrCopyNew(sb2.ToString()).Ptr());
		}
		s->Release();
	}
	return shpType;
}

Int32 SSWR::SHPConv::SHPConvMainForm::ConvertShp(Text::CStringNN sourceFile, Text::CString outFilePrefix, Data::ArrayList<const UTF8Char*> *dbCols, Int32 blkScale, NN<Data::ArrayListNN<MapFilter>> filters, IO::ProgressHandler *progress, NN<Data::ArrayList<UInt32>> dbCols2)
{
	Text::StringBuilderUTF8 sb;
	UInt8 buff[259];
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;
	OSInt si;
	OSInt sj;
	OSInt sk;
	OSInt sl;
	OSInt sm;
	UInt32 cipPos;
	Optional<DB::DBReader> dbfr;
	NN<DB::DBReader> r;
	Data::ArrayList<Block*> blks;
	Int32 currRec;
	UInt32 nRecords;
	UInt64 fileLeng;
	UInt64 filePos;
	Int32 shpType;
	UInt32 nParts;
	UInt32 nPoints;
	Double xMin;
	Double yMin;
	Double xMax;
	Double yMax;
	Double currX;
	Double currY;

	Int32 left;
	Int32 top;
	Int32 right;
	Int32 bottom;

	UInt8 *outBuff;
	Block *theBlk;
//	Dim tmpWriter As IO.StreamWriter
	UInt32 tRec;
	NN<Text::String> str;

	IO::FileStream fs(sourceFile, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (fs.IsError())
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendC(UTF8STRC("Failed in converting "));
		sb.Append(sourceFile);
		this->ui->ShowMsgOK(sb.ToCString(), CSTR("Error"), this);
		return 0;
	}
	fs.Read(Data::ByteArray(buff, 100));
	fileLeng = ReadMUInt32(&buff[24]) * 2;
	shpType = ReadInt32(&buff[32]);
	filePos = 100;
	nRecords = 0;
	while (filePos < fileLeng)
	{
		if (fs.Read(Data::ByteArray(buff, 8)) != 8)
			break;
		fs.SeekFromCurrent(ReadMUInt32(&buff[4]) * 2);
		filePos += ReadMUInt32(&buff[4]) * 2 + 8;
		nRecords += 1;
	}

	fs.SeekFromBeginning(100);
	if (shpType == 3 || shpType == 5)
	{
		sb.ClearStr();
		sb.AppendOpt(outFilePrefix);
		sb.AppendC(UTF8STRC(".cip"));
		IO::FileStream cip(sb.ToCString(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		sb.RemoveChars(4);
		sb.AppendC(UTF8STRC(".cix"));
		IO::FileStream cix(sb.ToCString(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		sb.RemoveChars(4);
		sb.AppendC(UTF8STRC(".blk"));
		IO::FileStream blk(sb.ToCString(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		sb.RemoveChars(4);
		sb.AppendC(UTF8STRC(".ciu"));
		IO::FileStream cib(sb.ToCString(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		if (!cip.IsError() && !cix.IsError() && !blk.IsError() && !cib.IsError())
		{
			sb.ClearStr();
			sb.Append(sourceFile);
			i = sb.LastIndexOf('.');
			if (i != INVALID_INDEX)
			{
				sb.RemoveChars(sb.GetLength() - i);
			}
			sb.AppendC(UTF8STRC(".dbf"));
			IO::StmData::FileData fd(sb.ToCString(), false);
			DB::DBFFile dbf(fd, (UInt32)(UOSInt)this->lstLang->GetSelectedItem().p);
			dbfr = dbf.QueryTableData(nullptr, CSTR(""), 0, 0, 0, nullptr, 0);
			StrRecord *strRec;

//			tmpWriter = New IO.StreamWriter(sourceFile.Substring(0, sourceFile.LastIndexOf(".")) + ".txt")

			WriteUInt32(&buff[0], nRecords);
			WriteInt32(&buff[4], shpType);
			cip.Write(Data::ByteArrayR(buff, 8));
			cix.Write(Data::ByteArrayR(buff, 4));
			tRec = 0;
			currRec = 0;
			cipPos = 8;
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Reading "));
			sb.Append(sourceFile);
			progress->ProgressStart(sb.ToCString(), nRecords);

			while (tRec < nRecords)
			{
				if ((tRec % 100) == 0)
				{
					progress->ProgressUpdate(tRec, nRecords);
				}
				fs.Read(Data::ByteArray(buff, 12));

				Int32 recSize = ReadMInt32(&buff[4]);
				Bool chkVal = true;
				if (recSize == 2 && ReadInt32(&buff[8]) == 0)
				{
//					tmpWriter.WriteLine(ControlChars.Tab + ControlChars.Tab + ControlChars.Tab)
					if (dbfr.SetTo(r))
					{
						r->ReadNext();
					}
					chkVal = false;
				}
				else if (ReadInt32(&buff[8]) != shpType)
				{
					fs.SeekFromCurrent(recSize * 2 - 4);
//					tmpWriter.WriteLine(ControlChars.Tab + ControlChars.Tab + ControlChars.Tab)
					if (dbfr.SetTo(r))
					{
						r->ReadNext();
					}
					chkVal = false;
				}
				else
				{
					fs.Read(Data::ByteArray(&buff[12], 32));
					xMin = ReadDouble(&buff[12]);
					yMin = ReadDouble(&buff[20]);
					xMax = ReadDouble(&buff[28]);
					yMax = ReadDouble(&buff[36]);
					if (Math::IsNAN(xMin))
					{
						if (dbfr.SetTo(r))
						{
							r->ReadNext();
						}
						chkVal = false;
					}

					NN<MapFilter> f;
					if (chkVal && dbfr.SetTo(r))
					{
						if (r->ReadNext())
						{
							i = filters->GetCount();
							while (i-- > 0)
							{
								f = filters->GetItemNoCheck(i);
								chkVal = chkVal && f->IsValid(xMin, yMin, xMax, yMax, r);
							}
						}
						else
						{
							chkVal = false;
						}
					}
					if (!chkVal)
					{
						fs.SeekFromCurrent(recSize * 2 - 36);
//						tmpWriter.WriteLine(ControlChars.Tab + ControlChars.Tab + ControlChars.Tab)
					}
				}
				if (chkVal)
				{
					fs.Read(Data::ByteArray(buff, 8));
					nParts = ReadUInt32(&buff[0]);
					nPoints = ReadUInt32(&buff[4]);
//					'tmpWriter.WriteLine(nParts.ToString() + ControlChars.Tab + nPoints.ToString())

					WriteInt32(&buff[0], currRec);
					WriteUInt32(&buff[4], cipPos);
					cix.Write(Data::ByteArrayR(buff, 8));

					WriteUInt32(&buff[4], nParts);
					cip.Write(Data::ByteArrayR(buff, 8));
					cipPos += 8;
					outBuff = MemAlloc(UInt8, nParts * 4 + 4);
					fs.Read(Data::ByteArray(outBuff, nParts * 4));
					WriteUInt32(&outBuff[nParts * 4], nPoints);
					cip.Write(Data::ByteArrayR(outBuff, nParts * 4 + 4));
					cipPos += nParts * 4 + 4;
					MemFree(outBuff);

					if (isGrid80)
					{
						fs.Read(Data::ByteArray(&buff[16], 16));
						currX = ReadDouble(&buff[16]);
						currY = ReadDouble(&buff[24]);
						WriteInt32(&buff[0], Double2Int32(currX));
						WriteInt32(&buff[4], Double2Int32(currY));
						cip.Write(Data::ByteArrayR(buff, 8));
						cipPos += 8;
						xMin = currX;
						xMax = currX;
						yMin = currY;
						yMax = currY;
						i = 1;
						while (i < nPoints)
						{
							fs.Read(Data::ByteArray(&buff[16], 16));
							currX = ReadDouble(&buff[16]);
							currY = ReadDouble(&buff[24]);
							WriteInt32(&buff[0], Double2Int32(currX));
							WriteInt32(&buff[4], Double2Int32(currY));
							cip.Write(Data::ByteArrayR(buff, 8));
							cipPos += 8;

							if (currX > xMax) xMax = currX;
							if (currX < xMin) xMin = currX;
							if (currY > yMax) yMax = currY;
							if (currY < yMin) yMin = currY;
							i += 1;
						}
					}
					else
					{
//						Double lastX;
//						Double lastY;
						fs.Read(Data::ByteArray(&buff[16], 16));
						currX = ReadDouble(&buff[16]);
						currY = ReadDouble(&buff[24]);
						WriteInt32(&buff[0], Double2Int32(currX * LATSCALE));
						WriteInt32(&buff[4], Double2Int32(currY * LATSCALE));
						cip.Write(Data::ByteArrayR(buff, 8));
						cipPos += 8;
						xMin = currX;
						xMax = currX;
						yMin = currY;
						yMax = currY;
//						lastX = currX;
//						lastY = currY;

//						Double rLen = 0;
						i = 1;
						while (i < nPoints)
						{
							fs.Read(Data::ByteArray(&buff[16], 16));
							currX = ReadDouble(&buff[16]);
							currY = ReadDouble(&buff[24]);
							WriteInt32(&buff[0], Double2Int32(currX * LATSCALE));
							WriteInt32(&buff[4], Double2Int32(currY * LATSCALE));
							cip.Write(Data::ByteArrayR(buff, 8));
							cipPos += 8;
							if (i == nPoints >> 1)
							{
//								tmpWriter.Write(((currX + lastX) / 2).ToString() + ControlChars.Tab + ((currY + lastY) / 2).ToString())
							}
							if (currX > xMax) xMax = currX;
							if (currX < xMin) xMin = currX;
							if (currY > yMax) yMax = currY;
							if (currY < yMin) yMin = currY;
//							rLen += Math_Sqrt((currX - lastX) * (currX - lastX) + (currY - lastY) * (currY - lastY));
//							lastX = currX;
//							lastY = currY;
							i += 1;
						}
//						tmpWriter.Write(ControlChars.Tab + rLen.ToString("0.0000000"))
					}
//					tmpWriter.WriteLine(ControlChars.Tab + GetDBFName(dbf, dbCols, tRec, dbCols2))

					if (isGrid80)
					{
						left = Double2Int32(xMin) / blkScale;
						right = Double2Int32(xMax) / blkScale;
						top = Double2Int32(yMin) / blkScale;
						bottom = Double2Int32(yMax) / blkScale;
					}
					else
					{
						left = Double2Int32(xMin * LATSCALE) / blkScale;
						right = Double2Int32(xMax * LATSCALE) / blkScale;
						top = Double2Int32(yMin * LATSCALE) / blkScale;
						bottom = Double2Int32(yMax * LATSCALE) / blkScale;
					}

					si = top;
					while (si <= bottom)
					{
						sj = left;
						while (sj <= right)
						{
							sk = 0;
							sl = (OSInt)blks.GetCount() - 1;
							while (sk <= sl)
							{
								sm = (sk + sl) >> 1;
								theBlk = blks.GetItem((UOSInt)sm);
								if (theBlk->blockX > sj)
								{
									sl = sm - 1;
								}
								else if (theBlk->blockX < sj)
								{
									sk = sm + 1;
								}
								else if (theBlk->blockY > si)
								{
									sl = sm - 1;
								}
								else if (theBlk->blockY < si)
								{
									sk = sm + 1;
								}
								else
								{
									strRec = MemAlloc(StrRecord, 1);
									strRec->recId = currRec;
									if (!dbfr.IsNull())
									{
										strRec->str = this->GetNewDBFName(&dbf, dbCols, tRec, dbCols2);
									}
									else
									{
										strRec->str = 0;
									}
									theBlk->records->Add(strRec);
									sk = -1;
									break;
								}
							}
							if (sk >= 0)
							{
								theBlk = MemAlloc(Block, 1);
								theBlk->blockX = (Int32)sj;
								theBlk->blockY = (Int32)si;
								NEW_CLASS(theBlk->records, Data::ArrayList<StrRecord*>());

								strRec = MemAlloc(StrRecord, 1);
								strRec->recId = currRec;
								if (!dbfr.IsNull())
								{
									strRec->str = this->GetNewDBFName(&dbf, dbCols, tRec, dbCols2).Ptr();
								}
								else
								{
									strRec->str = 0;
								}
								theBlk->records->Add(strRec);

								blks.Insert((UOSInt)sk, theBlk);
							}

							sj += 1;
						}
						si += 1;
					}
					currRec++;
				}
				tRec++;
			}

			sb.ClearStr();
			sb.AppendC(UTF8STRC("Writing "));
			sb.Append(sourceFile);
			progress->ProgressStart(sb.ToCString(), blks.GetCount());

			WriteInt32(&buff[0], (Int32)blks.GetCount());
			WriteInt32(&buff[4], blkScale);
			blk.Write(Data::ByteArrayR(buff, 8));
			cib.Write(Data::ByteArrayR(buff, 8));

			i = 0;
			j = blks.GetCount();
			while (i < j)
			{
				theBlk = blks.GetItem(i);
				WriteInt32(&buff[0], (Int32)theBlk->records->GetCount());
				WriteInt32(&buff[4], theBlk->blockX);
				WriteInt32(&buff[8], theBlk->blockY);
				blk.Write(Data::ByteArrayR(buff, 12));
				k = 0;
				l = theBlk->records->GetCount();
				while (k < l)
				{
					WriteInt32(&buff[0], theBlk->records->GetItem(k)->recId);
					blk.Write(Data::ByteArrayR(buff, 4));
					k++;
				}
				cib.Write(Data::ByteArrayR(buff, 16));
				i++;
			}

			filePos = cib.GetPosition();
			Text::StringBuilderUTF16 u16buff;

			i = 0;
			j = blks.GetCount();
			while (i < j)
			{
				theBlk = blks.GetItem(i);
				cib.SeekFromBeginning(8 + i * 16);

				WriteInt32(&buff[0], theBlk->blockX);
				WriteInt32(&buff[4], theBlk->blockY);
				WriteInt32(&buff[8], (Int32)theBlk->records->GetCount());
				WriteInt32(&buff[12], (Int32)filePos);
				cib.Write(Data::ByteArrayR(buff, 16));

				cib.SeekFromBeginning(filePos);
				k = 0;
				l = theBlk->records->GetCount();
				while (k < l)
				{
					strRec = theBlk->records->GetItem(k);
					WriteInt32(&buff[0], strRec->recId);
					u16buff.ClearStr();
					if (strRec->str.SetTo(str))
					{
						u16buff.Append(str);
					}
					if (u16buff.GetLength() > 127)
					{
						u16buff.RemoveChars(u16buff.GetLength() - 127);
					}
					buff[4] = (UInt8)(u16buff.GetLength() << 1);
					cib.Write(Data::ByteArrayR(buff, 5));
					if (buff[4] > 0)
					{
						cib.Write(Data::ByteArrayR((const UInt8*)u16buff.ToPtr(), buff[4]));
					}
					filePos += (UOSInt)buff[4] + 5;
					k++;
				}
				i++;
			}

			i = 0;
			j = blks.GetCount();
			while (i < j)
			{
				theBlk = blks.GetItem(i);

				k = 0;
				l = theBlk->records->GetCount();
				while (k < l)
				{
					strRec = theBlk->records->GetItem(k);
					OPTSTR_DEL(strRec->str);
					MemFree(strRec);
					k++;
				}
				DEL_CLASS(theBlk->records);
				MemFree(theBlk);
				i++;
			}

//			tmpWriter.Close()
			if (dbfr.SetTo(r))
			{
				dbf.CloseReader(r);
			}
		}
		else
		{
			this->ui->ShowMsgOK(CSTR("Error in writing output files"), CSTR("Error"), this);
		}
		progress->ProgressEnd();
	}
	else if (shpType == 1 || shpType == 11)
	{
		sb.ClearStr();
		sb.AppendOpt(outFilePrefix);
		sb.AppendC(UTF8STRC(".cip"));
		IO::FileStream cip(sb.ToCString(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		sb.RemoveChars(4);
		sb.AppendC(UTF8STRC(".cix"));
		IO::FileStream cix(sb.ToCString(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		sb.RemoveChars(4);
		sb.AppendC(UTF8STRC(".blk"));
		IO::FileStream blk(sb.ToCString(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		sb.RemoveChars(4);
		sb.AppendC(UTF8STRC(".ciu"));
		IO::FileStream cib(sb.ToCString(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		if (!cip.IsError() && !cix.IsError() && !blk.IsError() && !cib.IsError())
		{
			sb.ClearStr();
			sb.Append(sourceFile);
			i = sb.LastIndexOf('.');
			if (i != INVALID_INDEX)
			{
				sb.RemoveChars(sb.GetLength() - i);
			}
			sb.AppendC(UTF8STRC(".dbf"));
			IO::StmData::FileData fd(sb.ToCString(), false);
			DB::DBFFile dbf(fd, (UInt32)(UOSInt)this->lstLang->GetSelectedItem().p);
			dbfr = dbf.QueryTableData(nullptr, CSTR(""), 0, 0, 0, nullptr, 0);

			StrRecord *strRec;

			cipPos = 0;
			WriteUInt32(&buff[0], nRecords);
			WriteInt32(&buff[4], 1); //shpType;
			cip.Write(Data::ByteArrayR(buff, 8));
			cix.Write(Data::ByteArrayR(buff, 4));
			cipPos += 8;
			tRec = 0;
			currRec = 0;
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Reading "));
			sb.Append(sourceFile);
			progress->ProgressStart(sb.ToCString(), nRecords);

			while (tRec < nRecords)
			{
				fs.Read(Data::ByteArray(buff, 12));
				if ((tRec % 100) == 0)
				{
					progress->ProgressUpdate(tRec, nRecords);
				}

				UInt32 recSize = ReadMUInt32(&buff[4]);
				Bool chkVal = true;
				if (recSize == 2 && ReadInt32(&buff[8]) == 0)
				{
					if (dbfr.SetTo(r))
					{
						r->ReadNext();
					}
					chkVal = false;
					currX = 0;
					currY = 0;
				}
				else
				{
					fs.Read(Data::ByteArray(&buff[12], recSize * 2 - 4));
					if (shpType == 11)
					{
						currX = ReadDouble(&buff[12]);
						currY = ReadDouble(&buff[20]);
	//					Double currZ = ReadDouble(&buff[28]);
						if (recSize >= 18)
						{
	//						Double currM = ReadDouble(&buff[36]);
						}
					}
					else
					{
						currX = ReadDouble(&buff[12]);
						currY = ReadDouble(&buff[20]);
					}

					NN<MapFilter> f;
					if (dbfr.SetTo(r))
					{
						if (r->ReadNext())
						{
							i = filters->GetCount();
							while (i-- > 0)
							{
								f = filters->GetItemNoCheck(i);
								chkVal = chkVal && f->IsValid(currX, currY, currX, currY, r);
							}
						}
						else
						{
							chkVal = false;
						}
					}
				}

				if (chkVal)
				{
					WriteInt32(&buff[0], currRec);
					WriteUInt32(&buff[4], cipPos);
					cix.Write(Data::ByteArrayR(buff, 8));

					WriteInt32(&buff[4], 1);
					WriteInt32(&buff[8], 0);
					WriteInt32(&buff[12], 1);
					cip.Write(Data::ByteArrayR(buff, 16));
					cipPos += 16;

					if (isGrid80)
					{
						WriteInt32(&buff[0], Double2Int32(currX));
						WriteInt32(&buff[4], Double2Int32(currY));
						cip.Write(Data::ByteArrayR(buff, 8));

						left = Double2Int32(currX) / blkScale;
						right = 1 + Double2Int32(currX) / blkScale;
						top = Double2Int32(currY) / blkScale;
						bottom = 1 + Double2Int32(currY) / blkScale;
					}
					else
					{
						WriteInt32(&buff[0], Double2Int32(currX * LATSCALE));
						WriteInt32(&buff[4], Double2Int32(currY * LATSCALE));
						cip.Write(Data::ByteArrayR(buff, 8));

						left = Double2Int32(currX * LATSCALE) / blkScale;
						right = 1 + Double2Int32(currX * LATSCALE) / blkScale;
						top = Double2Int32(currY * LATSCALE) / blkScale;
						bottom = 1 + Double2Int32(currY * LATSCALE) / blkScale;
					}
					cipPos += 8;

					si = top;
					while (si < bottom)
					{
						sj = left;
						while (sj < right)
						{
							Bool found = false;
							sk = 0;
							sl = (OSInt)blks.GetCount() - 1;
							while (sk <= sl)
							{
								sm = (sk + sl) >> 1;
								theBlk = blks.GetItem((UOSInt)sk);
								if (theBlk->blockX > sj)
								{
									sl = sm - 1;
								}
								else if (theBlk->blockX < sj)
								{
									sk = sm + 1;
								}
								else if (theBlk->blockY > si)
								{
									sl = sm - 1;
								}
								else if (theBlk->blockX < si)
								{
									sk = sm + 1;
								}
								else
								{
									found = true;
									strRec = MemAlloc(StrRecord, 1);
									strRec->recId = currRec;
									if (!dbfr.IsNull())
									{
										strRec->str = GetNewDBFName(&dbf, dbCols, tRec, dbCols2).Ptr();
									}
									else
									{
										strRec->str = 0;
									}
									theBlk->records->Add(strRec);
									break;
								}
							}
							if (!found)
							{
								theBlk = MemAlloc(Block, 1);
								theBlk->blockX = (Int32)sj;
								theBlk->blockY = (Int32)si;
								NEW_CLASS(theBlk->records, Data::ArrayList<StrRecord*>());

								strRec = MemAlloc(StrRecord, 1);
								strRec->recId = currRec;
								if (!dbfr.IsNull())
								{
									strRec->str = GetNewDBFName(&dbf, dbCols, tRec, dbCols2).Ptr();
								}
								else
								{
									strRec->str = 0;
								}
								theBlk->records->Add(strRec);

								blks.Insert((UOSInt)sk, theBlk);
							}

							sj += 1;
						}
						si += 1;
					}
					currRec += 1;
				}

				tRec += 1;
			}

			sb.ClearStr();
			sb.AppendC(UTF8STRC("Writing "));
			sb.Append(sourceFile);
			progress->ProgressStart(sb.ToCString(), blks.GetCount());

			WriteInt32(&buff[0], (Int32)blks.GetCount());
			WriteInt32(&buff[4], blkScale);
			blk.Write(Data::ByteArrayR(buff, 8));
			cib.Write(Data::ByteArrayR(buff, 8));

			i = 0;
			j = blks.GetCount();
			while (i < j)
			{
				theBlk = blks.GetItem(i);
				WriteInt32(&buff[0], (Int32)theBlk->records->GetCount());
				WriteInt32(&buff[4], theBlk->blockX);
				WriteInt32(&buff[8], theBlk->blockY);
				blk.Write(Data::ByteArrayR(buff, 12));
				k = 0;
				l = theBlk->records->GetCount();
				while (k < l)
				{
					WriteInt32(&buff[0], theBlk->records->GetItem(k)->recId);
					blk.Write(Data::ByteArrayR(buff, 4));
					k++;
				}
				cib.Write(Data::ByteArrayR(buff, 16));
				i++;
			}

			filePos = cib.GetPosition();
			Text::StringBuilderUTF16 u16buff;

			i = 0;
			j = blks.GetCount();
			while (i < j)
			{
				theBlk = blks.GetItem(i);
				cib.SeekFromBeginning(8 + i * 16);

				WriteInt32(&buff[0], theBlk->blockX);
				WriteInt32(&buff[4], theBlk->blockY);
				WriteInt32(&buff[8], (Int32)theBlk->records->GetCount());
				WriteInt32(&buff[12], (Int32)filePos);
				cib.Write(Data::ByteArrayR(buff, 16));

				cib.SeekFromBeginning(filePos);
				k = 0;
				l = theBlk->records->GetCount();
				while (k < l)
				{
					strRec = theBlk->records->GetItem(k);
					WriteInt32(&buff[0], strRec->recId);
					u16buff.ClearStr();
					if (strRec->str.SetTo(str))
					{
						u16buff.Append(str);
					}
					if (u16buff.GetLength() > 127)
					{
						u16buff.RemoveChars(u16buff.GetLength() - 127);
					}
					buff[4] = (UInt8)(u16buff.GetLength() << 1);
					cib.Write(Data::ByteArrayR(buff, 5));
					if (buff[4] > 0)
					{
						cib.Write(Data::ByteArrayR((const UInt8*)u16buff.ToPtr(), buff[4]));
					}
					filePos += (UOSInt)buff[4] + 5;
					k++;
				}
				i++;
			}

			i = 0;
			j = blks.GetCount();
			while (i < j)
			{
				theBlk = blks.GetItem(i);

				k = 0;
				l = theBlk->records->GetCount();
				while (k < l)
				{
					strRec = theBlk->records->GetItem(k);
					OPTSTR_DEL(strRec->str);
					MemFree(strRec);
					k++;
				}
				DEL_CLASS(theBlk->records);
				MemFree(theBlk);
				i++;
			}

			if (dbfr.SetTo(r))
			{
				dbf.CloseReader(r);
			}
		}
		else
		{
			this->ui->ShowMsgOK(CSTR("Error in writing output files"), CSTR("Error"), this);
		}
		progress->ProgressEnd();
	}
	else
	{
		this->ui->ShowMsgOK(CSTR("Unsupported shape type"), CSTR("Error"), this);
	}
	return shpType;
}

Int32 SSWR::SHPConv::SHPConvMainForm::LoadShape(Text::CStringNN fileName, Bool updateTxt)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	UInt8 buff[100];
	Double xMin;
	Double yMin;
	Double xMax;
	Double yMax;
	Int32 shpType;
	Int32 retV = 0;

	{
		IO::FileStream fs(fileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		if (fs.IsError())
		{
			this->ui->ShowMsgOK(CSTR("Failed opening the file"), CSTR("Error"), this);
			return 0;
		}

		fs.Read(BYTEARR(buff));
		if (ReadMInt32(buff) != 9994)
		{
			this->ui->ShowMsgOK(CSTR("File is not valid shape file"), CSTR("Error"), this);
			return 0;
		}
		xMin = ReadDouble(&buff[36]);
		yMin = ReadDouble(&buff[44]);
		xMax = ReadDouble(&buff[52]);
		yMax = ReadDouble(&buff[60]);
		shpType = ReadInt32(&buff[32]);
		if (updateTxt)
		{
			sptr = Text::StrInt32(sbuff, ReadMInt32(&buff[24]) * 2);
			this->txtFileLength->SetText(CSTRP(sbuff, sptr));
			sptr = Text::StrInt32(sbuff, ReadInt32(&buff[28]));
			this->txtVersion->SetText(CSTRP(sbuff, sptr));
			this->txtShpType->SetText(this->typeName[ReadInt32(&buff[32])]);
			sptr = Text::StrDouble(sbuff, xMin);
			this->txtXMin->SetText(CSTRP(sbuff, sptr));
			sptr = Text::StrDouble(sbuff, yMin);
			this->txtYMin->SetText(CSTRP(sbuff, sptr));
			sptr = Text::StrDouble(sbuff, xMax);
			this->txtXMax->SetText(CSTRP(sbuff, sptr));
			sptr = Text::StrDouble(sbuff, yMax);
			this->txtYMax->SetText(CSTRP(sbuff, sptr));
			this->isGrid80 = (xMin >= 180);
			sptr = Text::StrDouble(sbuff, ReadDouble(&buff[68]));
			this->txtZMin->SetText(CSTRP(sbuff, sptr));
			sptr = Text::StrDouble(sbuff, ReadDouble(&buff[76]));
			this->txtZMax->SetText(CSTRP(sbuff, sptr));
			sptr = Text::StrDouble(sbuff, ReadDouble(&buff[84]));
			this->txtMMin->SetText(CSTRP(sbuff, sptr));
			sptr = Text::StrDouble(sbuff, ReadDouble(&buff[92]));
			this->txtMMax->SetText(CSTRP(sbuff, sptr));
			this->txtSource->SetText(fileName);
			this->ClearFilter();
			this->btnConvert->SetEnabled(true);
		}
	}

	UOSInt dbRecCnt = 1;
	UOSInt dbOfst;
	UOSInt i;
	fileName.ConcatTo(sbuff);
	i = Text::StrLastIndexOfChar(sbuff, '.');
	sptr = Text::StrConcatC(&sbuff[i + 1], UTF8STRC("dbf"));

	{
		IO::StmData::FileData fd(CSTRP(sbuff, sptr), false);
		DB::DBFFile dbf(fd, (UInt32)(UOSInt)this->lstLang->GetSelectedItem().p);
		if (!dbf.IsError())
		{
			this->lstRecords->ClearItems();
			dbRecCnt = dbf.GetColCount();
			dbOfst = 0;
			while (dbOfst < dbRecCnt)
			{
				sbuff[0] = 0;
				sptr = dbf.GetColumnName(dbOfst, sbuff).Or(sbuff);
				this->lstRecords->AddItem(CSTRP(sbuff, sptr), 0);
				dbOfst++;
			}
			dbRecCnt = dbf.GetRowCnt();
			sptr = Text::StrUOSInt(sbuff, dbRecCnt);
			this->txtRecCnt->SetText(CSTRP(sbuff, sptr));
		}
	}

	this->currGroup = (UOSInt)-1;

	if (shpType == 1)
	{
		retV = 5000;
	}
	else
	{
		Double tVal = (yMax - yMin) * (xMax - xMin) / UOSInt2Double(dbRecCnt);
		retV = (Int32)(Math_Sqrt(tVal) * 500000);
		if (retV < 5000)
		{
			retV = 5000;
		}
	}
	return retV;
}

void SSWR::SHPConv::SHPConvMainForm::ClearFilter()
{
	this->globalFilters.DeleteAll();
}

void SSWR::SHPConv::SHPConvMainForm::ParseLabelStr(Text::CStringNN labelStr, Data::ArrayList<const UTF8Char*> *dbCols, Data::ArrayList<UInt32> *dbCols2)
{
	Text::StringBuilderUTF8 sb;
	sb.Append(labelStr);
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt strType;
	UnsafeArray<const UTF8Char> strTmp;
	UTF8Char sbuff[256];
	UTF8Char c;
	while (true)
	{
		i = sb.IndexOf(UTF8STRC("<%="));
		if (i == INVALID_INDEX)
		{
			dbCols->Add(Text::StrCopyNew(sb.ToString()).Ptr());
			break;
		}
		sb.v[i] = 0;
		dbCols->Add(Text::StrCopyNew(sb.ToString()).Ptr());
		sb.SetSubstr((UOSInt)i + 3);
		i = sb.IndexOf(UTF8STRC("%>"));
		if (i != INVALID_INDEX)
		{
			sb.v[i] = 0;
		}
		c = sb.ToString()[0];
		if (c == '@')
		{
			strTmp = sb.ToString() + 1;
			strType = 1;
		}
		else if (c == '*')
		{
			strTmp = sb.ToString() + 1;
			strType = 2;
		}
		else if (c == '#')
		{
			strTmp = sb.ToString() + 1;
			strType = 3;
		}
		else
		{
			strTmp = sb.ToString();
			strType = 0;
		}
		j = 0;
		k = this->lstRecords->GetCount();
		while (j < k)
		{
			this->lstRecords->GetItemText(sbuff, j);
			if (Text::StrStartsWith(sbuff, strTmp))
			{
				break;
			}
			j += 1;
		}
		dbCols2->Add((UInt32)(j + (strType * 256)));

		if (i >= 0)
		{
			sb.SetSubstr((UOSInt)i + 2);
		}
		else
		{
			sb.ClearStr();
		}
	}
}

void SSWR::SHPConv::SHPConvMainForm::FreeLabelStr(Data::ArrayList<const UTF8Char*> *dbCols, Data::ArrayList<UInt32> *dbCols2)
{
	UOSInt i;
	i = dbCols->GetCount();
	while (i-- > 0)
	{
		Text::StrDelNew(dbCols->GetItem(i));
	}
	dbCols->Clear();
	dbCols2->Clear();
}

NN<Text::String> SSWR::SHPConv::SHPConvMainForm::GetNewDBFName(DB::DBFFile *dbf, Data::ArrayList<const UTF8Char*> *dbCols, UOSInt currRec, NN<Data::ArrayList<UInt32>> dbCols2)
{
	Text::StringBuilderUTF16 output;
	UOSInt i;
	UOSInt j;
	UInt32 col;
	UInt32 shpType;
	UTF8Char sbuff[256];
	i = 0;
	j = dbCols2->GetCount();
	if (dbCols->GetCount() > 0)
	{
		output.Append(dbCols->GetItem(0));
		while (i < j)
		{
			col = dbCols2->GetItem(i);
			shpType = col >> 8;
			col = col & 255;
			i++;
			if (shpType == 1)
			{
				dbf->GetRecord(sbuff, currRec, col);
				Text::StrTrim(sbuff);
				output.Append(sbuff);
				output.Append(dbCols->GetItem(i));
			}
			else if (shpType == 2)
			{
				dbf->GetRecord(sbuff, currRec, col);
				Text::StrTrim(sbuff);
				output.AppendI32(Double2Int32(Text::StrToDoubleOrNAN(sbuff)));
				output.Append(dbCols->GetItem(i));
			}
			else if (shpType == 3)
			{
				dbf->GetRecord(sbuff, currRec, col);
				Text::StrTrim(sbuff);
				output.AppendI32(Double2Int32(Text::StrToDoubleOrNAN(sbuff)));
				output.Append(dbCols->GetItem(i));
			}
			else
			{
				dbf->GetRecord(sbuff, currRec, col);
				Text::StrTrim(sbuff);
				Text::StrToCapital(sbuff, sbuff);
				output.Append(sbuff);
				output.Append(dbCols->GetItem(i));
			}
		}
	}
	this->hkscsConv.FixString(output.ToPtr());
	output.Trim();
	return Text::String::NewNotNull(output.ToPtr());
}

SSWR::SHPConv::SHPConvMainForm::SHPConvMainForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<Media::DrawEngine> deng, NN<Media::MonitorMgr> monMgr) : UI::GUIForm(parent, 576, 464, ui)
{
	this->SetText(CSTR("SHPConv"));
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);
	this->monMgr = monMgr;
	this->deng = deng;
	this->SetDPI(this->monMgr->GetMonitorHDPI(this->GetHMonitor()), this->monMgr->GetMonitorDDPI(this->GetHMonitor()));
	
	this->lblDirectory = ui->NewLabel(*this, CSTR("Search Dir"));
	this->lblDirectory->SetRect(8, 8, 100, 23, false);
	this->txtDirectory = ui->NewTextBox(*this, CSTR(""));
	this->txtDirectory->SetRect(120, 8, 232, 20, false);
	this->btnDirectory = ui->NewButton(*this, CSTR("&Search"));
	this->btnDirectory->SetRect(360, 8, 75, 23, false);
	this->btnDirectory->HandleButtonClick(OnDirectoryClicked, this);
	this->lblSource = ui->NewLabel(*this, CSTR("Shape File"));
	this->lblSource->SetRect(8, 40, 100, 23, false);
	this->txtSource = ui->NewTextBox(*this, CSTR(""));
	this->txtSource->SetReadOnly(true);
	this->txtSource->SetRect(120, 40, 232, 20, false);
	this->btnSBrowse = ui->NewButton(*this, CSTR("B&rowse"));
	this->btnSBrowse->SetRect(360, 40, 75, 23, false);
	this->btnSBrowse->HandleButtonClick(OnSBrowseClicked, this);

	this->lblFileLength = ui->NewLabel(*this, CSTR("File Length"));
	this->lblFileLength->SetRect(8, 80, 100, 23, false);
	this->txtFileLength = ui->NewTextBox(*this, CSTR(""));
	this->txtFileLength->SetReadOnly(true);
	this->txtFileLength->SetRect(120, 80, 112, 20, false);
	this->lblVersion = ui->NewLabel(*this, CSTR("Version"));
	this->lblVersion->SetRect(8, 112, 100, 23, false);
	this->txtVersion = ui->NewTextBox(*this, CSTR(""));
	this->txtVersion->SetReadOnly(true);
	this->txtVersion->SetRect(120, 112, 112, 20, false);
	this->lblShpType = ui->NewLabel(*this, CSTR("Shape Type"));
	this->lblShpType->SetRect(8, 144, 100, 23, false);
	this->txtShpType = ui->NewTextBox(*this, CSTR(""));
	this->txtShpType->SetReadOnly(true);
	this->txtShpType->SetRect(120, 144, 112, 20, false);
	this->lblMMin = ui->NewLabel(*this, CSTR("MMin"));
	this->lblMMin->SetRect(8, 176, 100, 23, false);
	this->txtMMin = ui->NewTextBox(*this, CSTR(""));
	this->txtMMin->SetReadOnly(true);
	this->txtMMin->SetRect(120, 176, 112, 20, false);
	this->lblMMax = ui->NewLabel(*this, CSTR("MMax"));
	this->lblMMax->SetRect(8, 200, 100, 23, false);
	this->txtMMax = ui->NewTextBox(*this, CSTR(""));
	this->txtMMax->SetReadOnly(true);
	this->txtMMax->SetRect(120, 200, 112, 20, false);
	this->lstRecords = ui->NewListBox(*this, true);
	this->lstRecords->SetRect(16, 241, 216, 95, false);
	this->lstRecords->HandleSelectionChange(OnRecordsSelChg, this);

	this->lblXMin = ui->NewLabel(*this, CSTR("XMin"));
	this->lblXMin->SetRect(240, 80, 72, 23, false);
	this->txtXMin = ui->NewTextBox(*this, CSTR(""));
	this->txtXMin->SetReadOnly(true);
	this->txtXMin->SetRect(320, 80, 112, 20, false);
	this->lblXMax = ui->NewLabel(*this, CSTR("XMax"));
	this->lblXMax->SetRect(240, 112, 72, 23, false);
	this->txtXMax = ui->NewTextBox(*this, CSTR(""));
	this->txtXMax->SetReadOnly(true);
	this->txtXMax->SetRect(320, 112, 112, 20, false);
	this->lblYMin = ui->NewLabel(*this, CSTR("YMin"));
	this->lblYMin->SetRect(240, 152, 72, 23, false);
	this->txtYMin = ui->NewTextBox(*this, CSTR(""));
	this->txtYMin->SetReadOnly(true);
	this->txtYMin->SetRect(320, 152, 112, 20, false);
	this->lblYMax = ui->NewLabel(*this, CSTR("YMax"));
	this->lblYMax->SetRect(240, 184, 72, 23, false);
	this->txtYMax = ui->NewTextBox(*this, CSTR(""));
	this->txtYMax->SetReadOnly(true);
	this->txtYMax->SetRect(320, 184, 112, 20, false);
	this->lblZMin = ui->NewLabel(*this, CSTR("ZMin"));
	this->lblZMin->SetRect(240, 224, 72, 23, false);
	this->txtZMin = ui->NewTextBox(*this, CSTR(""));
	this->txtZMin->SetReadOnly(true);
	this->txtZMin->SetRect(320, 224, 112, 20, false);
	this->lblZMax = ui->NewLabel(*this, CSTR("ZMax"));
	this->lblZMax->SetRect(240, 256, 72, 23, false);
	this->txtZMax = ui->NewTextBox(*this, CSTR(""));
	this->txtZMax->SetReadOnly(true);
	this->txtZMax->SetRect(320, 256, 112, 20, false);
	this->lblRecCnt = ui->NewLabel(*this, CSTR("Rec Count"));
	this->lblRecCnt->SetRect(240, 288, 72, 23, false);
	this->txtRecCnt = ui->NewTextBox(*this, CSTR(""));
	this->txtRecCnt->SetReadOnly(true);
	this->txtRecCnt->SetRect(320, 288, 112, 20, false);
	this->lblBlkScale = ui->NewLabel(*this, CSTR("Block Scale"));
	this->lblBlkScale->SetRect(240, 320, 72, 23, false);
	this->txtBlkScale = ui->NewTextBox(*this, CSTR(""));
	this->txtBlkScale->SetRect(320, 320, 112, 20, false);

	this->lstLang = ui->NewListBox(*this, false);
	this->lstLang->SetRect(440, 9, 120, 303, false);
	this->lstLang->HandleSelectionChange(OnLangSelChg, this);
	this->txtCodePage = ui->NewTextBox(*this, CSTR(""));
	this->txtCodePage->SetReadOnly(true);
	this->txtCodePage->SetRect(440, 312, 120, 20, false);

	this->lblSeperator = ui->NewLabel(*this, CSTR("Seperator"));
	this->lblSeperator->SetRect(16, 344, 75, 25, false);
	this->cboSeperator = ui->NewComboBox(*this, true);
	this->cboSeperator->SetText(CSTR(", "));
	this->cboSeperator->AddItem(CSTR(", "), 0);
	this->cboSeperator->AddItem(CSTR(""), 0);
	this->cboSeperator->AddItem(CSTR("-"), 0);
	this->cboSeperator->SetRect(104, 344, 96, 21, false);
	this->btnGroup = ui->NewButton(*this, CSTR("Group"));
	this->btnGroup->SetRect(240, 344, 104, 23, false);
	this->btnGroup->HandleButtonClick(OnGroupClicked, this);
	this->btnFilter = ui->NewButton(*this, CSTR("Filter"));
	this->btnFilter->SetRect(352, 344, 104, 23, false);
	this->btnFilter->HandleButtonClick(OnFilterClicked, this);
	this->btnPreview = ui->NewButton(*this, CSTR("Preview"));
	this->btnPreview->SetRect(240, 368, 104, 23, false);
	this->btnPreview->HandleButtonClick(OnPreviewClicked, this);
	this->btnConvert = ui->NewButton(*this, CSTR("Convert"));
	this->btnConvert->SetRect(352, 368, 104, 23, false);
	this->btnConvert->HandleButtonClick(OnConvertClicked, this);
	this->txtLabel = ui->NewTextBox(*this, CSTR(""));
	this->txtLabel->SetRect(16, 392, 520, 20, false);
	this->lblLabelLegend = ui->NewLabel(*this, CSTR("No prefix = To Capital, @ = Keep original, * = to integer, # = to integer"));
	this->lblLabelLegend->SetRect(16, 416, 520, 23, false);
	this->lblProgress = ui->NewLabel(*this, CSTR(""));
	this->lblProgress->SetRect(0, 416, 570, 23, false);
	this->lblProgress->SetDockType(UI::GUIControl::DOCK_BOTTOM);

	this->progressName = nullptr;
	this->totalVal = 1;
	UInt32 sysCP = Text::EncodingFactory::GetSystemCodePage();
	UInt32 cp;
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	Data::ArrayList<UInt32> codePages;
	Text::EncodingFactory::GetCodePages(codePages);
	UOSInt i;
	i = this->lstLang->AddItem(CSTR("utf16"), (void*)1200);
	if (sysCP == 1200)
	{
		this->lstLang->SetSelectedIndex(i);
	}
	i = this->lstLang->AddItem(CSTR("utf8"), (void*)65001);
	if (sysCP == 65001)
	{
		this->lstLang->SetSelectedIndex(i);
	}

	i = 0;
	UOSInt j = codePages.GetCount();
	UOSInt k;
	while (i < j)
	{
		cp = codePages.GetItem(i);
		if (cp == 1200)
		{
		}
		else if (cp == 65001)
		{
		}
		else
		{
			sptr = Text::EncodingFactory::GetDotNetName(sbuff, cp);
			k = this->lstLang->AddItem(CSTRP(sbuff, sptr), (void*)(OSInt)cp);
			if (cp == sysCP)
			{
				this->lstLang->SetSelectedIndex(k);
			}
		}
		i++;
	}

	this->HandleDropFiles(OnFile, this);
}

SSWR::SHPConv::SHPConvMainForm::~SHPConvMainForm()
{
	this->ClearFilter();
}

void SSWR::SHPConv::SHPConvMainForm::OnMonitorChanged()
{
	this->SetDPI(this->monMgr->GetMonitorHDPI(this->GetHMonitor()), this->monMgr->GetMonitorDDPI(this->GetHMonitor()));
}

void SSWR::SHPConv::SHPConvMainForm::ProgressStart(Text::CStringNN name, UInt64 count)
{
	this->progressName = name;
	this->totalVal = count;
}

void SSWR::SHPConv::SHPConvMainForm::ProgressUpdate(UInt64 currCount, UInt64 newCount)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr = sbuff;
	Text::CStringNN progressName;
	if (this->progressName.SetTo(progressName) && progressName.leng)
	{
		sptr = progressName.ConcatTo(sptr);
	}
	*sptr++ = ' ';
	sptr = Text::StrUInt64(sptr, currCount);
	sptr = Text::StrConcatC(sptr, UTF8STRC(" out of "));
	sptr = Text::StrUInt64(sptr, this->totalVal);
	sptr = Text::StrConcatC(sptr, UTF8STRC(" ("));
	sptr = Text::StrUInt32(sptr, (UInt32)(currCount * 100 / totalVal));
	this->lblProgress->SetText(CSTRP(sbuff, sptr));
	this->ui->ProcessMessages();
}

void SSWR::SHPConv::SHPConvMainForm::ProgressEnd()
{
	this->progressName = nullptr;
	this->totalVal = 1;
	this->lblProgress->SetText(CSTR(""));
}
