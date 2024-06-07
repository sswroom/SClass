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
#include "Sync/SimpleThread.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/Clipboard.h"

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
	sb.AppendSlow(videoFile);
	sb.AppendC(UTF8STRC("\" -i \""));
	sb.AppendSlow(audioFile);
	sb.AppendC(UTF8STRC("\" -map 0:0 -map 1:0 -c:a:0 copy -vcodec copy \""));
	sb.AppendSlow(outFile);
	sb.AppendC(UTF8STRC("\""));
	Manage::Process proc(sb.ToString());
	while (proc.IsRunning())
	{
		Sync::SimpleThread::Sleep(100);
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
	sb.AppendSlow(videoFile);
	sb.AppendC(UTF8STRC("\" -i \""));
	sb.AppendSlow(audioFile);
	sb.AppendC(UTF8STRC("\" -map 0:0 -map 1:0 -c:a:0 aac -vcodec copy \""));
	sb.AppendSlow(outFile);
	sb.AppendC(UTF8STRC("\""));
	Manage::Process proc(sb.ToString());
	while (proc.IsRunning())
	{
		Sync::SimpleThread::Sleep(100);
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

Bool SSWR::DownloadMonitor::DownMonCore::ExtractZIP(Text::CStringNN zipFile, Text::CStringNN mp4File)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	NN<IO::PackageFile> pkgFile;
	Bool valid = false;
	IO::StmData::FileData fd(zipFile, false);
	if (Optional<IO::PackageFile>::ConvertFrom(this->parsers->ParseFileType(fd, IO::ParserType::PackageFile)).SetTo(pkgFile))
	{
		if (pkgFile->GetCount() == 1)
		{
			sbuff[0] = 0;
			if (pkgFile->GetItemName(sbuff, 0).SetTo(sptr) && Text::StrEndsWithICaseC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC(".MP4")) && pkgFile->GetItemType(0) == IO::PackageFile::PackObjectType::StreamData)
			{
				valid = pkgFile->CopyTo(0, mp4File, true);
			}
		}
		pkgFile.Delete();
	}
	return valid;
}

Bool SSWR::DownloadMonitor::DownMonCore::VideoValid(Text::CStringNN fileName)
{
	NN<Media::MediaFile> mediaFile;
	Bool valid = false;
	IO::StmData::FileData fd(fileName, false);
	if (Optional<Media::MediaFile>::ConvertFrom(this->parsers->ParseFileType(fd, IO::ParserType::MediaFile)).SetTo(mediaFile))
	{
		valid = this->checker.IsValid(mediaFile);
		mediaFile.Delete();
	}
	return valid;
}

