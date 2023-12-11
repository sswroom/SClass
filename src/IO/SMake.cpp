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

//#include <stdio.h>

#define OBJECTPATH "obj"

/*
<name>: [!/@]<source>
@<object>
!<lib>
!?(ANDROID_API >= 24)<lib>
$<compileCfg>

<var> := <value> means assign value to var
<var> += <value> means append value to var
$(<var>) means replace var as value
`<command>' means run command and replace as command result
!<source> means force compile
@<source> means skip dependency check
*/
void IO::SMake::AppendCfgItem(NotNullPtr<Text::StringBuilderUTF8> sb, Text::CStringNN val)
{
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	IO::SMake::ConfigItem *cfg;
	const UTF8Char *valEnd = &val.v[val.leng];
	UOSInt i = 0;
	UOSInt j;
	while ((j = Text::StrIndexOfC(&val.v[i], (UOSInt)(valEnd - &val.v[i]), UTF8STRC("$("))) != INVALID_INDEX)
	{
		if (j > 0)
		{
			sb->AppendC(&val.v[i], j);
			i += j;
		}
		j = Text::StrIndexOfCharC(&val.v[i], (UOSInt)(valEnd - &val.v[i]), ')');
		if (j == INVALID_INDEX)
			break;
		if (Text::StrStartsWithC(&val.v[i + 2], (UOSInt)(valEnd - &val.v[i + 1]), UTF8STRC("shell ")))
		{
			Text::StringBuilderUTF8 sbCmd;
			sbCmd.AppendC(&val.v[i + 8], (UOSInt)j - 8);
			i += j + 1;
			Manage::Process::ExecuteProcess(sbCmd.ToCString(), sb);
			while (sb->EndsWith('\r') || sb->EndsWith('\n'))
			{
				sb->RemoveChars(1);
			}
		}
		else
		{
			sptr = Text::StrConcatC(sbuff, &val.v[i + 2], (UOSInt)j - 2);
			i += j + 1;
			cfg = this->cfgMap.Get(CSTRP(sbuff, sptr));
			if (cfg)
			{
				sb->Append(cfg->value);
			}
		}
	}
	sb->AppendC(&val.v[i], (UOSInt)(valEnd - &val.v[i]));
}

