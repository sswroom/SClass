#include "Stdafx.h"
#include "IO/IniFile.h"
#include "IO/Path.h"
#include "IO/Registry.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/Cpp/CppEnv.h"

Bool Text::Cpp::CppEnv::InitVSEnv(Text::VSProject::VisualStudioVersion vsv)
{
	UTF8Char sbuff[512];
	UTF8Char sbuff2[512];
	UTF8Char sbuff3[512];
	UTF8Char *sptr;
	UTF8Char c;
	const UTF8Char *csptr;
	IO::ConfigFile *cfg;
	UOSInt i;
	if (!IsCompilerExist(vsv))
		return false;
	sptr = IO::Path::GetLocAppDataPath(sbuff);
	switch (vsv)
	{
	case Text::VSProject::VSV_VS71:
		Text::StrConcat(sptr, (const UTF8Char*)"\\Microsoft\\VisualStudio\\7.1\\VCComponents.dat");
		cfg = IO::IniFile::Parse(sbuff, 0);
		break;
	case Text::VSProject::VSV_VS8:
		Text::StrConcat(sptr, (const UTF8Char*)"\\Microsoft\\VisualStudio\\8.0\\VCComponents.dat");
		cfg= IO::IniFile::Parse(sbuff, 0);
		break;
	case Text::VSProject::VSV_VS9:
		Text::StrConcat(sptr, (const UTF8Char*)"\\Microsoft\\VisualStudio\\9.0\\VCComponents.dat");
		if ((cfg = IO::IniFile::Parse(sbuff, 0)) == 0)
		{
			Text::StrConcat(sptr, (const UTF8Char*)"\\Microsoft\\VCExpress\\9.0\\VCComponents.dat");
			cfg = IO::IniFile::Parse(sbuff, 0);
		}
		break;
	case Text::VSProject::VSV_VS10:
		Text::StrConcat(sptr, (const UTF8Char*)"\\Microsoft\\VisualStudio\\10.0\\VCComponents.dat");
		if ((cfg = IO::IniFile::Parse(sbuff, 0)) == 0)
		{
			Text::StrConcat(sptr, (const UTF8Char*)"\\Microsoft\\VCExpress\\10.0\\VCComponents.dat");
			cfg = IO::IniFile::Parse(sbuff, 0);
		}
		break;
	case Text::VSProject::VSV_VS11:
		Text::StrConcat(sptr, (const UTF8Char*)"\\Microsoft\\VisualStudio\\11.0\\VCComponents.dat");
		if ((cfg = IO::IniFile::Parse(sbuff, 0)) == 0)
		{
			Text::StrConcat(sptr, (const UTF8Char*)"\\Microsoft\\VCExpress\\11.0\\VCComponents.dat");
			cfg = IO::IniFile::Parse(sbuff, 0);
		}
		break;
	case Text::VSProject::VSV_VS12:
		Text::StrConcat(sptr, (const UTF8Char*)"\\Microsoft\\VisualStudio\\12.0\\VCComponents.dat");
		if ((cfg = IO::IniFile::Parse(sbuff, 0)) == 0)
		{
			Text::StrConcat(sptr, (const UTF8Char*)"\\Microsoft\\VCExpress\\12.0\\VCComponents.dat");
			cfg = IO::IniFile::Parse(sbuff, 0);
		}
		break;
	case Text::VSProject::VSV_VS6:
	case Text::VSProject::VSV_UNKNOWN:
	default:
		return 0;
	}
	 
	if (cfg == 0)
		return false;
	const UTF8Char *paths = cfg->GetValue((const UTF8Char*)"VC\\VC_OBJECTS_PLATFORM_INFO\\Win32\\Directories", (const UTF8Char*)"Include Dirs");
	if (paths)
	{
		csptr = paths;
		sptr = sbuff;
		while (true)
		{
			c = *csptr++;
			if (c == 0 || c == ';')
			{
				*sptr = 0;
				while ((i = Text::StrIndexOf(sbuff, (const UTF8Char*)"$(VCInstallDir)")) != INVALID_INDEX)
				{
					sptr = sbuff3;
					if (i > 0)
					{
						sbuff[i] = 0;
						sptr = Text::StrConcat(sptr, sbuff);
					}
					GetVCInstallDir(sbuff2, vsv);
					sptr = Text::StrConcat(sptr, sbuff2);
					if (sptr[-1] == '\\' && sbuff[i + 15] == '\\')
						sptr--;
					sptr = Text::StrConcat(sptr, &sbuff[i + 15]);
					Text::StrConcat(sbuff, sbuff3);
				}
				while ((i = Text::StrIndexOf(sbuff, (const UTF8Char*)"$(WindowsSdkDir)")) != INVALID_INDEX)
				{
					sptr = sbuff3;
					if (i > 0)
					{
						sbuff[i] = 0;
						sptr = Text::StrConcat(sptr, sbuff);
					}
					GetWindowsSdkDir(sbuff2);
					sptr = Text::StrConcat(sptr, sbuff2);
					if (sptr[-1] == '\\' && sbuff[i + 16] == '\\')
						sptr--;
					sptr = Text::StrConcat(sptr, &sbuff[i + 16]);
					Text::StrConcat(sbuff, sbuff3);
				}
				if (Text::StrIndexOf(sbuff, (const UTF8Char*)"$(") != INVALID_INDEX)
				{
				}
				else
				{
					this->includePaths->Add(Text::StrCopyNew(sbuff));
				}
				if (c == 0)
					break;
				sptr = sbuff;
			}
			else
			{
				*sptr++ = c;
			}
		}
		DEL_CLASS(cfg);
		return true;
	}
	else
	{
		DEL_CLASS(cfg);
		return false;
	}
}

