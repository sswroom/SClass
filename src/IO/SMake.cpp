#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/FileFindRecur.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/SMake.h"
#include "Manage/EnvironmentVar.h"
#include "Manage/Process.h"
#include "Sync/MutexUsage.h"
#include "Text/UTF8Reader.h"

#define OBJECTPATH ((const UTF8Char*)"obj")

void IO::SMake::AppendCfgItem(Text::StringBuilderUTF *sb, const UTF8Char *val)
{
	UTF8Char sbuff[64];
	IO::SMake::ConfigItem *cfg;
	OSInt i = 0;
	OSInt j;
	while ((j = Text::StrIndexOf(&val[i], (const UTF8Char*)"$(")) >= 0)
	{
		if (j > 0)
		{
			sb->AppendC(&val[i], j);
			i += j;
		}
		j = Text::StrIndexOf(&val[i], ')');
		if (j < 0)
			break;
		if (Text::StrStartsWith(&val[i + 2], (const UTF8Char*)"shell "))
		{
			Text::StringBuilderUTF8 sbCmd;
			sbCmd.AppendC(&val[i + 8], j - 8);
			i += j + 1;
			Manage::Process::ExecuteProcess(sbCmd.ToString(), sb);
			while (sb->EndsWith('\r') || sb->EndsWith('\n'))
			{
				sb->RemoveChars(1);
			}
		}
		else
		{
			Text::StrConcatC(sbuff, &val[i + 2], j - 2);
			i += j + 1;
			cfg = cfgMap->Get(sbuff);
			if (cfg)
			{
				sb->Append(cfg->value);
			}
		}
	}
	sb->Append(&val[i]);
}

void IO::SMake::AppendCfgPath(Text::StringBuilderUTF *sb, const UTF8Char *path)
{
	if (Text::StrStartsWith(path, (const UTF8Char*)"~/"))
	{
		Manage::EnvironmentVar env;
		const UTF8Char *csptr = env.GetValue((const UTF8Char*)"HOME");
		if (csptr)
		{
			sb->Append(csptr);
			sb->Append(&path[1]);
		}
		else
		{
			sb->Append(path);
		}
	}
	else
	{
		sb->Append(path);
	}
}

void IO::SMake::AppendCfg(Text::StringBuilderUTF *sb, const UTF8Char *compileCfg)
{
	OSInt i = Text::StrIndexOf(compileCfg, '`');
	if (i >= 0)
	{
		Text::StringBuilderUTF8 sb2;
		while (true)
		{
			if (i > 0)
			{
				sb->AppendC(compileCfg, i);
				compileCfg += i;
			}
			compileCfg++;
			i = Text::StrIndexOf(compileCfg, '`');
			if (i < 0)
			{
				sb->Append(compileCfg);
				break;
			}
			sb2.ClearStr();
			sb2.AppendC(compileCfg, i);
			Manage::Process::ExecuteProcess(sb2.ToString(), sb);
			while (sb->EndsWith('\r') || sb->EndsWith('\n'))
			{
				sb->RemoveChars(1);
			}
			compileCfg += i + 1;
			i = Text::StrIndexOf(compileCfg, '`');
			if (i < 0)
			{
				sb->Append(compileCfg);
				break;
			}
		}
	}
	else
	{
		sb->Append(compileCfg);
	}
}

Bool IO::SMake::ExecuteCmd(const UTF8Char *cmd)
{
	if (this->cmdWriter)
	{
		this->cmdWriter->WriteLine(cmd);
	}
	Text::StringBuilderUTF8 sbRet;
	Int32 ret = Manage::Process::ExecuteProcess(cmd, &sbRet);
	if (ret != 0)
	{
		this->SetErrorMsg(sbRet.ToString());
		return false;
	}
	if (sbRet.GetLength() > 0)
	{
		if (this->cmdWriter)
		{
			this->cmdWriter->WriteLine(sbRet.ToString());
		}
	}
	return true;
}

