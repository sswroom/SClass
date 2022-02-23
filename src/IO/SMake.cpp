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

#define OBJECTPATH "obj"

void IO::SMake::AppendCfgItem(Text::StringBuilderUTF8 *sb, const UTF8Char *val, UOSInt valLen)
{
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	IO::SMake::ConfigItem *cfg;
	const UTF8Char *valEnd = &val[valLen];
	UOSInt i = 0;
	UOSInt j;
	while ((j = Text::StrIndexOfC(&val[i], (UOSInt)(valEnd - &val[i]), UTF8STRC("$("))) != INVALID_INDEX)
	{
		if (j > 0)
		{
			sb->AppendC(&val[i], (UOSInt)j);
			i += j;
		}
		j = Text::StrIndexOfChar(&val[i], ')');
		if (j == INVALID_INDEX)
			break;
		if (Text::StrStartsWithC(&val[i + 2], (UOSInt)(valEnd - &val[i + 1]), UTF8STRC("shell ")))
		{
			Text::StringBuilderUTF8 sbCmd;
			sbCmd.AppendC(&val[i + 8], (UOSInt)j - 8);
			i += j + 1;
			Manage::Process::ExecuteProcess(sbCmd.ToString(), sbCmd.GetLength(), sb);
			while (sb->EndsWith('\r') || sb->EndsWith('\n'))
			{
				sb->RemoveChars(1);
			}
		}
		else
		{
			sptr = Text::StrConcatC(sbuff, &val[i + 2], (UOSInt)j - 2);
			i += j + 1;
			cfg = cfgMap->Get(CSTRP(sbuff, sptr));
			if (cfg)
			{
				sb->Append(cfg->value);
			}
		}
	}
	sb->AppendC(&val[i], (UOSInt)(valEnd - &val[i]));
}

void IO::SMake::AppendCfgPath(Text::StringBuilderUTF8 *sb, const UTF8Char *path, UOSInt pathLen)
{
	if (Text::StrStartsWithC(path, pathLen, UTF8STRC("~/")))
	{
		Manage::EnvironmentVar env;
		const UTF8Char *csptr = env.GetValue((const UTF8Char*)"HOME");
		if (csptr)
		{
			sb->AppendSlow(csptr);
			sb->AppendC(&path[1], pathLen - 1);
		}
		else
		{
			sb->AppendC(path, pathLen);
		}
	}
	else
	{
		sb->AppendC(path, pathLen);
	}
}

void IO::SMake::AppendCfg(Text::StringBuilderUTF8 *sb, const UTF8Char *compileCfg, UOSInt cfgLen)
{
	UOSInt i = Text::StrIndexOfCharC(compileCfg, cfgLen, '`');
	if (i != INVALID_INDEX)
	{
		Text::StringBuilderUTF8 sb2;
		while (true)
		{
			if (i > 0)
			{
				sb->AppendC(compileCfg, i);
				compileCfg += i;
				cfgLen -= i;
			}
			compileCfg++;
			cfgLen -= 1;
			i = Text::StrIndexOfCharC(compileCfg, cfgLen, '`');
			if (i == INVALID_INDEX)
			{
				sb->AppendC(compileCfg, cfgLen);
				break;
			}
			sb2.ClearStr();
			sb2.AppendC(compileCfg, (UOSInt)i);
			Manage::Process::ExecuteProcess(sb2.ToString(), sb2.GetLength(), sb);
			while (sb->EndsWith('\r') || sb->EndsWith('\n'))
			{
				sb->RemoveChars(1);
			}
			compileCfg += i + 1;
			cfgLen -= i + 1;
			i = Text::StrIndexOfCharC(compileCfg, cfgLen, '`');
			if (i == INVALID_INDEX)
			{
				sb->AppendC(compileCfg, cfgLen);
				break;
			}
		}
	}
	else
	{
		sb->AppendC(compileCfg, cfgLen);
	}
}

Bool IO::SMake::ExecuteCmd(const UTF8Char *cmd, UOSInt cmdLen)
{
	if (this->cmdWriter)
	{
		this->cmdWriter->WriteLineC(cmd, cmdLen);
	}
	Text::StringBuilderUTF8 sbRet;
	Int32 ret = Manage::Process::ExecuteProcess(cmd, cmdLen, &sbRet);
	if (ret != 0)
	{
		this->SetErrorMsg(sbRet.ToString(), sbRet.GetLength());
		return false;
	}
	if (sbRet.GetLength() > 0)
	{
		if (this->cmdWriter)
		{
			this->cmdWriter->WriteLineC(sbRet.ToString(), sbRet.GetLength());
		}
	}
	return true;
}