Text::Cpp::CppEnv::CppEnv(Text::VSProject::VisualStudioVersion vsv)
{
	this->pt = Text::CodeProject::PROJT_VSPROJECT;
	NEW_CLASS(this->includePaths, Data::ArrayListStrUTF8());
	this->vsv = vsv;
	this->baseFile = 0;
}

Text::Cpp::CppEnv::CppEnv(Text::CodeProject *proj, IO::ConfigFile *cfg)
{
	this->pt = proj->GetProjectType();
	this->baseFile = proj->GetSourceNameObj()->Clone();
	NEW_CLASS(this->includePaths, Data::ArrayListStrUTF8());
	if (this->pt == Text::CodeProject::PROJT_VSPROJECT)
	{
		this->vsv = ((Text::VSProject*)proj)->GetVSVersion();
		if (cfg)
		{
			const UTF8Char *inclDir = cfg->GetValue((const UTF8Char*)"AdditionalIncludeDirectories");
			if (inclDir)
			{
				UTF8Char *sarr[2];
				UOSInt cnt;
				Text::StringBuilderUTF8 sb;
				sb.Append(inclDir);
				sarr[1] = sb.ToString();
				while (true)
				{
					cnt = Text::StrSplit(sarr, 2, sarr[1], ';');
					AddIncludePath(sarr[0]);
					if (cnt <= 1)
						break;
				}
			}
		}
		InitVSEnv(this->vsv);
	}
}

Text::Cpp::CppEnv::~CppEnv()
{
	UOSInt i = this->includePaths->GetCount();
	while (i-- > 0)
	{
		Text::StrDelNew(this->includePaths->GetItem(i));
	}
	DEL_CLASS(this->includePaths);
	SDEL_STRING(this->baseFile);
}

void Text::Cpp::CppEnv::AddIncludePath(const UTF8Char *includePath)
{
	this->includePaths->Add(Text::StrCopyNew(includePath));
}

UTF8Char *Text::Cpp::CppEnv::GetIncludeFilePath(UTF8Char *buff, const UTF8Char *includeFile, Text::String *sourceFile)
{
	UTF8Char *sptr;
	UTF8Char *sptr2;
	UOSInt i;
	UOSInt j;
	if (Text::StrIndexOf(includeFile, (const UTF8Char*)"opengl.hpp") != INVALID_INDEX)
	{
		i = 0;
	}
	if (sourceFile)
	{
		sourceFile->ConcatTo(buff);
		i = Text::StrLastIndexOf(buff, IO::Path::PATH_SEPERATOR);
		sptr2 = Text::StrConcat(&buff[i + 1], includeFile);
		if (IO::Path::GetPathType(buff) == IO::Path::PathType::File)
			return sptr2;
	}
	i = 0;
	j = this->includePaths->GetCount();
	while (i < j)
	{
		if (this->baseFile)
		{
			this->baseFile->ConcatTo(buff);
			sptr = IO::Path::AppendPath(buff, this->includePaths->GetItem(i));
			*sptr++ = IO::Path::PATH_SEPERATOR;
		}
		else
		{
			sptr = Text::StrConcat(buff, this->includePaths->GetItem(i));
			*sptr++ = IO::Path::PATH_SEPERATOR;
		}
		sptr2 = Text::StrConcat(sptr, includeFile);
		if (IO::Path::PATH_SEPERATOR != '/')
			Text::StrReplace(sptr, '/', IO::Path::PATH_SEPERATOR);
		if (IO::Path::GetPathType(buff) == IO::Path::PathType::File)
			return sptr2;
		i++;
	}
	return 0;
}

