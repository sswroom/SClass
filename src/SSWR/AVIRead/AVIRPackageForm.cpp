#include "Stdafx.h"
#include "Crypto/Hash/HashCreator.h"
#include "IO/Path.h"
#include "IO/VirtualPackageFile.h"
#include "SSWR/AVIRead/AVIRPackageForm.h"
#include "SSWR/AVIRead/AVIRHexViewerForm.h"
#include "Sync/MutexUsage.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/StringTool.h"
#include "Text/URLString.h"
#include "UI/Clipboard.h"
#include "UI/GUIFileDialog.h"
#include "UI/GUIFolderDialog.h"
#include "UtilUI/TextViewerForm.h"

#include <stdio.h>

typedef enum
{
	MNU_PASTE = 100,
	MNU_COPYTO,
	MNU_COPYALLTO,
	MNU_SAVEAS,
	MNU_DELETE_ITEM,
	MNU_OPEN_TEXT_VIEWER,
	MNU_OPEN_HEX_VIEWER,
	MNU_APPEND_ZIP,
	MNU_TEST,
	MNU_GO_UP_LEVEL,
	MNU_OPEN_SELECTED
} MenuItem;

void AVIRPackageForm_TestHandler(const UInt8 *buff, UOSInt buffSize, AnyType userData)
{
	NN<SSWR::AVIRead::AVIRPackageForm::ReadSession> sess = userData.GetNN<SSWR::AVIRead::AVIRPackageForm::ReadSession>();
	NN<Crypto::Hash::IHash> hash;
	if (sess->hash.SetTo(hash))
	{
		hash->Calc(buff, buffSize);
	}
	sess->fileReadSize += buffSize;
}

UInt32 __stdcall SSWR::AVIRead::AVIRPackageForm::ProcessThread(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRPackageForm> me = userObj.GetNN<SSWR::AVIRead::AVIRPackageForm>();
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	NN<Text::String> fname;
	Text::CStringNN fileName;
	ActionType atype = AT_COPY;
	UOSInt i;
	UOSInt j;
	Optional<Text::String> foundName;
	Bool lastFound = false;
	me->threadRunning = true;
	while (!me->threadToStop)
	{
		foundName = 0;
		Sync::MutexUsage mutUsage(me->fileMut);
		i = 0;
		j = me->fileNames.GetCount();
		while (foundName.IsNull() && i < j)
		{
			if (me->fileNames.GetItem(i).SetTo(fname))
			{
				atype = me->fileAction.GetItem(i);
				switch (atype)
				{
				case AT_COPY:
					foundName = fname;
					break;
				case AT_MOVE:
					foundName = fname;
					break;
				case AT_RETRYCOPY:
					foundName = fname;
					break;
				case AT_RETRYMOVE:
					foundName = fname;
					break;
				case AT_DELETE:
				case AT_DELETEFAIL:
				case AT_COPYFAIL:
				case AT_MOVEFAIL:
				case AT_SUCCEED:
				default:
					break;
				}
			}
			i++;
		}
		mutUsage.EndUse();
		if (foundName.SetTo(fname))
		{
		
			if (fname->StartsWith(UTF8STRC("file:///")))
			{
				sbuff[0] = 0;
				sptr = Text::URLString::GetURLFilePath(sbuff, fname->v, fname->leng).Or(sbuff);
				fileName = CSTRP(sbuff, sptr);
			}
			else
			{
				fileName = fname->ToCString();
			}
			lastFound = true;
			if (atype == AT_COPY)
			{
				Bool found = me->packFile->CopyFrom(fileName, me, me->statusBNT);
				mutUsage.BeginUse();
				i = me->fileNames.GetCount();
				while (i-- > 0)
				{
					if (me->fileNames.GetItem(i).OrNull() == fname.Ptr())
					{
						if (found)
						{
							me->fileAction.SetItem(i, AT_SUCCEED);
						}
						else
						{
							me->fileAction.SetItem(i, AT_COPYFAIL);
						}
						me->statusChg = true;
						break;
					}
				}
				mutUsage.EndUse();
			}
			else if (atype == AT_MOVE)
			{
				Bool found = me->packFile->MoveFrom(fileName, me, me->statusBNT);
				mutUsage.BeginUse();
				i = me->fileNames.GetCount();
				while (i-- > 0)
				{
					if (me->fileNames.GetItem(i).OrNull() == fname.Ptr())
					{
						if (found)
						{
							me->fileAction.SetItem(i, AT_SUCCEED);
						}
						else
						{
							me->fileAction.SetItem(i, AT_MOVEFAIL);
						}
						me->statusChg = true;
						break;
					}
				}
				mutUsage.EndUse();
			}
			else if (atype == AT_RETRYCOPY)
			{
				Bool found = me->packFile->RetryCopyFrom(fileName, me, me->statusBNT);
				mutUsage.BeginUse();
				i = me->fileNames.GetCount();
				while (i-- > 0)
				{
					if (me->fileNames.GetItem(i).OrNull() == fname.Ptr())
					{
						if (found)
						{
							me->fileAction.SetItem(i, AT_SUCCEED);
						}
						else
						{
							me->fileAction.SetItem(i, AT_COPYFAIL);
						}
						me->statusChg = true;
						break;
					}
				}
				mutUsage.EndUse();
			}
			else if (atype == AT_RETRYMOVE)
			{
				Bool found = me->packFile->RetryCopyFrom(fileName, me, me->statusBNT);
				mutUsage.BeginUse();
				i = me->fileNames.GetCount();
				while (i-- > 0)
				{
					if (me->fileNames.GetItem(i).OrNull() == fname.Ptr())
					{
						if (found)
						{
							me->fileAction.SetItem(i, AT_SUCCEED);
						}
						else
						{
							me->fileAction.SetItem(i, AT_MOVEFAIL);
						}
						me->statusChg = true;
						break;
					}
				}
				mutUsage.EndUse();
			}
		}
		else
		{
			if (lastFound)
			{
				me->ProgressEnd();
			}
			lastFound = false;
			me->threadEvt.Wait(1000);
		}
	}
	me->threadRunning = false;
	return 0;
}

