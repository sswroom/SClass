#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/FileUtil.h"
#include "IO/IniFile.h"
#include "IO/PackageFile.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Manage/Process.h"
#include "Net/OSSocketFactory.h"
#include "Net/SSLEngineFactory.h"
#include "Net/WebSite/WebSite48IdolControl.h"
#include "Parser/FullParserList.h"
#include "SSWR/DownloadMonitor/DownMonCore.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"
#include "Text/StringBuilderUTF8.h"
#include "Win32/Clipboard.h"

Bool SSWR::DownloadMonitor::DownMonCore::FFMPEGMux(const UTF8Char *videoFile, const UTF8Char *audioFile, const UTF8Char *outFile)
{
	UInt64 fileSize1 = IO::Path::GetFileSize(videoFile);
	UInt64 fileSize2 = IO::Path::GetFileSize(audioFile);
	if (fileSize1 < 1024)
		return false;
	if (fileSize2 < 1024)
		return false;
	fileSize1 += fileSize2;
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("\""));
	sb.Append(this->ffmpegPath);
	sb.AppendC(UTF8STRC("\" -i \""));
	sb.Append(videoFile);
	sb.AppendC(UTF8STRC("\" -i \""));
	sb.Append(audioFile);
	sb.AppendC(UTF8STRC("\" -map 0:0 -map 1:0 -c:a:0 copy -vcodec copy \""));
	sb.Append(outFile);
	sb.AppendC(UTF8STRC("\""));
	Manage::Process proc(sb.ToString());
	while (proc.IsRunning())
	{
		Sync::Thread::Sleep(100);
	}
	fileSize2 = IO::Path::GetFileSize(outFile);
	if ((Double)fileSize1 > (Double)fileSize2 * 0.9 && (Double)fileSize1 < (Double)fileSize2 * 1.1)
	{
		return true;
	}
	else
	{
		return false;
	}
}

Bool SSWR::DownloadMonitor::DownMonCore::FFMPEGMuxAAC(const UTF8Char *videoFile, const UTF8Char *audioFile, const UTF8Char *outFile)
{
	UInt64 fileSize1 = IO::Path::GetFileSize(videoFile);
	UInt64 fileSize2 = IO::Path::GetFileSize(audioFile);
	if (fileSize1 < 1024)
		return false;
	if (fileSize2 < 1024)
		return false;
	fileSize1 += fileSize2;
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("\""));
	sb.Append(this->ffmpegPath);
	sb.AppendC(UTF8STRC("\" -i \""));
	sb.Append(videoFile);
	sb.AppendC(UTF8STRC("\" -i \""));
	sb.Append(audioFile);
	sb.AppendC(UTF8STRC("\" -map 0:0 -map 1:0 -c:a:0 aac -vcodec copy \""));
	sb.Append(outFile);
	sb.AppendC(UTF8STRC("\""));
	Manage::Process proc(sb.ToString());
	while (proc.IsRunning())
	{
		Sync::Thread::Sleep(100);
	}
	fileSize2 = IO::Path::GetFileSize(outFile);
	if ((Double)fileSize1 > (Double)fileSize2 * 0.9 && (Double)fileSize1 < (Double)fileSize2 * 1.1)
	{
		return true;
	}
	else
	{
		return false;
	}
}

Bool SSWR::DownloadMonitor::DownMonCore::ExtractZIP(const UTF8Char *zipFile, const UTF8Char *mp4File)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	IO::StmData::FileData *fd;
	IO::PackageFile *pkgFile;
	Bool valid = false;
	NEW_CLASS(fd, IO::StmData::FileData(zipFile, false));
	pkgFile = (IO::PackageFile*)this->parsers->ParseFileType(fd, IO::ParserType::PackageFile);
	DEL_CLASS(fd);
	if (pkgFile)
	{
		if (pkgFile->GetCount() == 1)
		{
			sbuff[0] = 0;
			sptr = pkgFile->GetItemName(sbuff, 0);
			if (Text::StrEndsWithICaseC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC(".MP4")) && pkgFile->GetItemType(0) == IO::PackageFile::POT_STREAMDATA)
			{
				valid = pkgFile->CopyTo(0, mp4File, true);
			}
		}
		DEL_CLASS(pkgFile);
	}
	return valid;
}