void IO::SMake::AppendCfgPath(NotNullPtr<Text::StringBuilderUTF8> sb, Text::CString path)
{
	if (path.StartsWith(UTF8STRC("~/")))
	{
		Manage::EnvironmentVar env;
		const UTF8Char *csptr = env.GetValue(CSTR("HOME"));
		if (csptr)
		{
			sb->AppendSlow(csptr);
			sb->Append(path.Substring(1));
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

void IO::SMake::AppendCfg(NotNullPtr<Text::StringBuilderUTF8> sb, Text::CString compileCfgC)
{
	Text::CString compileCfg = compileCfgC;
	UOSInt i = compileCfg.IndexOf('`');
	if (i != INVALID_INDEX)
	{
		Text::StringBuilderUTF8 sb2;
		while (true)
		{
			if (i > 0)
			{
				sb->AppendC(compileCfg.v, i);
				compileCfg = compileCfg.Substring(i);
			}
			compileCfg = compileCfg.Substring(1);
			i = compileCfg.IndexOf('`');
			if (i == INVALID_INDEX)
			{
				sb->Append(compileCfg);
				break;
			}
			sb2.ClearStr();
			sb2.AppendC(compileCfg.v, (UOSInt)i);
			Manage::Process::ExecuteProcess(sb2.ToCString(), sb);
			while (sb->EndsWith('\r') || sb->EndsWith('\n'))
			{
				sb->RemoveChars(1);
			}
			compileCfg = compileCfg.Substring(i + 1);
			i = compileCfg.IndexOf('`');
			if (i == INVALID_INDEX)
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

Bool IO::SMake::ExecuteCmd(Text::CString cmd)
{
	if (this->cmdWriter)
	{
		this->cmdWriter->WriteLineC(cmd.v, cmd.leng);
	}
	Text::StringBuilderUTF8 sbRet;
	Int32 ret = Manage::Process::ExecuteProcess(cmd, sbRet);
	if (ret != 0)
	{
		this->SetErrorMsg(sbRet.ToCString());
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

Bool IO::SMake::LoadConfigFile(Text::CStringNN cfgFile)
{
	Bool ret = false;
	if (this->messageWriter)
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendC(UTF8STRC("Loading "));
		sb.Append(cfgFile);
		this->messageWriter->WriteLineC(sb.ToString(), sb.GetLength());
	}
	IO::FileStream fs(cfgFile, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (fs.IsError())
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendC(UTF8STRC("Error in opening "));
		sb.Append(cfgFile);
		this->SetErrorMsg(sb.ToCString());
		return false;
	}
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sb2;
	Text::PString str1;
	Text::PString str2;
	Text::PString str3;
	UOSInt i;
	IO::SMake::ConfigItem *cfg;
	IO::SMake::ProgramItem *prog = 0;
	Text::UTF8Reader reader(fs);
	sb.ClearStr();
	ret = true;
	while (reader.ReadLine(sb, 1024))
	{
		if (sb.ToString()[0] == '#')
		{

		}
		else if (sb.ToString()[0] == '@')
		{
			if (prog)
			{
				prog->subItems.Add(Text::String::New(sb.SubstrTrim(1).ToCString()));
			}
		}
		else if (sb.ToString()[0] == '!')
		{
			Bool valid = true;
			str1 = sb.SubstrTrim(1);
			if (str1.StartsWith(UTF8STRC("?(")))
			{
				i = str1.IndexOf(')');
				if (i != INVALID_INDEX)
				{
					str2 = str1.Substring(i + 1);
					sb2.ClearStr();
					sb2.AppendC(&str1.v[2], (UOSInt)i - 2);
					str1 = sb2.TrimAsNew();
					if ((i = str1.IndexOf(UTF8STRC(">="))) != INVALID_INDEX)
					{
						str3 = str1.SubstrTrim(i + 2);
						str1 = str1.SubstrTrim(0, i);
						Int32 val1 = 0;
						Int32 val2 = 0;
						if (str1.v[0] >= '0' && str1.v[0] <= '9')
						{
							val1 = str1.ToInt32();
						}
						else
						{
							cfg = this->cfgMap.Get(str1.ToCString());
							if (cfg)
							{
								val1 = cfg->value->ToInt32();
							}
						}
						if (str3.v[0] >= '0' && str3.v[0] <= '9')
						{
							val2 = str3.ToInt32();
						}
						else
						{
							cfg = this->cfgMap.Get(str3.ToCString());
							if (cfg)
							{
								val2 = cfg->value->ToInt32();
							}
						}
						if (val1 >= val2)
						{
							str1 = str2;
						}
						else
						{
							valid = false;
						}
					}
				}
			}
			else if (str1.StartsWith(UTF8STRC("@(")))
			{
				i = str1.IndexOf(')');
				if (i != INVALID_INDEX && i > 1)
				{
					Text::StringBuilderUTF8 result;
					NotNullPtr<Text::String> cmd = Text::String::New(&str1.v[2], i - 2);
					Int32 ret;
					if ((ret = Manage::Process::ExecuteProcess(cmd->ToCString(), result)) != 0)
					{
						valid = false;
					}
					else
					{
						str1 = str1.Substring(i + 1);
					}
					cmd->Release();
				}
			}
			if (valid && prog)
			{
				prog->libs.Add(Text::String::New(str1.ToCString()));
			}
		}
		else if (sb.ToString()[0] == '$')
		{
			Bool valid = true;
			sb2.ClearStr();
			AppendCfgItem(sb2, sb.ToCString().Substring(1));
			str1 = sb2;
			if (str1.StartsWith(UTF8STRC("@(")))
			{
				i = str1.IndexOf(')');
				if (i != INVALID_INDEX && i > 1)
				{
					Text::StringBuilderUTF8 result;
					NotNullPtr<Text::String> cmd = Text::String::New(&str1.v[2], (UOSInt)i - 2);
					Int32 ret;
					if ((ret = Manage::Process::ExecuteProcess(cmd->ToCString(), result)) != 0)
					{
						valid = false;
					}
					else
					{
						str1 = str1.Substring(i + 1);
					}
					cmd->Release();
				}
			}
			if (prog && valid)
			{
				Text::CString ccfg = str1.ToCString();
				IO::SMake::ConfigItem *cfg = this->cfgMap.Get(str1.ToCString());
				if (cfg)
				{
					ccfg = cfg->value->ToCString();
				}
				if (prog->compileCfg)
				{
					sb.ClearStr();
					sb.Append(prog->compileCfg);
					sb.AppendUTF8Char(' ');
					sb.Append(ccfg);
					prog->compileCfg->Release();
					prog->compileCfg = Text::String::New(sb.ToString(), sb.GetLength()).Ptr();
				}
				else
				{
					prog->compileCfg = Text::String::New(ccfg).Ptr();
				}
			}
		}
		else if (sb.StartsWith(UTF8STRC("export ")))
		{
			cfg = this->cfgMap.Get({sb.ToString() + 7, sb.GetLength() - 7});
			if (cfg)
			{
				Manage::EnvironmentVar env;
				env.SetValue(cfg->name->ToCString(), cfg->value->ToCString());
			}
		}
		else if ((i = sb.IndexOf(UTF8STRC("+="))) != INVALID_INDEX)
		{
			str2 = sb.SubstrTrim(i + 2);
			str1 = sb.SubstrTrim(0, i);
			cfg = this->cfgMap.Get(str1.ToCString());
			if (cfg)
			{
				if (cfg->value->leng > 0)
				{
					sb2.ClearStr();
					sb2.Append(cfg->value);
					sb2.AppendUTF8Char(' ');
					AppendCfgItem(sb2, str2.ToCString());
					cfg->value->Release();
					cfg->value = Text::String::New(sb2.ToString(), sb2.GetLength());
				}
				else
				{
					cfg->value->Release();
					sb2.ClearStr();
					AppendCfgItem(sb2, str2.ToCString());
					cfg->value = Text::String::New(sb2.ToString(), sb2.GetLength());
				}
			}
			else
			{
				cfg = MemAlloc(IO::SMake::ConfigItem, 1);
				cfg->name = Text::String::New(str1.ToCString());
				sb2.ClearStr();
				AppendCfgItem(sb2, str2.ToCString());
				cfg->value = Text::String::New(sb2.ToString(), sb2.GetLength());
				this->cfgMap.PutNN(cfg->name, cfg);
			}
		}
		else if ((i = sb.IndexOf(UTF8STRC(":"))) != INVALID_INDEX)
		{
			if (sb.ToString()[i + 1] == '=')
			{
				str1 = sb.SubstrTrim(0, i);
				str2 = sb.SubstrTrim(i + 2);
				cfg = this->cfgMap.Get(str1.ToCString());
				if (cfg)
				{
					cfg->value->Release();
					sb2.ClearStr();
					AppendCfgItem(sb2, str2.ToCString());
					cfg->value = Text::String::New(sb2.ToString(), sb2.GetLength());
				}
				else
				{
					cfg = MemAlloc(IO::SMake::ConfigItem, 1);
					cfg->name = Text::String::New(str1.ToCString());
					sb2.ClearStr();
					AppendCfgItem(sb2, str2.ToCString());
					cfg->value = Text::String::New(sb2.ToString(), sb2.GetLength());
					this->cfgMap.PutNN(cfg->name, cfg);
				}
			}
			else
			{
				str1 = sb.SubstrTrim(0, i);
				str2 = sb.SubstrTrim(i + 1);
				if (str1.v[0] == '+')
				{
					prog = this->progMap.GetC(str1.Substring(1).ToCString());
					if (prog)
					{
					}
					else
					{
						NEW_CLASS(prog, IO::SMake::ProgramItem());
						prog->name = Text::String::New(str1.Substring(1).ToCString());
						if (str2.v[0])
						{
							prog->srcFile = Text::String::New(str2.ToCString()).Ptr();
						}
						else
						{
							prog->srcFile = 0;
						}
						prog->compileCfg = 0;
						prog->compiled = false;
						this->progMap.PutNN(prog->name, prog);
					}
				}
				else
				{
					prog = this->progMap.GetC(str1.ToCString());
					if (prog)
					{
						ret = false;
						sb2.ClearStr();
						sb2.AppendC(UTF8STRC("Program Item "));
						sb2.Append(str1);
						sb2.AppendC(UTF8STRC(" duplicated"));
						this->SetErrorMsg(sb2.ToCString());
					}
					else
					{
						NEW_CLASS(prog, IO::SMake::ProgramItem());
						prog->name = Text::String::New(str1.ToCString());
						if (str2.v[0])
						{
							prog->srcFile = Text::String::New(str2.ToCString()).Ptr();
						}
						else
						{
							prog->srcFile = 0;
						}
						prog->compileCfg = 0;
						prog->compiled = false;
						this->progMap.PutNN(prog->name, prog);
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
			str1 = sb.SubstrTrim(8);
			if (IO::Path::IsSearchPattern(str1.v))
			{
				IO::FileFindRecur srch(str1.ToCString());
				Text::CString cstr;
				while (true)
				{
					cstr = srch.NextFile(0);
					if (cstr.IsNull())
						break;
					if (!LoadConfigFile(cstr.OrEmpty()))
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
				if (!LoadConfigFile(str1.ToCString()))
				{
					ret = false;
					break;
				}
			}
		}
		sb.ClearStr();
	}
	return ret;
}


Bool IO::SMake::ParseSource(NotNullPtr<Data::FastStringMap<Int32>> objList,
	NotNullPtr<Data::FastStringMap<Int32>> libList,
	NotNullPtr<Data::FastStringMap<Int32>> procList,
	Optional<Data::ArrayListStringNN> headerList,
	OutParam<Int64> latestTime,
	Text::CStringNN sourceFile,
	NotNullPtr<Text::StringBuilderUTF8> tmpSb)
{
	Bool skipCheck = false;
	if (sourceFile.StartsWith('@'))
		sourceFile = sourceFile.Substring(1);
	if (sourceFile.StartsWith('!'))
	{
		skipCheck = true;
		sourceFile = sourceFile.Substring(1);
	}
	if (sourceFile.EndsWith(UTF8STRC(".asm")))
	{
		skipCheck = true;
	}
	Text::CStringNN fileName;
	if (IO::Path::PATH_SEPERATOR == '\\')
	{
		tmpSb->ClearStr();
		tmpSb->Append(sourceFile);
		tmpSb->Replace('/', '\\');
		fileName = tmpSb->ToCString();
	}
	else
	{
		fileName = sourceFile;
	}
	if (skipCheck)
	{
		Data::Timestamp t = IO::Path::GetModifyTime(fileName.v);
		if (t.IsNull())
		{
			tmpSb->ClearStr();
			tmpSb->AppendC(UTF8STRC("Error in opening source \""));
			tmpSb->Append(sourceFile);
			tmpSb->AppendC(UTF8STRC("\""));
			this->SetErrorMsg(tmpSb->ToCString());
			return false;
		}
		latestTime.Set(t.ToTicks());
		return true;
	}
	IO::FileStream fs(fileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
//		printf("Opening %s\r\n", fileName.v);
	if (fs.IsError())
	{
		tmpSb->ClearStr();
		tmpSb->AppendC(UTF8STRC("Error in opening source \""));
		tmpSb->Append(sourceFile);
		tmpSb->AppendC(UTF8STRC("\""));
		this->SetErrorMsg(tmpSb->ToCString());
		return false;
	}
	Int64 lastTime;
	Int64 thisTime;
	lastTime = fs.GetModifyTime().ToTicks();
	Text::PString line;
	Text::PString str1;
	UOSInt i;
	IO::SMake::ProgramItem *prog;
	Text::UTF8Reader reader(fs);
	tmpSb->ClearStr();
	while (reader.ReadLine(tmpSb, 1024))
	{
		line = tmpSb->TrimAsNew();
		if (line.StartsWith(UTF8STRC("#include")))
		{
			str1 = line.SubstrTrim(8);
			if (str1.v[0] == '"')
			{
				str1 = str1.Substring(1);
				i = str1.IndexOf('"');
				if (i != INVALID_INDEX)
				{
					str1.TrimToLength(i);
					if (procList->IndexOfC(str1.ToCString()) >= 0)
					{
						thisTime = this->fileTimeMap.GetC(str1.ToCString());
						if (thisTime && thisTime > lastTime)
						{
							lastTime = thisTime;
						}
					}
					else if (str1.EndsWith(UTF8STRC(".cpp")))
					{
						
					}
					else
					{
						prog = this->progMap.GetC(str1.ToCString());
						if (prog == 0)
						{
							Text::StringBuilderUTF8 sb2;
							sb2.Append(str1);
							sb2.AppendC(UTF8STRC(" not found in "));
							sb2.Append(sourceFile);
							this->SetErrorMsg(sb2.ToCString());
							return false;
						}
						else
						{
							NotNullPtr<Text::String> debugObj;
							procList->PutNN(prog->name, 1);

							Data::ArrayIterator<NotNullPtr<Text::String>> it = prog->subItems.Iterator();
							while (it.HasNext())
							{
								NotNullPtr<Text::String> subItem = it.Next();
								if (debugObj.Set(this->debugObj) && this->messageWriter && subItem->Equals(debugObj))
								{
									Text::StringBuilderUTF8 sb2;
									sb2.Append(debugObj);
									sb2.AppendC(UTF8STRC(" found in "));
									sb2.Append(sourceFile);
									this->messageWriter->WriteLineC(sb2.ToString(), sb2.GetLength());
								}
								if (objList->PutNN(subItem, 1) != 1)
								{
									if (!this->ParseObject(objList, libList, procList, headerList, subItem, prog->name->ToCString(), tmpSb))
									{
										return false;
									}
									/////////////////////////////////
								}
							}
							it = prog->libs.Iterator();
							while (it.HasNext())
							{
								libList->PutNN(it.Next(), 1);
							}
							if (debugObj.Set(this->debugObj) && this->messageWriter && prog->name->Equals(debugObj))
							{
								Text::StringBuilderUTF8 sb2;
								sb2.Append(debugObj);
								sb2.AppendC(UTF8STRC(" found in "));
								sb2.Append(sourceFile);
								this->messageWriter->WriteLineC(sb2.ToString(), sb2.GetLength());
							}
							if (!this->ParseHeader(objList, libList, procList, headerList, thisTime, prog->name, sourceFile, tmpSb))
							{
								return false;
							}
							if (thisTime > lastTime)
							{
								lastTime = thisTime;
							}
/*							NotNullPtr<Text::String> s;
							///////////////////////////////////
							if (s.Set(prog->srcFile))
							{
								if (!this->ParseSource(objList, libList, procList, headerList, thisTime, s->ToCString(), tmpSb))
								{
									return false;
								}
								this->fileTimeMap.PutNN(s, thisTime);
							}*/
						}
					}
				}
			}
		}
		tmpSb->ClearStr();
	}
	latestTime.Set(lastTime);
	return true;
}

Bool IO::SMake::ParseHeader(NotNullPtr<Data::FastStringMap<Int32>> objList,
	NotNullPtr<Data::FastStringMap<Int32>> libList,
	NotNullPtr<Data::FastStringMap<Int32>> procList,
	Optional<Data::ArrayListStringNN> headerList,
	OutParam<Int64> latestTime,
	NotNullPtr<Text::String> headerFile,
	Text::CStringNN sourceFile,
	NotNullPtr<Text::StringBuilderUTF8> tmpSb)
{
	IO::SMake::ConfigItem *cfg = this->cfgMap.Get(CSTR("INCLUDEPATH"));
	if (cfg == 0)
	{
		this->SetErrorMsg(CSTR("INCLUDEPATH config not found"));
		return false;
	}
	Text::StringBuilderUTF8 sb2;
	Text::PString sarr[2];
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UOSInt i;
	Int64 thisTime;
	NotNullPtr<Data::ArrayListStringNN> nnheaderList;
	sb2.Append(cfg->value);
	sarr[1] = sb2;
	i = 2;
	while (i == 2)
	{
		i = Text::StrSplitTrimP(sarr, 2, sarr[1], ':');
		sptr = IO::Path::GetRealPath(sbuff, sarr[0].v, sarr[0].leng);
		*sptr++ = IO::Path::PATH_SEPERATOR;
		sptr = headerFile->ConcatTo(sptr);

		if (IO::Path::GetPathType(CSTRP(sbuff, sptr)) == IO::Path::PathType::File)
		{
			if (headerList.SetTo(nnheaderList) && nnheaderList->SortedIndexOf(headerFile) < 0)
			{
				nnheaderList->SortedInsert(headerFile);
			}
			if (this->ParseSource(objList, libList, procList, headerList, thisTime, CSTRP(sbuff, sptr), tmpSb))
			{
				this->fileTimeMap.PutNN(headerFile, thisTime);
				latestTime.Set(thisTime);
				return true;
			}
			else
			{
				return false;
			}
		}
	}
	tmpSb->ClearStr();
	tmpSb->Append(sourceFile);
	tmpSb->Replace('\\', '/');
	i = tmpSb->LastIndexOf('/');
	tmpSb->TrimToLength(i);
	const UTF8Char *currHeader = headerFile->v;
	const UTF8Char *currHeaderEnd = &headerFile->v[headerFile->leng];
	while (Text::StrStartsWithC(currHeader, (UOSInt)(currHeaderEnd - currHeader), UTF8STRC("../")))
	{
		i = tmpSb->LastIndexOf('/');
		tmpSb->TrimToLength(i);
		currHeader = currHeader + 3;
	}
	tmpSb->AppendUTF8Char(IO::Path::PATH_SEPERATOR);
	tmpSb->AppendC(currHeader, (UOSInt)(currHeaderEnd - currHeader));
	sptr = IO::Path::GetRealPath(sbuff, tmpSb->ToString(), tmpSb->GetLength());
	if (IO::Path::GetPathType(CSTRP(sbuff, sptr)) == IO::Path::PathType::File)
	{
		if (headerList.SetTo(nnheaderList) && nnheaderList->SortedIndexOf(headerFile) < 0)
		{
			nnheaderList->SortedInsert(headerFile);
		}
		if (this->ParseSource(objList, libList, procList, headerList, thisTime, CSTRP(sbuff, sptr), tmpSb))
		{
			this->fileTimeMap.PutNN(headerFile, thisTime);
			latestTime.Set(thisTime);
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
	sb2.Append(sourceFile);
	this->SetErrorMsg(sb2.ToCString());
	return false;
}

Bool IO::SMake::ParseObject(NotNullPtr<Data::FastStringMap<Int32>> objList, NotNullPtr<Data::FastStringMap<Int32>> libList, NotNullPtr<Data::FastStringMap<Int32>> procList, Optional<Data::ArrayListStringNN> headerList, NotNullPtr<Text::String> objectFile, Text::CStringNN sourceFile, NotNullPtr<Text::StringBuilderUTF8> tmpSb)
{
	if (!objectFile->EndsWith(UTF8STRC(".o")))
	{
		return true;
	}

	IO::SMake::ProgramItem *prog = this->progMap.GetNN(objectFile);
	if (prog == 0)
	{
		Text::StringBuilderUTF8 sb2;
		sb2.Append(objectFile);
		sb2.AppendC(UTF8STRC(" not found in "));
		sb2.Append(sourceFile);
		this->SetErrorMsg(sb2.ToCString());
		return false;
	}
	else
	{
		NotNullPtr<Text::String> debugObj;
		Data::ArrayIterator<NotNullPtr<Text::String>> it = prog->subItems.Iterator();
		while (it.HasNext())
		{
			NotNullPtr<Text::String> subItem = it.Next();
			if (debugObj.Set(this->debugObj) && this->messageWriter && subItem->Equals(debugObj))
			{
				Text::StringBuilderUTF8 sb2;
				sb2.Append(debugObj);
				sb2.AppendC(UTF8STRC(" found in "));
				sb2.Append(sourceFile);
				this->messageWriter->WriteLineC(sb2.ToString(), sb2.GetLength());
			}
			if (objList->PutNN(subItem, 1) != 1)
			{
				if (!this->ParseObject(objList, libList, procList, headerList, subItem, objectFile->ToCString(), tmpSb))
				{
					return false;
				}
			}
		}
		it = prog->libs.Iterator();
		while (it.HasNext())
		{
			libList->PutNN(it.Next(), 1);
		}
		if (debugObj.Set(this->debugObj) && this->messageWriter && prog->name->Equals(debugObj))
		{
			Text::StringBuilderUTF8 sb2;
			sb2.Append(debugObj);
			sb2.AppendC(UTF8STRC(" found in "));
			sb2.Append(sourceFile);
			this->messageWriter->WriteLineC(sb2.ToString(), sb2.GetLength());
		}
		NotNullPtr<Text::String> s;
		if (s.Set(prog->srcFile))
		{
			Int64 thisTime;
			if (!this->ParseSource(objList, libList, procList, headerList, thisTime, s->ToCString(), tmpSb))
			{
				return false;
			}
			this->fileTimeMap.PutNN(s, thisTime);
		}
		return true;
	}
}

Bool IO::SMake::ParseProgInternal(NotNullPtr<Data::FastStringMap<Int32>> objList,
	NotNullPtr<Data::FastStringMap<Int32>> libList,
	NotNullPtr<Data::FastStringMap<Int32>> procList,
	Optional<Data::ArrayListStringNN> headerList,
	OutParam<Int64> latestTime,
	OutParam<Bool> progGroup,
	NotNullPtr<const ProgramItem> prog,
	NotNullPtr<Text::StringBuilderUTF8> tmpSb)
{
	NotNullPtr<Text::String> subItem;
	NotNullPtr<Text::String> debugObj;
	IO::SMake::ProgramItem *subProg;
	Int64 thisTime;
	Int64 lastTime = 0;
	Bool progGrp = true;
	Data::ArrayIterator<NotNullPtr<Text::String>> it = prog->subItems.Iterator();
	while (it.HasNext())
	{
		subItem = it.Next();
		if (subItem->EndsWith(UTF8STRC(".o")))
		{
			progGrp = false;
		}
		if (debugObj.Set(this->debugObj) && this->messageWriter && subItem->Equals(debugObj))
		{
			tmpSb->ClearStr();
			tmpSb->Append(debugObj);
			tmpSb->AppendC(UTF8STRC(" depends by "));
			tmpSb->Append(prog->name);
			this->messageWriter->WriteLineC(tmpSb->ToString(), tmpSb->GetLength());
		}
		objList->PutNN(subItem, 1);
	}

	progGroup.Set(progGrp);
	if (progGrp)
	{
		latestTime.Set(lastTime);
		return true;
	}

	IO::SMake::ConfigItem *cfg = this->cfgMap.Get(CSTR("DEPS"));
	if (cfg)
	{
		subProg = this->progMap.GetNN(cfg->value);
		if (subProg == 0)
		{
			tmpSb->AppendC(UTF8STRC("Item "));
			tmpSb->Append(cfg->value);
			tmpSb->AppendC(UTF8STRC(" not found"));
			this->SetErrorMsg(tmpSb->ToCString());
			return false;
		}
		objList->PutNN(cfg->value, 1);
		if (!this->ParseSource(objList, libList, procList, headerList, thisTime, subProg->srcFile->ToCString(), tmpSb))
		{
			return false;
		}
		this->fileTimeMap.Put(subProg->srcFile, thisTime);
		lastTime = thisTime;
	}

	it = prog->subItems.Iterator();
	while (it.HasNext())
	{
		subItem = it.Next();
		subProg = this->progMap.GetNN(subItem);
		if (subProg == 0)
		{
			tmpSb->AppendC(UTF8STRC("Item "));
			tmpSb->Append(subItem);
			tmpSb->AppendC(UTF8STRC(" not found"));
			this->SetErrorMsg(tmpSb->ToCString());
			return false;
		}
		if (!this->ParseSource(objList, libList, procList, headerList, thisTime, subProg->srcFile->ToCString(), tmpSb))
		{
			return false;
		}
		this->fileTimeMap.Put(subProg->srcFile, thisTime);
		if (thisTime > lastTime)
		{
			lastTime = thisTime;
		}
	}
	latestTime.Set(lastTime);
	return true;
}

void IO::SMake::CompileTask(void *userObj)
{
	CompileReq *req = (CompileReq *)userObj;
	if (!req->me->ExecuteCmd(req->cmd->ToCString()))
	{
		req->me->error = true;
	}
	req->cmd->Release();
	MemFree(req);
}

void IO::SMake::CompileObject(Text::CStringNN cmd)
{
	CompileReq *req = MemAlloc(CompileReq, 1);
	req->cmd = Text::String::New(cmd);
	req->me = this;
	this->tasks->AddTask(CompileTask, req);
}

void IO::SMake::CompileObject(NotNullPtr<Text::String> cmd)
{
	CompileReq *req = MemAlloc(CompileReq, 1);
	req->cmd = cmd;
	req->me = this;
	this->tasks->AddTask(CompileTask, req);
}

Bool IO::SMake::CompileProgInternal(NotNullPtr<const ProgramItem> prog, Bool asmListing, Bool enableTest)
{
	Data::FastStringMap<Int32> libList;
	Data::FastStringMap<Int32> objList;
	Data::FastStringMap<Int32> procList;
	Int64 latestTime = 0;
	Int64 thisTime;
	NotNullPtr<IO::SMake::ProgramItem> subProg;
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
		IO::SMake::ConfigItem *testCfg = this->cfgMap.Get(CSTR("ENABLE_TEST"));
		if (testCfg && testCfg->value->Equals(UTF8STRC("1")))
		{
			enableTest = true;
		}
	}

	Text::StringBuilderUTF8 sb;

	if (!this->ParseProgInternal(objList, libList, procList, 0, latestTime, progGroup, prog, sb))
	{
		return false;
	}
	if (progGroup)
	{
		Data::FastStringKeyIterator<Int32> iterator = objList.KeyIterator();
		while (iterator.HasNext())
		{
			NotNullPtr<Text::String> objName = iterator.Next();
			if (!subProg.Set(this->progMap.GetNN(objName)))
			{
				sb.ClearStr();
				sb.AppendC(UTF8STRC("Program "));
				sb.Append(objName);
				sb.AppendC(UTF8STRC(" not found"));
				this->SetErrorMsg(sb.ToCString());
				return false;
			}
			if (!this->CompileProgInternal(subProg, asmListing, enableTest))
			{
				return false;
			}
		}
		return true;
	}

	IO::SMake::ConfigItem *cppCfg = this->cfgMap.Get(CSTR("CXX"));
	IO::SMake::ConfigItem *ccCfg = this->cfgMap.Get(CSTR("CC"));
	IO::SMake::ConfigItem *asmCfg = this->cfgMap.Get(CSTR("ASM"));
	IO::SMake::ConfigItem *asmflagsCfg = this->cfgMap.Get(CSTR("ASMFLAGS"));
	IO::SMake::ConfigItem *cflagsCfg = this->cfgMap.Get(CSTR("CFLAGS"));
	IO::SMake::ConfigItem *cppflagsCfg = this->cfgMap.Get(CSTR("CPPFLAGS"));
	IO::SMake::ConfigItem *libsCfg = this->cfgMap.Get(CSTR("LIBS"));
	Data::Timestamp dt1;
	Data::Timestamp dt2;
	if (cppCfg == 0)
	{
		this->SetErrorMsg(CSTR("CXX config not found"));
		return false;
	}
	if (ccCfg == 0)
	{
		this->SetErrorMsg(CSTR("CC config not found"));
		return false;
	}
	if (asmCfg == 0)
	{
		this->SetErrorMsg(CSTR("ASM config not found"));
		return false;
	}
	
	IO::Path::CreateDirectory(CSTR(OBJECTPATH));
	Data::FastStringKeyIterator<Int32> iterator = objList.KeyIterator();
	while (iterator.HasNext())
	{
		NotNullPtr<Text::String> objName = iterator.Next();
		if (!subProg.Set(this->progMap.GetNN(objName)))
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Object "));
			sb.Append(objName);
			sb.AppendC(UTF8STRC(" not found"));
			this->SetErrorMsg(sb.ToCString());
			return false;
		}
		NotNullPtr<Text::String> srcFile;
		if (!srcFile.Set(subProg->srcFile))
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Object "));
			sb.Append(objName);
			sb.AppendC(UTF8STRC(" does not have source file"));
			this->SetErrorMsg(sb.ToCString());
			return false;
		}

		Data::ArrayIterator<NotNullPtr<Text::String>> itLibs = subProg->libs.Iterator();
		while (itLibs.HasNext())
		{
			libList.PutNN(itLibs.Next(), 1);
		}

		Bool updateToDate = false;
		Int64 lastTime;
		if (this->HasError())
		{
			this->tasks->WaitForIdle();
			return false;
		}

		if (subProg->compiled)
		{
			updateToDate = true;
		}
		else if (srcFile->v[0] != '@')
		{
			sb.ClearStr();
			sb.Append(this->basePath);
			if (srcFile->StartsWith('!'))
			{
				IO::Path::AppendPath(sb, srcFile->v + 1, srcFile->leng - 1);
			}
			else
			{
				IO::Path::AppendPath(sb, srcFile->v, srcFile->leng);
			}
			if ((dt1 = IO::Path::GetModifyTime(sb.ToString())).IsNull())
			{
				sb.ClearStr();
				sb.AppendC(UTF8STRC("Source file time cannot get: "));
				sb.Append(srcFile);
				this->SetErrorMsg(sb.ToCString());
				return false;
			}
			lastTime = dt1.ToTicks();
			thisTime = this->fileTimeMap.GetNN(srcFile);
			if (thisTime && thisTime > lastTime)
			{
				lastTime = thisTime;
			}

			sb.ClearStr();
			sb.AppendC(UTF8STRC(OBJECTPATH));
			sb.AppendUTF8Char(IO::Path::PATH_SEPERATOR);
			sb.Append(subProg->name);
			if (!(dt2 = IO::Path::GetModifyTime(sb.ToString())).IsNull())
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
						sb.AppendTSNoZone(dt2);
						sb.AppendC(UTF8STRC(", Obj time: "));
						sb.AppendTSNoZone(dt1);
						sb.AppendC(UTF8STRC(", Last time: "));
						dt1 = Data::Timestamp(lastTime, Data::DateTimeUtil::GetLocalTzQhr());
						sb.AppendTSNoZone(dt1);
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
			if (this->messageWriter)
			{
				sb.ClearStr();
				sb.AppendC(UTF8STRC("Obj "));
				sb.Append(subProg->name);
				sb.AppendC(UTF8STRC(" creating from "));
				sb.Append(srcFile);
				this->messageWriter->WriteLineC(sb.ToString(), sb.GetLength());
			}
			if (srcFile->EndsWith(UTF8STRC(".cpp")))
			{
				sb.ClearStr();
				AppendCfgPath(sb, cppCfg->value->ToCString());
				sb.AppendUTF8Char(' ');
				if (cflagsCfg)
				{
					sb.Append(cflagsCfg->value);
					sb.AppendUTF8Char(' ');
				}
				if (cppflagsCfg)
				{
					sb.Append(cppflagsCfg->value);
					sb.AppendUTF8Char(' ');
				}
				if (subProg->compileCfg)
				{
					this->AppendCfg(sb, subProg->compileCfg->ToCString());
					sb.AppendUTF8Char(' ');
				}
				if (asmListing)
				{
					sb.AppendC(UTF8STRC("-Wa,-adhln="));
					sb.AppendC(UTF8STRC(OBJECTPATH));
					sb.AppendUTF8Char(IO::Path::PATH_SEPERATOR);
					sb.Append(subProg->name);
					sb.RemoveChars(1);
					sb.AppendC(UTF8STRC("s "));
				}
				sb.AppendC(UTF8STRC("-c -o "));
				sb.AppendC(UTF8STRC(OBJECTPATH));
				sb.AppendUTF8Char(IO::Path::PATH_SEPERATOR);
				sb.Append(subProg->name);
				sb.AppendUTF8Char(' ');
				if (srcFile->v[0] == '@' || srcFile->v[0] == '!')
				{
					sb.AppendC(&srcFile->v[1], srcFile->leng - 1);
				}
				else
				{
					sb.Append(srcFile);
				}

				this->CompileObject(sb.ToCString());
				subProg->compiled = true;
			}
			else if (srcFile->EndsWith(UTF8STRC(".c")))
			{
				sb.ClearStr();
				AppendCfgPath(sb, ccCfg->value->ToCString());
				sb.AppendUTF8Char(' ');
				if (cflagsCfg)
				{
					sb.Append(cflagsCfg->value);
					sb.AppendUTF8Char(' ');
				}
				if (subProg->compileCfg)
				{
					this->AppendCfg(sb, subProg->compileCfg->ToCString());
					sb.AppendUTF8Char(' ');
				}
				if (asmListing)
				{
					sb.AppendC(UTF8STRC("-Wa,-adhln="));
					sb.AppendC(UTF8STRC(OBJECTPATH));
					sb.AppendUTF8Char(IO::Path::PATH_SEPERATOR);
					sb.Append(subProg->name);
					sb.RemoveChars(1);
					sb.AppendC(UTF8STRC("s "));
				}
				sb.AppendC(UTF8STRC("-c -o "));
				sb.AppendC(UTF8STRC(OBJECTPATH));
				sb.AppendUTF8Char(IO::Path::PATH_SEPERATOR);
				sb.Append(subProg->name);
				sb.AppendUTF8Char(' ');
				if (srcFile->v[0] == '@' || srcFile->v[0] == '!')
				{
					sb.AppendC(&srcFile->v[1], srcFile->leng - 1);
				}
				else
				{
					sb.Append(srcFile);
				}
				this->CompileObject(sb.ToCString());
				subProg->compiled = true;
			}
			else if (srcFile->EndsWith(UTF8STRC(".asm")))
			{
				sb.ClearStr();
				AppendCfgPath(sb, asmCfg->value->ToCString());
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
					sb.AppendUTF8Char(IO::Path::PATH_SEPERATOR);
					sb.Append(subProg->name);
					sb.RemoveChars(1);
					sb.AppendC(UTF8STRC("lst "));
				}
				sb.AppendC(UTF8STRC("-o "));
				sb.AppendC(UTF8STRC(OBJECTPATH));
				sb.AppendUTF8Char(IO::Path::PATH_SEPERATOR);
				sb.Append(subProg->name);
				sb.AppendUTF8Char(' ');
				if (srcFile->v[0] == '@' || srcFile->v[0] == '!')
				{
					sb.AppendC(&srcFile->v[1], srcFile->leng - 1);
				}
				else
				{
					sb.Append(srcFile);
				}
				this->CompileObject(sb.ToCString());
				subProg->compiled = true;
			}
			else if (srcFile->EndsWith(UTF8STRC(".s")))
			{

			}
			else
			{
				this->tasks->WaitForIdle();
				sb.ClearStr();
				sb.AppendC(UTF8STRC("Unknown source file format "));
				sb.Append(srcFile);
				this->SetErrorMsg(sb.ToCString());
				return false;
			}
		}
	}
	if (!this->asyncMode)
		this->tasks->WaitForIdle();
	if (this->HasError())
	{
		return false;
	}

	IO::SMake::ConfigItem *postfixItem = this->cfgMap.Get(CSTR("OUTPOSTFIX"));
	sb.ClearStr();
	sb.Append(this->basePath);
	sb.AppendC(UTF8STRC("bin"));
	IO::Path::CreateDirectory(sb.ToCString());
	sb.AppendUTF8Char(IO::Path::PATH_SEPERATOR);
	sb.Append(prog->name);
	if (postfixItem)
	{
		sb.Append(postfixItem->value);
	}
	Bool skipLink = false;
	if (!(dt2 = IO::Path::GetModifyTime(sb.ToString())).IsNull())
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
		AppendCfgPath(sb, cppCfg->value->ToCString());
		sb.AppendUTF8Char(' ');
		sb.AppendC(UTF8STRC("-o bin/"));
		sb.Append(prog->name);
		if (postfixItem)
		{
			sb.Append(postfixItem->value);
		}
		Data::FastStringKeyIterator<Int32> iterator = objList.KeyIterator();
		while (iterator.HasNext())
		{
			sb.AppendUTF8Char(' ');
			sb.AppendC(UTF8STRC(OBJECTPATH));
			sb.AppendUTF8Char(IO::Path::PATH_SEPERATOR);
			sb.Append(iterator.Next());
		}
		if (libsCfg)
		{
			sb.AppendUTF8Char(' ');
			sb.Append(libsCfg->value);
		}
		UOSInt i;
		UOSInt j;
		i = 0;
		j = libList.GetCount();
		while (i < j)
		{
	//		printf("Libs: %s\r\n", libList.GetItem(i));
			sb.AppendUTF8Char(' ');
			Text::String *lib = libList.GetKey(i);
			AppendCfg(sb, lib->ToCString());
			i++;
		}
#if defined(_WIN32) || defined(__CYGWIN__)
		if (sb.leng > 32767)
		{
			this->tasks->WaitForIdle();
			if (this->HasError())
			{
				return false;
			}
			Data::FastStringKeyIterator<Int32> iterator = objList.KeyIterator();
			NotNullPtr<Text::String> objName;
			sb.ClearStr();
			while (iterator.HasNext())
			{
				objName = iterator.Next();
				sb.AppendUTF8Char(' ');
				sb.AppendC(UTF8STRC(OBJECTPATH));
				sb.AppendUTF8Char('/');
				sb.Append(objName);
			}
			if (sb.leng == 0)
				return false;
			else
			{
				IO::FileStream fs(CSTR("filelist.txt"), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
				fs.Write(sb.v + 1, sb.leng - 1);
			}
			sb.ClearStr();
			AppendCfgPath(sb, cppCfg->value->ToCString());
			sb.AppendUTF8Char(' ');
			sb.AppendC(UTF8STRC("-o bin/"));
			sb.Append(prog->name);
			if (postfixItem)
			{
				sb.Append(postfixItem->value);
			}
			sb.AppendC(UTF8STRC(" @filelist.txt"));
			if (libsCfg)
			{
				sb.AppendUTF8Char(' ');
				sb.Append(libsCfg->value);
			}
			UOSInt i;
			UOSInt j;
			i = 0;
			j = libList.GetCount();
			while (i < j)
			{
		//		printf("Libs: %s\r\n", libList.GetItem(i));
				sb.AppendUTF8Char(' ');
				Text::String *lib = libList.GetKey(i);
				AppendCfg(sb, lib->ToCString());
				i++;
			}
			if (!this->ExecuteCmd(sb.ToCString()))
			{
				IO::Path::DeleteFile((const UTF8Char*)"filelist.txt");
				return false;
			}
			IO::Path::DeleteFile((const UTF8Char*)"filelist.txt");
		}
		else if (this->asyncMode)
		{
			this->linkCmds.Add(Text::String::New(sb.ToCString()));
		}
		else if (!this->ExecuteCmd(sb.ToCString()))
		{
			return false;
		}
#else
		if (this->asyncMode)
		{
			this->linkCmds.Add(Text::String::New(sb.ToCString()));
		}
		else
		{
			if (!this->ExecuteCmd(sb.ToCString()))
			{
				return false;
			}
		}
#endif
	}

	if (enableTest)
	{
		if (this->asyncMode)
		{
			this->testProgs.Add(prog);
		}
		else
		{
			if (!this->TestProg(prog, sb))
			{
				return false;
			}
		}
	}

	return true;
}

Bool IO::SMake::TestProg(NotNullPtr<const ProgramItem> prog, NotNullPtr<Text::StringBuilderUTF8> sb)
{
	if (this->cmdWriter)
	{
		sb->ClearStr();
		sb->AppendC(UTF8STRC("Testing "));
		sb->Append(prog->name);
		this->cmdWriter->WriteLineC(sb->ToString(), sb->GetLength());
	}
	sb->ClearStr();
	sb->Append(this->basePath);
	sb->AppendC(UTF8STRC("bin"));
	sb->AppendUTF8Char(IO::Path::PATH_SEPERATOR);
	sb->Append(prog->name);

	Text::StringBuilderUTF8 sbRet;
	Int32 ret = Manage::Process::ExecuteProcess(sb->ToCString(), sbRet);
	if (ret != 0)
	{
		sb->ClearStr();
		sb->AppendC(UTF8STRC("Test failed: "));
		sb->Append(prog->name);
		sb->AppendC(UTF8STRC(", return "));
		sb->AppendI32(ret);
		this->SetErrorMsg(sb->ToCString());
		this->error = true;
		return false;
	}
	return true;
}

void IO::SMake::SetErrorMsg(Text::CString msg)
{
	Sync::MutexUsage mutUsage(this->errorMsgMut);
	SDEL_STRING(this->errorMsg);
	this->errorMsg = Text::String::New(msg).Ptr();
}

IO::SMake::SMake(Text::CStringNN cfgFile, UOSInt threadCnt, IO::Writer *messageWriter) : IO::ParsedObject(cfgFile)
{
	NEW_CLASS(this->tasks, Sync::ParallelTask(threadCnt, false));
	this->errorMsg = 0;
	this->error = false;
	this->asyncMode = false;
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
	NotNullPtr<const Data::ArrayList<IO::SMake::ConfigItem *>> cfgList = this->cfgMap.GetValues();
	i = cfgList->GetCount();
	while (i-- > 0)
	{
		cfg = cfgList->GetItem(i);
		cfg->name->Release();
		cfg->value->Release();
		MemFree(cfg);
	}
	this->ClearLinks();

//	Data::ArrayList<IO::SMake::ProgramItem*> *progList = progMap->GetValues();
	IO::SMake::ProgramItem *prog;
//	i = progList->GetCount();
	i = this->progMap.GetCount();
	while (i-- > 0)
	{
//		prog = progList->GetItem(i);
		prog = this->progMap.GetItem(i);
		prog->name->Release();
		SDEL_STRING(prog->srcFile);
		SDEL_STRING(prog->compileCfg);
		j = prog->subItems.GetCount();
		while (j-- > 0)
		{
			prog->subItems.GetItem(j)->Release();
		}
		j = prog->libs.GetCount();
		while (j-- > 0)
		{
			prog->libs.GetItem(j)->Release();
		}
		DEL_CLASS(prog);
	}
	DEL_CLASS(this->tasks);
	SDEL_STRING(this->errorMsg);
	this->basePath->Release();
	SDEL_STRING(this->debugObj);
}

IO::ParserType IO::SMake::GetParserType() const
{
	return ParserType::Smake;
}

void IO::SMake::ClearLinks()
{
	LIST_FREE_STRING(&this->linkCmds);
	this->testProgs.Clear();
}

void IO::SMake::ClearStatus()
{
	this->error = false;
	UOSInt i = this->progMap.GetCount();
	while (i-- > 0)
	{
		this->progMap.GetItem(i)->compiled = false;
	}
}

Bool IO::SMake::IsLoadFailed() const
{
	return this->errorMsg != 0;
}

Bool IO::SMake::HasError() const
{
	return this->error;
}

Bool IO::SMake::GetLastErrorMsg(NotNullPtr<Text::StringBuilderUTF8> sb) const
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
		this->debugObj = Text::String::New(debugObj.v, debugObj.leng).Ptr();
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

void IO::SMake::SetAsyncMode(Bool asyncMode)
{
	this->asyncMode = asyncMode;
}

void IO::SMake::AsyncPostCompile()
{
	this->tasks->WaitForIdle();
	NotNullPtr<Text::String> cmd;
	UOSInt i = this->linkCmds.GetCount();
	while (!this->error)
	{
		if (i == 0)
			break;
		i--;
		if (cmd.Set(this->linkCmds.RemoveAt(i)))
		{
			CompileObject(cmd);
		}
	}
	this->tasks->WaitForIdle();

	if (!this->error)
	{
		i = this->testProgs.GetCount();
		if (i > 0)
		{
			Text::StringBuilderUTF8 sb;
			NotNullPtr<const ProgramItem> prog;
			while (i-- > 0)
			{
				if (prog.Set(this->testProgs.RemoveAt(i)))
				{
					if (!TestProg(prog, sb))
					{
					}
				}
			}
		}
	}
}

NotNullPtr<const Data::ArrayList<IO::SMake::ConfigItem*>> IO::SMake::GetConfigList() const
{
	return this->cfgMap.GetValues();
}

Bool IO::SMake::HasProg(Text::CStringNN progName) const
{
	return this->progMap.GetC(progName) != 0;
}

Bool IO::SMake::CompileProg(Text::CStringNN progName, Bool asmListing)
{
	NotNullPtr<const IO::SMake::ProgramItem> prog;
	if (!prog.Set(this->progMap.GetC(progName)))
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendC(UTF8STRC("Program "));
		sb.Append(progName);
		sb.AppendC(UTF8STRC(" not found"));
		this->SetErrorMsg(sb.ToCString());
		return false;
	}
	else
	{
		return this->CompileProgInternal(prog, asmListing, false);
	}
}

Bool IO::SMake::ParseProg(NotNullPtr<Data::FastStringMap<Int32>> objList,
	NotNullPtr<Data::FastStringMap<Int32>> libList,
	NotNullPtr<Data::FastStringMap<Int32>> procList,
	Optional<Data::ArrayListStringNN> headerList,
	OutParam<Int64> latestTime,
	OutParam<Bool> progGroup,
	NotNullPtr<Text::String> progName)
{
	NotNullPtr<const IO::SMake::ProgramItem> prog;
	Text::StringBuilderUTF8 sb;
	if (!prog.Set(this->progMap.GetC(progName->ToCString())))
	{
		sb.AppendC(UTF8STRC("Program "));
		sb.AppendC(progName->v, progName->leng);
		sb.AppendC(UTF8STRC(" not found"));
		this->SetErrorMsg(sb.ToCString());
		return false;
	}
	else
	{
		return this->ParseProgInternal(objList, libList, procList, headerList, latestTime, progGroup, prog, sb);
	}
}

void IO::SMake::CleanFiles()
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UTF8Char *sptr2;
	IO::Path::PathType pt;
	sptr = Text::StrConcatC(this->basePath->ConcatTo(sbuff), UTF8STRC(OBJECTPATH));
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr2 = Text::StrConcatC(sptr, UTF8STRC("*.o"));
	IO::Path::FindFileSession *sess = IO::Path::FindFile(CSTRP(sbuff, sptr2));
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

UOSInt IO::SMake::GetProgList(NotNullPtr<Data::ArrayList<Text::String*>> progList)
{
//	Data::ArrayList<Text::String *> *names = this->progMap->GetKeys();
	Text::String *prog;
	UOSInt ret = 0;
	UOSInt i = 0;
	UOSInt j = this->progMap.GetCount();
	while (i < j)
	{
		prog = this->progMap.GetKey(i);
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

Bool IO::SMake::IsProgGroup(Text::CStringNN progName) const
{
	IO::SMake::ProgramItem *prog = this->progMap.GetC(progName);
	if (prog == 0)
	{
		return false;
	}

	Text::String *subItem;
	UOSInt i = prog->subItems.GetCount();
	while (i-- > 0)
	{
		subItem = prog->subItems.GetItem(i);
		if (subItem->EndsWith(UTF8STRC(".o")))
		{
			return false;
		}
	}
	return true;
}

const IO::SMake::ProgramItem *IO::SMake::GetProgItem(Text::CStringNN progName) const
{
	return this->progMap.GetC(progName);
}
