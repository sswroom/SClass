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
#include "UI/FileDialog.h"
#include "UI/FolderDialog.h"
#include "UI/MessageDialog.h"
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

void AVIRPackageForm_TestHandler(const UInt8 *buff, UOSInt buffSize, void *userData)
{
	SSWR::AVIRead::AVIRPackageForm::ReadSession *sess = (SSWR::AVIRead::AVIRPackageForm::ReadSession*)userData;
	if (sess->hash)
	{
		sess->hash->Calc(buff, buffSize);
	}
	sess->fileReadSize += buffSize;
}

UInt32 __stdcall SSWR::AVIRead::AVIRPackageForm::ProcessThread(void *userObj)
{
	SSWR::AVIRead::AVIRPackageForm *me = (SSWR::AVIRead::AVIRPackageForm*)userObj;
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	Text::String *fname = 0;
	Text::CStringNN fileName;
	ActionType atype = AT_COPY;
	UOSInt i;
	UOSInt j;
	Bool found;
	Bool lastFound = false;
	me->threadRunning = true;
	while (!me->threadToStop)
	{
		found = false;
		Sync::MutexUsage mutUsage(me->fileMut);
		i = 0;
		j = me->fileNames.GetCount();
		while (!found && i < j)
		{
			fname = me->fileNames.GetItem(i);
			atype = me->fileAction.GetItem(i);
			switch (atype)
			{
			case AT_COPY:
				found = true;
				break;
			case AT_MOVE:
				found = true;
				break;
			case AT_RETRYCOPY:
				found = true;
				break;
			case AT_RETRYMOVE:
				found = true;
				break;
			case AT_DELETE:
			case AT_DELETEFAIL:
			case AT_COPYFAIL:
			case AT_MOVEFAIL:
			case AT_SUCCEED:
			default:
				break;
			}
			i++;
		}
		mutUsage.EndUse();
		if (found)
		{
		
			if (fname->StartsWith(UTF8STRC("file:///")))
			{
				sptr = Text::URLString::GetURLFilePath(sbuff, fname->v, fname->leng);
				fileName = CSTRP(sbuff, sptr);
			}
			else
			{
				fileName = fname->ToCString();
			}
			lastFound = true;
			if (atype == AT_COPY)
			{
				found = me->packFile->CopyFrom(fileName, me, me->statusBNT);
				mutUsage.BeginUse();
				i = me->fileNames.GetCount();
				while (i-- > 0)
				{
					if (me->fileNames.GetItem(i) == fname)
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
				found = me->packFile->MoveFrom(fileName, me, me->statusBNT);
				mutUsage.BeginUse();
				i = me->fileNames.GetCount();
				while (i-- > 0)
				{
					if (me->fileNames.GetItem(i) == fname)
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
				found = me->packFile->RetryCopyFrom(fileName, me, me->statusBNT);
				mutUsage.BeginUse();
				i = me->fileNames.GetCount();
				while (i-- > 0)
				{
					if (me->fileNames.GetItem(i) == fname)
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
				found = me->packFile->RetryCopyFrom(fileName, me, me->statusBNT);
				mutUsage.BeginUse();
				i = me->fileNames.GetCount();
				while (i-- > 0)
				{
					if (me->fileNames.GetItem(i) == fname)
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

void __stdcall SSWR::AVIRead::AVIRPackageForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRPackageForm *me = (SSWR::AVIRead::AVIRPackageForm*)userObj;
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	if (me->statusChg)
	{
		UOSInt i;
		UOSInt j;
		UOSInt k;
		OSInt scrVPos = me->lvStatus->GetScrollVPos();
		Text::String *fname;
		me->statusChg = false;
		me->lvStatus->ClearItems();
		Sync::MutexUsage mutUsage(me->fileMut);
		i = 0;
		j = me->fileNames.GetCount();
		while (i < j)
		{
			fname = me->fileNames.GetItem(i);
			k = fname->LastIndexOf(IO::Path::PATH_SEPERATOR);
			k = me->lvStatus->AddItem(fname->ToCString().Substring(k + 1), (void*)fname->v);
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
	if (me->progStarted)
	{
		me->progStarted = false;
		me->prgStatus->ProgressStart(me->progName->ToCString(), me->progStartCnt);
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
		Sync::MutexUsage mutUsage(me->statusFileMut);
		hasFile = (me->statusFile != 0);
		if (hasFile)
		{
			me->txtStatusFile->SetText(me->statusFile->ToCString());
			sptr = Text::StrUInt64(sbuff, me->statusFileSize);
			me->txtStatusFileSize->SetText(CSTRP(sbuff, sptr));
			fileSize = me->statusFileSize;
		}
		else
		{
			me->txtStatusFile->SetText(CSTR(""));
			me->txtStatusFileSize->SetText(CSTR(""));
		}
		mutUsage.EndUse();
	}
	else
	{
		Sync::MutexUsage mutUsage(me->statusFileMut);
		hasFile = (me->statusFile != 0);
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

void __stdcall SSWR::AVIRead::AVIRPackageForm::LVDblClick(void *userObj, UOSInt index)
{
	SSWR::AVIRead::AVIRPackageForm *me = (SSWR::AVIRead::AVIRPackageForm*)userObj;
	me->OpenItem(index);
}

void __stdcall SSWR::AVIRead::AVIRPackageForm::OnStatusDblClick(void *userObj, UOSInt index)
{
	SSWR::AVIRead::AVIRPackageForm *me = (SSWR::AVIRead::AVIRPackageForm*)userObj;
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

void __stdcall SSWR::AVIRead::AVIRPackageForm::OnFilesRightClick(void *userObj, Math::Coord2DDbl coord, UOSInt index)
{
	SSWR::AVIRead::AVIRPackageForm *me = (SSWR::AVIRead::AVIRPackageForm*)userObj;
	me->mnuPopup->ShowMenu(me->lvFiles, Math::Coord2D<OSInt>(Double2OSInt(coord.x), Double2OSInt(coord.y)));
}

void __stdcall SSWR::AVIRead::AVIRPackageForm::OnFiles(void *userObj, NotNullPtr<Text::String> *files, UOSInt nFiles)
{
	SSWR::AVIRead::AVIRPackageForm *me = (SSWR::AVIRead::AVIRPackageForm*)userObj;
	Data::ArrayListNN<Text::String> fileNames(nFiles);
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
	NotNullPtr<IO::PackageFile> pkg;
	if (pkg.Set(this->packFile->GetParent(needRelease)))
	{
		NotNullPtr<Text::String> name = this->packFile->GetSourceNameObj();
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
		NotNullPtr<IO::PackageFile> pkg;
		if (pkg.Set(this->packFile->GetItemPack(index, needRelease)))
		{
			this->UpdatePackFile(pkg, needRelease, CSTR_NULL);
//			this->core->OpenObject(pkg);
		}
	}
	else if (pot == IO::PackageFile::PackObjectType::ParsedObject)
	{
		Bool needRelease;
		NotNullPtr<IO::ParsedObject> pobj;
		if (pobj.Set(this->packFile->GetItemPObj(index, needRelease)))
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
		NotNullPtr<IO::StreamData> data;
		if (data.Set(this->packFile->GetItemStmDataNew(index)))
		{
			this->core->LoadData(data, this->packFile.Ptr());
			data.Delete();
		}
	}
}

void SSWR::AVIRead::AVIRPackageForm::TestPackage(NotNullPtr<IO::ActiveStreamReader> reader, NotNullPtr<ReadSession> sess, NotNullPtr<IO::PackageFile> pack)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	NotNullPtr<IO::PackageFile> innerPack;
	NotNullPtr<IO::StreamData> stmData;
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
			if (innerPack.Set(pack->GetItemPack(i, needDelete)))
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
				sptr = pack->GetItemName(sbuff, i);
				if (sptr)
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
			if (stmData.Set(pack->GetItemStmDataNew(i)))
			{
				storeSize = pack->GetItemStoreSize(i);
				fileSize = stmData->GetDataSize();
				sess->fileReadSize = 0;
				sess->totalSize += fileSize;
				sess->totalStoreSize += storeSize;
				if (pack->GetItemComp(i) == Data::Compress::Decompressor::CM_UNCOMPRESSED)
				{
					sess->totalSizeUncomp += fileSize;
					if (sess->hash) sess->hash->Clear();
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
						sptr = pack->GetItemName(sbuff, i);
						if (sptr)
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
					if (sess->hash) sess->hash->Clear();
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
						sptr = pack->GetItemName(sbuff, i);
						if (sptr)
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
				sptr = pack->GetItemName(sbuff, i);
				if (sptr)
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
			sptr = pack->GetItemName(sbuff, i);
			if (sptr)
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

void SSWR::AVIRead::AVIRPackageForm::DisplayPackFile(NotNullPtr<IO::PackageFile> packFile)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
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
	i = 0;
	j = packFile->GetCount();
	while (i < j)
	{
		sptr = packFile->GetItemName(sbuff, i);
		pot = packFile->GetItemType(i);
		k = this->lvFiles->AddItem(CSTRP(sbuff, sptr), (void*)i);
		if (this->initSel && this->initSel->Equals(CSTRP(sbuff, sptr)))
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

void SSWR::AVIRead::AVIRPackageForm::UpdatePackFile(NotNullPtr<IO::PackageFile> packFile, Bool needDelete, Text::CString initSel)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	sptr = Text::StrConcatC(sbuff, UTF8STRC("Package Form - "));
	if (this->rootPackFile && packFile.Ptr() != this->rootPackFile)
	{
		sptr = this->rootPackFile->GetSourceNameObj()->ConcatTo(sptr);
		*sptr++ = '/';
	}
	sptr = packFile->GetSourceNameObj()->ConcatTo(sptr);
	this->SetText(CSTRP(sbuff, sptr));
	SDEL_STRING(this->initSel);
	if (!initSel.IsNull())
	{
		this->initSel = Text::String::New(initSel).Ptr();
	}
	if (this->packNeedDelete)
	{
		this->packFile.Delete();
	}
	this->packFile = packFile;
	this->packNeedDelete = needDelete;
	this->DisplayPackFile(packFile);
}

void SSWR::AVIRead::AVIRPackageForm::PasteFiles(NotNullPtr<Data::ArrayListNN<Text::String>> files, Bool move)
{
	UOSInt i;
	UOSInt j;
	if (move)
	{
		if (this->packFile->GetFileType() == IO::PackageFileType::Virtual)
		{
			UI::MessageDialog::ShowDialog(CSTR("Virtual Package does not support move"), CSTR("Package"), this);
		}
		else
		{
			Sync::MutexUsage mutUsage(this->fileMut);
			i = 0;
			j = files->GetCount();
			while (i < j)
			{
				this->fileNames.Add(files->GetItem(i)->Clone());
				this->fileAction.Add(AT_MOVE);
				i++;
			}
			this->statusChg = true;
		}
	}
	else
	{
		if (this->packFile->GetFileType() == IO::PackageFileType::Virtual)
		{
			Bool changed = false;
			i = 0;
			j = files->GetCount();
			while (i < j)
			{
				if (!this->packFile->CopyFrom(files->GetItem(i)->ToCString(), 0, 0))
				{
					Text::StringBuilderUTF8 sb;
					sb.Append(CSTR("Failed to copy "));
					sb.Append(files->GetItem(i));
					sb.Append(CSTR(", do you want to continue?"));
					if (!UI::MessageDialog::ShowYesNoDialog(sb.ToCString(), CSTR("Package"), this))
					{
						break;
					}
				}
				else
				{
					changed = true;
				}
				i++;
			}
			if (changed)
			{
				this->DisplayPackFile(this->packFile);
			}
		}
		else
		{
			Sync::MutexUsage mutUsage(this->fileMut);
			i = 0;
			j = files->GetCount();
			while (i < j)
			{
				this->fileNames.Add(files->GetItem(i)->Clone());
				this->fileAction.Add(AT_COPY);
				i++;
			}
			this->statusChg = true;
		}
	}

}

SSWR::AVIRead::AVIRPackageForm::AVIRPackageForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, NotNullPtr<IO::PackageFile> packFile) : UI::GUIForm(parent, 960, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	UTF8Char sbuff[512];
	UTF8Char *sptr;
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
	
/*	NEW_CLASS(this->pnlCtrl, UI::GUIPanel(ui, this));
	this->pnlCtrl->SetRect(0, 0, 100, 28, false);
	this->pnlCtrl->SetDockType(UI::GUIControl::DOCK_TOP);*/
	NEW_CLASS(this->prgStatus, UI::GUIProgressBar(ui, this, 1));
	this->prgStatus->SetRect(0, 0, 100, 23, false);
	this->prgStatus->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);


	this->tpFiles = this->tcMain->AddTabPage(CSTR("Files"));
	NEW_CLASS(this->lvFiles, UI::GUIListView(ui, this->tpFiles, UI::GUIListView::LVSTYLE_TABLE, 6));
	this->lvFiles->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvFiles->AddColumn(CSTR("Item Name"), 150);
	this->lvFiles->AddColumn(CSTR("Type"), 80);
	this->lvFiles->AddColumn(CSTR("Store Size"), 100);
	this->lvFiles->AddColumn(CSTR("File Size"), 100);
	this->lvFiles->AddColumn(CSTR("Modify Time"), 180);
	this->lvFiles->AddColumn(CSTR("Compression"), 100);

	this->lvFiles->HandleDblClk(LVDblClick, this);
	DisplayPackFile(this->packFile);

	UI::GUIMainMenu *mnuMain;
	UI::GUIMenu *mnu;
//	UI::GUIMenu *mnu2;
	NEW_CLASS(mnuMain, UI::GUIMainMenu());
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

	NEW_CLASS(this->mnuPopup, UI::GUIPopupMenu());
	this->mnuPopup->AddItem(CSTR("Copy To..."), MNU_COPYTO, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->mnuPopup->AddItem(CSTR("Open Text Viewer"), MNU_OPEN_TEXT_VIEWER, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->mnuPopup->AddItem(CSTR("Open Hex Viewer"), MNU_OPEN_HEX_VIEWER, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->mnuPopup->AddItem(CSTR("Delete Item"), MNU_DELETE_ITEM, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->lvFiles->HandleRightClick(OnFilesRightClick, this);

	this->tpStatus = this->tcMain->AddTabPage(CSTR("Status"));
	NEW_CLASS(this->pnlStatus, UI::GUIPanel(ui, this->tpStatus));
	this->pnlStatus->SetRect(0, 0, 100, 48, false);
	this->pnlStatus->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblStatusFile, UI::GUILabel(ui, this->pnlStatus, CSTR("Copy From")));
	this->lblStatusFile->SetRect(0, 0, 100, 23, false);
	NEW_CLASS(this->txtStatusFile, UI::GUITextBox(ui, this->pnlStatus, CSTR("")));
	this->txtStatusFile->SetRect(100, 0, 800, 23, false);
	this->txtStatusFile->SetReadOnly(true);
	NEW_CLASS(this->pnlStatusBNT, UI::GUIPanel(ui, this->pnlStatus));
	this->pnlStatusBNT->SetBGColor(0xffc0c0c0);
	this->pnlStatusBNT->SetRect(900, 0, 23, 23, false);
	NEW_CLASS(this->lblStatusFileSize, UI::GUILabel(ui, this->pnlStatus, CSTR("File Size")));
	this->lblStatusFileSize->SetRect(0, 24, 100, 23, false);
	NEW_CLASS(this->txtStatusFileSize, UI::GUITextBox(ui, this->pnlStatus, CSTR("")));
	this->txtStatusFileSize->SetRect(100, 24, 100, 23, false);
	this->txtStatusFileSize->SetReadOnly(true);
	NEW_CLASS(this->lblStatusCurrSize, UI::GUILabel(ui, this->pnlStatus, CSTR("Curr Size")));
	this->lblStatusCurrSize->SetRect(220, 24, 100, 23, false);
	NEW_CLASS(this->txtStatusCurrSize, UI::GUITextBox(ui, this->pnlStatus, CSTR("")));
	this->txtStatusCurrSize->SetRect(320, 24, 100, 23, false);
	this->txtStatusCurrSize->SetReadOnly(true);
	NEW_CLASS(this->lblStatusCurrSpeed, UI::GUILabel(ui, this->pnlStatus, CSTR("Curr Speed")));
	this->lblStatusCurrSpeed->SetRect(440, 24, 100, 23, false);
	NEW_CLASS(this->txtStatusCurrSpeed, UI::GUITextBox(ui, this->pnlStatus, CSTR("")));
	this->txtStatusCurrSpeed->SetRect(540, 24, 100, 23, false);
	this->txtStatusCurrSpeed->SetReadOnly(true);
	NEW_CLASS(this->lblStatusTimeLeft, UI::GUILabel(ui, this->pnlStatus, CSTR("Time Left")));
	this->lblStatusTimeLeft->SetRect(660, 24, 100, 23, false);
	NEW_CLASS(this->txtStatusTimeLeft, UI::GUITextBox(ui, this->pnlStatus, CSTR("")));
	this->txtStatusTimeLeft->SetRect(760, 24, 100, 23, false);
	this->txtStatusTimeLeft->SetReadOnly(true);
	NEW_CLASS(this->rlcStatus, UI::GUIRealtimeLineChart(ui, this->tpStatus, this->core->GetDrawEngine(), 1, 720, 1000));
	this->rlcStatus->SetRect(0, 0, 100, 360, false);
	this->rlcStatus->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->vspStatus, UI::GUIVSplitter(ui, this->tpStatus, 3, true));
	NEW_CLASS(this->lvStatus, UI::GUIListView(ui, this->tpStatus, UI::GUIListView::LVSTYLE_TABLE, 2));
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
		NEW_CLASS(this->txtInfo, UI::GUITextBox(ui, this->tpInfo, CSTR(""), true));
		this->txtInfo->SetReadOnly(true);
		this->txtInfo->SetDockType(UI::GUIControl::DOCK_FILL);
		Text::StringBuilderUTF8 sb;
		NotNullPtr<IO::VirtualPackageFile>::ConvertFrom(this->packFile)->GetInfoText(sb);
		this->txtInfo->SetText(sb.ToCString());
	}

	this->tpTest = this->tcMain->AddTabPage(CSTR("Test Result"));
	NEW_CLASS(this->txtTest, UI::GUITextBox(ui, this->tpTest, CSTR(""), true));
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
	LIST_FREE_STRING(&this->fileNames);
	SDEL_STRING(this->statusFile);
	DEL_CLASS(this->mnuPopup);
	SDEL_STRING(this->progName);
	SDEL_STRING(this->initSel);
}

void SSWR::AVIRead::AVIRPackageForm::EventMenuClicked(UInt16 cmdId)
{
	switch (cmdId)
	{
	case MNU_PASTE:
		{
			UI::Clipboard clipboard(this->hwnd);
			Data::ArrayListNN<Text::String> fileNames;
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
				UI::FolderDialog dlg(L"SSWR", L"AVIRead", L"PackageCopyTo");
				if (dlg.ShowDialog(this->GetHandle()) == UI::GUIForm::DR_OK)
				{
					UOSInt j;
					j = 0;
					while (j < i)
					{
						if (!packFile->CopyTo(PackFileIndex(selIndices.GetItem(j)), dlg.GetFolder()->ToCString(), false))
						{
							UI::MessageDialog::ShowDialog(CSTR("Error in copying"), CSTR("Copy To"), this);
							break;
						}
						j++;
					}
				}
			}
		}
		break;
	case MNU_COPYALLTO:
		if (packFile->GetCount() > 0)
		{
			UI::FolderDialog dlg(L"SSWR", L"AVIRead", L"PackageCopyAllTo");
			if (dlg.ShowDialog(this->GetHandle()) == UI::GUIForm::DR_OK)
			{
				UOSInt i = 0;
				UOSInt j = packFile->GetCount();
				while (i < j)
				{
					if (!packFile->CopyTo(i, dlg.GetFolder()->ToCString(), false))
					{
						UI::MessageDialog::ShowDialog(CSTR("Error in copying"), CSTR("Copy To"), this);
						break;
					}
					i++;
				}
			}
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
					UI::MessageDialog::ShowDialog(CSTR("Viewing directory is not supported"), CSTR("Package"), this);
					return;
				}
				else if (pot == IO::PackageFile::PackObjectType::StreamData)
				{
					NotNullPtr<IO::StreamData> fd;
					if (!fd.Set(packFile->GetItemStmDataNew(i)))
					{
						UI::MessageDialog::ShowDialog(CSTR("Error in opening file"), CSTR("Package"), this);
						return;
					}
					else
					{
						SSWR::AVIRead::AVIRHexViewerForm *frm;
						NEW_CLASS(frm, SSWR::AVIRead::AVIRHexViewerForm(0, this->ui, this->core));
						frm->SetData(fd, IO::FileAnalyse::IFileAnalyse::AnalyseFile(fd));
						fd.Delete();
						this->core->ShowForm(frm);
					}
				}
				else
				{
					UI::MessageDialog::ShowDialog(CSTR("Viewing of this file is not supported"), CSTR("Package"), this);
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
					UI::MessageDialog::ShowDialog(CSTR("Viewing directory is not supported"), CSTR("Package"), this);
					return;
				}
				else if (pot == IO::PackageFile::PackObjectType::StreamData)
				{
					NotNullPtr<IO::StreamData> fd;
					if (!fd.Set(packFile->GetItemStmDataNew(i)))
					{
						UI::MessageDialog::ShowDialog(CSTR("Error in opening file"), CSTR("Package"), this);
						return;
					}
					else
					{
						UInt8 buff[2048];
						UOSInt buffSize = fd->GetRealData(0, 2048, BYTEARR(buff));
						if (buffSize == 0)
						{
							UI::MessageDialog::ShowDialog(CSTR("Cannot read content of the file"), CSTR("Package"), this);
							fd.Delete();
							return;
						}
						if (!Text::StringTool::IsASCIIText(Data::ByteArrayR(buff, buffSize)))
						{
							UI::MessageDialog::ShowDialog(CSTR("The file seems not a text file"), CSTR("Package"), this);
							fd.Delete();
							return;
						}
						UtilUI::TextViewerForm *frm;
						NEW_CLASS(frm, UtilUI::TextViewerForm(0, this->ui, this->core->GetMonitorMgr(), this->core->GetDrawEngine(), 0));
						frm->LoadStreamData(fd);
						fd.Delete();
						this->core->ShowForm(frm);
					}
				}
				else
				{
					UI::MessageDialog::ShowDialog(CSTR("Viewing of this file is not supported"), CSTR("Package"), this);
					return;
				}
			}
		}
		break;
	case MNU_DELETE_ITEM:
		{
			UOSInt i = this->PackFileIndex(this->lvFiles->GetSelectedIndex());
			if (i != INVALID_INDEX)
			{
				UTF8Char sbuff[512];
				UTF8Char *sptr;
				sptr = this->packFile->GetItemName(sbuff, i);
				Text::StringBuilderUTF8 sb;
				sb.Append(CSTR("Are you sure to delete \""));
				sb.AppendP(sbuff, sptr);
				sb.Append(CSTR("\"?"));
				if (UI::MessageDialog::ShowYesNoDialog(sb.ToCString(), CSTR("Package"), this))
				{
					if (!this->packFile->DeleteItem(i))
					{
						UI::MessageDialog::ShowDialog(CSTR("Cannot delete item"), CSTR("Package"), this);
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
			NotNullPtr<IO::VirtualPackageFile> vpkg = NotNullPtr<IO::VirtualPackageFile>::ConvertFrom(this->packFile);
			Parser::ParserList *parsers = this->core->GetParserList();
			UI::FileDialog dlg(L"SSWR", L"AVIRead", L"PackageFileZip", false);
			dlg.SetAllowMultiSel(false);
			parsers->PrepareSelector(dlg, IO::ParserType::PackageFile);
			if (dlg.ShowDialog(this->GetHandle()))
			{
				IO::StmData::FileData fd(dlg.GetFileName(), false);
				NotNullPtr<IO::PackageFile> zipPkg;
				if (zipPkg.Set((IO::PackageFile*)parsers->ParseFileType(fd, IO::ParserType::PackageFile)))
				{
					if (!vpkg->MergePackage(zipPkg))
					{
						UI::MessageDialog::ShowDialog(CSTR("Error occurs during append zip file"), CSTR("Package"), this);
					}
					zipPkg.Delete();
					this->DisplayPackFile(this->packFile);
				}
				else
				{
					UI::MessageDialog::ShowDialog(CSTR("Cannot parse file as Zip"), CSTR("Package"), this);
				}
			}
		}
		else
		{
			UI::MessageDialog::ShowDialog(CSTR("This directory is not supported to append zip content"), CSTR("Package"), this);
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
			sb.Append(CSTR("Start Time = "))->AppendTS(startTime)->Append(CSTR("\r\n"));
			sb.Append(CSTR("End Time = "))->AppendTS(endTime)->Append(CSTR("\r\n"));
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
		SDEL_STRING(this->statusFile);
		this->statusFile = Text::String::New(name).Ptr();
		this->statusFileSize = count;
		this->statusFileChg = true;
	}

	{
		Sync::MutexUsage mutUsage(this->progMut);
		this->progStarted = true;
		SDEL_STRING(this->progName);
		this->progName = Text::String::New(name).Ptr();
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
		SDEL_STRING(this->statusFile);
		this->statusFileChg = true;
	}
}