Bool IO::SMake::LoadConfigFile(Text::CString cfgFile)
{
	IO::FileStream *fs;
	Text::UTF8Reader *reader;
	Bool ret = false;
	if (this->messageWriter)
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendC(UTF8STRC("Loading "));
		sb.Append(cfgFile);
		this->messageWriter->WriteLineC(sb.ToString(), sb.GetLength());
	}
	NEW_CLASS(fs, IO::FileStream(cfgFile, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	if (fs->IsError())
	{
		DEL_CLASS(fs);
		Text::StringBuilderUTF8 sb;
		sb.AppendC(UTF8STRC("Error in opening "));
		sb.Append(cfgFile);
		this->SetErrorMsg(sb.ToString(), sb.GetLength());
		return false;
	}
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sb2;
	UTF8Char *sptr1;
	UTF8Char *sptr1End;
	UTF8Char *sptr2;
	UTF8Char *sptr2End;
	Text::PString line;
	UOSInt i;
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
		else if (sb.ToString()[0] == '@')
		{
			if (prog)
			{
				prog->subItems->Add(Text::String::New(sb.SubstrTrim(1).ToCString()));
			}
		}
		else if (sb.ToString()[0] == '!')
		{
			line = sb.SubstrTrim(1);
			Bool valid = true;
			sptr1 = line.v;
			sptr1End = line.GetEndPtr();
			if (Text::StrStartsWithC(sptr1, line.leng, UTF8STRC("?(")))
			{
				i = Text::StrIndexOfChar(sptr1, ')');
				if (i != INVALID_INDEX)
				{
					sptr2 = &sptr1[i + 1];
					sb2.ClearStr();
					sb2.AppendC(&sptr1[2], (UOSInt)i - 2);
					Text::PString phase = sb2.TrimAsNew();
					sptr1 = phase.v;
					sptr1End = phase.GetEndPtr();
					if ((i = Text::StrIndexOfC(sptr1, phase.leng, UTF8STRC(">="))) != INVALID_INDEX)
					{
						Text::StrTrimC(&sptr1[i + 2], phase.leng - i - 2);
						sptr1[i] = 0;
						sptr1End = Text::StrTrimC(sptr1, i);
						Int32 val1 = 0;
						Int32 val2 = 0;
						if (sptr1[0] >= '0' && sptr1[0] <= '9')
						{
							val1 = Text::StrToInt32(sptr1);
						}
						else
						{
							cfg = cfgMap->Get({sptr1, (UOSInt)(sptr1End - sptr1)});
							if (cfg)
							{
								val1 = cfg->value->ToInt32();
							}
						}
						if (sptr1[i + 2] >= '0' && sptr1[i + 2] <= '9')
						{
							val2 = Text::StrToInt32(&sptr1[i + 2]);
						}
						else
						{
							cfg = cfgMap->Get({&sptr1[i + 2], (UOSInt)(sptr1End - &sptr1[i + 2])});
							if (cfg)
							{
								val2 = cfg->value->ToInt32();
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
			else if (Text::StrStartsWithC(sptr1, sb.GetLength(), UTF8STRC("@(")))
			{
				i = Text::StrIndexOfChar(sptr1, ')');
				if (i != INVALID_INDEX && i > 1)
				{
					Text::StringBuilderUTF8 result;
					Text::String *cmd = Text::String::New(sptr1 + 2, i - 2);
					Int32 ret;
					if ((ret = Manage::Process::ExecuteProcess(cmd, &result)) != 0)
					{
						valid = false;
					}
					else
					{
						sptr1 += i + 1;
					}
					cmd->Release();
				}
			}
			if (valid && prog)
			{
				prog->libs->Add(Text::String::NewP(sptr1, sptr1End));
			}
		}
		else if (sb.ToString()[0] == '$')
		{
			Bool valid = true;
			line = sb.SubstrTrim(1);
			sptr1 = line.v;
			sptr1End = line.GetEndPtr();
			if (Text::StrStartsWithC(sptr1, line.leng, UTF8STRC("@(")))
			{
				i = Text::StrIndexOfCharC(sptr1, (UOSInt)(sptr1End - sptr1), ')');
				if (i != INVALID_INDEX && i > 1)
				{
					Text::StringBuilderUTF8 result;
					Text::String *cmd = Text::String::New(sptr1 + 2, (UOSInt)i - 2);
					Int32 ret;
					if ((ret = Manage::Process::ExecuteProcess(cmd, &result)) != 0)
					{
						valid = false;
					}
					else
					{
						sptr1 += i + 1;
					}
					cmd->Release();
				}
			}
			if (prog && valid)
			{
				const UTF8Char *ccfg = sptr1;
				const UTF8Char *ccfgEnd = sptr1End;
				IO::SMake::ConfigItem *cfg = cfgMap->Get({sptr1, (UOSInt)(sptr1End - sptr1)});
				if (cfg)
				{
					ccfg = cfg->value->v;
					ccfgEnd = &cfg->value->v[cfg->value->leng];
				}
				if (prog->compileCfg)
				{
					sb2.ClearStr();
					sb2.Append(prog->compileCfg);
					sb2.AppendUTF8Char(' ');
					sb2.AppendC(ccfg, (UOSInt)(ccfgEnd - ccfg));
					prog->compileCfg->Release();
					prog->compileCfg = Text::String::New(sb2.ToString(), sb2.GetLength());
				}
				else
				{
					prog->compileCfg = Text::String::NewP(ccfg, ccfgEnd);
				}
			}
		}
		else if (sb.StartsWith(UTF8STRC("export ")))
		{
			cfg = cfgMap->Get({sb.ToString() + 7, sb.GetLength() - 7});
			if (cfg)
			{
				Manage::EnvironmentVar env;
				env.SetValue(cfg->name->v, cfg->value->v);
			}
		}
		else if ((i = sb.IndexOf(UTF8STRC("+="))) != INVALID_INDEX)
		{
			sptr1 = sb.ToString();
			sptr2 = &sptr1[i + 2];
			sptr1[i] = 0;
			sptr1End = Text::StrTrimC(sptr1, i);
			sptr2End = Text::StrTrimC(sptr2, sb.GetLength() - i - 2);
			cfg = cfgMap->Get({sptr1, (UOSInt)(sptr1End - sptr1)});
			if (cfg)
			{
				if (cfg->value->leng > 0)
				{
					sb2.ClearStr();
					sb2.Append(cfg->value);
					sb2.AppendUTF8Char(' ');
					AppendCfgItem(&sb2, sptr2, (UOSInt)(sptr2End - sptr2));
					cfg->value->Release();
					cfg->value = Text::String::New(sb2.ToString(), sb2.GetLength());
				}
				else
				{
					cfg->value->Release();
					sb2.ClearStr();
					AppendCfgItem(&sb2, sptr2, (UOSInt)(sptr2End - sptr2));
					cfg->value = Text::String::New(sb2.ToString(), sb2.GetLength());
				}
			}
			else
			{
				cfg = MemAlloc(IO::SMake::ConfigItem, 1);
				cfg->name = Text::String::NewP(sptr1, sptr1End);
				sb2.ClearStr();
				AppendCfgItem(&sb2, sptr2, (UOSInt)(sptr2End - sptr2));
				cfg->value = Text::String::New(sb2.ToString(), sb2.GetLength());
				cfgMap->Put(cfg->name, cfg);
			}
		}
		else if ((i = sb.IndexOf(UTF8STRC(":"))) != INVALID_INDEX)
		{
			if (sb.ToString()[i + 1] == '=')
			{
				sptr1 = sb.ToString();
				sptr2 = &sptr1[i + 2];
				sptr1[i] = 0;
				sptr1End = Text::StrTrimC(sptr1, i);
				sptr2End = Text::StrTrimC(sptr2, sb.GetLength() - i - 2);
				cfg = cfgMap->Get({sptr1, (UOSInt)(sptr1End - sptr1)});
				if (cfg)
				{
					cfg->value->Release();
					sb2.ClearStr();
					AppendCfgItem(&sb2, sptr2, (UOSInt)(sptr2End - sptr2));
					cfg->value = Text::String::New(sb2.ToString(), sb2.GetLength());
				}
				else
				{
					cfg = MemAlloc(IO::SMake::ConfigItem, 1);
					cfg->name = Text::String::NewP(sptr1, sptr1End);
					sb2.ClearStr();
					AppendCfgItem(&sb2, sptr2, (UOSInt)(sptr2End - sptr2));
					cfg->value = Text::String::New(sb2.ToString(), sb2.GetLength());
					cfgMap->Put(cfg->name, cfg);
				}
			}
			else
			{
				sptr1 = sb.ToString();
				sptr2 = &sptr1[i + 1];
				sptr1[i] = 0;
				sptr1End = Text::StrTrimC(sptr1, i);
				sptr2End = Text::StrTrimC(sptr2, sb.GetLength() - i - 1);
				if (sptr1[0] == '+')
				{
					prog = progMap->GetC({sptr1 + 1, (UOSInt)(sptr1End - sptr1 - 1)});
					if (prog)
					{
					}
					else
					{
						prog = MemAlloc(IO::SMake::ProgramItem, 1);
						prog->name = Text::String::New(sptr1 + 1, (UOSInt)(sptr1End - sptr1 - 1));
						if (sptr2[0])
						{
							prog->srcFile = Text::String::New(sptr2, (UOSInt)(sptr2End - sptr2));
						}
						else
						{
							prog->srcFile = 0;
						}
						NEW_CLASS(prog->subItems, Data::ArrayList<Text::String*>());
						NEW_CLASS(prog->libs, Data::ArrayList<Text::String*>());
						prog->compileCfg = 0;
						progMap->Put(prog->name, prog);
					}
				}
				else
				{
					prog = progMap->GetC({sptr1, (UOSInt)(sptr1End - sptr1)});
					if (prog)
					{
						ret = false;
						sb2.ClearStr();
						sb2.AppendC(UTF8STRC("Program Item "));
						sb2.AppendC(sptr1, (UOSInt)(sptr1End - sptr1));
						sb2.AppendC(UTF8STRC(" duplicated"));
						this->SetErrorMsg(sb2.ToString(), sb2.GetLength());
					}
					else
					{
						prog = MemAlloc(IO::SMake::ProgramItem, 1);
						prog->name = Text::String::New(sptr1, (UOSInt)(sptr1End - sptr1));
						if (sptr2[0])
						{
							prog->srcFile = Text::String::New(sptr2, (UOSInt)(sptr2End - sptr2));
						}
						else
						{
							prog->srcFile = 0;
						}
						NEW_CLASS(prog->subItems, Data::ArrayList<Text::String*>());
						NEW_CLASS(prog->libs, Data::ArrayList<Text::String*>());
						prog->compileCfg = 0;
						progMap->Put(prog->name, prog);
					}
				}
			}
		}
		else if (sb.StartsWith(UTF8STRC("include ")))
		{
			if (IO::Path::PATH_SEPERATOR != '/')
			{
				sb.Replace('/', IO::Path::PATH_SEPERATOR);
			}
			line = sb.SubstrTrim(8);
			sptr1 = line.v;
			if (IO::Path::IsSearchPattern(sptr1))
			{
				IO::FileFindRecur srch(sptr1);
				Text::CString cstr;
				while (true)
				{
					cstr = srch.NextFile(0);
					if (cstr.v == 0)
						break;
					if (!LoadConfigFile(cstr))
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
				if (!LoadConfigFile(line.ToCString()))
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


Bool IO::SMake::ParseSource(Data::ArrayListString *objList, Data::ArrayListString *libList, Data::ArrayListString *procList, Data::ArrayListString *headerList, Int64 *latestTime, const UTF8Char *sourceFile, UOSInt sourceFileLen)
{
	IO::FileStream *fs;
	Text::StringBuilderUTF8 sb;
	if (IO::Path::PATH_SEPERATOR == '\\')
	{
		sb.AppendC(sourceFile, sourceFileLen);
		sb.Replace('/', '\\');
		NEW_CLASS(fs, IO::FileStream(sb.ToCString(), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
//		printf("Opening %s\r\n", sb.ToString());
	}
	else
	{
		NEW_CLASS(fs, IO::FileStream({sourceFile, sourceFileLen}, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
//		printf("Opening %s\r\n", sourceFile);
	}
	if (fs->IsError())
	{
		Text::StringBuilderUTF8 sb2;
		sb2.AppendC(UTF8STRC("Error in opening source \""));
		sb2.AppendC(sourceFile, sourceFileLen);
		sb2.AppendC(UTF8STRC("\""));
		this->SetErrorMsg(sb2.ToString(), sb2.GetLength());
		DEL_CLASS(fs);
		return false;
	}
	Int64 lastTime;
	Int64 thisTime;
	Data::DateTime dt;
	fs->GetFileTimes(0, 0, &dt);
	lastTime = dt.ToTicks();
	Text::UTF8Reader *reader;
	Text::PString line;
	UTF8Char *sptr1;
	UTF8Char *sptr1End;
	UOSInt i;
	IO::SMake::ProgramItem *prog;
	NEW_CLASS(reader, Text::UTF8Reader(fs));
	sb.ClearStr();
	while (reader->ReadLine(&sb, 1024))
	{
		line = sb.TrimAsNew();
		if (line.StartsWith(UTF8STRC("#include")))
		{
			line = line.SubstrTrim(8);
			sptr1 = line.v;
			sptr1End = line.GetEndPtr();
			if (sptr1[0] == '"')
			{
				sptr1++;
				i = Text::StrIndexOfCharC(sptr1, (UOSInt)(sptr1End - sptr1), '"');
				if (i != INVALID_INDEX)
				{
					sptr1[i] = 0;
					sptr1End = &sptr1[i];
					if (procList->SortedIndexOfPtr(sptr1, i) >= 0)
					{
						thisTime = fileTimeMap->GetC({sptr1, i});
						if (thisTime && thisTime > lastTime)
						{
							lastTime = thisTime;
						}
					}
					else if (Text::StrEndsWithC(sptr1, i, UTF8STRC(".cpp")))
					{
						
					}
					else
					{
						prog = progMap->GetC({sptr1, i});
						if (prog == 0)
						{
							Text::StringBuilderUTF8 sb2;
							sb2.AppendC(sptr1, i);
							sb2.AppendC(UTF8STRC(" not found in "));
							sb2.AppendC(sourceFile, sourceFileLen);
							this->SetErrorMsg(sb2.ToString(), sb2.GetLength());
							DEL_CLASS(reader);
							DEL_CLASS(fs);
							return false;
						}
						else
						{
							procList->SortedInsert(prog->name);

							UOSInt i = prog->subItems->GetCount();
							while (i-- > 0)
							{
								Text::String *subItem = prog->subItems->GetItem(i);
								if (this->debugObj && this->messageWriter && subItem->Equals(this->debugObj))
								{
									Text::StringBuilderUTF8 sb2;
									sb2.Append(debugObj);
									sb2.AppendC(UTF8STRC(" found in "));
									sb2.AppendC(sourceFile, sourceFileLen);
									this->messageWriter->WriteLineC(sb2.ToString(), sb2.GetLength());
								}
								if (objList->SortedIndexOf(subItem) < 0)
								{
									objList->SortedInsert(subItem);
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
							if (this->debugObj && this->messageWriter && prog->name->Equals(this->debugObj))
							{
								Text::StringBuilderUTF8 sb2;
								sb2.Append(debugObj);
								sb2.AppendC(UTF8STRC(" found in "));
								sb2.AppendC(sourceFile, sourceFileLen);
								this->messageWriter->WriteLineC(sb2.ToString(), sb2.GetLength());
							}
							if (!this->ParseHeader(objList, libList, procList, headerList, &thisTime, prog->name, sourceFile, sourceFileLen))
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
								if (!this->ParseSource(objList, libList, procList, headerList, &thisTime, prog->srcFile->v, prog->srcFile->leng))
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

Bool IO::SMake::ParseHeader(Data::ArrayListString *objList, Data::ArrayListString *libList, Data::ArrayListString *procList, Data::ArrayListString *headerList, Int64 *latestTime, Text::String *headerFile, const UTF8Char *sourceFile, UOSInt sourceFileLen)
{
	IO::SMake::ConfigItem *cfg = this->cfgMap->Get(CSTR("INCLUDEPATH"));
	if (cfg == 0)
	{
		this->SetErrorMsg(UTF8STRC("INCLUDEPATH config not found"));
		return false;
	}
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sb2;
	Text::PString sarr[2];
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UOSInt i;
	sb.Append(cfg->value);
	sarr[1].v = sb.ToString();
	sarr[1].leng = sb.GetLength();
	i = 2;
	while (i == 2)
	{
		i = Text::StrSplitTrimP(sarr, 2, sarr[1], ':');
		sb2.ClearStr();
		sb2.AppendC(sarr[0].v, sarr[0].leng);
		sb2.AppendChar(IO::Path::PATH_SEPERATOR, 1);
		sb2.Append(headerFile);
		sptr = IO::Path::GetRealPath(sbuff, sb2.ToString(), sb2.GetLength());

		if (IO::Path::GetPathType(CSTRP(sbuff, sptr)) == IO::Path::PathType::File)
		{
			if (headerList && headerList->SortedIndexOf(headerFile) < 0)
			{
				headerList->SortedInsert(headerFile);
			}
			if (this->ParseSource(objList, libList, procList, headerList, latestTime, sbuff, (UOSInt)(sptr - sbuff)))
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
	sb.AppendC(sourceFile, sourceFileLen);
	sb.Replace('\\', '/');
	i = sb.LastIndexOf('/');
	sb.TrimToLength(i);
	const UTF8Char *currHeader = headerFile->v;
	const UTF8Char *currHeaderEnd = &headerFile->v[headerFile->leng];
	while (Text::StrStartsWithC(currHeader, (UOSInt)(currHeaderEnd - currHeader), UTF8STRC("../")))
	{
		i = sb.LastIndexOf('/');
		sb.TrimToLength(i);
		currHeader = currHeader + 3;
	}
	sb.AppendChar(IO::Path::PATH_SEPERATOR, 1);
	sb.AppendC(currHeader, (UOSInt)(currHeaderEnd - currHeader));
	sptr = IO::Path::GetRealPath(sbuff, sb.ToString(), sb.GetLength());
	if (IO::Path::GetPathType(CSTRP(sbuff, sptr)) == IO::Path::PathType::File)
	{
		if (headerList && headerList->SortedIndexOf(headerFile) < 0)
		{
			headerList->SortedInsert(headerFile);
		}
		if (this->ParseSource(objList, libList, procList, headerList, latestTime, sbuff, (UOSInt)(sptr - sbuff)))
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
	sb2.AppendC(UTF8STRC("Include file "));
	sb2.Append(headerFile);
	sb2.AppendC(UTF8STRC(" not found in "));
	sb2.AppendC(sourceFile, sourceFileLen);
	this->SetErrorMsg(sb2.ToString(), sb2.GetLength());
	return false;
}

Bool IO::SMake::ParseProgInternal(Data::ArrayListString *objList, Data::ArrayListString *libList, Data::ArrayListString *procList, Data::ArrayListString *headerList, Int64 *latestTime, Bool *progGroup, const IO::SMake::ProgramItem *prog)
{
	UOSInt i;
	Text::String *subItem;
	IO::SMake::ProgramItem *subProg;
	Int64 thisTime;
	*latestTime = 0;
	*progGroup = true;
	i = prog->subItems->GetCount();
	while (i-- > 0)
	{
		subItem = prog->subItems->GetItem(i);
		if (subItem->EndsWith(UTF8STRC(".o")))
		{
			*progGroup = false;
		}
		if (this->debugObj && this->messageWriter && subItem->Equals(this->debugObj))
		{
			Text::StringBuilderUTF8 sb2;
			sb2.Append(debugObj);
			sb2.AppendC(UTF8STRC(" depends by "));
			sb2.Append(prog->name);
			this->messageWriter->WriteLineC(sb2.ToString(), sb2.GetLength());
		}
		objList->SortedInsert(subItem);
	}

	if (*progGroup)
	{
		return true;
	}

	IO::SMake::ConfigItem *cfg = cfgMap->Get(CSTR("DEPS"));
	if (cfg)
	{
		subProg = progMap->Get(cfg->value);
		if (subProg == 0)
		{
			Text::StringBuilderUTF8 sb;
			sb.AppendC(UTF8STRC("Item "));
			sb.Append(cfg->value);
			sb.AppendC(UTF8STRC(" not found"));
			this->SetErrorMsg(sb.ToString(), sb.GetLength());
			return false;
		}
		objList->SortedInsert(cfg->value);
		if (!this->ParseSource(objList, libList, procList, headerList, &thisTime, subProg->srcFile->v, subProg->srcFile->leng))
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
			sb.AppendC(UTF8STRC("Item "));
			sb.Append(subItem);
			sb.AppendC(UTF8STRC(" not found"));
			this->SetErrorMsg(sb.ToString(), sb.GetLength());
			return false;
		}
		if (!this->ParseSource(objList, libList, procList, headerList, &thisTime, subProg->srcFile->v, subProg->srcFile->leng))
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
	if (!req->me->ExecuteCmd(req->cmd->v, req->cmd->leng))
	{
		req->errorState[0] = true;
	}
	req->cmd->Release();
	MemFree(req);
}

void IO::SMake::CompileObject(Bool *errorState, const UTF8Char *cmd, UOSInt cmdLeng)
{
	CompileReq *req = MemAlloc(CompileReq, 1);
	req->errorState = errorState;
	req->cmd = Text::String::New(cmd, cmdLeng);
	req->me = this;
	this->tasks->AddTask(CompileTask, req);
}

Bool IO::SMake::CompileProgInternal(IO::SMake::ProgramItem *prog, Bool asmListing, Bool enableTest)
{
	Data::ArrayListString libList;
	Data::ArrayListString objList;
	Data::ArrayListString procList;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	OSInt l;
	Int64 latestTime = 0;
	Int64 thisTime;
	IO::SMake::ProgramItem *subProg;
	Bool progGroup;
	if (this->messageWriter)
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendC(UTF8STRC("Compiling Program "));
		sb.Append(prog->name);
		this->messageWriter->WriteLineC(sb.ToString(), sb.GetLength());
	}

	if (!enableTest && prog->name->Equals(UTF8STRC("test")))
	{
		IO::SMake::ConfigItem *testCfg = cfgMap->Get(CSTR("ENABLE_TEST"));
		if (testCfg && testCfg->value->Equals(UTF8STRC("1")))
		{
			enableTest = true;
		}
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
				sb.AppendC(UTF8STRC("Program "));
				sb.Append(objList.GetItem(i));
				sb.AppendC(UTF8STRC(" not found"));
				this->SetErrorMsg(sb.ToString(), sb.GetLength());
				return false;
			}
			if (!this->CompileProgInternal(subProg, asmListing, enableTest))
			{
				return false;
			}
			i++;
		}
		return true;
	}

	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sb2;
	IO::SMake::ConfigItem *cppCfg = cfgMap->Get(CSTR("CXX"));
	IO::SMake::ConfigItem *ccCfg = cfgMap->Get(CSTR("CC"));
	IO::SMake::ConfigItem *asmCfg = cfgMap->Get(CSTR("ASM"));
	IO::SMake::ConfigItem *asmflagsCfg = cfgMap->Get(CSTR("ASMFLAGS"));
	IO::SMake::ConfigItem *cflagsCfg = cfgMap->Get(CSTR("CFLAGS"));
	IO::SMake::ConfigItem *libsCfg = cfgMap->Get(CSTR("LIBS"));
	Data::DateTime dt1;
	Data::DateTime dt2;
	Bool errorState = false;
	if (cppCfg == 0)
	{
		this->SetErrorMsg(UTF8STRC("CXX config not found"));
		return false;
	}
	if (ccCfg == 0)
	{
		this->SetErrorMsg(UTF8STRC("CC config not found"));
		return false;
	}
	if (asmCfg == 0)
	{
		this->SetErrorMsg(UTF8STRC("ASM config not found"));
		return false;
	}
	
	IO::Path::CreateDirectory(CSTR(OBJECTPATH));
	i = 0;
	j = objList.GetCount();
	while (i < j)
	{
		subProg = progMap->Get(objList.GetItem(i));
		if (subProg == 0)
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Object "));
			sb.Append(objList.GetItem(i));
			sb.AppendC(UTF8STRC(" not found"));
			this->SetErrorMsg(sb.ToString(), sb.GetLength());
			return false;
		}
		if (subProg->srcFile == 0)
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Object "));
			sb.Append(objList.GetItem(i));
			sb.AppendC(UTF8STRC(" does not have source file"));
			this->SetErrorMsg(sb.ToString(), sb.GetLength());
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

		if (subProg->srcFile->v[0] != '@')
		{
			sb.ClearStr();
			sb.Append(this->basePath);
			IO::Path::AppendPath(&sb, subProg->srcFile->v, subProg->srcFile->leng);
			if (!IO::Path::GetFileTime(sb.ToString(), &dt1, 0, 0))
			{
				sb.ClearStr();
				sb.AppendC(UTF8STRC("Source file time cannot get: "));
				sb.Append(subProg->srcFile);
				this->SetErrorMsg(sb.ToString(), sb.GetLength());
				return false;
			}
			lastTime = dt1.ToTicks();
			thisTime = fileTimeMap->Get(subProg->srcFile);
			if (thisTime && thisTime > lastTime)
			{
				lastTime = thisTime;
			}

			sb.ClearStr();
			sb.AppendC(UTF8STRC(OBJECTPATH));
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
						sb.AppendC(UTF8STRC("Obj "));
						sb.Append(subProg->name);
						sb.AppendC(UTF8STRC(" Src time: "));
						sb.AppendDate(&dt2);
						sb.AppendC(UTF8STRC(", Obj time: "));
						sb.AppendDate(&dt1);
						sb.AppendC(UTF8STRC(", Last time: "));
						dt1.SetTicks(lastTime);
						sb.AppendDate(&dt1);
						sb.AppendC(UTF8STRC(", Skip"));
						this->messageWriter->WriteLineC(sb.ToString(), sb.GetLength());
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
			if (subProg->srcFile->EndsWith(UTF8STRC(".cpp")))
			{
				sb.ClearStr();
				AppendCfgPath(&sb, cppCfg->value->v, cppCfg->value->leng);
				sb.AppendUTF8Char(' ');
				if (cflagsCfg)
				{
					sb.Append(cflagsCfg->value);
					sb.AppendUTF8Char(' ');
				}
				if (subProg->compileCfg)
				{
					this->AppendCfg(&sb, subProg->compileCfg->v, subProg->compileCfg->leng);
					sb.AppendUTF8Char(' ');
				}
				if (asmListing)
				{
					sb.AppendC(UTF8STRC("-Wa,-adhln="));
					sb.AppendC(UTF8STRC(OBJECTPATH));
					sb.AppendChar(IO::Path::PATH_SEPERATOR, 1);
					sb.Append(subProg->name);
					sb.RemoveChars(1);
					sb.AppendC(UTF8STRC("s "));
				}
				sb.AppendC(UTF8STRC("-c -o "));
				sb.AppendC(UTF8STRC(OBJECTPATH));
				sb.AppendChar(IO::Path::PATH_SEPERATOR, 1);
				sb.Append(subProg->name);
				sb.AppendUTF8Char(' ');
				if (subProg->srcFile->v[0] == '@')
				{
					sb.AppendC(&subProg->srcFile->v[1], subProg->srcFile->leng - 1);
				}
				else
				{
					sb.Append(subProg->srcFile);
				}

				this->CompileObject(&errorState, sb.ToString(), sb.GetLength());
			}
			else if (subProg->srcFile->EndsWith(UTF8STRC(".c")))
			{
				sb.ClearStr();
				AppendCfgPath(&sb, ccCfg->value->v, ccCfg->value->leng);
				sb.AppendUTF8Char(' ');
				if (cflagsCfg)
				{
					sb.Append(cflagsCfg->value);
					sb.AppendUTF8Char(' ');
				}
				if (asmListing)
				{
					sb.AppendC(UTF8STRC("-Wa,-adhln="));
					sb.AppendC(UTF8STRC(OBJECTPATH));
					sb.AppendChar(IO::Path::PATH_SEPERATOR, 1);
					sb.Append(subProg->name);
					sb.RemoveChars(1);
					sb.AppendC(UTF8STRC("s "));
				}
				sb.AppendC(UTF8STRC("-c -o "));
				sb.AppendC(UTF8STRC(OBJECTPATH));
				sb.AppendChar(IO::Path::PATH_SEPERATOR, 1);
				sb.Append(subProg->name);
				sb.AppendUTF8Char(' ');
				if (subProg->srcFile->v[0] == '@')
				{
					sb.AppendC(&subProg->srcFile->v[1], subProg->srcFile->leng - 1);
				}
				else
				{
					sb.Append(subProg->srcFile);
				}
				this->CompileObject(&errorState, sb.ToString(), sb.GetLength());
			}
			else if (subProg->srcFile->EndsWith(UTF8STRC(".asm")))
			{
				sb.ClearStr();
				AppendCfgPath(&sb, asmCfg->value->v, asmCfg->value->leng);
				sb.AppendUTF8Char(' ');
				if (asmflagsCfg)
				{
					sb.Append(asmflagsCfg->value);
					sb.AppendUTF8Char(' ');
				}
				if (asmListing)
				{
					sb.AppendC(UTF8STRC("-l "));
					sb.AppendC(UTF8STRC(OBJECTPATH));
					sb.AppendChar(IO::Path::PATH_SEPERATOR, 1);
					sb.Append(subProg->name);
					sb.RemoveChars(1);
					sb.AppendC(UTF8STRC("lst "));
				}
				sb.AppendC(UTF8STRC("-o "));
				sb.AppendC(UTF8STRC(OBJECTPATH));
				sb.AppendChar(IO::Path::PATH_SEPERATOR, 1);
				sb.Append(subProg->name);
				sb.AppendUTF8Char(' ');
				if (subProg->srcFile->v[0] == '@')
				{
					sb.AppendC(&subProg->srcFile->v[1], subProg->srcFile->leng - 1);
				}
				else
				{
					sb.Append(subProg->srcFile);
				}
				this->CompileObject(&errorState, sb.ToString(), sb.GetLength());
			}
			else if (subProg->srcFile->EndsWith(UTF8STRC(".s")))
			{

			}
			else
			{
				this->tasks->WaitForIdle();
				sb.ClearStr();
				sb.AppendC(UTF8STRC("Unknown source file format "));
				sb.Append(subProg->srcFile);
				this->SetErrorMsg(sb.ToString(), sb.GetLength());
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

	IO::SMake::ConfigItem *postfixItem = this->cfgMap->Get(CSTR("OUTPOSTFIX"));
	sb.ClearStr();
	sb.Append(this->basePath);
	sb.AppendC(UTF8STRC("bin"));
	IO::Path::CreateDirectory(sb.ToCString());
	sb.AppendChar(IO::Path::PATH_SEPERATOR, 1);
	sb.Append(prog->name);
	if (postfixItem)
	{
		sb.Append(postfixItem->value);
	}
	Bool skipLink = false;
	if (IO::Path::GetFileTime(sb.ToString(), &dt2, 0, 0))
	{
		thisTime = dt2.ToTicks();
		if (thisTime >= latestTime)
		{
			skipLink = true;
		}
	}

	if (!skipLink)
	{
		sb.ClearStr();
		AppendCfgPath(&sb, cppCfg->value->v, cppCfg->value->leng);
		sb.AppendUTF8Char(' ');
		sb.AppendC(UTF8STRC("-o bin/"));
		sb.Append(prog->name);
		if (postfixItem)
		{
			sb.Append(postfixItem->value);
		}
		i = 0;
		j = objList.GetCount();
		while (i < j)
		{
			sb.AppendUTF8Char(' ');
			sb.AppendC(UTF8STRC(OBJECTPATH));
			sb.AppendChar(IO::Path::PATH_SEPERATOR, 1);
			sb.Append(objList.GetItem(i));
			i++;
		}
		if (libsCfg)
		{
			sb.AppendUTF8Char(' ');
			sb.Append(libsCfg->value);
		}
		i = 0;
		j = libList.GetCount();
		while (i < j)
		{
	//		printf("Libs: %s\r\n", libList.GetItem(i));
			sb.AppendUTF8Char(' ');
			Text::String *lib = libList.GetItem(i);
			AppendCfg(&sb, lib->v, lib->leng);
			i++;
		}
		if (!this->ExecuteCmd(sb.ToString(), sb.GetLength()))
		{
			return false;
		}
	}

	if (enableTest)
	{
		if (this->cmdWriter)
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Testing "));
			sb.Append(prog->name);
			this->cmdWriter->WriteLineC(sb.ToString(), sb.GetLength());
		}
		sb.ClearStr();
		sb.Append(this->basePath);
		sb.AppendC(UTF8STRC("bin"));
		sb.AppendChar(IO::Path::PATH_SEPERATOR, 1);
		sb.Append(prog->name);

		Text::StringBuilderUTF8 sbRet;
		Int32 ret = Manage::Process::ExecuteProcess(sb.ToString(), sb.GetLength(), &sbRet);
		if (ret != 0)
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Test failed: "));
			sb.Append(prog->name);
			this->SetErrorMsg(sb.ToString(), sb.GetLength());
			return false;
		}
	}

	return true;
}

void IO::SMake::SetErrorMsg(const UTF8Char *msg, UOSInt msgLen)
{
	Sync::MutexUsage mutUsage(this->errorMsgMut);
	SDEL_STRING(this->errorMsg);
	this->errorMsg = Text::String::New(msg, msgLen);
}

IO::SMake::SMake(Text::CString cfgFile, UOSInt threadCnt, IO::Writer *messageWriter) : IO::ParsedObject(cfgFile)
{
	NEW_CLASS(this->cfgMap, Data::StringMap<IO::SMake::ConfigItem*>());
	NEW_CLASS(this->progMap, Data::FastStringMap<IO::SMake::ProgramItem*>());
	NEW_CLASS(this->fileTimeMap, Data::FastStringMap<Int64>());
	NEW_CLASS(this->tasks, Sync::ParallelTask(threadCnt, false));
	NEW_CLASS(this->errorMsgMut, Sync::Mutex());
	this->errorMsg = 0;
	UOSInt i = cfgFile.LastIndexOf(IO::Path::PATH_SEPERATOR);
	UTF8Char sbuff[512];
	if (i != INVALID_INDEX)
	{
		this->basePath = Text::String::New(cfgFile.v, i + 1);
	}
	else
	{
		UTF8Char *sptr = IO::Path::GetCurrDirectory(sbuff);
		if (sptr[-1] != IO::Path::PATH_SEPERATOR)
		{
			*sptr++ = IO::Path::PATH_SEPERATOR;
			*sptr = 0;
		}
		this->basePath = Text::String::New(sbuff, (UOSInt)(sptr - sbuff));
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
		cfg->name->Release();
		cfg->value->Release();
		MemFree(cfg);
	}
	DEL_CLASS(cfgMap);

//	Data::ArrayList<IO::SMake::ProgramItem*> *progList = progMap->GetValues();
	IO::SMake::ProgramItem *prog;
//	i = progList->GetCount();
	i = progMap->GetCount();
	while (i-- > 0)
	{
//		prog = progList->GetItem(i);
		prog = progMap->GetItem(i);
		prog->name->Release();
		SDEL_STRING(prog->srcFile);
		SDEL_STRING(prog->compileCfg);
		j = prog->subItems->GetCount();
		while (j-- > 0)
		{
			prog->subItems->GetItem(j)->Release();
		}
		DEL_CLASS(prog->subItems);
		j = prog->libs->GetCount();
		while (j-- > 0)
		{
			prog->libs->GetItem(j)->Release();
		}
		DEL_CLASS(prog->libs);
		MemFree(prog);
	}
	DEL_CLASS(progMap);
	DEL_CLASS(fileTimeMap);
	DEL_CLASS(this->tasks);
	DEL_CLASS(this->errorMsgMut);
	SDEL_STRING(this->errorMsg);
	SDEL_STRING(this->basePath);
	SDEL_STRING(this->debugObj);
}

IO::ParserType IO::SMake::GetParserType()
{
	return ParserType::Smake;
}

Bool IO::SMake::IsLoadFailed()
{
	return this->errorMsg != 0;
}

Bool IO::SMake::GetErrorMsg(Text::StringBuilderUTF8 *sb)
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

void IO::SMake::SetDebugObj(Text::CString debugObj)
{
	SDEL_STRING(this->debugObj);
	if (debugObj.v)
	{
		this->debugObj = Text::String::New(debugObj.v, debugObj.leng);
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

Bool IO::SMake::HasProg(Text::CString progName)
{
	return this->progMap->GetC(progName) != 0;
}

Bool IO::SMake::CompileProg(Text::CString progName, Bool asmListing)
{
	IO::SMake::ProgramItem *prog = this->progMap->GetC(progName);
	if (prog == 0)
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendC(UTF8STRC("Program "));
		sb.Append(progName);
		sb.AppendC(UTF8STRC(" not found"));
		this->SetErrorMsg(sb.ToString(), sb.GetLength());
		return false;
	}
	else
	{
		return this->CompileProgInternal(prog, asmListing, false);
	}
}

Bool IO::SMake::ParseProg(Data::ArrayListString *objList, Data::ArrayListString *libList, Data::ArrayListString *procList, Data::ArrayListString *headerList, Int64 *latestTime, Bool *progGroup, Text::String *progName)
{
	IO::SMake::ProgramItem *prog = this->progMap->GetC(progName->ToCString());
	if (prog == 0)
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendC(UTF8STRC("Program "));
		sb.AppendC(progName->v, progName->leng);
		sb.AppendC(UTF8STRC(" not found"));
		this->SetErrorMsg(sb.ToString(), sb.GetLength());
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
	UTF8Char *sptr2;
	IO::Path::PathType pt;
	sptr = Text::StrConcatC(this->basePath->ConcatTo(sbuff), UTF8STRC(OBJECTPATH));
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr2 = Text::StrConcatC(sptr, UTF8STRC("*.o"));
	IO::Path::FindFileSession *sess = IO::Path::FindFile(sbuff, (UOSInt)(sptr2 - sbuff));
	if (sess)
	{
		while (IO::Path::FindNextFile(sptr, sess, 0, &pt, 0))
		{
			if (pt == IO::Path::PathType::File)
			{
				IO::Path::DeleteFile(sbuff);
			}
		}
		IO::Path::FindFileClose(sess);
	}
}

UOSInt IO::SMake::GetProgList(Data::ArrayList<Text::String*> *progList)
{
//	Data::ArrayList<Text::String *> *names = this->progMap->GetKeys();
	Text::String *prog;
	UOSInt ret = 0;
	UOSInt i = 0;
	UOSInt j = this->progMap->GetCount();
	while (i < j)
	{
		prog = this->progMap->GetKey(i);
		if (prog->EndsWith(UTF8STRC(".o")))
		{

		}
		else if (prog->EndsWith(UTF8STRC(".h")))
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

Bool IO::SMake::IsProgGroup(Text::CString progName)
{
	IO::SMake::ProgramItem *prog = this->progMap->GetC(progName);
	if (prog == 0)
	{
		return false;
	}

	Text::String *subItem;
	UOSInt i = prog->subItems->GetCount();
	while (i-- > 0)
	{
		subItem = prog->subItems->GetItem(i);
		if (subItem->EndsWith(UTF8STRC(".o")))
		{
			return false;
		}
	}
	return true;
}

const IO::SMake::ProgramItem *IO::SMake::GetProgItem(Text::CString progName)
{
	return this->progMap->GetC(progName);
}