void Text::Cpp::CppEnv::InitEnvStatus(Text::Cpp::CppParseStatus *status)
{
	if (this->pt == Text::CodeProject::PROJT_VSPROJECT)
	{
		status->AddGlobalDef((const UTF8Char*)"_WIN32", 0);
		if (this->vsv == Text::VSProject::VSV_VS8 || this->vsv == Text::VSProject::VSV_VS9 || this->vsv == Text::VSProject::VSV_VS10)
		{
			status->AddGlobalDef((const UTF8Char*)"_USE_ATTRIBUTES_FOR_SAL", (const UTF8Char*)"0"); // /analyze

			status->AddGlobalDef((const UTF8Char*)"_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES", (const UTF8Char*)"1");
			status->AddGlobalDef((const UTF8Char*)"_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT", (const UTF8Char*)"1");
			status->AddGlobalDef((const UTF8Char*)"_CRT_SECURE_CPP_OVERLOAD_SECURE_NAMES", (const UTF8Char*)"1");
		}

		if (this->vsv == Text::VSProject::VSV_VS6)
		{
			status->AddGlobalDef((const UTF8Char*)"_MSC_VER", (const UTF8Char*)"1200");
			status->AddGlobalDef((const UTF8Char*)"_MSC_FULL_VER", (const UTF8Char*)"12008804");
		}
		else if (this->vsv == Text::VSProject::VSV_VS71)
		{
			status->AddGlobalDef((const UTF8Char*)"_MSC_VER", (const UTF8Char*)"1310");
			status->AddGlobalDef((const UTF8Char*)"_MSC_FULL_VER", (const UTF8Char*)"13106030");
		}
		else if (this->vsv == Text::VSProject::VSV_VS8)
		{
			status->AddGlobalDef((const UTF8Char*)"_MSC_VER", (const UTF8Char*)"1400");
			status->AddGlobalDef((const UTF8Char*)"_MSC_FULL_VER", (const UTF8Char*)"140050727");
		}
		else if (this->vsv == Text::VSProject::VSV_VS9)
		{
			status->AddGlobalDef((const UTF8Char*)"_MSC_VER", (const UTF8Char*)"1500");
			status->AddGlobalDef((const UTF8Char*)"_MSC_FULL_VER", (const UTF8Char*)"150030729");
		}
		else if (this->vsv == Text::VSProject::VSV_VS10)
		{
			status->AddGlobalDef((const UTF8Char*)"_MSC_VER", (const UTF8Char*)"1600");
			status->AddGlobalDef((const UTF8Char*)"_MSC_FULL_VER", (const UTF8Char*)"160040219");
		}
		else if (this->vsv == Text::VSProject::VSV_VS11)
		{
			status->AddGlobalDef((const UTF8Char*)"_MSC_VER", (const UTF8Char*)"1700");
			status->AddGlobalDef((const UTF8Char*)"_MSC_FULL_VER", (const UTF8Char*)"170051025");
		}
		else if (this->vsv == Text::VSProject::VSV_VS12)
		{
			status->AddGlobalDef((const UTF8Char*)"_MSC_VER", (const UTF8Char*)"1800");
			status->AddGlobalDef((const UTF8Char*)"_MSC_FULL_VER", (const UTF8Char*)"180031101");
		}
		status->AddGlobalDef((const UTF8Char*)"__midl", (const UTF8Char*)"600");
		status->AddGlobalDef((const UTF8Char*)"_INTEGRAL_MAX_BITS", (const UTF8Char*)"64");
		status->AddGlobalDef((const UTF8Char*)"_DBG_MEMCPY_INLINE_", (const UTF8Char*)"0");
	}
}