void __stdcall SSWR::AVIRead::AVIRPackageForm::OnTimerTick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRPackageForm> me = userObj.GetNN<SSWR::AVIRead::AVIRPackageForm>();
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	if (me->statusChg)
	{
		UOSInt k;
		OSInt scrVPos = me->lvStatus->GetScrollVPos();
		NN<Text::String> fname;
		me->statusChg = false;
		me->lvStatus->ClearItems();
		Sync::MutexUsage mutUsage(me->fileMut);
		Data::ArrayIterator<NN<Text::String>> it = me->fileNames.Iterator();
		UOSInt i = 0;
		while (it.HasNext())
		{
			fname = it.Next();
			k = fname->LastIndexOf(IO::Path::PATH_SEPERATOR);
			k = me->lvStatus->AddItem(fname->ToCString().Substring(k + 1), fname->v.Ptr());
			switch (me->fileAction.GetItem(i))
			{
			case AT_COPY:
				me->lvStatus->SetSubItem(k, 1, CSTR("Copy"));
				break;
			case AT_MOVE:
				me->lvStatus->SetSubItem(k, 1, CSTR("Move"));
				break;
			case AT_DELETE:
				me->lvStatus->SetSubItem(k, 1, CSTR("Delete"));
				break;
			case AT_COPYFAIL:
				me->lvStatus->SetSubItem(k, 1, CSTR("Copy Failed"));
				break;
			case AT_MOVEFAIL:
				me->lvStatus->SetSubItem(k, 1, CSTR("Move Failed"));
				break;
			case AT_DELETEFAIL:
				me->lvStatus->SetSubItem(k, 1, CSTR("Delete Failed"));
				break;
			case AT_SUCCEED:
				me->lvStatus->SetSubItem(k, 1, CSTR("Succeed"));
				break;
			case AT_RETRYCOPY:
				me->lvStatus->SetSubItem(k, 1, CSTR("Retry Copy"));
				break;
			case AT_RETRYMOVE:
				me->lvStatus->SetSubItem(k, 1, CSTR("Retry Move"));
				break;
			}
			i++;
		}
		mutUsage.EndUse();
		me->lvStatus->ScrollTo(0, scrVPos);
	}

	UInt64 readPos;
	UInt64 readCurr;
	Int64 timeDiff;
	Double spd;
	Sync::MutexUsage readMutUsage(me->readMut);
	readPos = me->readLast;
	readCurr = me->readCurr;
	me->readCurr = 0;
	timeDiff = me->readReadTime.DiffMS(me->readLastTimer);
	me->readLastTimer.SetValue(me->readReadTime);
	readMutUsage.EndUse();
	if (timeDiff > 0)
	{
		spd = (Double)readCurr * 1000 / (Double)timeDiff;
		
	}
	else
	{
		spd = 0;
	}
	me->rlcStatus->AddSample(&spd);

	Sync::MutexUsage progMutUsage(me->progMut);
	NN<Text::String> progName;
	if (me->progStarted && me->progName.SetTo(progName))
	{
		me->progStarted = false;
		me->prgStatus->ProgressStart(progName->ToCString(), me->progStartCnt);
	}
	UInt64 progUpdateCurr = me->progUpdateCurr;
	UInt64 progUpdateNew = me->progUpdateNew;
	Bool progUpdated = me->progUpdated;
	Bool progEnd = me->progEnd;
	me->progUpdated = false;
	me->progEnd = false;
	progMutUsage.EndUse();
	if (progUpdated)
	{
		me->prgStatus->ProgressUpdate(progUpdateCurr, progUpdateNew);
	}
	if (progEnd)
	{
		me->prgStatus->ProgressEnd();
	}

	Bool hasFile;
	UInt64 fileSize = 0;
	if (me->statusFileChg)
	{
		NN<Text::String> statusFile;
		Sync::MutexUsage mutUsage(me->statusFileMut);
		if (me->statusFile.SetTo(statusFile))
		{
			hasFile = true;
			me->txtStatusFile->SetText(statusFile->ToCString());
			sptr = Text::StrUInt64(sbuff, me->statusFileSize);
			me->txtStatusFileSize->SetText(CSTRP(sbuff, sptr));
			fileSize = me->statusFileSize;
		}
		else
		{
			hasFile = false;
			me->txtStatusFile->SetText(CSTR(""));
			me->txtStatusFileSize->SetText(CSTR(""));
		}
		mutUsage.EndUse();
	}
	else
	{
		Sync::MutexUsage mutUsage(me->statusFileMut);
		hasFile = me->statusFile.NotNull();
		fileSize = me->statusFileSize;
		mutUsage.EndUse();
	}
	if (readPos != me->statusDispSize)
	{
		sptr = Text::StrUInt64(sbuff, readPos);
		me->txtStatusCurrSize->SetText(CSTRP(sbuff, sptr));
		me->statusDispSize = readPos;
	}
	if (spd != me->statusDispSpd)
	{
		sptr = Text::StrDoubleFmt(sbuff, spd, "0.0");
		me->txtStatusCurrSpeed->SetText(CSTRP(sbuff, sptr));
		me->statusDispSpd = spd;
	}
	if (hasFile && fileSize != 0 && spd != 0)
	{
		Double t = (Double)(fileSize - readPos) / spd;
		sptr = Text::StrDoubleFmt(sbuff, t, "0.0");
		me->txtStatusTimeLeft->SetText(CSTRP(sbuff, sptr));
	}
	else
	{
		me->txtStatusTimeLeft->SetText(CSTR("?"));
	}
	if (me->statusDispBNT != me->statusBNT)
	{
		me->statusDispBNT = me->statusBNT;
		if (me->statusDispBNT == IO::ActiveStreamReader::BottleNeckType::Unknown)
		{
			me->pnlStatusBNT->SetBGColor(0xffc0c0c0);
		}
		else if (me->statusDispBNT == IO::ActiveStreamReader::BottleNeckType::Read)
		{
			me->pnlStatusBNT->SetBGColor(0xff60ff60);
		}
		else if (me->statusDispBNT == IO::ActiveStreamReader::BottleNeckType::Write)
		{
			me->pnlStatusBNT->SetBGColor(0xffff4040);
		}
		me->pnlStatus->Redraw();
	}
}

void __stdcall SSWR::AVIRead::AVIRPackageForm::LVDblClick(AnyType userObj, UOSInt index)
{
	NN<SSWR::AVIRead::AVIRPackageForm> me = userObj.GetNN<SSWR::AVIRead::AVIRPackageForm>();
	me->OpenItem(index);
}

void __stdcall SSWR::AVIRead::AVIRPackageForm::OnStatusDblClick(AnyType userObj, UOSInt index)
{
	NN<SSWR::AVIRead::AVIRPackageForm> me = userObj.GetNN<SSWR::AVIRead::AVIRPackageForm>();
	Sync::MutexUsage mutUsage(me->fileMut);
	if (me->fileAction.GetItem(index) == AT_COPYFAIL)
	{
		me->fileAction.SetItem(index, AT_RETRYCOPY);
		me->statusChg = true;
	}
	else if (me->fileAction.GetItem(index) == AT_MOVEFAIL)
	{
		me->fileAction.SetItem(index, AT_RETRYMOVE);
		me->statusChg = true;
	}
}

