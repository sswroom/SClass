#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/Condition.h"
#include "IO/FileFindRecur.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/SMake.h"
#include "Manage/EnvironmentVar.h"
#include "Manage/Process.h"
#include "Sync/MutexUsage.h"
#include "Text/UTF8Reader.h"

//#define VERBOSE
#if defined(VERBOSE)
#include <stdio.h>
#endif

#define OBJECTPATH "obj"

/*
<name>: [!/@]<source>
@<object>
@?(NO_MINIZ > 0)<object>
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
void IO::SMake::AppendCfgItem(NN<Text::StringBuilderUTF8> sb, Text::CStringNN val)
{
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	NN<IO::SMake::ConfigItem> cfg;
	UnsafeArray<const UTF8Char> valEnd = &val.v[val.leng];
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
			if (this->cfgMap.GetC(CSTRP(sbuff, sptr)).SetTo(cfg))
			{
				sb->Append(cfg->value);
			}
		}
	}
	sb->AppendC(&val.v[i], (UOSInt)(valEnd - &val.v[i]));
}

void IO::SMake::AppendCfgPath(NN<Text::StringBuilderUTF8> sb, Text::CStringNN path)
{
	if (path.StartsWith(UTF8STRC("~/")))
	{
		Manage::EnvironmentVar env;
		UnsafeArray<const UTF8Char> csptr;
		if (env.GetValue(CSTR("HOME")).SetTo(csptr))
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

void IO::SMake::AppendCfg(NN<Text::StringBuilderUTF8> sb, Text::CString compileCfgC)
{
	Text::CStringNN compileCfg = compileCfgC.OrEmpty();
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
			AppendCfgItem(sb2, Text::CStringNN(compileCfg.v, (UOSInt)i));
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

Bool IO::SMake::ExecuteCmd(Text::CStringNN cmd)
{
	NN<IO::Writer> cmdWriter;
	if (this->cmdWriter.SetTo(cmdWriter))
	{
		cmdWriter->WriteLine(cmd);
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
		if (this->cmdWriter.SetTo(cmdWriter))
		{
			cmdWriter->WriteLine(sbRet.ToCString());
		}
	}
	return true;
}

Bool IO::SMake::LoadConfigFile(Text::CStringNN cfgFile)
{
	Bool ret = false;
	NN<IO::Writer> messageWriter;
	if (this->messageWriter.SetTo(messageWriter))
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendC(UTF8STRC("Loading "));
		sb.Append(cfgFile);
		messageWriter->WriteLine(sb.ToCString());
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
	UOSInt i;
	NN<IO::SMake::ConfigItem> cfg;
	Optional<IO::SMake::ProgramItem> prog = nullptr;
	NN<ProgramItem> nnprog;
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
			Bool valid = true;
			str1 = this->ParseCond(sb.SubstrTrim(1), valid);
			if (valid && prog.SetTo(nnprog))
			{
				nnprog->subItems.Add(Text::String::New(str1.ToCString()));
			}
		}
		else if (sb.ToString()[0] == '!')
		{
			Bool valid = true;
			str1 = this->ParseCond(sb.SubstrTrim(1), valid);
			if (valid && prog.SetTo(nnprog))
			{
				nnprog->libs.Add(Text::String::New(str1.ToCString()));
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
					Text::StringBuilderUTF8 sbCmd;
					AppendCfgItem(sbCmd, Text::CStringNN(&str1.v[2], (UOSInt)i - 2));
					Text::StringBuilderUTF8 result;
					Int32 ret;
					if ((ret = Manage::Process::ExecuteProcess(sbCmd.ToCString(), result)) != 0)
					{
						valid = false;
					}
					else
					{
						str1 = str1.Substring(i + 1);
					}
				}
			}
			if (prog.SetTo(nnprog) && valid)
			{
				Text::CStringNN ccfg = str1.ToCString();
				NN<IO::SMake::ConfigItem> cfg;
				NN<Text::String> compileCfg;
				if (this->cfgMap.GetC(str1.ToCString()).SetTo(cfg))
				{
					ccfg = cfg->value->ToCString();
				}
				if (nnprog->compileCfg.SetTo(compileCfg))
				{
					sb.ClearStr();
					sb.Append(compileCfg);
					sb.AppendUTF8Char(' ');
					sb.Append(ccfg);
					compileCfg->Release();
					nnprog->compileCfg = Text::String::New(sb.ToString(), sb.GetLength());
				}
				else
				{
					nnprog->compileCfg = Text::String::New(ccfg);
				}
			}
		}
		else if (sb.StartsWith(UTF8STRC("export ")))
		{
			if (this->cfgMap.GetC({sb.ToString() + 7, sb.GetLength() - 7}).SetTo(cfg))
			{
				Manage::EnvironmentVar env;
				env.SetValue(cfg->name->ToCString(), cfg->value->ToCString());
			}
		}
		else if ((i = sb.IndexOf(UTF8STRC("+="))) != INVALID_INDEX)
		{
			str2 = sb.SubstrTrim(i + 2);
			str1 = sb.SubstrTrim(0, i);
			if (this->cfgMap.GetC(str1.ToCString()).SetTo(cfg))
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
				cfg = MemAllocNN(IO::SMake::ConfigItem);
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
				if (this->cfgMap.GetC(str1.ToCString()).SetTo(cfg))
				{
					cfg->value->Release();
					sb2.ClearStr();
					AppendCfgItem(sb2, str2.ToCString());
					cfg->value = Text::String::New(sb2.ToString(), sb2.GetLength());
				}
				else
				{
					cfg = MemAllocNN(IO::SMake::ConfigItem);
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
					if (this->progMap.GetC(str1.Substring(1).ToCString()).SetTo(nnprog))
					{
						prog = nnprog;
					}
					else
					{
						NEW_CLASSNN(nnprog, IO::SMake::ProgramItem());
						nnprog->name = Text::String::New(str1.Substring(1).ToCString());
						if (str2.v[0])
						{
							nnprog->srcFile = Text::String::New(str2.ToCString());
						}
						else
						{
							nnprog->srcFile = nullptr;
						}
						nnprog->compileCfg = nullptr;
						nnprog->compiled = false;
						this->progMap.PutNN(nnprog->name, nnprog);
						prog = nnprog;
					}
				}
				else
				{
					if (this->progMap.GetC(str1.ToCString()).SetTo(nnprog))
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
						NEW_CLASSNN(nnprog, IO::SMake::ProgramItem());
						nnprog->name = Text::String::New(str1.ToCString());
						if (str2.v[0])
						{
							nnprog->srcFile = Text::String::New(str2.ToCString());
						}
						else
						{
							nnprog->srcFile = nullptr;
						}
						nnprog->compileCfg = nullptr;
						nnprog->compiled = false;
						this->progMap.PutNN(nnprog->name, nnprog);
						prog = nnprog;
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

Text::PString IO::SMake::ParseCond(Text::PString str1, OutParam<Bool> valid)
{
	UOSInt i;
	Text::PString str2;
	Text::PString str3;
	if (str1.StartsWith(UTF8STRC("?(")))
	{
		i = str1.BranketSearch(2, ')');
		if (i != INVALID_INDEX)
		{
			str2 = str1.Substring(i + 1);
			Text::StringBuilderUTF8 sb2;
			sb2.AppendC(&str1.v[2], (UOSInt)i - 2);
			str1 = sb2.TrimAsNew();
			Data::CompareCondition cond = Data::CompareCondition::Unknown;
			if ((i = str1.IndexOf(UTF8STRC(">="))) != INVALID_INDEX)
			{
				str3 = str1.SubstrTrim(i + 2);
				str1 = str1.SubstrTrim(0, i);
				cond = Data::CompareCondition::GreaterOrEqual;
			}
			else if ((i = str1.IndexOf(UTF8STRC("<="))) != INVALID_INDEX)
			{
				str3 = str1.SubstrTrim(i + 2);
				str1 = str1.SubstrTrim(0, i);
				cond = Data::CompareCondition::LessOrEqual;
			}
			else if ((i = str1.IndexOf(UTF8STRC(">"))) != INVALID_INDEX)
			{
				str3 = str1.SubstrTrim(i + 1);
				str1 = str1.SubstrTrim(0, i);
				cond = Data::CompareCondition::Greater;
			}
			else if ((i = str1.IndexOf(UTF8STRC("<"))) != INVALID_INDEX)
			{
				str3 = str1.SubstrTrim(i + 1);
				str1 = str1.SubstrTrim(0, i);
				cond = Data::CompareCondition::Less;
			}
			else if ((i = str1.IndexOf(UTF8STRC("=="))) != INVALID_INDEX)
			{
				str3 = str1.SubstrTrim(i + 2);
				str1 = str1.SubstrTrim(0, i);
				cond = Data::CompareCondition::Equal;
			}
			else if ((i = str1.IndexOf(UTF8STRC("!="))) != INVALID_INDEX)
			{
				str3 = str1.SubstrTrim(i + 2);
				str1 = str1.SubstrTrim(0, i);
				cond = Data::CompareCondition::NotEqual;
			}
			if (cond != Data::CompareCondition::Unknown)
			{
				Int32 val1 = 0;
				Int32 val2 = 0;
				NN<ConfigItem> cfg;
				if (str1.v[0] >= '0' && str1.v[0] <= '9')
				{
					val1 = str1.ToInt32();
				}
				else
				{
					if (this->cfgMap.GetC(str1.ToCString()).SetTo(cfg))
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
					if (this->cfgMap.GetC(str3.ToCString()).SetTo(cfg))
					{
						val2 = cfg->value->ToInt32();
					}
				}
				switch (cond)
				{
				case Data::CompareCondition::Equal:
					valid.Set(val1 == val2);
					break;
				case Data::CompareCondition::Greater:
					valid.Set(val1 > val2);
					break;
				case Data::CompareCondition::GreaterOrEqual:
					valid.Set(val1 >= val2);
					break;
				case Data::CompareCondition::Less:
					valid.Set(val1 < val2);
					break;
				case Data::CompareCondition::LessOrEqual:
					valid.Set(val1 <= val2);
					break;
				case Data::CompareCondition::NotEqual:
					valid.Set(val1 != val2);
					break;
				case Data::CompareCondition::Unknown:
				default:
					valid.Set(false);
					break;
				}
			}
			str1 = str2;
		}
	}
	else if (str1.StartsWith(UTF8STRC("@(")))
	{
		i = str1.BranketSearch(2, ')');
		if (i != INVALID_INDEX)
		{
			Text::StringBuilderUTF8 sbCmd;
			Text::StringBuilderUTF8 result;
			AppendCfgItem(sbCmd, Text::CStringNN(&str1.v[2], i - 2));
			Int32 ret;
			if ((ret = Manage::Process::ExecuteProcess(sbCmd.ToCString(), result)) != 0)
			{
				valid.Set(false);
			}
			else
			{
				str1 = str1.Substring(i + 1);
			}
		}
	}
	return str1;
}

Bool IO::SMake::ParseSource(NN<Data::FastStringMapNative<Int32>> objList,
	NN<Data::FastStringMapNative<Int32>> libList,
	NN<Data::FastStringMapNative<Int32>> procList,
	Optional<Data::ArrayListStringNN> headerList,
	OutParam<Int64> latestTime,
	Text::CStringNN sourceFile,
	Text::CString sourceRefPath,
	NN<Data::ArrayListUInt64> objParsedProgs,
	NN<Text::StringBuilderUTF8> tmpSb)
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
	NN<IO::Writer> messageWriter;
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
	if (this->messageWriter.SetTo(messageWriter))
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendC(UTF8STRC("ParseSource "));
		sb.Append(sourceFile);
		messageWriter->WriteLine(sb.ToCString());
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
#if defined(VERBOSE)
	printf("Parsing source %s\r\n", fileName.v.Ptr());
#endif
	Int64 lastTime;
	Int64 thisTime;
	lastTime = fs.GetModifyTime().ToTicks();
	Text::PString line;
	Text::PString str1;
	UOSInt i;
	NN<IO::SMake::ProgramItem> prog;
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
						if (!this->progMap.GetC(str1.ToCString()).SetTo(prog))
						{
							Text::CStringNN nnsrc;
							Text::PString currPath = str1;
							UOSInt j;
							Text::StringBuilderUTF8 sb2;
							if (!sourceRefPath.SetTo(nnsrc) || (j = nnsrc.LastIndexOf('/')) == INVALID_INDEX)
							{
								sb2.Append(str1);
								sb2.AppendC(UTF8STRC(" not found in "));
								sb2.Append(sourceFile);
								this->SetErrorMsg(sb2.ToCString());
								return false;
							}
							sb2.AppendC(nnsrc.v, j + 1);
							while (currPath.StartsWith(CSTR("../")) && sb2.EndsWith('/'))
							{
								sb2.RemoveChars(1);
								j = sb2.LastIndexOf('/');
								sb2.TrimToLength(j + 1);
								currPath = currPath.Substring(3);
							}
							sb2.Append(currPath);
							if (!this->progMap.GetC(sb2.ToCString()).SetTo(prog))
							{
								sb2.ClearStr();
								sb2.Append(str1);
								sb2.AppendC(UTF8STRC(" not found in "));
								sb2.Append(sourceFile);
								this->SetErrorMsg(sb2.ToCString());
								return false;
							}
						}
						if (objParsedProgs->SortedIndexOf((UOSInt)prog.Ptr()) < 0)
						{
							objParsedProgs->SortedInsert((UOSInt)prog.Ptr());
							NN<Text::String> debugObj;
							procList->PutNN(prog->name, 1);

							if (this->debugObj.SetTo(debugObj) && this->messageWriter.SetTo(messageWriter) && prog->name->Equals(debugObj))
							{
								Text::StringBuilderUTF8 sb2;
								sb2.Append(debugObj);
								sb2.AppendC(UTF8STRC(" found in "));
								sb2.Append(sourceFile);
								messageWriter->WriteLine(sb2.ToCString());
							}
#if defined(VERBOSE)
							printf("Parsing header \"%s\" in source %s\r\n", prog->name->v.Ptr(), sourceFile.v.Ptr());
#endif
							if (!this->ParseHeader(objList, libList, procList, headerList, thisTime, prog->name, sourceFile, objParsedProgs, tmpSb))
							{
								return false;
							}
#if defined(VERBOSE)
							tmpSb->ClearStr();
							tmpSb->AppendTSNoZone(Data::Timestamp(thisTime, Data::DateTimeUtil::GetLocalTzQhr()));
							printf("Found new header \"%s\" with time = %s\r\n", prog->name->v.Ptr(), tmpSb->v.Ptr());
#endif
							if (thisTime > lastTime)
							{
								lastTime = thisTime;
							}

							Data::ArrayIterator<NN<Text::String>> it = prog->subItems.Iterator();
							while (it.HasNext())
							{
								NN<Text::String> subItem = it.Next();
								if (this->debugObj.SetTo(debugObj) && this->messageWriter.SetTo(messageWriter) && subItem->Equals(debugObj))
								{
									Text::StringBuilderUTF8 sb2;
									sb2.Append(debugObj);
									sb2.AppendC(UTF8STRC(" found in "));
									sb2.Append(sourceFile);
									messageWriter->WriteLine(sb2.ToCString());
								}
								if (objList->PutNN(subItem, 1) != 1)
								{
#if defined(VERBOSE)
									printf("Parsing object \"%s\"\r\n", prog->name->v.Ptr());
#endif
									if (!this->ParseObject(objList, libList, procList, headerList, thisTime, subItem, prog->name->ToCString(), tmpSb))
									{
										return false;
									}
	/*									if (thisTime > lastTime)
									{
										lastTime = thisTime;
									}*/
								}
							}
							it = prog->libs.Iterator();
							while (it.HasNext())
							{
								NN<Text::String> lib = it.Next();
								if (this->messageWriter.SetTo(messageWriter))
								{
									tmpSb->ClearStr();
									tmpSb->AppendC(UTF8STRC("Add lib "));
									tmpSb->Append(lib);
									messageWriter->WriteLine(tmpSb->ToCString());
								}
								libList->PutNN(lib, 1);
							}
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