void SSWR::DownloadMonitor::DownMonCore::ProcessDir(Text::String *downPath, Text::String *succPath, Text::String *errPath)
{
	Bool downFound = false;;
	UTF8Char sbuff[512];
	UTF8Char sbuff2[512];
	UTF8Char sbuff3[512];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> sptrEnd;
	UnsafeArray<UTF8Char> sptr2;
	UnsafeArray<UTF8Char> sptr3;
//	printf("ProcessDir\r\n");
	sptr = downPath->ConcatTo(sbuff);
	if (sptr[-1] != IO::Path::PATH_SEPERATOR)
	{
		*sptr++ = IO::Path::PATH_SEPERATOR;
	}
	sptr2 = Text::StrConcatC(sptr, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
	IO::Path::FindFileSession *sess = IO::Path::FindFile(CSTRP(sbuff, sptr2));
	if (sess)
	{
//		printf("checking: %s\r\n", sbuff);
		this->chkStatus = CS_CHECKING;
		UInt64 fileSize;
		UInt64 fileSize2;
		IO::Path::PathType pt;
		Data::Timestamp modTime;
		IO::ActiveStreamReader::BottleNeckType bnt;

		while (IO::Path::FindNextFile(sptr, sess, &modTime, &pt, &fileSize).SetTo(sptr2))
		{
//			printf("File: %s\r\n", sptr);
			if (Text::StrEndsWithICaseC(sptr, (UOSInt)(sptr2 - sptr), UTF8STRC(" - DASH.MP4")))
			{
				sptrEnd = sptr2;
				sptr2 = Text::StrConcatC(sbuff2, sbuff, (UOSInt)(sptrEnd - sbuff));
				sptr2 = Text::StrConcatC(sptr2, UTF8STRC(".part"));
				if (IO::Path::GetPathType(CSTRP(sbuff2, sptr2)) == IO::Path::PathType::File)
				{
				}
				else
				{
					sptr2 = Text::StrConcatC(sbuff2, sbuff, (UOSInt)(sptrEnd - sbuff)) - 4;
					sptr2 = Text::StrConcatC(sptr2, UTF8STRC(".aac"));
					if (IO::Path::GetPathType(CSTRP(sbuff2, sptr2)) == IO::Path::PathType::File)
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
							sptr3 = Text::StrConcatC(sptr3, sptr, (UOSInt)(sptrEnd - sptr)) - 11;
							sptr3 = Text::StrConcatC(sptr3, UTF8STRC(".mp4"));
							this->chkStatus = CS_MUXING;
							if (FFMPEGMux(sbuff, sbuff2, sbuff3))
							{
								this->chkStatus = CS_VALIDATING;
								if (VideoValid({sbuff3, (UOSInt)(sptr3 - sbuff3)}))
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
								sptr2 = Text::StrConcatC(sptr2, sptr, (UOSInt)(sptrEnd - sptr));
								this->chkStatus = CS_MOVING;
								IO::FileUtil::MoveFile(CSTRP(sbuff, sptrEnd), CSTRP(sbuff2, sptr2), IO::FileUtil::FileExistAction::Overwrite, 0, bnt);
								this->chkStatus = CS_CHECKING;
							}
						}
					}
				}
			}
			else if (Text::StrEndsWithICaseC(sptr, (UOSInt)(sptr2 - sptr), UTF8STRC(" - DASH.WEBM")))
			{
				sptrEnd = sptr2;
				sptr2 = Text::StrConcatC(sbuff2, sbuff, (UOSInt)(sptrEnd - sbuff));
				sptr2 = Text::StrConcatC(sptr2, UTF8STRC(".part"));
				if (IO::Path::GetPathType(CSTRP(sbuff2, sptr2)) == IO::Path::PathType::File)
				{
				}
				else
				{
					sptr2 = Text::StrConcatC(sbuff2, sbuff, (UOSInt)(sptrEnd - sbuff)) - 5;
					sptr2 = Text::StrConcatC(sptr2, UTF8STRC(".opus"));
					if (IO::Path::GetPathType(CSTRP(sbuff2, sptr2)) == IO::Path::PathType::File)
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
							sptr3 = Text::StrConcatC(sptr3, sptr, (UOSInt)(sptrEnd - sptr)) - 12;
							sptr3 = Text::StrConcatC(sptr3, UTF8STRC(".mp4"));
							this->chkStatus = CS_MUXING;
							if (FFMPEGMuxAAC(sbuff, sbuff2, sbuff3))
							{
								this->chkStatus = CS_VALIDATING;
								if (VideoValid({sbuff3, (UOSInt)(sptr3 - sbuff3)}))
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
									sptr2 = Text::StrConcatC(sptr2, sptr, (UOSInt)(sptrEnd - sptr));
									this->chkStatus = CS_MOVING;
									IO::FileUtil::MoveFile(CSTRP(sbuff, sptrEnd), CSTRP(sbuff2, sptr2), IO::FileUtil::FileExistAction::Overwrite, 0, bnt);
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
								sptr2 = Text::StrConcatC(sptr2, sptr, (UOSInt)(sptrEnd - sptr));
								this->chkStatus = CS_MOVING;
								IO::FileUtil::MoveFile(CSTRP(sbuff, sptrEnd), CSTRP(sbuff2, sptr2), IO::FileUtil::FileExistAction::Overwrite, 0, bnt);
								this->chkStatus = CS_CHECKING;
							}
						}
					}
				}
			}
			else if (Text::StrEndsWithICaseC(sptr, (UOSInt)(sptr2 - sptr), UTF8STRC(".MP4")))
			{
				sptrEnd = sptr2;
//				printf("MP4 found: %s\r\n", sptr);
				sptr2 = Text::StrConcatC(sbuff2, sbuff, (UOSInt)(sptrEnd - sbuff));
				sptr2 = Text::StrConcatC(sptr2, UTF8STRC(".part"));
				if (IO::Path::GetPathType(CSTRP(sbuff2, sptr2)) == IO::Path::PathType::File)
				{
					downFound = true;
				}
				else
				{
					this->chkStatus = CS_VALIDATING;
					if (VideoValid({sbuff, (UOSInt)(sptrEnd - sbuff)}))
					{
						sptr2 = succPath->ConcatTo(sbuff2);
						Int32 webType = 0;
						Int32 id = this->FileGetByName({sptr, (UOSInt)(sptrEnd - sptr)}, webType);
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
					sptr2 = Text::StrConcatC(sptr2, sptr, (UOSInt)(sptrEnd - sptr));
					this->chkStatus = CS_MOVING;
					IO::FileUtil::MoveFile(CSTRP(sbuff, sptrEnd), CSTRP(sbuff2, sptr2), IO::FileUtil::FileExistAction::Overwrite, 0, bnt);
					this->chkStatus = CS_CHECKING;
				}
			}
			else if (Text::StrEndsWithICaseC(sptr, (UOSInt)(sptr2 - sptr), UTF8STRC(".ZIP")))
			{
				sptrEnd = sptr2;
				sptr2 = Text::StrConcatC(sbuff2, sbuff, (UOSInt)(sptrEnd - sbuff));
				sptr2 = Text::StrConcatC(sptr2, UTF8STRC(".part"));
				if (IO::Path::GetPathType(CSTRP(sbuff2, sptr2)) == IO::Path::PathType::File)
				{
				}
				else
				{
					sptr2 = Text::StrConcatC(sbuff2, sbuff, (UOSInt)(sptrEnd - sbuff)) - 4;
					sptr2 = Text::StrConcatC(sptr2, UTF8STRC(".mp4"));
					this->chkStatus = CS_EXTRACTING;
					if (ExtractZIP(CSTRP(sbuff, sptrEnd), CSTRP(sbuff2, sptr2)))
					{
						this->chkStatus = CS_VALIDATING;
						if (VideoValid(CSTRP(sbuff2, sptr2)))
						{
							IO::Path::DeleteFile(sbuff);
							Text::StrConcatC(sbuff, sbuff2, (UOSInt)(sptr2 - sbuff2));

							Int32 webType = 0;
							Int32 id = this->FileGetByName({sptr, (UOSInt)(sptrEnd - sptr)}, webType);
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
							sptr2 = Text::StrConcatC(sptr2, UTF8STRC(".mp4"));
							this->chkStatus = CS_MOVING;
							IO::FileUtil::MoveFile(CSTRP(sbuff, sptrEnd), CSTRP(sbuff2, sptr2), IO::FileUtil::FileExistAction::Overwrite, 0, bnt);
						}
						else
						{
							IO::Path::DeleteFile(sbuff);

							sptr2 = errPath->ConcatTo(sbuff2);
							if (sptr2[-1] != IO::Path::PATH_SEPERATOR)
							{
								*sptr2++ = IO::Path::PATH_SEPERATOR;
							}
							sptr2 = Text::StrConcat(sptr2, sptr);
							this->chkStatus = CS_MOVING;
							IO::FileUtil::MoveFile(CSTRP(sbuff, sptrEnd), CSTRP(sbuff2, sptr2), IO::FileUtil::FileExistAction::Fail, 0, bnt);
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
					sptrEnd = sptr2;
					sptr2 = Text::StrConcatC(sbuff2, sbuff, (UOSInt)(sptrEnd - sbuff));
					sptr2 = Text::StrConcatC(sptr2, UTF8STRC(".part"));
					if (IO::Path::GetPathType(CSTRP(sbuff2, sptr2)) == IO::Path::PathType::File)
					{
					}
					else
					{
						sptr2 = Text::StrConcatC(sbuff2, sbuff, (UOSInt)(sptrEnd - sbuff)) - 4;
						sptr2 = Text::StrConcatC(sptr2, UTF8STRC(".mp4"));
						this->chkStatus = CS_EXTRACTING;
						if (ExtractZIP(CSTRP(sbuff, sptrEnd), CSTRP(sbuff2, sptr2)))
						{
							this->chkStatus = CS_VALIDATING;
							if (VideoValid(CSTRP(sbuff2, sptr2)))
							{
								IO::Path::DeleteFile(sbuff);
								Text::StrConcatC(sbuff, sbuff2, (UOSInt)(sptr2 - sbuff2));

								Int32 webType = 0;
								Int32 id = this->FileGetByName({sptr, (UOSInt)(sptrEnd - sptr)}, webType);
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
								sptr2 = Text::StrConcatC(sptr2, sptr, (UOSInt)(sptrEnd - sptr)) - 4;
								sptr2 = Text::StrConcatC(sptr2, UTF8STRC(".mp4"));
								this->chkStatus = CS_MOVING;
								IO::FileUtil::MoveFile(CSTRP(sbuff, sptrEnd), CSTRP(sbuff2, sptr2), IO::FileUtil::FileExistAction::Overwrite, 0, bnt);
							}
							else
							{
								IO::Path::DeleteFile(sbuff);

								sptr2 = errPath->ConcatTo(sbuff2);
								if (sptr2[-1] != IO::Path::PATH_SEPERATOR)
								{
									*sptr2++ = IO::Path::PATH_SEPERATOR;
								}
								sptr2 = Text::StrConcat(sptr2, sptr);
								this->chkStatus = CS_MOVING;
								IO::FileUtil::MoveFile(CSTRP(sbuff, sptrEnd), CSTRP(sbuff2, sptr2), IO::FileUtil::FileExistAction::Fail, 0, bnt);
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

void __stdcall SSWR::DownloadMonitor::DownMonCore::CheckThread(NN<Sync::Thread> thread)
{
	NN<SSWR::DownloadMonitor::DownMonCore> me = thread->GetUserObj().GetNN<SSWR::DownloadMonitor::DownMonCore>();
	while (!thread->IsStopping())
	{
		me->ProcessDir(me->downPath, me->succPath, me->errPath);

		thread->Wait(10000);
	}
}

SSWR::DownloadMonitor::DownMonCore::DownMonCore() : thread(CheckThread, this, CSTR("DownMonCore")), checker(false)
{
	NEW_CLASSNN(this->sockf, Net::OSSocketFactory(true));
	this->ssl = Net::SSLEngineFactory::Create(this->sockf, true);
	this->chkStatus = CS_IDLE;

	NEW_CLASS(this->parsers, Parser::FullParserList());
	
	this->downPath = 0;
	this->succPath = 0;
	this->errPath = 0;
	this->ytPath = 0;
	this->ffmpegPath = 0;
	this->firefoxPath = 0;
	this->listFile = 0;
	NN<IO::ConfigFile> cfg;
	if (IO::IniFile::ParseProgConfig(0).SetTo(cfg))
	{
		NN<Text::String> s;
		if (cfg->GetValue(CSTR("DownPath")).SetTo(s))
			this->downPath = s->Clone().Ptr();
		if (cfg->GetValue(CSTR("SuccPath")).SetTo(s))
			this->succPath = s->Clone().Ptr();
		if (cfg->GetValue(CSTR("ErrPath")).SetTo(s))
			this->errPath = s->Clone().Ptr();
		if (cfg->GetValue(CSTR("YTPath")).SetTo(s))
			this->ytPath = s->Clone().Ptr();
		if (cfg->GetValue(CSTR("FFMPEGPath")).SetTo(s))
			this->ffmpegPath = s->Clone().Ptr();
		if (cfg->GetValue(CSTR("FirefoxPath")).SetTo(s))
			this->firefoxPath = s->Clone().Ptr();
		if (cfg->GetValue(CSTR("ListFile")).SetTo(s))
			this->listFile = s->Clone().Ptr();
		cfg.Delete();
	}
	if (this->downPath == 0) this->downPath = Text::String::New(UTF8STRC("D:\\DownTemp")).Ptr();
	if (this->succPath == 0) this->succPath = Text::String::New(UTF8STRC("\\\\192.168.0.21\\disk4\\DownVideo\\ToCheck")).Ptr();
	if (this->errPath == 0) this->errPath = Text::String::New(UTF8STRC("D:\\DownTemp\\Err")).Ptr();
	if (this->ytPath == 0) this->ytPath = Text::String::New(UTF8STRC("D:\\DownTemp\\Youtube")).Ptr();
	if (this->ffmpegPath == 0) this->ffmpegPath = Text::String::New(UTF8STRC("C:\\BDTools\\ffmpeg.exe")).Ptr();
	if (this->firefoxPath == 0) this->firefoxPath = Text::String::New(UTF8STRC("C:\\Program Files\\Firefox Developer Edition\\firefox.exe")).Ptr();
	if (this->listFile == 0) this->listFile = Text::String::New(UTF8STRC("I:\\PROGS\\DownList2.txt")).Ptr();
	this->thread.Start();
}

SSWR::DownloadMonitor::DownMonCore::~DownMonCore()
{
	this->thread.Stop();
	DEL_CLASS(this->parsers);

	this->fileTypeMap.FreeAll(FileFree);
	this->ssl.Delete();
	this->sockf.Delete();
}

Bool SSWR::DownloadMonitor::DownMonCore::IsError()
{
	return !this->thread.IsRunning();
}

NN<Net::SocketFactory> SSWR::DownloadMonitor::DownMonCore::GetSocketFactory()
{
	return this->sockf;
}

Optional<Net::SSLEngine> SSWR::DownloadMonitor::DownMonCore::GetSSLEngine()
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

void SSWR::DownloadMonitor::DownMonCore::FileFree(NN<SSWR::DownloadMonitor::DownMonCore::FileInfo> file)
{
	file->dbName->Release();
	file->fileName->Release();
	file.Delete();
}

Bool SSWR::DownloadMonitor::DownMonCore::FileAdd(Int32 id, Int32 webType, NN<Text::String> dbName)
{
	NN<SSWR::DownloadMonitor::DownMonCore::FileInfo> file;
	Text::StringBuilderUTF8 sb;
	Sync::MutexUsage mutUsage(this->fileMut);
	if (this->fileTypeMap.Get((webType << 24) | id).SetTo(file))
	{
		return false;
	}

	NEW_CLASSNN(file, SSWR::DownloadMonitor::DownMonCore::FileInfo());
	file->id = id;
	file->webType = webType;
	file->dbName = dbName->Clone();
	Net::WebSite::WebSite48IdolControl::Title2DisplayName(dbName, sb);
	sb.AppendC(UTF8STRC(".mp4"));
	file->fileName = Text::String::New(sb.ToString(), sb.GetLength());
	file->status = FS_NORMAL;

	this->fileTypeMap.Put((file->webType << 24) | file->id, file);
	this->fileNameMap.PutNN(file->fileName, file);
	return true;
}

Optional<SSWR::DownloadMonitor::DownMonCore::FileInfo> SSWR::DownloadMonitor::DownMonCore::FileGet(Int32 id, Int32 webType, NN<Sync::MutexUsage> mutUsage)
{
	Optional<SSWR::DownloadMonitor::DownMonCore::FileInfo> file;
	NN<SSWR::DownloadMonitor::DownMonCore::FileInfo> nnfile;
	mutUsage->ReplaceMutex(this->fileMut);
	file = this->fileTypeMap.Get((webType << 24) | id);
	if (file.SetTo(nnfile))
	{
		mutUsage->ReplaceMutex(nnfile->mut);
	}
	return file;
}

Int32 SSWR::DownloadMonitor::DownMonCore::FileGetByName(Text::CStringNN fileName, OutParam<Int32> webType)
{
	Int32 id = 0;
	NN<SSWR::DownloadMonitor::DownMonCore::FileInfo> file;
	Sync::MutexUsage mutUsage(this->fileMut);
	if (this->fileNameMap.GetC(fileName).SetTo(file))
	{
		id = file->id;
		webType.Set(file->webType);
	}
	return id;
}

Bool SSWR::DownloadMonitor::DownMonCore::FileEnd(Int32 id, Int32 webType)
{
	Bool ret = false;
	NN<SSWR::DownloadMonitor::DownMonCore::FileInfo> file;
	Sync::MutexUsage mutUsage(this->fileMut);
	if (this->fileTypeMap.Remove((webType << 24) | id).SetTo(file))
	{
		if (this->fileNameMap.GetNN(file->fileName) == file)
		{
			this->fileNameMap.RemoveNN(file->fileName);
		}
		this->FileFree(file);
		ret = true;
	}
	return ret;
}

Bool SSWR::DownloadMonitor::DownMonCore::FileStart(Int32 id, Int32 webType, ControlHandle *formHand)
{
	Bool ret = false;
	NN<SSWR::DownloadMonitor::DownMonCore::FileInfo> file;
	Sync::MutexUsage mutUsage(this->fileMut);
	if (this->fileTypeMap.Get((webType << 24) | id).SetTo(file))
	{
		UI::Clipboard::SetString(formHand, file->fileName->ToCString());
		Text::StringBuilderUTF8 sb;
		sb.AppendUTF8Char('"');
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
	OSInt i = this->fileTypeMap.GetIndex((webType << 24) | 0xffffff);
	Int32 id = this->fileTypeMap.GetKey((UOSInt)(~i - 1));
	if ((id >> 24) == webType)
	{
		return id & 0xffffff;
	}
	else
	{
		return 0;
	}
}