void __stdcall SSWR::AVIRead::AVIRPackageForm::OnFilesRightClick(AnyType userObj, Math::Coord2DDbl coord, UOSInt index)
{
	NN<SSWR::AVIRead::AVIRPackageForm> me = userObj.GetNN<SSWR::AVIRead::AVIRPackageForm>();
	me->mnuPopup->ShowMenu(me->lvFiles, Math::Coord2D<OSInt>(Double2OSInt(coord.x), Double2OSInt(coord.y)));
}

void __stdcall SSWR::AVIRead::AVIRPackageForm::OnFiles(AnyType userObj, Data::DataArray<NN<Text::String>> files)
{
	NN<SSWR::AVIRead::AVIRPackageForm> me = userObj.GetNN<SSWR::AVIRead::AVIRPackageForm>();
	UOSInt nFiles = files.GetCount();
	Data::ArrayListStringNN fileNames(nFiles);
	UOSInt i = 0;
	while (i < nFiles)
	{
		fileNames.Add(files[i]);
		i++;
	}
	me->PasteFiles(fileNames, false);
}

void SSWR::AVIRead::AVIRPackageForm::GoUpLevel()
{
	Bool needRelease;
	NN<IO::PackageFile> pkg;
	if (this->packFile->GetParent(needRelease).SetTo(pkg))
	{
		NN<Text::String> name = this->packFile->GetSourceNameObj();
		UOSInt i = name->LastIndexOf(IO::Path::PATH_SEPERATOR);
		this->UpdatePackFile(pkg, needRelease, name->ToCString().Substring(i + 1));
	}
}

void SSWR::AVIRead::AVIRPackageForm::OpenItem(UOSInt index)
{
	if (index == 0 && this->packFile->HasParent())
	{
		this->GoUpLevel();
		return;
	}
	index = this->PackFileIndex(index);
	IO::PackageFile::PackObjectType pot = this->packFile->GetItemType(index);
	if (pot == IO::PackageFile::PackObjectType::PackageFileType)
	{
		Bool needRelease;
		NN<IO::PackageFile> pkg;
		if (this->packFile->GetItemPack(index, needRelease).SetTo(pkg))
		{
			this->UpdatePackFile(pkg, needRelease, CSTR_NULL);
//			this->core->OpenObject(pkg);
		}
	}
	else if (pot == IO::PackageFile::PackObjectType::ParsedObject)
	{
		Bool needRelease;
		NN<IO::ParsedObject> pobj;
		if (this->packFile->GetItemPObj(index, needRelease).SetTo(pobj))
		{
			if (!needRelease)
			{
				printf("AVIRPackageForm: Memory problem occurred\r\n");
			}
			this->core->OpenObject(pobj);
		}
	}
	else if (pot == IO::PackageFile::PackObjectType::StreamData)
	{
		NN<IO::StreamData> data;
		if (this->packFile->GetItemStmDataNew(index).SetTo(data))
		{
			this->core->LoadData(data, this->packFile.Ptr());
			data.Delete();
		}
	}
}

void SSWR::AVIRead::AVIRPackageForm::TestPackage(NN<IO::ActiveStreamReader> reader, NN<ReadSession> sess, NN<IO::PackageFile> pack)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	NN<IO::PackageFile> innerPack;
	NN<IO::StreamData> stmData;
	UInt64 fileSize;
	UInt64 storeSize;
	Bool needDelete;
	Data::Timestamp startTime;
	Data::Timestamp endTime;
	UOSInt i = 0;
	UOSInt j = pack->GetCount();
	while (i < j)
	{
		switch (pack->GetItemType(i))
		{
		case IO::PackageFile::PackObjectType::PackageFileType:
			sess->dirCnt++;
			if (pack->GetItemPack(i, needDelete).SetTo(innerPack))
			{
				TestPackage(reader, sess, innerPack);
				if (needDelete)
				{
					innerPack.Delete();
				}
			}
			else
			{
				sess->errorCnt++;
				sess->sbError->Append(pack->GetSourceNameObj());
				sess->sbError->AppendUTF8Char(IO::Path::PATH_SEPERATOR);
				if (pack->GetItemName(sbuff, i).SetTo(sptr))
				{
					sess->sbError->AppendP(sbuff, sptr);
				}
				else
				{
					sess->sbError->Append(CSTR("file["));
					sess->sbError->AppendUOSInt(i);
					sess->sbError->AppendUTF8Char(']');
				}
				sess->sbError->Append(CSTR(" Error in getting directory object\r\n"));
				break;
			}
			break;
		case IO::PackageFile::PackObjectType::StreamData:
			sess->fileCnt++;
			if (pack->GetItemStmDataNew(i).SetTo(stmData))
			{
				NN<Crypto::Hash::IHash> hash;
				storeSize = pack->GetItemStoreSize(i);
				fileSize = stmData->GetDataSize();
				sess->fileReadSize = 0;
				sess->totalSize += fileSize;
				sess->totalStoreSize += storeSize;
				if (pack->GetItemComp(i) == Data::Compress::Decompressor::CM_UNCOMPRESSED)
				{
					sess->totalSizeUncomp += fileSize;
					if (sess->hash.SetTo(hash)) hash->Clear();
					startTime = Data::Timestamp::UtcNow();
					reader->ReadStreamData(stmData, 0);
					endTime = Data::Timestamp::UtcNow();
					if (sess->fileReadSize == fileSize)
					{
						Data::Duration thisTime = endTime.Diff(startTime);
						sess->totalDurUncomp += thisTime;
						Double thisSpeed = UInt64_Double(fileSize) / thisTime.GetTotalSec();
						if (sess->slowestSpeedUncomp == 0 || sess->slowestSpeedUncomp > thisSpeed)
						{
							sess->slowestSpeedUncomp = thisSpeed;
						}
					}
					else
					{
						sess->errorCnt++;
						sess->sbError->Append(pack->GetSourceNameObj());
						sess->sbError->AppendUTF8Char(IO::Path::PATH_SEPERATOR);
						if (pack->GetItemName(sbuff, i).SetTo(sptr))
						{
							sess->sbError->AppendP(sbuff, sptr);
						}
						else
						{
							sess->sbError->Append(CSTR("file["));
							sess->sbError->AppendUOSInt(i);
							sess->sbError->AppendUTF8Char(']');
						}
						sess->sbError->Append(CSTR(" read size different from file size\r\n"));
					}
				}
				else
				{
					sess->totalCompSize += fileSize;
					if (sess->hash.SetTo(hash)) hash->Clear();
					startTime = Data::Timestamp::UtcNow();
					reader->ReadStreamData(stmData, 0);
					endTime = Data::Timestamp::UtcNow();
					if (sess->fileReadSize == fileSize)
					{
						Data::Duration thisTime = endTime.Diff(startTime);
						sess->totalDurComp += thisTime;
						Double thisSpeed = UInt64_Double(fileSize) / thisTime.GetTotalSec();
						if (sess->slowestSpeedComp == 0 || sess->slowestSpeedComp > thisSpeed)
						{
							sess->slowestSpeedComp = thisSpeed;
						}
					}
					else
					{
						sess->errorCnt++;
						sess->sbError->Append(pack->GetSourceNameObj());
						sess->sbError->AppendUTF8Char(IO::Path::PATH_SEPERATOR);
						if (pack->GetItemName(sbuff, i).SetTo(sptr))
						{
							sess->sbError->AppendP(sbuff, sptr);
						}
						else
						{
							sess->sbError->Append(CSTR("file["));
							sess->sbError->AppendUOSInt(i);
							sess->sbError->AppendUTF8Char(']');
						}
						sess->sbError->Append(CSTR(" read size different from file size\r\n"));
					}
				}
				endTime = Data::Timestamp::UtcNow();
				stmData.Delete();
			}
			else
			{
				sess->errorCnt++;
				sess->sbError->Append(pack->GetSourceNameObj());
				sess->sbError->AppendUTF8Char(IO::Path::PATH_SEPERATOR);
				if (pack->GetItemName(sbuff, i).SetTo(sptr))
				{
					sess->sbError->AppendP(sbuff, sptr);
				}
				else
				{
					sess->sbError->Append(CSTR("file["));
					sess->sbError->AppendUOSInt(i);
					sess->sbError->AppendUTF8Char(']');
				}
				sess->sbError->Append(CSTR(" Error in opening file for reading\r\n"));
			}
			break;
		case IO::PackageFile::PackObjectType::ParsedObject:
		case IO::PackageFile::PackObjectType::Unknown:
			sess->errorCnt++;
			sess->sbError->Append(pack->GetSourceNameObj());
			sess->sbError->AppendUTF8Char(IO::Path::PATH_SEPERATOR);
			if (pack->GetItemName(sbuff, i).SetTo(sptr))
			{
				sess->sbError->AppendP(sbuff, sptr);
			}
			else
			{
				sess->sbError->Append(CSTR("file["));
				sess->sbError->AppendUOSInt(i);
				sess->sbError->AppendUTF8Char(']');
			}
			sess->sbError->Append(CSTR(" file type is not supported\r\n"));
			break;
		}
		i++;
	}
}