Text::Cpp::CppEnv *Text::Cpp::CppEnv::LoadVSEnv(Text::VSProject::VisualStudioVersion vsv)
{
	UTF8Char sbuff[512];
	UTF8Char sbuff2[512];
	UTF8Char sbuff3[512];
	UTF8Char *sptr;
	UTF8Char c;
	const UTF8Char *csptr;
	IO::ConfigFile *cfg;
	UOSInt i;
	if (!IsCompilerExist(vsv))
		return 0;
	sptr = IO::Path::GetLocAppDataPath(sbuff);
	switch (vsv)
	{
	case Text::VSProject::VSV_VS71:
		Text::StrConcat(sptr, (const UTF8Char*)"\\Microsoft\\VisualStudio\\7.1\\VCComponents.dat");
		cfg = IO::IniFile::Parse(sbuff, 0);
		break;
	case Text::VSProject::VSV_VS8:
		Text::StrConcat(sptr, (const UTF8Char*)"\\Microsoft\\VisualStudio\\8.0\\VCComponents.dat");
		cfg= IO::IniFile::Parse(sbuff, 0);
		break;
	case Text::VSProject::VSV_VS9:
		Text::StrConcat(sptr, (const UTF8Char*)"\\Microsoft\\VisualStudio\\9.0\\VCComponents.dat");
		if ((cfg = IO::IniFile::Parse(sbuff, 0)) == 0)
		{
			Text::StrConcat(sptr, (const UTF8Char*)"\\Microsoft\\VCExpress\\9.0\\VCComponents.dat");
			cfg = IO::IniFile::Parse(sbuff, 0);
		}
		break;
	case Text::VSProject::VSV_VS10:
		Text::StrConcat(sptr, (const UTF8Char*)"\\Microsoft\\VisualStudio\\10.0\\VCComponents.dat");
		if ((cfg = IO::IniFile::Parse(sbuff, 0)) == 0)
		{
			Text::StrConcat(sptr, (const UTF8Char*)"\\Microsoft\\VCExpress\\10.0\\VCComponents.dat");
			cfg = IO::IniFile::Parse(sbuff, 0);
		}
		break;
	case Text::VSProject::VSV_VS11:
		Text::StrConcat(sptr, (const UTF8Char*)"\\Microsoft\\VisualStudio\\11.0\\VCComponents.dat");
		if ((cfg = IO::IniFile::Parse(sbuff, 0)) == 0)
		{
			Text::StrConcat(sptr, (const UTF8Char*)"\\Microsoft\\VCExpress\\11.0\\VCComponents.dat");
			cfg = IO::IniFile::Parse(sbuff, 0);
		}
		break;
	case Text::VSProject::VSV_VS12:
		Text::StrConcat(sptr, (const UTF8Char*)"\\Microsoft\\VisualStudio\\12.0\\VCComponents.dat");
		if ((cfg = IO::IniFile::Parse(sbuff, 0)) == 0)
		{
			Text::StrConcat(sptr, (const UTF8Char*)"\\Microsoft\\VCExpress\\12.0\\VCComponents.dat");
			cfg = IO::IniFile::Parse(sbuff, 0);
		}
		break;
	case Text::VSProject::VSV_VS6:
	case Text::VSProject::VSV_UNKNOWN:
	default:
		return 0;
	}
	 
	if (cfg == 0)
		return 0;
	Text::Cpp::CppEnv *env = 0;
	const UTF8Char *paths = cfg->GetValue((const UTF8Char*)"VC\\VC_OBJECTS_PLATFORM_INFO\\Win32\\Directories", (const UTF8Char*)"Include Dirs");
	if (paths)
	{
		NEW_CLASS(env, Text::Cpp::CppEnv(vsv));

		csptr = paths;
		sptr = sbuff;
		while (true)
		{
			c = *csptr++;
			if (c == 0 || c == ';')
			{
				*sptr = 0;
				while ((i = Text::StrIndexOf(sbuff, (const UTF8Char*)"$(VCInstallDir)")) != INVALID_INDEX)
				{
					sptr = sbuff3;
					if (i > 0)
					{
						sbuff[i] = 0;
						sptr = Text::StrConcat(sptr, sbuff);
					}
					GetVCInstallDir(sbuff2, vsv);
					sptr = Text::StrConcat(sptr, sbuff2);
					if (sptr[-1] == '\\' && sbuff[i + 15] == '\\')
						sptr--;
					sptr = Text::StrConcat(sptr, &sbuff[i + 15]);
					Text::StrConcat(sbuff, sbuff3);
				}
				while ((i = Text::StrIndexOf(sbuff, (const UTF8Char*)"$(WindowsSdkDir)")) != INVALID_INDEX)
				{
					sptr = sbuff3;
					if (i > 0)
					{
						sbuff[i] = 0;
						sptr = Text::StrConcat(sptr, sbuff);
					}
					GetWindowsSdkDir(sbuff2);
					sptr = Text::StrConcat(sptr, sbuff2);
					if (sptr[-1] == '\\' && sbuff[i + 16] == '\\')
						sptr--;
					sptr = Text::StrConcat(sptr, &sbuff[i + 16]);
					Text::StrConcat(sbuff, sbuff3);
				}
				if (Text::StrIndexOf(sbuff, (const UTF8Char*)"$(") != INVALID_INDEX)
				{
				}
				else
				{
					env->includePaths->Add(Text::StrCopyNew(sbuff));
				}
				if (c == 0)
					break;
				sptr = sbuff;
			}
			else
			{
				*sptr++ = c;
			}
		}
	}
	DEL_CLASS(cfg);
	return env;
}