Bool IO::SMake::LoadConfigFile(const UTF8Char *cfgFile)
{
	IO::FileStream *fs;
	Text::UTF8Reader *reader;
	Bool ret = false;
	if (this->messageWriter)
	{
		Text::StringBuilderUTF8 sb;
		sb.Append((const UTF8Char*)"Loading ");
		sb.Append(cfgFile);
		this->messageWriter->WriteLine(sb.ToString());
	}
	NEW_CLASS(fs, IO::FileStream(cfgFile, IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
	if (fs->IsError())
	{
		DEL_CLASS(fs);
		Text::StringBuilderUTF8 sb;
		sb.Append((const UTF8Char*)"Error in opening ");
		sb.Append(cfgFile);
		this->SetErrorMsg(sb.ToString());
		return false;
	}
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sb2;
	UTF8Char *sptr1;
	UTF8Char *sptr2;
	OSInt i;
	IO::SMake::ConfigItem *cfg;
	IO::SMake::ProgramItem *prog = 0;
	NEW_CLASS(reader, Text::UTF8Reader(fs));
	sb.ClearStr();
	ret = true;
	while (reader->ReadLine(&sb, 1024))
	{
		if (sb.ToString()[0] == '#')
		{

		}
		else if ((i = Text::StrIndexOf(sb.ToString(), (const UTF8Char*)":=")) >= 0)
		{
			sptr1 = sb.ToString();
			sptr2 = &sptr1[i + 2];
			sptr1[i] = 0;
			Text::StrTrim(sptr1);
			Text::StrTrim(sptr2);
			cfg = cfgMap->Get(sptr1);
			if (cfg)
			{
				Text::StrDelNew(cfg->value);
				sb2.ClearStr();
				AppendCfgItem(&sb2, sptr2);
				cfg->value = Text::StrCopyNew(sb2.ToString());
			}
			else
			{
				cfg = MemAlloc(IO::SMake::ConfigItem, 1);
				cfg->name = Text::StrCopyNew(sptr1);
				sb2.ClearStr();
				AppendCfgItem(&sb2, sptr2);
				cfg->value = Text::StrCopyNew(sb2.ToString());
				cfgMap->Put(cfg->name, cfg);
			}
		}
		else if ((i = Text::StrIndexOf(sb.ToString(), (const UTF8Char*)"+=")) >= 0)
		{
			sptr1 = sb.ToString();
			sptr2 = &sptr1[i + 2];
			sptr1[i] = 0;
			Text::StrTrim(sptr1);
			Text::StrTrim(sptr2);	
			cfg = cfgMap->Get(sptr1);
			if (cfg)
			{
				if (cfg->value[0])
				{
					sb2.ClearStr();
					sb2.Append(cfg->value);
					sb2.AppendChar(' ', 1);
					AppendCfgItem(&sb2, sptr2);
					Text::StrDelNew(cfg->value);
					cfg->value = Text::StrCopyNew(sb2.ToString());
				}
				else
				{
					Text::StrDelNew(cfg->value);
					sb2.ClearStr();
					AppendCfgItem(&sb2, sptr2);
					cfg->value = Text::StrCopyNew(sb2.ToString());
				}
			}
			else
			{
				cfg = MemAlloc(IO::SMake::ConfigItem, 1);
				cfg->name = Text::StrCopyNew(sptr1);
				sb2.ClearStr();
				AppendCfgItem(&sb2, sptr2);
				cfg->value = Text::StrCopyNew(sb2.ToString());
				cfgMap->Put(cfg->name, cfg);
			}
		}
		else if (sb.ToString()[0] == '@')
		{
			sptr1 = sb.ToString() + 1;
			Text::StrTrim(sptr1);
			if (prog)
			{
				prog->subItems->Add(Text::StrCopyNew(sptr1));
			}
		}
		else if (sb.ToString()[0] == '!')
		{
			sptr1 = sb.ToString() + 1;
			Text::StrTrim(sptr1);
			Bool valid = true;
			if (Text::StrStartsWith(sptr1, (const UTF8Char*)"?("))
			{
				i = Text::StrIndexOf(sptr1, ')');
				if (i >= 0)
				{
					sptr2 = &sptr1[i + 1];
					Text::StrTrim(sptr2);
					sb2.ClearStr();
					sb2.AppendC(&sptr1[2], i - 2);
					sptr1 = sb2.ToString();
					if ((i = Text::StrIndexOf(sptr1, (const UTF8Char*)">=")) >= 0)
					{
						Text::StrTrim(&sptr1[i + 2]);
						sptr1[i] = 0;
						Text::StrTrim(sptr1);
						Int32 val1 = 0;
						Int32 val2 = 0;
						if (sptr1[0] >= '0' && sptr1[0] <= '9')
						{
							val1 = Text::StrToInt32(sptr1);
						}
						else
						{
							cfg = cfgMap->Get(sptr1);
							if (cfg)
							{
								val1 = Text::StrToInt32(cfg->value);
							}
						}
						if (sptr1[i + 2] >= '0' && sptr1[i + 2] <= '9')
						{
							val2 = Text::StrToInt32(&sptr1[i + 2]);
						}
						else
						{
							cfg = cfgMap->Get(&sptr1[i + 2]);
							if (cfg)
							{
								val2 = Text::StrToInt32(cfg->value);
							}
						}
						if (val1 >= val2)
						{
							sptr1 = sptr2;
						}
						else
						{
							valid = false;
						}
					}
				}
			}
			else if (Text::StrStartsWith(sptr1, (const UTF8Char*)"@("))
			{
				i = Text::StrIndexOf(sptr1, ')');
				if (i > 1)
				{
					Text::StringBuilderUTF8 result;
					const UTF8Char *cmd = Text::StrCopyNewC(sptr1 + 2, i - 2);
					if (Manage::Process::ExecuteProcess(cmd, &result) != 0)
					{
						valid = false;
					}
					else
					{
						sptr1 += i + 1;
					}
					Text::StrDelNew(cmd);
				}
			}
			if (valid && prog)
			{
				prog->libs->Add(Text::StrCopyNew(sptr1));
			}
		}
		else if (sb.ToString()[0] == '$')
		{
			Bool valid = true;
			sptr1 = sb.ToString() + 1;
			Text::StrTrim(sptr1);
			if (Text::StrStartsWith(sptr1, (const UTF8Char*)"@("))
			{
				i = Text::StrIndexOf(sptr1, ')');
				if (i > 1)
				{
					Text::StringBuilderUTF8 result;
					const UTF8Char *cmd = Text::StrCopyNewC(sptr1 + 2, i - 2);
					if (Manage::Process::ExecuteProcess(cmd, &result) != 0)
					{
						valid = false;
					}
					else
					{
						sptr1 += i + 1;
					}
					Text::StrDelNew(cmd);
				}
			}
			if (prog && valid)
			{
				const UTF8Char *ccfg = sptr1;
				IO::SMake::ConfigItem *cfg = cfgMap->Get(sptr1);
				if (cfg)
				{
					ccfg = cfg->value;
				}
				if (prog->compileCfg)
				{
					sb2.ClearStr();
					sb2.Append(prog->compileCfg);
					sb2.AppendChar(' ', 1);
					sb2.Append(ccfg);
					Text::StrDelNew(prog->compileCfg);
					prog->compileCfg = Text::StrCopyNew(sb2.ToString());
				}
				else
				{
					prog->compileCfg = Text::StrCopyNew(ccfg);
				}
			}
		}
		else if (sb.StartsWith((const UTF8Char*)"export "))
		{
			cfg = cfgMap->Get(sb.ToString() + 7);
			if (cfg)
			{
				Manage::EnvironmentVar env;
				env.SetValue(cfg->name, cfg->value);
			}
		}
		else if ((i = Text::StrIndexOf(sb.ToString(), (const UTF8Char*)":")) >= 0)
		{
			sptr1 = sb.ToString();
			sptr2 = &sptr1[i + 1];
			sptr1[i] = 0;
			Text::StrTrim(sptr1);
			Text::StrTrim(sptr2);
			if (sptr1[0] == '+')
			{
				prog = progMap->Get(sptr1 + 1);
				if (prog)
				{
				}
				else
				{
					prog = MemAlloc(IO::SMake::ProgramItem, 1);
					prog->name = Text::StrCopyNew(sptr1 + 1);
					if (sptr2[0])
					{
						prog->srcFile = Text::StrCopyNew(sptr2);
					}
					else
					{
						prog->srcFile = 0;
					}
					NEW_CLASS(prog->subItems, Data::ArrayList<const UTF8Char*>());
					NEW_CLASS(prog->libs, Data::ArrayList<const UTF8Char*>());
					prog->compileCfg = 0;
					progMap->Put(prog->name, prog);
				}
			}
			else
			{
				prog = progMap->Get(sptr1);
				if (prog)
				{
					ret = false;
					sb2.ClearStr();
					sb2.Append((const UTF8Char*)"Program Item ");
					sb2.Append(sptr1);
					sb2.Append((const UTF8Char*)" duplicated");
					this->SetErrorMsg(sb2.ToString());
				}
				else
				{
					prog = MemAlloc(IO::SMake::ProgramItem, 1);
					prog->name = Text::StrCopyNew(sptr1);
					if (sptr2[0])
					{
						prog->srcFile = Text::StrCopyNew(sptr2);
					}
					else
					{
						prog->srcFile = 0;
					}
					NEW_CLASS(prog->subItems, Data::ArrayList<const UTF8Char*>());
					NEW_CLASS(prog->libs, Data::ArrayList<const UTF8Char*>());
					prog->compileCfg = 0;
					progMap->Put(sptr1, prog);
				}
			}
		}
		else if (Text::StrStartsWith(sb.ToString(), (const UTF8Char*)"include "))
		{
			if (IO::Path::PATH_SEPERATOR != '/')
			{
				sb.Replace('/', IO::Path::PATH_SEPERATOR);
			}
			sptr1 = sb.ToString() + 8;
			Text::StrTrim(sptr1);
			if (IO::Path::IsSearchPattern(sptr1))
			{
				IO::FileFindRecur srch(sptr1);
				const UTF8Char *csptr;
				while ((csptr = srch.NextFile(0)) != 0)
				{
					if (!LoadConfigFile(csptr))
					{
						ret = false;
						break;
					}
				}

				if (!ret)
				{
					break;
				}
			}
			else
			{
				if (!LoadConfigFile(sptr1))
				{
					ret = false;
					break;
				}
			}
		}
		sb.ClearStr();
	}
	DEL_CLASS(reader);
	DEL_CLASS(fs);
	return ret;
}


Bool IO::SMake::ParseSource(Data::ArrayListStrUTF8 *objList, Data::ArrayListStrUTF8 *libList, Data::ArrayListStrUTF8 *procList, Data::ArrayListStrUTF8 *headerList, Int64 *latestTime, const UTF8Char *sourceFile)
{
	IO::FileStream *fs;
	Text::StringBuilderUTF8 sb;
	if (IO::Path::PATH_SEPERATOR == '\\')
	{
		sb.Append(sourceFile);
		sb.Replace('/', '\\');
		NEW_CLASS(fs, IO::FileStream(sb.ToString(), IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
//		printf("Opening %s\r\n", sb.ToString());
	}
	else
	{
		NEW_CLASS(fs, IO::FileStream(sourceFile, IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
//		printf("Opening %s\r\n", sourceFile);
	}
	if (fs->IsError())
	{
		Text::StringBuilderUTF8 sb2;
		sb2.Append((const UTF8Char*)"Error in opening source \"");
		sb2.Append(sourceFile);
		sb2.Append((const UTF8Char*)"\"");
		this->SetErrorMsg(sb2.ToString());
		DEL_CLASS(fs);
		return false;
	}
	Int64 lastTime;
	Int64 thisTime;
	Data::DateTime dt;
	fs->GetFileTimes(0, 0, &dt);
	lastTime = dt.ToTicks();
	Text::UTF8Reader *reader;
	UTF8Char *sptr1;
	OSInt i;
	IO::SMake::ProgramItem *prog;
	NEW_CLASS(reader, Text::UTF8Reader(fs));
	sb.ClearStr();
	while (reader->ReadLine(&sb, 1024))
	{
		sb.Trim();
		if (sb.StartsWith((const UTF8Char*)"#include"))
		{
			sptr1 = sb.ToString() + 8;
			Text::StrTrim(sptr1);
			if (sptr1[0] == '"')
			{
				sptr1++;
				i = Text::StrIndexOf(sptr1, '"');
				if (i >= 0)
				{
					sptr1[i] = 0;
					if (procList->SortedIndexOf(sptr1) >= 0)
					{
						thisTime = fileTimeMap->Get(sptr1);
						if (thisTime && thisTime > lastTime)
						{
							lastTime = thisTime;
						}
					}
					else if (Text::StrEndsWith(sptr1, (const UTF8Char*)".cpp"))
					{
						
					}
					else
					{
						prog = progMap->Get(sptr1);
						if (prog == 0)
						{
							Text::StringBuilderUTF8 sb2;
							sb2.Append(sptr1);
							sb2.Append((const UTF8Char*)" not found in ");
							sb2.Append(sourceFile);
							this->SetErrorMsg(sb2.ToString());
							DEL_CLASS(reader);
							DEL_CLASS(fs);
							return false;
						}
						else
						{
							procList->SortedInsert(prog->name);

							i = prog->subItems->GetCount();
							while (i-- > 0)
							{
								if (this->debugObj && this->messageWriter && Text::StrEquals(prog->subItems->GetItem(i), this->debugObj))
								{
									Text::StringBuilderUTF8 sb2;
									sb2.Append(debugObj);
									sb2.Append((const UTF8Char*)" found in ");
									sb2.Append(sourceFile);
									this->messageWriter->WriteLine(sb2.ToString());
								}
								if (objList->SortedIndexOf(prog->subItems->GetItem(i)) < 0)
								{
									objList->SortedInsert(prog->subItems->GetItem(i));
								}
							}
							i = prog->libs->GetCount();
							while (i-- > 0)
							{
								if (libList->SortedIndexOf(prog->libs->GetItem(i)) < 0)
								{
									libList->SortedInsert(prog->libs->GetItem(i));
								}
							}
							if (this->debugObj && this->messageWriter && Text::StrEquals(prog->name, this->debugObj))
							{
								Text::StringBuilderUTF8 sb2;
								sb2.Append(debugObj);
								sb2.Append((const UTF8Char*)" found in ");
								sb2.Append(sourceFile);
								this->messageWriter->WriteLine(sb2.ToString());
							}
							if (!this->ParseHeader(objList, libList, procList, headerList, &thisTime, prog->name, sourceFile))
							{
								DEL_CLASS(reader);
								DEL_CLASS(fs);
								return false;
							}
							if (thisTime > lastTime)
							{
								lastTime = thisTime;
							}
							if (prog->srcFile)
							{
								if (!this->ParseSource(objList, libList, procList, headerList, &thisTime, prog->srcFile))
								{
									DEL_CLASS(reader);
									DEL_CLASS(fs);
									return false;
								}
								fileTimeMap->Put(prog->srcFile, thisTime);
							}
						}
					}
				}
			}
		}
		sb.ClearStr();
	}
	*latestTime = lastTime;
	DEL_CLASS(reader);
	DEL_CLASS(fs);
	return true;
}

Bool IO::SMake::ParseHeader(Data::ArrayListStrUTF8 *objList, Data::ArrayListStrUTF8 *libList, Data::ArrayListStrUTF8 *procList, Data::ArrayListStrUTF8 *headerList, Int64 *latestTime, const UTF8Char *headerFile, const UTF8Char *sourceFile)
{
	IO::SMake::ConfigItem *cfg = this->cfgMap->Get((const UTF8Char*)"INCLUDEPATH");
	if (cfg == 0)
	{
		this->SetErrorMsg((const UTF8Char*)"INCLUDEPATH config not found");
		return false;
	}
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sb2;
	UTF8Char *sarr[2];
	OSInt i;
	sb.Append(cfg->value);
	sarr[1] = sb.ToString();
	i = 2;
	while (i == 2)
	{
		i = Text::StrSplitTrim(sarr, 2, sarr[1], ':');
		sb2.ClearStr();
		sb2.Append(sarr[0]);
		sb2.AppendChar(IO::Path::PATH_SEPERATOR, 1);
		sb2.Append(headerFile);
		if (IO::Path::PATH_SEPERATOR == '\\')
		{
			sb2.Replace('/', '\\');
		}

		if (IO::Path::GetPathType(sb2.ToString()) == IO::Path::PT_FILE)
		{
			if (headerList && headerList->SortedIndexOf(headerFile) < 0)
			{
				headerList->SortedInsert(headerFile);
			}
			if (this->ParseSource(objList, libList, procList, headerList, latestTime, sb2.ToString()))
			{
				fileTimeMap->Put(headerFile, *latestTime);
				return true;
			}
			else
			{
				return false;
			}
		}
	}
	sb.ClearStr();
	sb.Append(sourceFile);
	sb.Replace('\\', '/');
	i = sb.LastIndexOf('/');
	sb.TrimToLength(i);
	const UTF8Char *currHeader = headerFile;
	while (Text::StrStartsWith(currHeader, (const UTF8Char*)"../"))
	{
		i = sb.LastIndexOf('/');
		sb.TrimToLength(i);
		currHeader = currHeader + 3;
	}
	sb.AppendChar(IO::Path::PATH_SEPERATOR, 1);
	sb.Append(currHeader);
	if (IO::Path::PATH_SEPERATOR == '\\')
	{
		sb.Replace('/', '\\');
	}
	if (IO::Path::GetPathType(sb.ToString()) == IO::Path::PT_FILE)
	{
		if (headerList && headerList->SortedIndexOf(headerFile) < 0)
		{
			headerList->SortedInsert(headerFile);
		}
		if (this->ParseSource(objList, libList, procList, headerList, latestTime, sb.ToString()))
		{
			fileTimeMap->Put(headerFile, *latestTime);
			return true;
		}
		else
		{
			return false;
		}
	}
	

	sb2.ClearStr();
	sb2.Append((const UTF8Char*)"Include file ");
	sb2.Append(headerFile);
	sb2.Append((const UTF8Char*)" not found");
	this->SetErrorMsg(sb2.ToString());
	return false;
}

Bool IO::SMake::ParseProgInternal(Data::ArrayListStrUTF8 *objList, Data::ArrayListStrUTF8 *libList, Data::ArrayListStrUTF8 *procList, Data::ArrayListStrUTF8 *headerList, Int64 *latestTime, Bool *progGroup, const IO::SMake::ProgramItem *prog)
{
	UOSInt i;
	const UTF8Char *subItem;
	IO::SMake::ProgramItem *subProg;
	Int64 thisTime;
	*latestTime = 0;
	*progGroup = true;
	i = prog->subItems->GetCount();
	while (i-- > 0)
	{
		subItem = prog->subItems->GetItem(i);
		if (Text::StrEndsWith(subItem, (const UTF8Char*)".o"))
		{
			*progGroup = false;
		}
		objList->SortedInsert(subItem);
	}

	if (*progGroup)
	{
		return true;
	}

	IO::SMake::ConfigItem *cfg = cfgMap->Get((const UTF8Char*)"DEPS");
	if (cfg)
	{
		subProg = progMap->Get(cfg->value);
		if (subProg == 0)
		{
			Text::StringBuilderUTF8 sb;
			sb.Append((const UTF8Char*)"Item ");
			sb.Append(cfg->value);
			sb.Append((const UTF8Char*)" not found");
			this->SetErrorMsg(sb.ToString());
			return false;
		}
		objList->SortedInsert(cfg->value);
		if (!this->ParseSource(objList, libList, procList, headerList, &thisTime, subProg->srcFile))
		{
			return false;
		}
		fileTimeMap->Put(subProg->srcFile, thisTime);
		*latestTime = thisTime;
	}

	i = prog->subItems->GetCount();
	while (i-- > 0)
	{
		subItem = prog->subItems->GetItem(i);
		subProg = progMap->Get(subItem);
		if (subProg == 0)
		{
			Text::StringBuilderUTF8 sb;
			sb.Append((const UTF8Char*)"Item ");
			sb.Append(subItem);
			sb.Append((const UTF8Char*)" not found");
			this->SetErrorMsg(sb.ToString());
			return false;
		}
		if (!this->ParseSource(objList, libList, procList, headerList, &thisTime, subProg->srcFile))
		{
			return false;
		}
		fileTimeMap->Put(subProg->srcFile, thisTime);
		if (thisTime > *latestTime)
		{
			*latestTime = thisTime;
		}
	}
	return true;
}

void IO::SMake::CompileTask(void *userObj)
{
	CompileReq *req = (CompileReq *)userObj;
	if (!req->me->ExecuteCmd(req->cmd))
	{
		req->errorState[0] = true;
	}
	Text::StrDelNew(req->cmd);
	MemFree(req);
}

void IO::SMake::CompileObject(Bool *errorState, const UTF8Char *cmd)
{
	CompileReq *req = MemAlloc(CompileReq, 1);
	req->errorState = errorState;
	req->cmd = Text::StrCopyNew(cmd);
	req->me = this;
	this->tasks->AddTask(CompileTask, req);
}

Bool IO::SMake::CompileProgInternal(IO::SMake::ProgramItem *prog, Bool asmListing)
{
	Data::ArrayListStrUTF8 libList;
	Data::ArrayListStrUTF8 objList;
	Data::ArrayListStrUTF8 procList;
	OSInt i;
	OSInt j;
	OSInt k;
	OSInt l;
	Int64 latestTime = 0;
	Int64 thisTime;
	IO::SMake::ProgramItem *subProg;
	Bool progGroup;
	if (this->messageWriter)
	{
		Text::StringBuilderUTF8 sb;
		sb.Append((const UTF8Char*)"Compiling Program ");
		sb.Append(prog->name);
		this->messageWriter->WriteLine(sb.ToString());
	}

	if (!this->ParseProgInternal(&objList, &libList, &procList, 0, &latestTime, &progGroup, prog))
	{
		return false;
	}
	if (progGroup)
	{
		i = 0;
		j = objList.GetCount();
		while (i < j)
		{
			subProg = progMap->Get(objList.GetItem(i));
			if (subProg == 0)
			{
				Text::StringBuilderUTF8 sb;
				sb.Append((const UTF8Char*)"Program ");
				sb.Append(objList.GetItem(i));
				sb.Append((const UTF8Char*)" not found");
				this->SetErrorMsg(sb.ToString());
				return false;
			}
			if (!this->CompileProgInternal(subProg, asmListing))
			{
				return false;
			}
			i++;
		}
		return true;
	}

	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sb2;
	IO::SMake::ConfigItem *cppCfg = cfgMap->Get((const UTF8Char*)"CXX");
	IO::SMake::ConfigItem *ccCfg = cfgMap->Get((const UTF8Char*)"CC");
	IO::SMake::ConfigItem *asmCfg = cfgMap->Get((const UTF8Char*)"ASM");
	IO::SMake::ConfigItem *asmflagsCfg = cfgMap->Get((const UTF8Char*)"ASMFLAGS");
	IO::SMake::ConfigItem *cflagsCfg = cfgMap->Get((const UTF8Char*)"CFLAGS");
	IO::SMake::ConfigItem *libsCfg = cfgMap->Get((const UTF8Char*)"LIBS");
	Data::DateTime dt1;
	Data::DateTime dt2;
	Bool errorState = false;
	if (cppCfg == 0)
	{
		this->SetErrorMsg((const UTF8Char*)"CXX config not found");
		return false;
	}
	if (ccCfg == 0)
	{
		this->SetErrorMsg((const UTF8Char*)"CC config not found");
		return false;
	}
	if (asmCfg == 0)
	{
		this->SetErrorMsg((const UTF8Char*)"ASM config not found");
		return false;
	}
	
	IO::Path::CreateDirectory(OBJECTPATH);
	i = 0;
	j = objList.GetCount();
	while (i < j)
	{
		subProg = progMap->Get(objList.GetItem(i));
		if (subProg == 0)
		{
			sb.ClearStr();
			sb.Append((const UTF8Char*)"Object ");
			sb.Append(objList.GetItem(i));
			sb.Append((const UTF8Char*)" not found");
			this->SetErrorMsg(sb.ToString());
			return false;
		}
		if (subProg->srcFile == 0)
		{
			sb.ClearStr();
			sb.Append((const UTF8Char*)"Object ");
			sb.Append(objList.GetItem(i));
			sb.Append((const UTF8Char*)" does not have source file");
			this->SetErrorMsg(sb.ToString());
			return false;
		}

		k = subProg->libs->GetCount();
		while (k-- > 0)
		{
			l = libList.SortedIndexOf(subProg->libs->GetItem(k));
			if (l < 0)
			{
				libList.SortedInsert(subProg->libs->GetItem(k));
			}
		}

		Bool updateToDate = false;
		Int64 lastTime;
		if (errorState)
		{
			this->tasks->WaitForIdle();
			return false;
		}

		if (subProg->srcFile[0] != '@')
		{
			sb.ClearStr();
			sb.Append(this->basePath);
			IO::Path::AppendPath(&sb, subProg->srcFile);
			if (!IO::Path::GetFileTime(sb.ToString(), &dt1, 0, 0))
			{
				sb.ClearStr();
				sb.Append((const UTF8Char*)"Source file time cannot get: ");
				sb.Append(subProg->srcFile);
				this->SetErrorMsg(sb.ToString());
				return false;
			}
			lastTime = dt1.ToTicks();
			thisTime = fileTimeMap->Get(subProg->srcFile);
			if (thisTime && thisTime > lastTime)
			{
				lastTime = thisTime;
			}

			sb.ClearStr();
			sb.Append(OBJECTPATH);
			sb.AppendChar(IO::Path::PATH_SEPERATOR, 1);
			sb.Append(subProg->name);
			if (IO::Path::GetFileTime(sb.ToString(), &dt2, 0, 0))
			{
				thisTime = dt2.ToTicks();
				if (thisTime >= lastTime)
				{
					updateToDate = true;
					if (this->messageWriter)
					{
						sb.ClearStr();
						sb.Append((const UTF8Char*)"Obj ");
						sb.Append(subProg->name);
						sb.Append((const UTF8Char*)" Src time: ");
						sb.AppendDate(&dt2);
						sb.Append((const UTF8Char*)", Obj time: ");
						sb.AppendDate(&dt1);
						sb.Append((const UTF8Char*)", Last time: ");
						dt1.SetTicks(lastTime);
						sb.AppendDate(&dt1);
						sb.Append((const UTF8Char*)", Skip");
						this->messageWriter->WriteLine(sb.ToString());
					}
				}
			}
			if (lastTime > latestTime)
			{
				latestTime = lastTime;
			}
		}
		
		if (!updateToDate)
		{
			if (Text::StrEndsWith(subProg->srcFile, (const UTF8Char*)".cpp"))
			{
				sb.ClearStr();
				AppendCfgPath(&sb, cppCfg->value);
				sb.AppendChar(' ', 1);
				if (cflagsCfg)
				{
					sb.Append(cflagsCfg->value);
					sb.AppendChar(' ', 1);
				}
				if (subProg->compileCfg)
				{
					this->AppendCfg(&sb, subProg->compileCfg);
					sb.AppendChar(' ', 1);
				}
				if (asmListing)
				{
					sb.Append((const UTF8Char*)"-Wa,-adhln=");
					sb.Append(OBJECTPATH);
					sb.AppendChar(IO::Path::PATH_SEPERATOR, 1);
					sb.Append(subProg->name);
					sb.RemoveChars(1);
					sb.Append((const UTF8Char*)"s ");
				}
				sb.Append((const UTF8Char*)"-c -o ");
				sb.Append(OBJECTPATH);
				sb.AppendChar(IO::Path::PATH_SEPERATOR, 1);
				sb.Append(subProg->name);
				sb.AppendChar(' ', 1);
				if (subProg->srcFile[0] == '@')
				{
					sb.Append(&subProg->srcFile[1]);
				}
				else
				{
					sb.Append(subProg->srcFile);
				}

				this->CompileObject(&errorState, sb.ToString());
			}
			else if (Text::StrEndsWith(subProg->srcFile, (const UTF8Char*)".c"))
			{
				sb.ClearStr();
				AppendCfgPath(&sb, ccCfg->value);
				sb.AppendChar(' ', 1);
				if (cflagsCfg)
				{
					sb.Append(cflagsCfg->value);
					sb.AppendChar(' ', 1);
				}
				if (asmListing)
				{
					sb.Append((const UTF8Char*)"-Wa,-adhln=");
					sb.Append(OBJECTPATH);
					sb.AppendChar(IO::Path::PATH_SEPERATOR, 1);
					sb.Append(subProg->name);
					sb.RemoveChars(1);
					sb.Append((const UTF8Char*)"s ");
				}
				sb.Append((const UTF8Char*)"-c -o ");
				sb.Append(OBJECTPATH);
				sb.AppendChar(IO::Path::PATH_SEPERATOR, 1);
				sb.Append(subProg->name);
				sb.AppendChar(' ', 1);
				if (subProg->srcFile[0] == '@')
				{
					sb.Append(&subProg->srcFile[1]);
				}
				else
				{
					sb.Append(subProg->srcFile);
				}
				this->CompileObject(&errorState, sb.ToString());
			}
			else if (Text::StrEndsWith(subProg->srcFile, (const UTF8Char*)".asm"))
			{
				sb.ClearStr();
				AppendCfgPath(&sb, asmCfg->value);
				sb.AppendChar(' ', 1);
				if (asmflagsCfg)
				{
					sb.Append(asmflagsCfg->value);
					sb.AppendChar(' ', 1);
				}
				sb.Append((const UTF8Char*)"-o ");
				sb.Append(OBJECTPATH);
				sb.AppendChar(IO::Path::PATH_SEPERATOR, 1);
				sb.Append(subProg->name);
				sb.AppendChar(' ', 1);
				if (subProg->srcFile[0] == '@')
				{
					sb.Append(&subProg->srcFile[1]);
				}
				else
				{
					sb.Append(subProg->srcFile);
				}
				this->CompileObject(&errorState, sb.ToString());
			}
			else if (Text::StrEndsWith(subProg->srcFile, (const UTF8Char*)".s"))
			{

			}
			else
			{
				this->tasks->WaitForIdle();
				sb.ClearStr();
				sb.Append((const UTF8Char*)"Unknown source file format ");
				sb.Append(subProg->srcFile);
				this->SetErrorMsg(sb.ToString());
				return false;
			}
		}

		i++;
	}
	this->tasks->WaitForIdle();
	if (errorState)
	{
		return false;
	}

	IO::SMake::ConfigItem *postfixItem = this->cfgMap->Get((const UTF8Char*)"OUTPOSTFIX");
	sb.ClearStr();
	sb.Append(this->basePath);
	sb.Append((const UTF8Char*)"bin");
	IO::Path::CreateDirectory(sb.ToString());
	sb.AppendChar(IO::Path::PATH_SEPERATOR, 1);
	sb.Append(prog->name);
	if (postfixItem)
	{
		sb.Append(postfixItem->value);
	}
	if (IO::Path::GetFileTime(sb.ToString(), &dt2, 0, 0))
	{
		thisTime = dt2.ToTicks();
		if (thisTime >= latestTime)
		{
			return true;
		}
	}

	sb.ClearStr();
	AppendCfgPath(&sb, cppCfg->value);
	sb.AppendChar(' ', 1);
	sb.Append((const UTF8Char*)"-o bin/");
	sb.Append(prog->name);
	if (postfixItem)
	{
		sb.Append(postfixItem->value);
	}
	i = 0;
	j = objList.GetCount();
	while (i < j)
	{
		sb.AppendChar(' ', 1);
		sb.Append(OBJECTPATH);
		sb.AppendChar(IO::Path::PATH_SEPERATOR, 1);
		sb.Append(objList.GetItem(i));
		i++;
	}
	if (libsCfg)
	{
		sb.AppendChar(' ', 1);
		sb.Append(libsCfg->value);
	}
	i = 0;
	j = libList.GetCount();
	while (i < j)
	{
//		printf("Libs: %s\r\n", libList.GetItem(i));
		sb.AppendChar(' ', 1);
		AppendCfg(&sb, libList.GetItem(i));
		i++;
	}
	if (!this->ExecuteCmd(sb.ToString()))
	{
		return false;
	}
	return true;
}

void IO::SMake::SetErrorMsg(const UTF8Char *msg)
{
	Sync::MutexUsage mutUsage(this->errorMsgMut);
	SDEL_TEXT(this->errorMsg);
	this->errorMsg = Text::StrCopyNew(msg);
}

IO::SMake::SMake(const UTF8Char *cfgFile, UOSInt threadCnt, IO::Writer *messageWriter) : IO::ParsedObject(cfgFile)
{
	NEW_CLASS(this->cfgMap, Data::StringUTF8Map<IO::SMake::ConfigItem*>());
	NEW_CLASS(this->progMap, Data::StringUTF8Map<IO::SMake::ProgramItem*>());
	NEW_CLASS(this->fileTimeMap, Data::StringUTF8Map<Int64>());
	NEW_CLASS(this->tasks, Sync::ParallelTask(threadCnt, false));
	NEW_CLASS(this->errorMsgMut, Sync::Mutex());
	this->errorMsg = 0;
	OSInt i = Text::StrLastIndexOf(cfgFile, IO::Path::PATH_SEPERATOR);
	UTF8Char sbuff[512];
	if (i >= 0)
	{
		this->basePath = Text::StrCopyNewC(cfgFile, i + 1);
	}
	else
	{
		UTF8Char *sptr = IO::Path::GetCurrDirectory(sbuff);
		if (sptr[-1] != IO::Path::PATH_SEPERATOR)
		{
			*sptr++ = IO::Path::PATH_SEPERATOR;
			*sptr = 0;
		}
		this->basePath = Text::StrCopyNew(sbuff);
	}
	this->messageWriter = messageWriter;
	this->debugObj = 0;

	this->LoadConfigFile(cfgFile);
}

IO::SMake::~SMake()
{
	UOSInt i;
	UOSInt j;
	IO::SMake::ConfigItem *cfg;
	Data::ArrayList<IO::SMake::ConfigItem *> *cfgList = this->cfgMap->GetValues();
	i = cfgList->GetCount();
	while (i-- > 0)
	{
		cfg = cfgList->GetItem(i);
		Text::StrDelNew(cfg->name);
		Text::StrDelNew(cfg->value);
		MemFree(cfg);
	}
	DEL_CLASS(cfgMap);

	Data::ArrayList<IO::SMake::ProgramItem*> *progList = progMap->GetValues();
	IO::SMake::ProgramItem *prog;
	i = progList->GetCount();
	while (i-- > 0)
	{
		prog = progList->GetItem(i);
		Text::StrDelNew(prog->name);
		SDEL_TEXT(prog->srcFile);
		SDEL_TEXT(prog->compileCfg);
		j = prog->subItems->GetCount();
		while (j-- > 0)
		{
			Text::StrDelNew(prog->subItems->GetItem(j));
		}
		DEL_CLASS(prog->subItems);
		j = prog->libs->GetCount();
		while (j-- > 0)
		{
			Text::StrDelNew(prog->libs->GetItem(j));
		}
		DEL_CLASS(prog->libs);
		MemFree(prog);
	}
	DEL_CLASS(progMap);
	DEL_CLASS(fileTimeMap);
	DEL_CLASS(this->tasks);
	DEL_CLASS(this->errorMsgMut);
	SDEL_TEXT(this->errorMsg);
	SDEL_TEXT(this->basePath);
	SDEL_TEXT(this->debugObj);
}

IO::ParsedObject::ParserType IO::SMake::GetParserType()
{
	return PT_SMAKE;
}

Bool IO::SMake::IsLoadFailed()
{
	return this->errorMsg != 0;
}

Bool IO::SMake::GetErrorMsg(Text::StringBuilderUTF *sb)
{
	Bool ret;
	Sync::MutexUsage mutUsage(this->errorMsgMut);
	if (this->errorMsg)
	{
		sb->Append(this->errorMsg);
		ret = true;
	}
	else
	{
		ret = false;
	}
	return ret;
}

void IO::SMake::SetMessageWriter(IO::Writer *messageWriter)
{
	this->messageWriter = messageWriter;
}

void IO::SMake::SetCommandWriter(IO::Writer *cmdWriter)
{
	this->cmdWriter = cmdWriter;
}

void IO::SMake::SetDebugObj(const UTF8Char *debugObj)
{
	SDEL_TEXT(this->debugObj);
	if (debugObj)
	{
		this->debugObj = Text::StrCopyNew(debugObj);
	}
}

void IO::SMake::SetThreadCnt(UOSInt threadCnt)
{
	if (this->tasks->GetThreadCnt() != threadCnt)
	{
		DEL_CLASS(this->tasks);
		NEW_CLASS(this->tasks, Sync::ParallelTask(threadCnt, false));
	}
}

Data::ArrayList<IO::SMake::ConfigItem*> *IO::SMake::GetConfigList()
{
	return this->cfgMap->GetValues();
}

Bool IO::SMake::HasProg(const UTF8Char *progName)
{
	return this->progMap->Get(progName) != 0;
}

Bool IO::SMake::CompileProg(const UTF8Char *progName, Bool asmListing)
{
	IO::SMake::ProgramItem *prog = this->progMap->Get(progName);
	if (prog == 0)
	{
		Text::StringBuilderUTF8 sb;
		sb.Append((const UTF8Char*)"Program ");
		sb.Append(progName);
		sb.Append((const UTF8Char*)" not found");
		this->SetErrorMsg(sb.ToString());
		return false;
	}
	else
	{
		return this->CompileProgInternal(prog, asmListing);
	}
}

Bool IO::SMake::ParseProg(Data::ArrayListStrUTF8 *objList, Data::ArrayListStrUTF8 *libList, Data::ArrayListStrUTF8 *procList, Data::ArrayListStrUTF8 *headerList, Int64 *latestTime, Bool *progGroup, const UTF8Char *progName)
{
	IO::SMake::ProgramItem *prog = this->progMap->Get(progName);
	if (prog == 0)
	{
		Text::StringBuilderUTF8 sb;
		sb.Append((const UTF8Char*)"Program ");
		sb.Append(progName);
		sb.Append((const UTF8Char*)" not found");
		this->SetErrorMsg(sb.ToString());
		return false;
	}
	else
	{
		return this->ParseProgInternal(objList, libList, procList, headerList, latestTime, progGroup, prog);
	}
}

void IO::SMake::CleanFiles()
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	IO::Path::PathType pt;
	sptr = Text::StrConcat(Text::StrConcat(sbuff, this->basePath), OBJECTPATH);
	*sptr++ = IO::Path::PATH_SEPERATOR;
	Text::StrConcat(sptr, (const UTF8Char*)"*.o");
	IO::Path::FindFileSession *sess = IO::Path::FindFile(sbuff);
	if (sess)
	{
		while (IO::Path::FindNextFile(sptr, sess, 0, &pt, 0))
		{
			if (pt == IO::Path::PT_FILE)
			{
				IO::Path::DeleteFile(sbuff);
			}
		}
		IO::Path::FindFileClose(sess);
	}
}

UOSInt IO::SMake::GetProgList(Data::ArrayList<const UTF8Char*> *progList)
{
	Data::ArrayList<const UTF8Char *> *names = this->progMap->GetKeys();
	const UTF8Char *prog;
	UOSInt ret = 0;
	UOSInt i = 0;
	UOSInt j = names->GetCount();
	while (i < j)
	{
		prog = names->GetItem(i);
		if (Text::StrEndsWith(prog, (const UTF8Char*)".o"))
		{

		}
		else if (Text::StrEndsWith(prog, (const UTF8Char*)".h"))
		{

		}
		else
		{
			progList->Add(prog);
			ret++;
		}
		i++;
	}
	return ret;
}

Bool IO::SMake::IsProgGroup(const UTF8Char *progName)
{
	IO::SMake::ProgramItem *prog = this->progMap->Get(progName);
	if (prog == 0)
	{
		return false;
	}

	const UTF8Char *subItem;
	UOSInt i = prog->subItems->GetCount();
	while (i-- > 0)
	{
		subItem = prog->subItems->GetItem(i);
		if (Text::StrEndsWith(subItem, (const UTF8Char*)".o"))
		{
			return false;
		}
	}
	return true;
}

const IO::SMake::ProgramItem *IO::SMake::GetProgItem(const UTF8Char *progName)
{
	return this->progMap->Get(progName);
}