void SSWR::AVIRead::AVIRPackageForm::DisplayPackFile(NN<IO::PackageFile> packFile)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	Data::Timestamp ts;
	UOSInt maxWidth = 0;
	UOSInt w;
	this->lvFiles->ClearItems();
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt selIndex = INVALID_INDEX;
	IO::PackageFile::PackObjectType pot;
	if (packFile->HasParent())
	{
		k = this->lvFiles->AddItem(CSTR(".."), (void*)INVALID_INDEX);
		this->lvFiles->SetSubItem(k, 1, CSTR("Folder"));
	}
	NN<Text::String> initSel;
	i = 0;
	j = packFile->GetCount();
	while (i < j)
	{
		sptr = packFile->GetItemName(sbuff, i).Or(sbuff);
		pot = packFile->GetItemType(i);
		k = this->lvFiles->AddItem(CSTRP(sbuff, sptr), (void*)i);

		if (this->initSel.SetTo(initSel) && initSel->Equals(CSTRP(sbuff, sptr)))
		{
			selIndex = k;
		}
		w = this->lvFiles->GetStringWidth(sbuff);
		if (w > maxWidth)
			maxWidth = w;

		ts = packFile->GetItemModTime(i).ToLocalTime();
		sptr = ts.ToStringNoZone(sbuff);
		this->lvFiles->SetSubItem(k, 4, CSTRP(sbuff, sptr));
		if (pot == IO::PackageFile::PackObjectType::StreamData)
		{
			this->lvFiles->SetSubItem(k, 1, CSTR("File"));
			sptr = Text::StrUInt64(sbuff, packFile->GetItemStoreSize(i));
			this->lvFiles->SetSubItem(k, 2, CSTRP(sbuff, sptr));
			sptr = Text::StrUInt64(sbuff, packFile->GetItemSize(i));
			this->lvFiles->SetSubItem(k, 3, CSTRP(sbuff, sptr));
			if (packFile->IsCompressed(i))
			{
				this->lvFiles->SetSubItem(k, 5, Data::Compress::Decompressor::GetCompMethName(packFile->GetItemComp(i)));
			}
			else
			{
				this->lvFiles->SetSubItem(k, 5, CSTR("Uncompressed"));
			}
		}
		else if (pot == IO::PackageFile::PackObjectType::PackageFileType)
		{
			this->lvFiles->SetSubItem(k, 1, CSTR("Folder"));
		}
		else if (pot == IO::PackageFile::PackObjectType::ParsedObject)
		{
			this->lvFiles->SetSubItem(k, 1, CSTR("Object"));
		}
		else
		{
			this->lvFiles->SetSubItem(k, 1, CSTR("Unknown"));
		}

		i++;
	}
	if (maxWidth > 0)
	{
		this->lvFiles->SetColumnWidth(0, (Int32)(maxWidth + 6));
	}
	if (selIndex != INVALID_INDEX)
	{
		this->lvFiles->SetSelectedIndex(selIndex);
		this->lvFiles->EnsureVisible(selIndex);
	}
}

UOSInt SSWR::AVIRead::AVIRPackageForm::PackFileIndex(UOSInt lvIndex)
{
	if (lvIndex == INVALID_INDEX)
	{
		return INVALID_INDEX;
	}
	if (this->packFile->HasParent())
	{
		return lvIndex - 1;
	}
	return lvIndex;
}

void SSWR::AVIRead::AVIRPackageForm::UpdatePackFile(NN<IO::PackageFile> packFile, Bool needDelete, Text::CString initSel)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	sptr = Text::StrConcatC(sbuff, UTF8STRC("Package Form - "));
	if (this->rootPackFile && packFile.Ptr() != this->rootPackFile)
	{
		sptr = this->rootPackFile->GetSourceNameObj()->ConcatTo(sptr);
		*sptr++ = '/';
	}
	sptr = packFile->GetSourceNameObj()->ConcatTo(sptr);
	this->SetText(CSTRP(sbuff, sptr));
	OPTSTR_DEL(this->initSel);
	Text::CStringNN nninitSel;
	if (initSel.SetTo(nninitSel))
	{
		this->initSel = Text::String::New(nninitSel);
	}
	if (this->packNeedDelete)
	{
		this->packFile.Delete();
	}
	this->packFile = packFile;
	this->packNeedDelete = needDelete;
	this->DisplayPackFile(packFile);
}