Text::Cpp::CppEnv *Text::Cpp::CppEnv::LoadVSEnv()
{
	Text::Cpp::CppEnv *env;
	if ((env = LoadVSEnv(Text::VSProject::VSV_VS12)) != 0)
		return env;
	if ((env = LoadVSEnv(Text::VSProject::VSV_VS11)) != 0)
		return env;
	if ((env = LoadVSEnv(Text::VSProject::VSV_VS10)) != 0)
		return env;
	if ((env = LoadVSEnv(Text::VSProject::VSV_VS9)) != 0)
		return env;
	if ((env = LoadVSEnv(Text::VSProject::VSV_VS8)) != 0)
		return env;
	if ((env = LoadVSEnv(Text::VSProject::VSV_VS71)) != 0)
		return env;
	if ((env = LoadVSEnv(Text::VSProject::VSV_VS6)) != 0)
		return env;
	return 0;
}

UTF8Char *Text::Cpp::CppEnv::GetVCInstallDir(UTF8Char *sbuff, Text::VSProject::VisualStudioVersion vsv)
{
	WChar wbuff[512];
	UTF8Char *sptr = 0;
	IO::Registry *reg;
	IO::Registry *reg2;
#if defined(_WIN64)
	reg = IO::Registry::OpenLocalSoftware(L"Wow6432Node\\Microsoft\\VisualStudio");
#else
	reg = IO::Registry::OpenLocalSoftware(L"Microsoft\\VisualStudio");
#endif
	if (reg)
	{
		reg2 = 0;
		switch (vsv)
		{
		case Text::VSProject::VSV_VS71:
			reg2 = reg->OpenSubReg(L"7.1");
			break;
		case Text::VSProject::VSV_VS8:
			reg2 = reg->OpenSubReg(L"8.0");
			break;
		case Text::VSProject::VSV_VS9:
			reg2 = reg->OpenSubReg(L"9.0");
			break;
		case Text::VSProject::VSV_VS10:
			reg2 = reg->OpenSubReg(L"10.0");
			break;
		case Text::VSProject::VSV_VS11:
			reg2 = reg->OpenSubReg(L"11.0");
			break;
		case Text::VSProject::VSV_VS12:
			reg2 = reg->OpenSubReg(L"12.0");
			break;
		case Text::VSProject::VSV_VS6:
		case Text::VSProject::VSV_UNKNOWN:
		default:
			break;
		}
		if (reg2)
		{
			if (reg2->GetValueStr(L"InstallDir", wbuff))
			{
				Text::StrWChar_UTF8(sbuff, wbuff);
				if (vsv == Text::VSProject::VSV_VS71)
				{
					sptr = IO::Path::AppendPath(sbuff, (const UTF8Char*)"..\\..\\Vc7\\");
				}
				else
				{
					sptr = IO::Path::AppendPath(sbuff, (const UTF8Char*)"..\\..\\Vc\\");
				}
			}
			IO::Registry::CloseRegistry(reg2);
		}
		IO::Registry::CloseRegistry(reg);
	}
	return sptr;

/*	sptr = IO::Path::GetSystemProgramPath(sbuff);
	switch (vsv)
	{
	case Text::VSProject::VSV_VS71:
		return Text::StrConcat(sptr, L"\\Microsoft Visual Studio .NET 2003\\VC7\\");
	case Text::VSProject::VSV_VS8:
		return Text::StrConcat(sptr, L"\\Microsoft Visual Studio 8\\VC\\");
	case Text::VSProject::VSV_VS9:
		return Text::StrConcat(sptr, L"\\Microsoft Visual Studio 9.0\\VC\\");
	case Text::VSProject::VSV_VS10:
		return Text::StrConcat(sptr, L"\\Microsoft Visual Studio 10.0\\VC\\");
	case Text::VSProject::VSV_VS11:
		return Text::StrConcat(sptr, L"\\Microsoft Visual Studio 11.0\\VC\\");
	case Text::VSProject::VSV_VS12:
		return Text::StrConcat(sptr, L"\\Microsoft Visual Studio 12.0\\VC\\");
	default:
		return 0;
	}*/
}