Bool IO::SMake::ParseHeader(NN<Data::FastStringMapNative<Int32>> objList,
	NN<Data::FastStringMapNative<Int32>> libList,
	NN<Data::FastStringMapNative<Int32>> procList,
	Optional<Data::ArrayListStringNN> headerList,
	OutParam<Int64> latestTime,
	NN<Text::String> headerFile,
	Text::CStringNN sourceFile,
	NN<Data::ArrayListUInt64> objParsedProgs,
	NN<Text::StringBuilderUTF8> tmpSb)
{
	NN<IO::SMake::ConfigItem> cfg;
	if (!this->cfgMap.GetC(CSTR("INCLUDEPATH")).SetTo(cfg))
	{
		this->SetErrorMsg(CSTR("INCLUDEPATH config not found"));
		return false;
	}
	Text::StringBuilderUTF8 sb2;
	Text::PString sarr[2];
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	UOSInt i;
	Int64 thisTime;
	NN<Data::ArrayListStringNN> nnheaderList;
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
			if (this->ParseSource(objList, libList, procList, headerList, thisTime, CSTRP(sbuff, sptr), headerFile->ToCString(), objParsedProgs, tmpSb))
			{
#if defined(VERBOSE)
				tmpSb->ClearStr();
				tmpSb->AppendTSNoZone(Data::Timestamp(thisTime, Data::DateTimeUtil::GetLocalTzQhr()));
				printf("Header %s found time = %s\r\n", headerFile->v.Ptr(), tmpSb->v.Ptr());
#endif
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
	UnsafeArray<const UTF8Char> currHeader = headerFile->v;
	UnsafeArray<const UTF8Char> currHeaderEnd = &headerFile->v[headerFile->leng];
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
		if (this->ParseSource(objList, libList, procList, headerList, thisTime, CSTRP(sbuff, sptr), headerFile->ToCString(), objParsedProgs, tmpSb))
		{
#if defined(VERBOSE)
			tmpSb->ClearStr();
			tmpSb->AppendTSNoZone(Data::Timestamp(thisTime, Data::DateTimeUtil::GetLocalTzQhr()));
			printf("Header %s found time = %s\r\n", headerFile->v.Ptr(), tmpSb->v.Ptr());
#endif
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

Bool IO::SMake::ParseObject(NN<Data::FastStringMapNative<Int32>> objList, NN<Data::FastStringMapNative<Int32>> libList, NN<Data::FastStringMapNative<Int32>> procList, Optional<Data::ArrayListStringNN> headerList, OutParam<Int64> latestTime, NN<Text::String> objectFile, Text::CStringNN sourceFile, NN<Text::StringBuilderUTF8> tmpSb)
{
	if (!objectFile->EndsWith(UTF8STRC(".o")))
	{
		latestTime.Set(0);
		return true;
	}

	NN<IO::Writer> messageWriter;
	NN<IO::SMake::ProgramItem> prog;
	if (!this->progMap.GetNN(objectFile).SetTo(prog))
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
		NN<Text::String> debugObj;
		Data::ArrayIterator<NN<Text::String>> it = prog->subItems.Iterator();
		while (it.HasNext())
		{
			NN<Text::String> subItem = it.Next();
			if (this->debugObj.SetTo(debugObj) && this->messageWriter.SetTo(messageWriter) && subItem->Equals(debugObj))
			{
				Text::StringBuilderUTF8 sb2;
				sb2.Append(debugObj);
				sb2.AppendC(UTF8STRC(" found in "));
				sb2.Append(sourceFile);
				messageWriter->WriteLine(sb2.ToCString());
			}
			if (objList->PutNN(subItem, 1) != 1)
			{
				Int64 thisTime;
				if (!this->ParseObject(objList, libList, procList, headerList, thisTime, subItem, objectFile->ToCString(), tmpSb))
				{
					return false;
				}
			}
		}
		it = prog->libs.Iterator();
		while (it.HasNext())
		{
			NN<Text::String> lib = it.Next();
			if (this->messageWriter.SetTo(messageWriter))
			{
				tmpSb->ClearStr();
				tmpSb->AppendC(UTF8STRC("Add lib "));
				tmpSb->Append(lib);
				tmpSb->AppendC(UTF8STRC(" from "));
				tmpSb->Append(prog->name);
				messageWriter->WriteLine(tmpSb->ToCString());
			}
			libList->PutNN(lib, 1);
		}
		if (this->debugObj.SetTo(debugObj) && this->messageWriter.SetTo(messageWriter) && prog->name->Equals(debugObj))
		{
			Text::StringBuilderUTF8 sb2;
			sb2.Append(debugObj);
			sb2.AppendC(UTF8STRC(" found in "));
			sb2.Append(sourceFile);
			messageWriter->WriteLine(sb2.ToCString());
		}
		NN<Text::String> s;
		if (prog->srcFile.SetTo(s))
		{
			Data::ArrayListUInt64 objParsedProgs;
			Int64 thisTime;
			if (!this->ParseSource(objList, libList, procList, headerList, thisTime, s->ToCString(), 0, objParsedProgs, tmpSb))
			{
				return false;
			}
#if defined(VERBOSE)
			tmpSb->ClearStr();
			tmpSb->AppendTSNoZone(Data::Timestamp(thisTime, Data::DateTimeUtil::GetLocalTzQhr()));
			printf("Source file \"%s\" Time = %s\r\n", s->v.Ptr(), tmpSb->v.Ptr());
#endif
			this->fileTimeMap.PutNN(s, thisTime);
			latestTime.Set(thisTime);
		}
		return true;
	}
}

Bool IO::SMake::ParseProgInternal(NN<Data::FastStringMapNative<Int32>> objList,
	NN<Data::FastStringMapNative<Int32>> libList,
	NN<Data::FastStringMapNative<Int32>> procList,
	Optional<Data::ArrayListStringNN> headerList,
	OutParam<Int64> latestTime,
	OutParam<Bool> progGroup,
	NN<const ProgramItem> prog,
	NN<Text::StringBuilderUTF8> tmpSb)
{
	NN<IO::Writer> messageWriter;
	NN<Text::String> subItem;
	NN<Text::String> debugObj;
	NN<IO::SMake::ProgramItem> subProg;
	Int64 thisTime;
	Int64 lastTime = 0;
	Bool progGrp = true;
	Data::ArrayIterator<NN<Text::String>> it = prog->subItems.Iterator();
	while (it.HasNext())
	{
		subItem = it.Next();
		if (subItem->EndsWith(UTF8STRC(".o")))
		{
			progGrp = false;
		}
		if (this->debugObj.SetTo(debugObj) && this->messageWriter.SetTo(messageWriter) && subItem->Equals(debugObj))
		{
			tmpSb->ClearStr();
			tmpSb->Append(debugObj);
			tmpSb->AppendC(UTF8STRC(" depends by "));
			tmpSb->Append(prog->name);
			messageWriter->WriteLine(tmpSb->ToCString());
		}
		objList->PutNN(subItem, 1);
	}

	progGroup.Set(progGrp);
	if (progGrp)
	{
		latestTime.Set(lastTime);
		return true;
	}

	Data::ArrayListUInt64 objParsedProgs;
	NN<IO::SMake::ConfigItem> cfg;
	if (this->cfgMap.GetC(CSTR("DEPS")).SetTo(cfg))
	{
		if (!this->progMap.GetNN(cfg->value).SetTo(subProg))
		{
			tmpSb->AppendC(UTF8STRC("Item "));
			tmpSb->Append(cfg->value);
			tmpSb->AppendC(UTF8STRC(" not found"));
			this->SetErrorMsg(tmpSb->ToCString());
			return false;
		}
		objList->PutNN(cfg->value, 1);
		objParsedProgs.Clear();
		if (!this->ParseSource(objList, libList, procList, headerList, thisTime, Text::String::OrEmpty(subProg->srcFile)->ToCString(), 0, objParsedProgs, tmpSb))
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
		if (!this->progMap.GetNN(subItem).SetTo(subProg))
		{
			tmpSb->AppendC(UTF8STRC("Item "));
			tmpSb->Append(subItem);
			tmpSb->AppendC(UTF8STRC(" not found"));
			this->SetErrorMsg(tmpSb->ToCString());
			return false;
		}
		objParsedProgs.Clear();
		if (!this->ParseSource(objList, libList, procList, headerList, thisTime, Text::String::OrEmpty(subProg->srcFile)->ToCString(), 0, objParsedProgs, tmpSb))
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

void IO::SMake::CompileTask(AnyType userObj)
{
	NN<CompileReq> req = userObj.GetNN<CompileReq>();
	if (!req->me->ExecuteCmd(req->cmd->ToCString()))
	{
		req->me->error = true;
	}
	req->cmd->Release();
	MemFreeNN(req);
}

void IO::SMake::CompileObject(Text::CStringNN cmd)
{
	CompileReq *req = MemAlloc(CompileReq, 1);
	req->cmd = Text::String::New(cmd);
	req->me = *this;
	this->tasks->AddTask(CompileTask, req);
}

void IO::SMake::CompileObject(NN<Text::String> cmd)
{
	CompileReq *req = MemAlloc(CompileReq, 1);
	req->cmd = cmd;
	req->me = *this;
	this->tasks->AddTask(CompileTask, req);
}

Bool IO::SMake::CompileProgInternal(NN<const ProgramItem> prog, Bool asmListing, Bool enableTest, Text::CString parentName)
{
	Data::FastStringMapNative<Int32> libList;
	Data::FastStringMapNative<Int32> objList;
	Data::FastStringMapNative<Int32> procList;
	NN<IO::Writer> messageWriter;
	Int64 latestTime = 0;
	Int64 thisTime;
	NN<IO::SMake::ProgramItem> subProg;
	Bool progGroup;
	if (this->messageWriter.SetTo(messageWriter))
	{
		Text::StringBuilderUTF8 sb;
		Text::CStringNN nnname;
		sb.AppendC(UTF8STRC("Compiling Program "));
		sb.Append(prog->name);
		if (parentName.SetTo(nnname))
		{
			sb.AppendC(UTF8STRC(" from "));
			sb.Append(nnname);
		}
		messageWriter->WriteLine(sb.ToCString());
	}

	if (!enableTest && prog->name->Equals(UTF8STRC("test")))
	{
		NN<IO::SMake::ConfigItem> testCfg;
		if (this->cfgMap.GetC(CSTR("ENABLE_TEST")).SetTo(testCfg) && testCfg->value->Equals(UTF8STRC("1")))
		{
			enableTest = true;
		}
	}

	Text::StringBuilderUTF8 sb;

	if (!this->ParseProgInternal(objList, libList, procList, nullptr, latestTime, progGroup, prog, sb))
	{
		return false;
	}
	if (progGroup)
	{
		Data::FastStringNativeKeyIterator<Int32> iterator = objList.KeyIterator();
		while (iterator.HasNext())
		{
			NN<Text::String> objName = iterator.Next();
			if (!this->progMap.GetNN(objName).SetTo(subProg))
			{
				sb.ClearStr();
				sb.AppendC(UTF8STRC("Program "));
				sb.Append(objName);
				sb.AppendC(UTF8STRC(" not found"));
				this->SetErrorMsg(sb.ToCString());
				return false;
			}
			if (!this->CompileProgInternal(subProg, asmListing, enableTest, prog->name->ToCString()))
			{
				return false;
			}
		}
		return true;
	}

	NN<ConfigItem> cfg;
	NN<IO::SMake::ConfigItem> cppCfg;
	NN<IO::SMake::ConfigItem> ccCfg;
	NN<IO::SMake::ConfigItem> asmCfg;
	Optional<IO::SMake::ConfigItem> asmflagsCfg = this->cfgMap.GetC(CSTR("ASMFLAGS"));
	Optional<IO::SMake::ConfigItem> cflagsCfg = this->cfgMap.GetC(CSTR("CFLAGS"));
	Optional<IO::SMake::ConfigItem> cppflagsCfg = this->cfgMap.GetC(CSTR("CPPFLAGS"));
	Optional<IO::SMake::ConfigItem> libsCfg = this->cfgMap.GetC(CSTR("LIBS"));
	Data::Timestamp dt1;
	Data::Timestamp dt2;
	if (!this->cfgMap.GetC(CSTR("CXX")).SetTo(cppCfg))
	{
		this->SetErrorMsg(CSTR("CXX config not found"));
		return false;
	}
	if (!this->cfgMap.GetC(CSTR("CC")).SetTo(ccCfg))
	{
		this->SetErrorMsg(CSTR("CC config not found"));
		return false;
	}
	if (!this->cfgMap.GetC(CSTR("ASM")).SetTo(asmCfg))
	{
		this->SetErrorMsg(CSTR("ASM config not found"));
		return false;
	}
	
	IO::Path::CreateDirectory(CSTR(OBJECTPATH));
	Data::FastStringNativeKeyIterator<Int32> iterator = objList.KeyIterator();
	while (iterator.HasNext())
	{
		NN<Text::String> objName = iterator.Next();
		if (!this->progMap.GetNN(objName).SetTo(subProg))
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Object "));
			sb.Append(objName);
			sb.AppendC(UTF8STRC(" not found"));
			this->SetErrorMsg(sb.ToCString());
			return false;
		}
		NN<Text::String> srcFile;
		if (!subProg->srcFile.SetTo(srcFile))
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Object "));
			sb.Append(objName);
			sb.AppendC(UTF8STRC(" does not have source file"));
			this->SetErrorMsg(sb.ToCString());
			return false;
		}

		Data::ArrayIterator<NN<Text::String>> itLibs = subProg->libs.Iterator();
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
			if (this->messageWriter.SetTo(messageWriter))
			{
				sb.ClearStr();
				sb.AppendC(UTF8STRC("SrcFile "));
				sb.Append(srcFile);
				sb.AppendC(UTF8STRC(", time = "));
				if (thisTime == 0)
				{
					sb.AppendC(UTF8STRC("null"));
				}
				else
				{
					sb.AppendTSNoZone(Data::Timestamp(lastTime, Data::DateTimeUtil::GetLocalTzQhr()));
				}
				messageWriter->WriteLine(sb.ToCString());
			}
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
					if (this->messageWriter.SetTo(messageWriter))
					{
						sb.ClearStr();
						sb.AppendC(UTF8STRC("Obj "));
						sb.Append(subProg->name);
						sb.AppendC(UTF8STRC(" Obj time: "));
						sb.AppendTSNoZone(dt2);
						sb.AppendC(UTF8STRC(", Src time: "));
						sb.AppendTSNoZone(dt1);
						sb.AppendC(UTF8STRC(", Last time: "));
						dt1 = Data::Timestamp(lastTime, Data::DateTimeUtil::GetLocalTzQhr());
						sb.AppendTSNoZone(dt1);
						sb.AppendC(UTF8STRC(", Skip"));
						messageWriter->WriteLine(sb.ToCString());
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
			if (this->messageWriter.SetTo(messageWriter))
			{
				sb.ClearStr();
				sb.AppendC(UTF8STRC("Obj "));
				sb.Append(subProg->name);
				sb.AppendC(UTF8STRC(" creating from "));
				sb.Append(srcFile);
				messageWriter->WriteLine(sb.ToCString());
			}
			if (srcFile->EndsWith(UTF8STRC(".cpp")))
			{
				sb.ClearStr();
				AppendCfgPath(sb, cppCfg->value->ToCString());
				sb.AppendUTF8Char(' ');
				if (cflagsCfg.SetTo(cfg))
				{
					sb.Append(cfg->value);
					sb.AppendUTF8Char(' ');
				}
				if (cppflagsCfg.SetTo(cfg))
				{
					sb.Append(cfg->value);
					sb.AppendUTF8Char(' ');
				}
				NN<Text::String> compileCfg;
				if (subProg->compileCfg.SetTo(compileCfg))
				{
					this->AppendCfg(sb, compileCfg->ToCString());
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
				if (cflagsCfg.SetTo(cfg))
				{
					sb.Append(cfg->value);
					sb.AppendUTF8Char(' ');
				}
				NN<Text::String> compileCfg;
				if (subProg->compileCfg.SetTo(compileCfg))
				{
					this->AppendCfg(sb, compileCfg->ToCString());
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
				if (asmflagsCfg.SetTo(cfg))
				{
					sb.Append(cfg->value);
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

	Optional<IO::SMake::ConfigItem> postfixItem = this->cfgMap.GetC(CSTR("OUTPOSTFIX"));
	sb.ClearStr();
	sb.Append(this->basePath);
	sb.AppendC(UTF8STRC("bin"));
	IO::Path::CreateDirectory(sb.ToCString());
	sb.AppendUTF8Char(IO::Path::PATH_SEPERATOR);
	sb.Append(prog->name);
	if (postfixItem.SetTo(cfg))
	{
		sb.Append(cfg->value);
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
		if (postfixItem.SetTo(cfg))
		{
			sb.Append(cfg->value);
		}
		Data::FastStringNativeKeyIterator<Int32> iterator = objList.KeyIterator();
		while (iterator.HasNext())
		{
			sb.AppendUTF8Char(' ');
			sb.AppendC(UTF8STRC(OBJECTPATH));
			sb.AppendUTF8Char(IO::Path::PATH_SEPERATOR);
			sb.Append(iterator.Next());
		}
		if (libsCfg.SetTo(cfg))
		{
			sb.AppendUTF8Char(' ');
			sb.Append(cfg->value);
		}
		UOSInt i;
		UOSInt j;
		i = 0;
		j = libList.GetCount();
		while (i < j)
		{
	//		printf("Libs: %s\r\n", libList.GetItem(i));
			sb.AppendUTF8Char(' ');
			NN<Text::String> lib = Text::String::OrEmpty(libList.GetKey(i));
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
			NN<Text::String> objName;
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
				fs.Write(sb.ToByteArray().SubArray(1));
			}
			sb.ClearStr();
			AppendCfgPath(sb, cppCfg->value->ToCString());
			sb.AppendUTF8Char(' ');
			sb.AppendC(UTF8STRC("-o bin/"));
			sb.Append(prog->name);
			if (postfixItem.SetTo(cfg))
			{
				sb.Append(cfg->value);
			}
			sb.AppendC(UTF8STRC(" @filelist.txt"));
			if (libsCfg.SetTo(cfg))
			{
				sb.AppendUTF8Char(' ');
				sb.Append(cfg->value);
			}
			UOSInt i;
			UOSInt j;
			i = 0;
			j = libList.GetCount();
			while (i < j)
			{
		//		printf("Libs: %s\r\n", libList.GetItem(i));
				sb.AppendUTF8Char(' ');
				NN<Text::String> lib = Text::String::OrEmpty(libList.GetKey(i));
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

Bool IO::SMake::TestProg(NN<const ProgramItem> prog, NN<Text::StringBuilderUTF8> sb)
{
	NN<IO::Writer> cmdWriter;
	if (this->cmdWriter.SetTo(cmdWriter))
	{
		sb->ClearStr();
		sb->AppendC(UTF8STRC("Testing "));
		sb->Append(prog->name);
		cmdWriter->WriteLine(sb->ToCString());
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

void IO::SMake::SetErrorMsg(Text::CStringNN msg)
{
	Sync::MutexUsage mutUsage(this->errorMsgMut);
	OPTSTR_DEL(this->errorMsg);
	this->errorMsg = Text::String::New(msg);
}

IO::SMake::SMake(Text::CStringNN cfgFile, UOSInt threadCnt, Optional<IO::Writer> messageWriter) : IO::ParsedObject(cfgFile)
{
	NEW_CLASSNN(this->tasks, Sync::ParallelTask(threadCnt, false));
	this->errorMsg = nullptr;
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
		UnsafeArray<UTF8Char> sptr = IO::Path::GetCurrDirectory(sbuff).Or(sbuff);
		if (sptr[-1] != IO::Path::PATH_SEPERATOR)
		{
			*sptr++ = IO::Path::PATH_SEPERATOR;
			*sptr = 0;
		}
		this->basePath = Text::String::New(sbuff, (UOSInt)(sptr - sbuff));
	}
	this->messageWriter = messageWriter;
	this->cmdWriter = nullptr;
	this->debugObj = nullptr;

	this->LoadConfigFile(cfgFile);
}

IO::SMake::~SMake()
{
	UOSInt i;
	UOSInt j;
	NN<IO::SMake::ConfigItem> cfg;
	NN<const Data::ArrayListNN<IO::SMake::ConfigItem>> cfgList = this->cfgMap.GetValues();
	i = cfgList->GetCount();
	while (i-- > 0)
	{
		cfg = cfgList->GetItemNoCheck(i);
		cfg->name->Release();
		cfg->value->Release();
		MemFreeNN(cfg);
	}
	this->ClearLinks();

//	Data::ArrayList<IO::SMake::ProgramItem*> *progList = progMap->GetValues();
	NN<IO::SMake::ProgramItem> prog;
//	i = progList->GetCount();
	i = this->progMap.GetCount();
	while (i-- > 0)
	{
//		prog = progList->GetItem(i);
		prog = this->progMap.GetItemNoCheck(i);
		prog->name->Release();
		OPTSTR_DEL(prog->srcFile);
		OPTSTR_DEL(prog->compileCfg);
		j = prog->subItems.GetCount();
		while (j-- > 0)
		{
			OPTSTR_DEL(prog->subItems.GetItem(j));
		}
		j = prog->libs.GetCount();
		while (j-- > 0)
		{
			OPTSTR_DEL(prog->libs.GetItem(j));
		}
		prog.Delete();
	}
	this->tasks.Delete();
	OPTSTR_DEL(this->errorMsg);
	this->basePath->Release();
	OPTSTR_DEL(this->debugObj);
}

IO::ParserType IO::SMake::GetParserType() const
{
	return ParserType::Smake;
}

void IO::SMake::ClearLinks()
{
	this->linkCmds.FreeAll();
	this->testProgs.Clear();
}

void IO::SMake::ClearStatus()
{
	this->error = false;
	UOSInt i = this->progMap.GetCount();
	while (i-- > 0)
	{
		this->progMap.GetItemNoCheck(i)->compiled = false;
	}
}

Bool IO::SMake::IsLoadFailed() const
{
	return this->errorMsg.NotNull();
}

Bool IO::SMake::HasError() const
{
	return this->error;
}

Bool IO::SMake::GetLastErrorMsg(NN<Text::StringBuilderUTF8> sb) const
{
	Bool ret;
	Sync::MutexUsage mutUsage(this->errorMsgMut);
	NN<Text::String> errorMsg;
	if (this->errorMsg.SetTo(errorMsg))
	{
		sb->Append(errorMsg);
		ret = true;
	}
	else
	{
		ret = false;
	}
	return ret;
}

void IO::SMake::SetMessageWriter(Optional<IO::Writer> messageWriter)
{
	this->messageWriter = messageWriter;
}

void IO::SMake::SetCommandWriter(Optional<IO::Writer> cmdWriter)
{
	this->cmdWriter = cmdWriter;
}

void IO::SMake::SetDebugObj(Text::CString debugObj)
{
	OPTSTR_DEL(this->debugObj);
	Text::CStringNN nns;
	if (debugObj.SetTo(nns))
	{
		this->debugObj = Text::String::New(nns);
	}
}

void IO::SMake::SetThreadCnt(UOSInt threadCnt)
{
	if (this->tasks->GetThreadCnt() != threadCnt)
	{
		this->tasks.Delete();
		NEW_CLASSNN(this->tasks, Sync::ParallelTask(threadCnt, false));
	}
}

void IO::SMake::SetAsyncMode(Bool asyncMode)
{
	this->asyncMode = asyncMode;
}

void IO::SMake::AsyncPostCompile()
{
	this->tasks->WaitForIdle();
	NN<Text::String> cmd;
	UOSInt i = this->linkCmds.GetCount();
	while (!this->error)
	{
		if (i == 0)
			break;
		i--;
		if (this->linkCmds.RemoveAt(i).SetTo(cmd))
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
			NN<const ProgramItem> prog;
			while (i-- > 0)
			{
				if (this->testProgs.RemoveAt(i).SetTo(prog))
				{
					if (!TestProg(prog, sb))
					{
					}
				}
			}
		}
	}
}

NN<const Data::ArrayListNN<IO::SMake::ConfigItem>> IO::SMake::GetConfigList() const
{
	return this->cfgMap.GetValues();
}

Bool IO::SMake::HasProg(Text::CStringNN progName) const
{
	return this->progMap.GetC(progName).NotNull();
}

Bool IO::SMake::CompileProg(Text::CStringNN progName, Bool asmListing)
{
	NN<IO::SMake::ProgramItem> prog;
	if (!this->progMap.GetC(progName).SetTo(prog))
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
		return this->CompileProgInternal(prog, asmListing, false, 0);
	}
}

Bool IO::SMake::ParseProg(NN<Data::FastStringMapNative<Int32>> objList,
	NN<Data::FastStringMapNative<Int32>> libList,
	NN<Data::FastStringMapNative<Int32>> procList,
	Optional<Data::ArrayListStringNN> headerList,
	OutParam<Int64> latestTime,
	OutParam<Bool> progGroup,
	NN<Text::String> progName)
{
	NN<IO::SMake::ProgramItem> prog;
	Text::StringBuilderUTF8 sb;
	if (!this->progMap.GetC(progName->ToCString()).SetTo(prog))
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
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> sptr2;
	IO::Path::PathType pt;
	sptr = Text::StrConcatC(this->basePath->ConcatTo(sbuff), UTF8STRC(OBJECTPATH));
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr2 = Text::StrConcatC(sptr, UTF8STRC("*.o"));
	NN<IO::Path::FindFileSession> sess;
	if (IO::Path::FindFile(CSTRP(sbuff, sptr2)).SetTo(sess))
	{
		while (IO::Path::FindNextFile(sptr, sess, 0, pt, 0).NotNull())
		{
			if (pt == IO::Path::PathType::File)
			{
				IO::Path::DeleteFile(sbuff);
			}
		}
		IO::Path::FindFileClose(sess);
	}
}

UOSInt IO::SMake::GetProgList(NN<Data::ArrayListNN<Text::String>> progList)
{
//	Data::ArrayList<Text::String *> *names = this->progMap->GetKeys();
	NN<Text::String> prog;
	UOSInt ret = 0;
	UOSInt i = 0;
	UOSInt j = this->progMap.GetCount();
	while (i < j)
	{
		if (this->progMap.GetKey(i).SetTo(prog))
		{
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
		}
		i++;
	}
	return ret;
}

Bool IO::SMake::IsProgGroup(Text::CStringNN progName) const
{
	NN<IO::SMake::ProgramItem> prog;
	if (!this->progMap.GetC(progName).SetTo(prog))
	{
		return false;
	}

	Data::ArrayIterator<NN<Text::String>> it = prog->subItems.Iterator();;
	while (it.HasNext())
	{
		if (it.Next()->EndsWith(UTF8STRC(".o")))
		{
			return false;
		}
	}
	return true;
}

Optional<const IO::SMake::ProgramItem> IO::SMake::GetProgItem(Text::CStringNN progName) const
{
	return this->progMap.GetC(progName);
}