void SSWR::AVIRead::AVIRPackageForm::PasteFiles(NN<Data::ArrayListStringNN> files, Bool move)
{
	if (move)
	{
		if (this->packFile->GetFileType() == IO::PackageFileType::Virtual)
		{
			this->ui->ShowMsgOK(CSTR("Virtual Package does not support move"), CSTR("Package"), this);
		}
		else
		{
			Sync::MutexUsage mutUsage(this->fileMut);
			Data::ArrayIterator<NN<Text::String>> it = files->Iterator();
			while (it.HasNext())
			{
				this->fileNames.Add(it.Next()->Clone());
				this->fileAction.Add(AT_MOVE);
			}
			this->statusChg = true;
		}
	}
	else
	{
		if (this->packFile->GetFileType() == IO::PackageFileType::Virtual)
		{
			Bool changed = false;
			Data::ArrayIterator<NN<Text::String>> it = files->Iterator();
			NN<Text::String> s;
			while (it.HasNext())
			{
				s = it.Next();
				if (!this->packFile->CopyFrom(s->ToCString(), 0, 0))
				{
					Text::StringBuilderUTF8 sb;
					sb.Append(CSTR("Failed to copy "));
					sb.Append(s);
					sb.Append(CSTR(", do you want to continue?"));
					if (!this->ui->ShowMsgYesNo(sb.ToCString(), CSTR("Package"), this))
					{
						break;
					}
				}
				else
				{
					changed = true;
				}
			}
			if (changed)
			{
				this->DisplayPackFile(this->packFile);
			}
		}
		else
		{
			Sync::MutexUsage mutUsage(this->fileMut);
			Data::ArrayIterator<NN<Text::String>> it = files->Iterator();
			while (it.HasNext())
			{
				this->fileNames.Add(it.Next()->Clone());
				this->fileAction.Add(AT_COPY);
			}
			this->statusChg = true;
		}
	}

}

SSWR::AVIRead::AVIRPackageForm::AVIRPackageForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<IO::PackageFile> packFile) : UI::GUIForm(parent, 960, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	sptr = packFile->GetSourceNameObj()->ConcatTo(Text::StrConcatC(sbuff, UTF8STRC("Package Form - ")));
	this->SetText(CSTRP(sbuff, sptr));

	this->core = core;
	this->packFile = packFile;
	this->initSel = 0;
	if (this->packFile->GetFileType() == IO::PackageFileType::Directory)
	{
		this->packNeedDelete = true;
		this->rootPackFile = 0;
	}
	else
	{
		this->rootPackFile = packFile.Ptr();
		this->packNeedDelete = false;
	}
	this->statusChg = false;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->threadRunning = false;
	this->threadToStop = false;
	this->readLastTimer.SetCurrTimeUTC();
	this->readReadTime.SetValue(this->readLastTimer);
	this->readTotal = 0;
	this->readCurr = 0;
	this->readLast = 0;
	this->readCurrFile = CSTR_NULL;
	this->readFileCnt = 0;
	this->statusFileChg = false;
	this->statusFile = 0;
	this->statusFileSize = 0;
	this->statusDispSize = (UOSInt)-1;
	this->statusDispSpd = -1;
	this->statusBNT = IO::ActiveStreamReader::BottleNeckType::Unknown;
	this->statusDispBNT = IO::ActiveStreamReader::BottleNeckType::Unknown;
	this->progName = 0;
	this->progStarted = false;
	this->progUpdated = false;
	this->progEnd = false;
	
/*	NEW_CLASSNN(this->pnlCtrl = ui->NewPanel(*this));
	this->pnlCtrl->SetRect(0, 0, 100, 28, false);
	this->pnlCtrl->SetDockType(UI::GUIControl::DOCK_TOP);*/
	this->prgStatus = ui->NewProgressBar(*this, 1);
	this->prgStatus->SetRect(0, 0, 100, 23, false);
	this->prgStatus->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->tcMain = ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);


	this->tpFiles = this->tcMain->AddTabPage(CSTR("Files"));
	this->lvFiles = ui->NewListView(this->tpFiles, UI::ListViewStyle::Table, 6);
	this->lvFiles->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvFiles->AddColumn(CSTR("Item Name"), 150);
	this->lvFiles->AddColumn(CSTR("Type"), 80);
	this->lvFiles->AddColumn(CSTR("Store Size"), 100);
	this->lvFiles->AddColumn(CSTR("File Size"), 100);
	this->lvFiles->AddColumn(CSTR("Modify Time"), 180);
	this->lvFiles->AddColumn(CSTR("Compression"), 100);

	this->lvFiles->HandleDblClk(LVDblClick, this);
	DisplayPackFile(this->packFile);

	NN<UI::GUIMainMenu> mnuMain;
	NN<UI::GUIMenu> mnu;