Bool SSWR::DownloadMonitor::DownMonCore::VideoValid(const UTF8Char *fileName)
{
	IO::StmData::FileData *fd;
	Media::MediaFile *mediaFile;
	Bool valid = false;

	NEW_CLASS(fd, IO::StmData::FileData(fileName, false));
	mediaFile = (Media::MediaFile*)this->parsers->ParseFileType(fd, IO::ParserType::MediaFile);
	DEL_CLASS(fd);

	if (mediaFile)
	{
		valid = this->checker->IsValid(mediaFile);
		DEL_CLASS(mediaFile);
	}
	return valid;
}

void SSWR::DownloadMonitor::DownMonCore::ProcessDir(Text::String *downPath, Text::String *succPath, Text::String *errPath)
{
	Bool downFound = false;;
	UTF8Char sbuff[512];
	UTF8Char sbuff2[512];
	UTF8Char sbuff3[512];
	UTF8Char *sptr;
	UTF8Char *sptr2;
	UTF8Char *sptr3;
//	printf("ProcessDir\r\n");
	sptr = downPath->ConcatTo(sbuff);
	if (sptr[-1] != IO::Path::PATH_SEPERATOR)
	{
		*sptr++ = IO::Path::PATH_SEPERATOR;
	}
	Text::StrConcatC(sptr, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
	IO::Path::FindFileSession *sess = IO::Path::FindFile(sbuff);
	if (sess)
	{
//		printf("checking: %s\r\n", sbuff);
		this->chkStatus = CS_CHECKING;
		UInt64 fileSize;
		UInt64 fileSize2;
		IO::Path::PathType pt;
		Data::DateTime modTime;
		IO::ActiveStreamReader::BottleNeckType bnt;

		while ((sptr2 = IO::Path::FindNextFile(sptr, sess, &modTime, &pt, &fileSize)) != 0)
		{
//			printf("File: %s\r\n", sptr);
			if (Text::StrEndsWithICaseC(sptr, (UOSInt)(sptr2 - sptr), UTF8STRC(" - DASH.MP4")))
			{
				sptr2 = Text::StrConcat(sbuff2, sbuff);
				sptr2 = Text::StrConcatC(sptr2, UTF8STRC(".part"));
				if (IO::Path::GetPathType(sbuff2, (UOSInt)(sptr2 - sbuff2)) == IO::Path::PathType::File)
				{
				}
				else
				{
					sptr2 = Text::StrConcat(sbuff2, sbuff) - 4;
					sptr2 = Text::StrConcatC(sptr2, UTF8STRC(".aac"));
					if (IO::Path::GetPathType(sbuff2, (UOSInt)(sptr2 - sbuff2)) == IO::Path::PathType::File)
					{
						fileSize = IO::Path::GetFileSize(sbuff);
						fileSize2 = IO::Path::GetFileSize(sbuff2);
						if (fileSize >= 1024 && fileSize2 >= 1024)
						{
							sptr3 = this->ytPath->ConcatTo(sbuff3);
							if (sptr3[-1] != IO::Path::PATH_SEPERATOR)
							{
								*sptr3++ = IO::Path::PATH_SEPERATOR;
							}
							sptr3 = Text::StrConcat(sptr3, sptr) - 11;
							Text::StrConcatC(sptr3, UTF8STRC(".mp4"));
							this->chkStatus = CS_MUXING;
							if (FFMPEGMux(sbuff, sbuff2, sbuff3))
							{
								this->chkStatus = CS_VALIDATING;
								if (VideoValid(sbuff3))
								{
									IO::Path::DeleteFile(sbuff);
									IO::Path::DeleteFile(sbuff2);
								}
								else
								{
									IO::Path::DeleteFile(sbuff3);
								}
								this->chkStatus = CS_CHECKING;
							}
							else
							{
								sptr2 = errPath->ConcatTo(sbuff2);
								if (sptr2[-1] != IO::Path::PATH_SEPERATOR)
								{
									*sptr2++ = IO::Path::PATH_SEPERATOR;
								}
								Text::StrConcat(sptr2, sptr);
								this->chkStatus = CS_MOVING;
								IO::FileUtil::MoveFile(sbuff, sbuff2, IO::FileUtil::FileExistAction::Overwrite, 0, &bnt);
								this->chkStatus = CS_CHECKING;
							}
						}
					}
				}
			}
			else if (Text::StrEndsWithICaseC(sptr, (UOSInt)(sptr2 - sptr), UTF8STRC(" - DASH.WEBM")))
			{
				sptr2 = Text::StrConcat(sbuff2, sbuff);
				sptr2 = Text::StrConcatC(sptr2, UTF8STRC(".part"));
				if (IO::Path::GetPathType(sbuff2, (UOSInt)(sptr2 - sbuff2)) == IO::Path::PathType::File)
				{
				}
				else
				{
					sptr2 = Text::StrConcat(sbuff2, sbuff) - 5;
					sptr2 = Text::StrConcatC(sptr2, UTF8STRC(".opus"));
					if (IO::Path::GetPathType(sbuff2, (UOSInt)(sptr2 - sbuff2)) == IO::Path::PathType::File)
					{
						fileSize = IO::Path::GetFileSize(sbuff);
						fileSize2 = IO::Path::GetFileSize(sbuff2);
						if (fileSize >= 1024 && fileSize2 >= 1024)
						{
							sptr3 = this->ytPath->ConcatTo(sbuff3);
							if (sptr3[-1] != IO::Path::PATH_SEPERATOR)
							{
								*sptr3++ = IO::Path::PATH_SEPERATOR;
							}
							sptr3 = Text::StrConcat(sptr3, sptr) - 12;
							Text::StrConcatC(sptr3, UTF8STRC(".mp4"));
							this->chkStatus = CS_MUXING;
							if (FFMPEGMuxAAC(sbuff, sbuff2, sbuff3))
							{
								this->chkStatus = CS_VALIDATING;
								if (VideoValid(sbuff3))
								{
									IO::Path::DeleteFile(sbuff);
									IO::Path::DeleteFile(sbuff2);
								}
								else
								{
									IO::Path::DeleteFile(sbuff3);

									sptr2 = errPath->ConcatTo(sbuff2);
									if (sptr2[-1] != IO::Path::PATH_SEPERATOR)
									{
										*sptr2++ = IO::Path::PATH_SEPERATOR;
									}
									Text::StrConcat(sptr2, sptr);
									this->chkStatus = CS_MOVING;
									IO::FileUtil::MoveFile(sbuff, sbuff2, IO::FileUtil::FileExistAction::Overwrite, 0, &bnt);
								}
								this->chkStatus = CS_CHECKING;
							}
							else
							{
								sptr2 = errPath->ConcatTo(sbuff2);
								if (sptr2[-1] != IO::Path::PATH_SEPERATOR)
								{
									*sptr2++ = IO::Path::PATH_SEPERATOR;
								}
								Text::StrConcat(sptr2, sptr);
								this->chkStatus = CS_MOVING;
								IO::FileUtil::MoveFile(sbuff, sbuff2, IO::FileUtil::FileExistAction::Overwrite, 0, &bnt);
								this->chkStatus = CS_CHECKING;
							}
						}
					}
				}
			}
			else if (Text::StrEndsWithICaseC(sptr, (UOSInt)(sptr2 - sptr), UTF8STRC(".MP4")))
			{
//				printf("MP4 found: %s\r\n", sptr);
				sptr2 = Text::StrConcat(sbuff2, sbuff);
				sptr2 = Text::StrConcatC(sptr2, UTF8STRC(".part"));
				if (IO::Path::GetPathType(sbuff2, (UOSInt)(sptr2 - sbuff2)) == IO::Path::PathType::File)
				{
					downFound = true;
				}
				else
				{
					this->chkStatus = CS_VALIDATING;
					if (VideoValid(sbuff))
					{
						sptr2 = succPath->ConcatTo(sbuff2);
						Int32 webType = 0;
						Int32 id = this->FileGetByName(sptr, &webType);
						if (id != 0 && webType != 0)
						{
							this->FileEnd(id, webType);
							if (this->fileEndHdlr)
							{
								this->fileEndHdlr(this->fileEndObj, id, webType);
							}
						}
					}
					else
					{
						sptr2 = errPath->ConcatTo(sbuff2);
					}
					if (sptr2[-1] != IO::Path::PATH_SEPERATOR)
					{
						*sptr2++ = IO::Path::PATH_SEPERATOR;
					}
					Text::StrConcat(sptr2, sptr);
					this->chkStatus = CS_MOVING;
					IO::FileUtil::MoveFile(sbuff, sbuff2, IO::FileUtil::FileExistAction::Overwrite, 0, &bnt);
					this->chkStatus = CS_CHECKING;
				}
			}
			else if (Text::StrEndsWithICaseC(sptr, (UOSInt)(sptr2 - sptr), UTF8STRC(".ZIP")))
			{
				sptr2 = Text::StrConcat(sbuff2, sbuff);
				sptr2 = Text::StrConcatC(sptr2, UTF8STRC(".part"));
				if (IO::Path::GetPathType(sbuff2, (UOSInt)(sptr2 - sbuff2)) == IO::Path::PathType::File)
				{
				}
				else
				{
					sptr2 = Text::StrConcat(sbuff2, sbuff) - 4;
					sptr2 = Text::StrConcatC(sptr2, UTF8STRC(".mp4"));
					this->chkStatus = CS_EXTRACTING;
					if (ExtractZIP(sbuff, sbuff2))
					{
						this->chkStatus = CS_VALIDATING;
						if (VideoValid(sbuff2))
						{
							IO::Path::DeleteFile(sbuff);
							Text::StrConcatC(sbuff, sbuff2, (UOSInt)(sptr2 - sbuff2));

							Int32 webType = 0;
							Int32 id = this->FileGetByName(sptr, &webType);
							if (id != 0 && webType != 0)
							{
								this->FileEnd(id, webType);
								if (this->fileEndHdlr)
								{
									this->fileEndHdlr(this->fileEndObj, id, webType);
								}
							}

							sptr2 = succPath->ConcatTo(sbuff2);
							if (sptr2[-1] != IO::Path::PATH_SEPERATOR)
							{
								*sptr2++ = IO::Path::PATH_SEPERATOR;
							}
							sptr2 = Text::StrConcat(sptr2, sptr) - 4;
							Text::StrConcatC(sptr2, UTF8STRC(".mp4"));
							this->chkStatus = CS_MOVING;
							IO::FileUtil::MoveFile(sbuff, sbuff2, IO::FileUtil::FileExistAction::Overwrite, 0, &bnt);
						}
						else
						{
							IO::Path::DeleteFile(sbuff);

							sptr2 = errPath->ConcatTo(sbuff2);
							if (sptr2[-1] != IO::Path::PATH_SEPERATOR)
							{
								*sptr2++ = IO::Path::PATH_SEPERATOR;
							}
							Text::StrConcat(sptr2, sptr);
							this->chkStatus = CS_MOVING;
							IO::FileUtil::MoveFile(sbuff, sbuff2, IO::FileUtil::FileExistAction::Fail, 0, &bnt);
						}
					}
					this->chkStatus = CS_CHECKING;
				}
			}
			else if (Text::StrEndsWithICaseC(sptr, (UOSInt)(sptr2 - sptr), UTF8STRC(".RAR")))
			{
				if (Text::StrIndexOfC(sptr, (UOSInt)(sptr2 - sptr), UTF8STRC(".part")) != INVALID_INDEX)
				{

				}
				else
				{
					sptr2 = Text::StrConcat(sbuff2, sbuff);
					sptr2 = Text::StrConcatC(sptr2, UTF8STRC(".part"));
					if (IO::Path::GetPathType(sbuff2, (UOSInt)(sptr2 - sbuff2)) == IO::Path::PathType::File)
					{
					}
					else
					{
						sptr2 = Text::StrConcat(sbuff2, sbuff) - 4;
						Text::StrConcatC(sptr2, UTF8STRC(".mp4"));
						this->chkStatus = CS_EXTRACTING;
						if (ExtractZIP(sbuff, sbuff2))
						{
							this->chkStatus = CS_VALIDATING;
							if (VideoValid(sbuff2))
							{
								IO::Path::DeleteFile(sbuff);
								Text::StrConcat(sbuff, sbuff2);

								Int32 webType = 0;
								Int32 id = this->FileGetByName(sptr, &webType);
								if (id != 0 && webType != 0)
								{
									this->FileEnd(id, webType);
									if (this->fileEndHdlr)
									{
										this->fileEndHdlr(this->fileEndObj, id, webType);
									}
								}

								sptr2 = succPath->ConcatTo(sbuff2);
								if (sptr2[-1] != IO::Path::PATH_SEPERATOR)
								{
									*sptr2++ = IO::Path::PATH_SEPERATOR;
								}
								sptr2 = Text::StrConcat(sptr2, sptr) - 4;
								Text::StrConcatC(sptr2, UTF8STRC(".mp4"));
								this->chkStatus = CS_MOVING;
								IO::FileUtil::MoveFile(sbuff, sbuff2, IO::FileUtil::FileExistAction::Overwrite, 0, &bnt);
							}
							else
							{
								IO::Path::DeleteFile(sbuff);

								sptr2 = errPath->ConcatTo(sbuff2);
								if (sptr2[-1] != IO::Path::PATH_SEPERATOR)
								{
									*sptr2++ = IO::Path::PATH_SEPERATOR;
								}
								Text::StrConcat(sptr2, sptr);
								this->chkStatus = CS_MOVING;
								IO::FileUtil::MoveFile(sbuff, sbuff2, IO::FileUtil::FileExistAction::Fail, 0, &bnt);
							}
						}
						this->chkStatus = CS_CHECKING;
					}
				}
			}
		}
		IO::Path::FindFileClose(sess);
		if (downFound)
		{
			this->chkStatus = CS_DOWNLOADING;
		}
		else
		{
			this->chkStatus = CS_IDLE;
		}
	}
}

UInt32 __stdcall SSWR::DownloadMonitor::DownMonCore::CheckThread(void *userObj)
{
	SSWR::DownloadMonitor::DownMonCore *me = (SSWR::DownloadMonitor::DownMonCore *)userObj;
	me->chkRunning = true;
	while (!me->chkToStop)
	{
		me->ProcessDir(me->downPath, me->succPath, me->errPath);

		me->chkEvt->Wait(10000);
	}
	me->chkRunning = false;
	return 0;
}

SSWR::DownloadMonitor::DownMonCore::DownMonCore()
{
	this->chkRunning = false;
	this->chkToStop = false;
	NEW_CLASS(this->sockf, Net::OSSocketFactory(true));
	this->ssl = Net::SSLEngineFactory::Create(this->sockf, true);
	NEW_CLASS(this->chkEvt, Sync::Event(true, (const UTF8Char*)"SSWR.DownloadMonitor.DownMonCore.chkEvt"));
	this->chkStatus = CS_IDLE;
	NEW_CLASS(this->fileMut, Sync::Mutex());
	NEW_CLASS(this->fileTypeMap, Data::Int32Map<SSWR::DownloadMonitor::DownMonCore::FileInfo*>());
	NEW_CLASS(this->fileNameMap, Data::FastStringMap<SSWR::DownloadMonitor::DownMonCore::FileInfo*>());

	NEW_CLASS(this->parsers, Parser::FullParserList());
	NEW_CLASS(this->checker, Media::VideoChecker(false));
	
	this->downPath = 0;
	this->succPath = 0;
	this->errPath = 0;
	this->ytPath = 0;
	this->ffmpegPath = 0;
	this->firefoxPath = 0;
	this->listFile = 0;
	IO::ConfigFile *cfg = IO::IniFile::ParseProgConfig(0);
	if (cfg)
	{
		Text::String *s;
		s = cfg->GetValue(UTF8STRC("DownPath"));
		this->downPath = SCOPY_STRING(s);
		s = cfg->GetValue(UTF8STRC("SuccPath"));
		this->succPath = SCOPY_STRING(s);
		s = cfg->GetValue(UTF8STRC("ErrPath"));
		this->errPath = SCOPY_STRING(s);
		s = cfg->GetValue(UTF8STRC("YTPath"));
		this->ytPath = SCOPY_STRING(s);
		s = cfg->GetValue(UTF8STRC("FFMPEGPath"));
		this->ffmpegPath = SCOPY_STRING(s);
		s = cfg->GetValue(UTF8STRC("FirefoxPath"));
		this->firefoxPath = SCOPY_STRING(s);
		s = cfg->GetValue(UTF8STRC("ListFile"));
		this->listFile = SCOPY_STRING(s);
	}
	if (this->downPath == 0) this->downPath = Text::String::New(UTF8STRC("D:\\DownTemp"));
	if (this->succPath == 0) this->succPath = Text::String::New(UTF8STRC("\\\\192.168.0.21\\disk4\\DownVideo\\ToCheck"));
	if (this->errPath == 0) this->errPath = Text::String::New(UTF8STRC("D:\\DownTemp\\Err"));
	if (this->ytPath == 0) this->ytPath = Text::String::New(UTF8STRC("D:\\DownTemp\\Youtube"));
	if (this->ffmpegPath == 0) this->ffmpegPath = Text::String::New(UTF8STRC("C:\\BDTools\\ffmpeg.exe"));
	if (this->firefoxPath == 0) this->firefoxPath = Text::String::New(UTF8STRC("C:\\Program Files\\Firefox Developer Edition\\firefox.exe"));
	if (this->listFile == 0) this->listFile = Text::String::New(UTF8STRC("I:\\PROGS\\DownList2.txt"));

	Sync::Thread::Create(CheckThread, this);
	while (!this->chkRunning)
	{
		Sync::Thread::Sleep(10);
	}
}

SSWR::DownloadMonitor::DownMonCore::~DownMonCore()
{
	this->chkToStop = true;
	this->chkEvt->Set();
	while (this->chkRunning)
	{
		Sync::Thread::Sleep(10);
	}
	DEL_CLASS(this->checker);
	DEL_CLASS(this->parsers);
	DEL_CLASS(this->chkEvt);

	Data::ArrayList<SSWR::DownloadMonitor::DownMonCore::FileInfo *> *fileList;
	UOSInt i;
	fileList = this->fileTypeMap->GetValues();
	i = fileList->GetCount();
	while (i-- > 0)
	{
		this->FileFree(fileList->GetItem(i));
	}

	DEL_CLASS(this->fileNameMap);
	DEL_CLASS(this->fileTypeMap);
	DEL_CLASS(this->fileMut);
	SDEL_CLASS(this->ssl);
	DEL_CLASS(this->sockf);
}

Bool SSWR::DownloadMonitor::DownMonCore::IsError()
{
	return !this->chkRunning;
}

Net::SocketFactory *SSWR::DownloadMonitor::DownMonCore::GetSocketFactory()
{
	return this->sockf;
}

Net::SSLEngine *SSWR::DownloadMonitor::DownMonCore::GetSSLEngine()
{
	return this->ssl;
}

SSWR::DownloadMonitor::DownMonCore::CheckStatus SSWR::DownloadMonitor::DownMonCore::GetCurrStatus()
{
	return this->chkStatus;
}

void SSWR::DownloadMonitor::DownMonCore::SetFileEndHandler(FileEndHandler hdlr, void *userObj)
{
	this->fileEndObj = userObj;
	this->fileEndHdlr = hdlr;
}

Text::String *SSWR::DownloadMonitor::DownMonCore::GetListFile()
{
	return this->listFile;
}

void SSWR::DownloadMonitor::DownMonCore::FileFree(SSWR::DownloadMonitor::DownMonCore::FileInfo *file)
{
	file->dbName->Release();
	file->fileName->Release();
	DEL_CLASS(file->mut);
	MemFree(file);
}

Bool SSWR::DownloadMonitor::DownMonCore::FileAdd(Int32 id, Int32 webType, Text::String *dbName)
{
	SSWR::DownloadMonitor::DownMonCore::FileInfo *file;
	Text::StringBuilderUTF8 sb;
	Sync::MutexUsage mutUsage(this->fileMut);
	file = this->fileTypeMap->Get((webType << 24) | id);
	if (file)
	{
		return false;
	}

	file = MemAlloc(SSWR::DownloadMonitor::DownMonCore::FileInfo, 1);
	file->id = id;
	file->webType = webType;
	file->dbName = dbName->Clone();
	Net::WebSite::WebSite48IdolControl::Title2DisplayName(dbName, &sb);
	sb.AppendC(UTF8STRC(".mp4"));
	file->fileName = Text::String::New(sb.ToString(), sb.GetLength());
	file->status = FS_NORMAL;
	NEW_CLASS(file->mut, Sync::Mutex());

	this->fileTypeMap->Put((file->webType << 24) | file->id, file);
	this->fileNameMap->Put(file->fileName, file);
	return true;
}

SSWR::DownloadMonitor::DownMonCore::FileInfo *SSWR::DownloadMonitor::DownMonCore::FileGet(Int32 id, Int32 webType, Sync::MutexUsage *mutUsage)
{
	SSWR::DownloadMonitor::DownMonCore::FileInfo *file;
	mutUsage->ReplaceMutex(this->fileMut);
	file = this->fileTypeMap->Get((webType << 24) | id);
	if (file != 0 && mutUsage != 0)
	{
		mutUsage->ReplaceMutex(file->mut);
	}
	return file;
}

Int32 SSWR::DownloadMonitor::DownMonCore::FileGetByName(const UTF8Char *fileName, Int32 *webType)
{
	Int32 id = 0;
	SSWR::DownloadMonitor::DownMonCore::FileInfo *file;
	Sync::MutexUsage mutUsage(this->fileMut);
	file = this->fileNameMap->Get(fileName);
	if (file)
	{
		id = file->id;
		*webType = file->webType;
	}
	return id;
}

Bool SSWR::DownloadMonitor::DownMonCore::FileEnd(Int32 id, Int32 webType)
{
	Bool ret = false;
	SSWR::DownloadMonitor::DownMonCore::FileInfo *file;
	Sync::MutexUsage mutUsage(this->fileMut);
	file = this->fileTypeMap->Remove((webType << 24) | id);
	if (file)
	{
		if (this->fileNameMap->Get(file->fileName) == file)
		{
			this->fileNameMap->Remove(file->fileName);
		}
		this->FileFree(file);
		ret = true;
	}
	return ret;
}

Bool SSWR::DownloadMonitor::DownMonCore::FileStart(Int32 id, Int32 webType, ControlHandle *formHand)
{
	Bool ret = false;
	SSWR::DownloadMonitor::DownMonCore::FileInfo *file;
	Sync::MutexUsage mutUsage(this->fileMut);
	file = this->fileTypeMap->Get((webType << 24) | id);
	if (file)
	{
		Win32::Clipboard::SetString(formHand, file->fileName->v);
		Text::StringBuilderUTF8 sb;
		sb.AppendChar('"', 1);
		sb.Append(this->firefoxPath);
		sb.AppendC(UTF8STRC("\" "));
//		sb.AppendC(UTF8STRC("-private-window "));
		if (file->webType == 1 || file->webType == 2 || file->webType == 3)
		{
			sb.AppendC(UTF8STRC("https://48idol.tv/archive/video/"));
		}
		else
		{
			sb.AppendC(UTF8STRC("https://48idol.tv/video/"));
		}
		sb.AppendI32(file->id);
		Manage::Process proc(sb.ToString());
		ret = true;
	}
	return ret;
}

Int32 SSWR::DownloadMonitor::DownMonCore::FileGetMaxId(Int32 webType)
{
	OSInt i = this->fileTypeMap->GetKeys()->SortedIndexOf((webType << 24) | 0xffffff);
	Int32 id = this->fileTypeMap->GetKey((UOSInt)(~i - 1));
	if ((id >> 24) == webType)
	{
		return id & 0xffffff;
	}
	else
	{
		return 0;
	}
}