UTF8Char *Text::Cpp::CppEnv::GetWindowsSdkDir(UTF8Char *sbuff)
{
	IO::Registry *reg = IO::Registry::OpenLocalSoftware(L"Microsoft\\Microsoft SDKs\\Windows");
	if (reg == 0)
		return 0;
	WChar wbuff[512];
	WChar *sptr = reg->GetValueStr(L"CurrentInstallFolder", wbuff);
	IO::Registry::CloseRegistry(reg);
	if (sptr)
	{
		return Text::StrWChar_UTF8(sbuff, wbuff);
	}
	else
	{
		return 0;
	}
}

Bool Text::Cpp::CppEnv::IsCompilerExist(Text::VSProject::VisualStudioVersion vsv)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	sptr = GetVCInstallDir(sbuff, vsv);
	if (sptr == 0)
		return false;
	Text::StrConcat(sptr, (const UTF8Char*)"bin\\ml.exe");
	return IO::Path::GetPathType(sbuff) == IO::Path::PathType::File;
}

/*
Text::Cpp::CppEnv *Text::Cpp::CppEnv::CreateVSEnv()
{
	WChar sbuff[512];
	CompilerType cppVer = GetSystemCompiler(sbuff);
	if (cppVer == Text::Cpp::CppEnv::CT_UNKNOWN)
		return 0;
	Text::Cpp::CppEnv *env;
	NEW_CLASS(env, Text::Cpp::CppEnv());
	env->includePaths->Add(Text::StrCopyNew(sbuff));
	return env;
}

Text::Cpp::CppEnv::CompilerType Text::Cpp::CppEnv::GetSystemCompiler(WChar *includePath)
{
	WChar *sptr;
	sptr = IO::Path::GetSystemProgramPath(includePath);
	Text::StrConcat(sptr, L"\\Microsoft Visual Studio 10.0\\VC\\bin\\ml.exe");
	if (IO::Path::GetPathType(includePath) == IO::Path::PathType::File)
	{
		Text::StrConcat(sptr, L"\\Microsoft Visual Studio 10.0\\VC\\include");
		return Text::Cpp::CppEnv::CT_VS10;
	}

	Text::StrConcat(sptr, L"\\Microsoft Visual Studio 9.0\\VC\\bin\\ml.exe");
	if (IO::Path::GetPathType(includePath) == IO::Path::PathType::File)
	{
		Text::StrConcat(sptr, L"\\Microsoft Visual Studio 9.0\\VC\\include");
		return Text::Cpp::CppEnv::CT_VS9;
	}

	Text::StrConcat(sptr, L"\\Microsoft Visual Studio 8\\VC\\bin\\ml.exe");
	if (IO::Path::GetPathType(includePath) == IO::Path::PathType::File)
	{
		Text::StrConcat(sptr, L"\\Microsoft Visual Studio 8\\VC\\include");
		return Text::Cpp::CppEnv::CT_VS8;
	}

	Text::StrConcat(sptr, L"\\Microsoft Visual Studio .NET 2003\\VC7\\bin\\ml.exe");
	if (IO::Path::GetPathType(includePath) == IO::Path::PathType::File)
	{
		Text::StrConcat(sptr, L"\\Microsoft Visual Studio .NET 2003\\VC7\\include");
		return Text::Cpp::CppEnv::CT_VS71;
	}
	*includePath = 0;
	return Text::Cpp::CppEnv::CT_UNKNOWN;
}
*/