//	UI::GUIMenu *mnu2;
	NEW_CLASSNN(mnuMain, UI::GUIMainMenu());
	mnu = mnuMain->AddSubMenu(CSTR("&File"));
	mnu->AddItem(CSTR("Save &As..."), MNU_SAVEAS, UI::GUIMenu::KM_CONTROL, UI::GUIControl::GK_S);
	mnu->AddItem(CSTR("Append Zip Content"), MNU_APPEND_ZIP, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Test from this folder"), MNU_TEST, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu = mnuMain->AddSubMenu(CSTR("&Edit"));
	mnu->AddItem(CSTR("&Paste"), MNU_PASTE, UI::GUIMenu::KM_CONTROL, UI::GUIControl::GK_V);
	mnu->AddItem(CSTR("Copy To..."), MNU_COPYTO, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Copy All To..."), MNU_COPYALLTO, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Delete Item"), MNU_DELETE_ITEM, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_DELETE);
	mnu = mnuMain->AddSubMenu(CSTR("&Navigate"));
	mnu->AddItem(CSTR("Open Selected"), MNU_OPEN_SELECTED, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_ENTER);
	mnu->AddItem(CSTR("Go Up Level"), MNU_GO_UP_LEVEL, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_BACKSPACE);

	NEW_CLASSNN(this->mnuPopup, UI::GUIPopupMenu());
	this->mnuPopup->AddItem(CSTR("Copy To..."), MNU_COPYTO, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->mnuPopup->AddItem(CSTR("Open Text Viewer"), MNU_OPEN_TEXT_VIEWER, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->mnuPopup->AddItem(CSTR("Open Hex Viewer"), MNU_OPEN_HEX_VIEWER, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->mnuPopup->AddItem(CSTR("Delete Item"), MNU_DELETE_ITEM, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->lvFiles->HandleRightClick(OnFilesRightClick, this);

	this->tpStatus = this->tcMain->AddTabPage(CSTR("Status"));
	this->pnlStatus = ui->NewPanel(this->tpStatus);
	this->pnlStatus->SetRect(0, 0, 100, 48, false);
	this->pnlStatus->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblStatusFile = ui->NewLabel(this->pnlStatus, CSTR("Copy From"));
	this->lblStatusFile->SetRect(0, 0, 100, 23, false);
	this->txtStatusFile = ui->NewTextBox(this->pnlStatus, CSTR(""));
	this->txtStatusFile->SetRect(100, 0, 800, 23, false);
	this->txtStatusFile->SetReadOnly(true);
	this->pnlStatusBNT = ui->NewPanel(this->pnlStatus);
	this->pnlStatusBNT->SetBGColor(0xffc0c0c0);
	this->pnlStatusBNT->SetRect(900, 0, 23, 23, false);
	this->lblStatusFileSize = ui->NewLabel(this->pnlStatus, CSTR("File Size"));
	this->lblStatusFileSize->SetRect(0, 24, 100, 23, false);
	this->txtStatusFileSize = ui->NewTextBox(this->pnlStatus, CSTR(""));
	this->txtStatusFileSize->SetRect(100, 24, 100, 23, false);
	this->txtStatusFileSize->SetReadOnly(true);
	this->lblStatusCurrSize = ui->NewLabel(this->pnlStatus, CSTR("Curr Size"));
	this->lblStatusCurrSize->SetRect(220, 24, 100, 23, false);
	this->txtStatusCurrSize = ui->NewTextBox(this->pnlStatus, CSTR(""));
	this->txtStatusCurrSize->SetRect(320, 24, 100, 23, false);
	this->txtStatusCurrSize->SetReadOnly(true);
	this->lblStatusCurrSpeed = ui->NewLabel(this->pnlStatus, CSTR("Curr Speed"));
	this->lblStatusCurrSpeed->SetRect(440, 24, 100, 23, false);
	this->txtStatusCurrSpeed = ui->NewTextBox(this->pnlStatus, CSTR(""));
	this->txtStatusCurrSpeed->SetRect(540, 24, 100, 23, false);
	this->txtStatusCurrSpeed->SetReadOnly(true);
	this->lblStatusTimeLeft = ui->NewLabel(this->pnlStatus, CSTR("Time Left"));
	this->lblStatusTimeLeft->SetRect(660, 24, 100, 23, false);
	this->txtStatusTimeLeft = ui->NewTextBox(this->pnlStatus, CSTR(""));
	this->txtStatusTimeLeft->SetRect(760, 24, 100, 23, false);
	this->txtStatusTimeLeft->SetReadOnly(true);
	this->rlcStatus = ui->NewRealtimeLineChart(this->tpStatus, this->core->GetDrawEngine(), 1, 720, 1000);
	this->rlcStatus->SetRect(0, 0, 100, 360, false);
	this->rlcStatus->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->vspStatus = ui->NewVSplitter(this->tpStatus, 3, true);
	this->lvStatus = ui->NewListView(this->tpStatus, UI::ListViewStyle::Table, 2);
	this->lvStatus->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvStatus->SetFullRowSelect(true);
	this->lvStatus->AddColumn(CSTR("Source File"), 200);
	this->lvStatus->AddColumn(CSTR("Status"), 200);
	this->lvStatus->HandleDblClk(OnStatusDblClick, this);

	this->AddTimer(500, OnTimerTick, this);
	Sync::ThreadUtil::Create(ProcessThread, this);

	if (this->packFile->GetFileType() == IO::PackageFileType::Virtual)
	{
		this->tpInfo = this->tcMain->AddTabPage(CSTR("Info"));
		this->txtInfo = ui->NewTextBox(this->tpInfo, CSTR(""), true);
		this->txtInfo->SetReadOnly(true);
		this->txtInfo->SetDockType(UI::GUIControl::DOCK_FILL);
		Text::StringBuilderUTF8 sb;
		NN<IO::VirtualPackageFile>::ConvertFrom(this->packFile)->GetInfoText(sb);
		this->txtInfo->SetText(sb.ToCString());
	}

	this->tpTest = this->tcMain->AddTabPage(CSTR("Test Result"));
	this->txtTest = ui->NewTextBox(this->tpTest, CSTR(""), true);
	this->txtTest->SetReadOnly(true);
	this->txtTest->SetDockType(UI::GUIControl::DOCK_FILL);

	this->SetMenu(mnuMain);
	this->HandleDropFiles(OnFiles, this);
}

SSWR::AVIRead::AVIRPackageForm::~AVIRPackageForm()
{
	this->threadToStop = true;
	this->threadEvt.Set();
	while (this->threadRunning)
	{
		Sync::SimpleThread::Sleep(10);
	}
	if (this->packNeedDelete)
	{
		this->packFile.Delete();
	}
	SDEL_CLASS(this->rootPackFile);
	this->fileNames.FreeAll();
	OPTSTR_DEL(this->statusFile);
	this->mnuPopup.Delete();
	OPTSTR_DEL(this->progName);
	OPTSTR_DEL(this->initSel);
}

void SSWR::AVIRead::AVIRPackageForm::EventMenuClicked(UInt16 cmdId)
{
	switch (cmdId)
	{
	case MNU_PASTE:
		{
			UI::Clipboard clipboard(this->hwnd);
			Data::ArrayListStringNN fileNames;
			UI::Clipboard::FilePasteType fpt;
			fpt = clipboard.GetDataFiles(&fileNames);
			this->PasteFiles(fileNames, fpt == UI::Clipboard::FPT_MOVE);
			clipboard.FreeDataFiles(&fileNames);
		}
		break;
	case MNU_COPYTO:
		{
			Data::ArrayList<UOSInt> selIndices;
			this->lvFiles->GetSelectedIndices(&selIndices);
			UOSInt i = selIndices.GetCount();
			if (i > 0)
			{
				NN<UI::GUIFolderDialog> dlg = this->ui->NewFolderDialog();
				if (dlg->ShowDialog(this->GetHandle()) == UI::GUIForm::DR_OK)
				{
					UOSInt j;
					j = 0;
					while (j < i)
					{
						if (!packFile->CopyTo(PackFileIndex(selIndices.GetItem(j)), dlg->GetFolder()->ToCString(), false))
						{
							this->ui->ShowMsgOK(CSTR("Error in copying"), CSTR("Copy To"), this);
							break;
						}
						j++;
					}
				}
				dlg.Delete();
			}
		}
		break;
	case MNU_COPYALLTO:
		if (packFile->GetCount() > 0)
		{
			NN<UI::GUIFolderDialog> dlg = this->ui->NewFolderDialog();
			if (dlg->ShowDialog(this->GetHandle()) == UI::GUIForm::DR_OK)
			{
				UOSInt i = 0;
				UOSInt j = packFile->GetCount();
				while (i < j)
				{
					if (!packFile->CopyTo(i, dlg->GetFolder()->ToCString(), false))
					{
						this->ui->ShowMsgOK(CSTR("Error in copying"), CSTR("Copy To"), this);
						break;
					}
					i++;
				}
			}
			dlg.Delete();
		}
		break;
	case MNU_SAVEAS:
		{
			this->core->SaveData(this, this->packFile, L"PackageSave");
		}
		break;
	case MNU_OPEN_HEX_VIEWER:
		{
			UOSInt i = this->PackFileIndex(this->lvFiles->GetSelectedIndex());
			if (i != INVALID_INDEX)
			{
				IO::PackageFile::PackObjectType pot = packFile->GetItemType(i);
				if (pot == IO::PackageFile::PackObjectType::PackageFileType)
				{
					this->ui->ShowMsgOK(CSTR("Viewing directory is not supported"), CSTR("Package"), this);
					return;
				}
				else if (pot == IO::PackageFile::PackObjectType::StreamData)
				{
					NN<IO::StreamData> fd;
					if (!packFile->GetItemStmDataNew(i).SetTo(fd))
					{
						this->ui->ShowMsgOK(CSTR("Error in opening file"), CSTR("Package"), this);
						return;
					}
					else
					{
						NN<SSWR::AVIRead::AVIRHexViewerForm> frm;
						NEW_CLASSNN(frm, SSWR::AVIRead::AVIRHexViewerForm(0, this->ui, this->core));
						frm->SetData(fd, IO::FileAnalyse::IFileAnalyse::AnalyseFile(fd));
						fd.Delete();
						this->core->ShowForm(frm);
					}
				}
				else
				{
					this->ui->ShowMsgOK(CSTR("Viewing of this file is not supported"), CSTR("Package"), this);
					return;
				}
			}
		}
		break;
	case MNU_OPEN_TEXT_VIEWER:
		{
			UOSInt i = this->PackFileIndex(this->lvFiles->GetSelectedIndex());
			if (i != INVALID_INDEX)
			{
				IO::PackageFile::PackObjectType pot = packFile->GetItemType(i);
				if (pot == IO::PackageFile::PackObjectType::PackageFileType)
				{
					this->ui->ShowMsgOK(CSTR("Viewing directory is not supported"), CSTR("Package"), this);
					return;
				}
				else if (pot == IO::PackageFile::PackObjectType::StreamData)
				{
					NN<IO::StreamData> fd;
					if (!packFile->GetItemStmDataNew(i).SetTo(fd))
					{
						this->ui->ShowMsgOK(CSTR("Error in opening file"), CSTR("Package"), this);
						return;
					}
					else
					{
						UInt8 buff[2048];
						UOSInt buffSize = fd->GetRealData(0, 2048, BYTEARR(buff));
						if (buffSize == 0)
						{
							this->ui->ShowMsgOK(CSTR("Cannot read content of the file"), CSTR("Package"), this);
							fd.Delete();
							return;
						}
						if (!Text::StringTool::IsASCIIText(Data::ByteArrayR(buff, buffSize)))
						{
							this->ui->ShowMsgOK(CSTR("The file seems not a text file"), CSTR("Package"), this);
							fd.Delete();
							return;
						}
						NN<UtilUI::TextViewerForm> frm;
						NEW_CLASSNN(frm, UtilUI::TextViewerForm(0, this->ui, this->core->GetMonitorMgr(), this->core->GetDrawEngine(), 0));
						frm->LoadStreamData(fd);
						fd.Delete();
						this->core->ShowForm(frm);
					}
				}
				else
				{
					this->ui->ShowMsgOK(CSTR("Viewing of this file is not supported"), CSTR("Package"), this);
					return;
				}
			}
		}
		break;
	case MNU_DELETE_ITEM:
		{
			UOSInt i = this->PackFileIndex(this->lvFiles->GetSelectedIndex());
			UTF8Char sbuff[512];
			UnsafeArray<UTF8Char> sptr;
			if (i != INVALID_INDEX && this->packFile->GetItemName(sbuff, i).SetTo(sptr))
			{
				Text::StringBuilderUTF8 sb;
				sb.Append(CSTR("Are you sure to delete \""));
				sb.AppendP(sbuff, sptr);
				sb.Append(CSTR("\"?"));
				if (this->ui->ShowMsgYesNo(sb.ToCString(), CSTR("Package"), this))
				{
					if (!this->packFile->DeleteItem(i))
					{
						this->ui->ShowMsgOK(CSTR("Cannot delete item"), CSTR("Package"), this);
					}
					else
					{
						this->DisplayPackFile(packFile);
					}
				}
			}
		}
		break;
	case MNU_APPEND_ZIP:
		if (this->packFile->GetFileType() == IO::PackageFileType::Virtual)
		{
			NN<IO::VirtualPackageFile> vpkg = NN<IO::VirtualPackageFile>::ConvertFrom(this->packFile);
			NN<Parser::ParserList> parsers = this->core->GetParserList();
			NN<UI::GUIFileDialog> dlg = this->ui->NewFileDialog(L"SSWR", L"AVIRead", L"PackageFileZip", false);
			dlg->SetAllowMultiSel(false);
			parsers->PrepareSelector(dlg, IO::ParserType::PackageFile);
			if (dlg->ShowDialog(this->GetHandle()))
			{
				IO::StmData::FileData fd(dlg->GetFileName(), false);
				NN<IO::PackageFile> zipPkg;
				if (Optional<IO::PackageFile>::ConvertFrom(parsers->ParseFileType(fd, IO::ParserType::PackageFile)).SetTo(zipPkg))
				{
					if (!vpkg->MergePackage(zipPkg))
					{
						this->ui->ShowMsgOK(CSTR("Error occurs during append zip file"), CSTR("Package"), this);
					}
					zipPkg.Delete();
					this->DisplayPackFile(this->packFile);
				}
				else
				{
					this->ui->ShowMsgOK(CSTR("Cannot parse file as Zip"), CSTR("Package"), this);
				}
			}
			dlg.Delete();
		}
		else
		{
			this->ui->ShowMsgOK(CSTR("This directory is not supported to append zip content"), CSTR("Package"), this);
		}
		break;
	case MNU_TEST:
		{
			Text::StringBuilderUTF8 sbError;
			ReadSession sess;
			IO::ActiveStreamReader reader(AVIRPackageForm_TestHandler, &sess, 1048576);
			sess.currHash = Crypto::Hash::HashType::CRC32R_IEEE;
			sess.hash = Crypto::Hash::HashCreator().CreateHash(sess.currHash);
			sess.fileReadSize = 0;
			sess.fileCnt = 0;
			sess.dirCnt = 0;
			sess.errorCnt = 0;
			sess.totalSizeUncomp = 0;
			sess.totalCompSize = 0;
			sess.totalSize = 0;
			sess.totalStoreSize = 0;
			sess.totalDurUncomp = 0;
			sess.totalDurComp = 0;
			sess.slowestSpeedUncomp = 0;
			sess.slowestSpeedComp = 0;
			sess.sbError = &sbError;
			Data::Timestamp startTime = Data::Timestamp::Now();
			TestPackage(reader, sess, this->packFile);
			Data::Timestamp endTime = Data::Timestamp::Now();
			Text::StringBuilderUTF8 sb;
			if (sess.errorCnt == 0)
			{
				sb.Append(CSTR("Files Read ok\r\n"));
			}
			else
			{
				sb.Append(CSTR("Files Read error found: "))->AppendUOSInt(sess.errorCnt)->Append(CSTR("\r\n"));
				sb.Append(sbError);
			}
			sb.Append(CSTR("\r\n"));
			Data::Duration totalTime = endTime.Diff(startTime);
			sb.Append(CSTR("Start Time = "))->AppendTSNoZone(startTime)->Append(CSTR("\r\n"));
			sb.Append(CSTR("End Time = "))->AppendTSNoZone(endTime)->Append(CSTR("\r\n"));
			sb.Append(CSTR("Total Time = "))->AppendDur(totalTime)->Append(CSTR("\r\n"));
			sb.Append(CSTR("File Count = "))->AppendUOSInt(sess.fileCnt)->Append(CSTR("\r\n"));
			sb.Append(CSTR("Dir Count = "))->AppendUOSInt(sess.dirCnt)->Append(CSTR("\r\n"));
			sb.Append(CSTR("Total decompressed size = "))->AppendU64(sess.totalSize)->Append(CSTR("\r\n"));
			sb.Append(CSTR("Total store size = "))->AppendU64(sess.totalStoreSize)->Append(CSTR("\r\n"));
			sb.Append(CSTR("Average Rate = "))->AppendDouble(UInt64_Double(sess.totalSize) / totalTime.GetTotalSec())->Append(CSTR("\r\n"));
			sb.Append(CSTR("Total size of Uncomp file = "))->AppendU64(sess.totalSizeUncomp)->Append(CSTR("\r\n"));
			sb.Append(CSTR("Total size of Comp file = "))->AppendU64(sess.totalCompSize)->Append(CSTR("\r\n"));
			if (!sess.totalDurUncomp.IsZero())
			{
				sb.Append(CSTR("Average Uncomp Rate = "))->AppendDouble(UInt64_Double(sess.totalSizeUncomp) / sess.totalDurUncomp.GetTotalSec())->Append(CSTR("\r\n"));
				sb.Append(CSTR("Slowest Uncomp Rate = "))->AppendDouble(sess.slowestSpeedUncomp)->Append(CSTR("\r\n"));
			}
			if (!sess.totalDurComp.IsZero())
			{
				sb.Append(CSTR("Average Comp Rate = "))->AppendDouble(UInt64_Double(sess.totalCompSize) / sess.totalDurComp.GetTotalSec())->Append(CSTR("\r\n"));
				sb.Append(CSTR("Slowest Comp Rate = "))->AppendDouble(sess.slowestSpeedComp)->Append(CSTR("\r\n"));
			}
			this->txtTest->SetText(sb.ToCString());
			this->tcMain->SetSelectedPage(this->tpTest);
		}
		break;
	case MNU_GO_UP_LEVEL:
		this->GoUpLevel();
		break;
	case MNU_OPEN_SELECTED:
		{
			UOSInt i = this->lvFiles->GetSelectedIndex();
			if (i != INVALID_INDEX)
			{
				this->OpenItem(i);
			}
		}
		break;
	}
}

void SSWR::AVIRead::AVIRPackageForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

void SSWR::AVIRead::AVIRPackageForm::ProgressStart(Text::CString name, UInt64 count)
{
	{
		Sync::MutexUsage mutUsage(this->readMut);
		this->readLast = 0;
		this->readCurrFile = name;
		this->readFileCnt = count;
	}

	{
		Sync::MutexUsage mutUsage(this->statusFileMut);
		OPTSTR_DEL(this->statusFile);
		this->statusFile = Text::String::NewOrNull(name);
		this->statusFileSize = count;
		this->statusFileChg = true;
	}

	{
		Sync::MutexUsage mutUsage(this->progMut);
		this->progStarted = true;
		OPTSTR_DEL(this->progName);
		this->progName = Text::String::New(name.OrEmpty());
		this->progStartCnt = count;
		this->progEnd = false;
	}
}

void SSWR::AVIRead::AVIRPackageForm::ProgressUpdate(UInt64 currCount, UInt64 newCount)
{
	{
		Sync::MutexUsage mutUsage(this->readMut);
		UInt64 readThis = currCount - this->readLast;
		if ((Int64)readThis < 0)
			readThis = 0;
		this->readCurr += readThis;
		this->readTotal += readThis;
		this->readLast = currCount;
		this->readReadTime.SetCurrTimeUTC();
	}

	{
		Sync::MutexUsage mutUsage(this->progMut);
		this->progUpdated = true;
		this->progUpdateCurr = currCount;
		this->progUpdateNew = newCount;
		this->progEnd = false;
	}
}

void SSWR::AVIRead::AVIRPackageForm::ProgressEnd()
{
	this->readLast = 0;
	this->readFileCnt = 0;
	Sync::MutexUsage mutUsage(this->progMut);
	this->progEnd = true;
	mutUsage.EndUse();

	{
		Sync::MutexUsage mutUsage(this->statusFileMut);
		OPTSTR_DEL(this->statusFile);
		this->statusFileChg = true;
	}
}
