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
	UnsafeArray<UTF8Char> sptr;
	UTF8Char c;
	UnsafeArray<const UTF8Char> csptr;
	Optional<IO::ConfigFile> cfg;
	UOSInt i;
	if (!IsCompilerExist(vsv))
		return false;
	if (!IO::Path::GetLocAppDataPath(sbuff).SetTo(sptr))
		return false;
	switch (vsv)
	{
	case Text::VSProject::VSV_VS71:
		sptr = Text::StrConcatC(sptr, UTF8STRC("\\Microsoft\\VisualStudio\\7.1\\VCComponents.dat"));
		cfg = IO::IniFile::Parse(CSTRP(sbuff, sptr), 0);
		break;
	case Text::VSProject::VSV_VS8:
		sptr = Text::StrConcatC(sptr, UTF8STRC("\\Microsoft\\VisualStudio\\8.0\\VCComponents.dat"));
		cfg = IO::IniFile::Parse(CSTRP(sbuff, sptr), 0);
		break;
	case Text::VSProject::VSV_VS9:
		sptr = Text::StrConcatC(sptr, UTF8STRC("\\Microsoft\\VisualStudio\\9.0\\VCComponents.dat"));
		if ((cfg = IO::IniFile::Parse(CSTRP(sbuff, sptr), 0)).IsNull())
		{
			sptr = Text::StrConcatC(sptr, UTF8STRC("\\Microsoft\\VCExpress\\9.0\\VCComponents.dat"));
			cfg = IO::IniFile::Parse(CSTRP(sbuff, sptr), 0);
		}
		break;
	case Text::VSProject::VSV_VS10:
		sptr = Text::StrConcatC(sptr, UTF8STRC("\\Microsoft\\VisualStudio\\10.0\\VCComponents.dat"));
		if ((cfg = IO::IniFile::Parse(CSTRP(sbuff, sptr), 0)).IsNull())
		{
			sptr = Text::StrConcatC(sptr, UTF8STRC("\\Microsoft\\VCExpress\\10.0\\VCComponents.dat"));
			cfg = IO::IniFile::Parse(CSTRP(sbuff, sptr), 0);
		}
		break;
	case Text::VSProject::VSV_VS11:
		sptr = Text::StrConcatC(sptr, UTF8STRC("\\Microsoft\\VisualStudio\\11.0\\VCComponents.dat"));
		if ((cfg = IO::IniFile::Parse(CSTRP(sbuff, sptr), 0)).IsNull())
		{
			sptr = Text::StrConcatC(sptr, UTF8STRC("\\Microsoft\\VCExpress\\11.0\\VCComponents.dat"));
			cfg = IO::IniFile::Parse(CSTRP(sbuff, sptr), 0);
		}
		break;
	case Text::VSProject::VSV_VS12:
		sptr = Text::StrConcatC(sptr, UTF8STRC("\\Microsoft\\VisualStudio\\12.0\\VCComponents.dat"));
		if ((cfg = IO::IniFile::Parse(CSTRP(sbuff, sptr), 0)).IsNull())
		{
			sptr = Text::StrConcatC(sptr, UTF8STRC("\\Microsoft\\VCExpress\\12.0\\VCComponents.dat"));
			cfg = IO::IniFile::Parse(CSTRP(sbuff, sptr), 0);
		}
		break;
	case Text::VSProject::VSV_VS6:
	case Text::VSProject::VSV_UNKNOWN:
	default:
		return false;
	}
	
	NN<IO::ConfigFile> nncfg;
	if (!cfg.SetTo(nncfg))
		return false;
	NN<Text::String> paths;
	if (nncfg->GetCateValue(CSTR("VC\\VC_OBJECTS_PLATFORM_INFO\\Win32\\Directories"), CSTR("Include Dirs")).SetTo(paths))
	{
		csptr = paths->v;
		sptr = sbuff;
		while (true)
		{
			c = *csptr++;
			if (c == 0 || c == ';')
			{
				*sptr = 0;
				while ((i = Text::StrIndexOfC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("$(VCInstallDir)"))) != INVALID_INDEX)
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
					sptr = Text::StrConcatC(sbuff, sbuff3, (UOSInt)(sptr - sbuff3));
				}
				while ((i = Text::StrIndexOfC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("$(WindowsSdkDir)"))) != INVALID_INDEX)
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
					sptr = Text::StrConcatC(sbuff, sbuff3, (UOSInt)(sptr - sbuff3));
				}
				if (Text::StrIndexOfC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("$(")) != INVALID_INDEX)
				{
				}
				else
				{
					this->includePaths.Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
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
		nncfg.Delete();
		return true;
	}
	else
	{
		nncfg.Delete();
		return false;
	}
}

Text::Cpp::CppEnv::CppEnv(Text::VSProject::VisualStudioVersion vsv)
{
	this->pt = Text::CodeProject::PROJT_VSPROJECT;
	this->vsv = vsv;
	this->baseFile = 0;
}

Text::Cpp::CppEnv::CppEnv(NN<Text::CodeProject> proj, Optional<IO::ConfigFile> cfg)
{
	this->pt = proj->GetProjectType();
	this->baseFile = proj->GetSourceNameObj()->Clone().Ptr();
	if (this->pt == Text::CodeProject::PROJT_VSPROJECT)
	{
		this->vsv = NN<Text::VSProject>::ConvertFrom(proj)->GetVSVersion();
		NN<IO::ConfigFile> nncfg;
		if (cfg.SetTo(nncfg))
		{
			NN<Text::String> inclDir;
			if (nncfg->GetValue(CSTR("AdditionalIncludeDirectories")).SetTo(inclDir))
			{
				Text::PString sarr[2];
				UOSInt cnt;
				Text::StringBuilderUTF8 sb;
				sb.Append(inclDir);
				sarr[1] = sb;
				while (true)
				{
					cnt = Text::StrSplitP(sarr, 2, sarr[1], ';');
					AddIncludePath(sarr[0].ToCString());
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
	UOSInt i = this->includePaths.GetCount();
	while (i-- > 0)
	{
		OPTSTR_DEL(this->includePaths.GetItem(i));
	}
	SDEL_STRING(this->baseFile);
}

void Text::Cpp::CppEnv::AddIncludePath(Text::CStringNN includePath)
{
	this->includePaths.Add(Text::String::New(includePath));
}

UnsafeArrayOpt<UTF8Char> Text::Cpp::CppEnv::GetIncludeFilePath(UnsafeArray<UTF8Char> buff, Text::CStringNN includeFile, Text::String *sourceFile)
{
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> sptr2;
	UOSInt i;
/*	if (includeFile.IndexOf(UTF8STRC("opengl.hpp")) != INVALID_INDEX)
	{
		i = 0;
	}*/
	if (sourceFile)
	{
		sptr = sourceFile->ConcatTo(buff);
		i = Text::StrLastIndexOfCharC(buff, (UOSInt)(sptr - buff), IO::Path::PATH_SEPERATOR);
		sptr2 = includeFile.ConcatTo(&buff[i + 1]);
		if (IO::Path::GetPathType(CSTRP(buff, sptr2)) == IO::Path::PathType::File)
			return sptr2;
	}
	Data::ArrayIterator<NN<Text::String>> it = this->includePaths.Iterator();
	NN<Text::String> s;
	while (it.HasNext())
	{
		s = it.Next();
		if (this->baseFile)
		{
			sptr = this->baseFile->ConcatTo(buff);
			sptr = IO::Path::AppendPath(buff, sptr, s->ToCString());
			*sptr++ = IO::Path::PATH_SEPERATOR;
		}
		else
		{
			sptr = s->ConcatTo(buff);
			*sptr++ = IO::Path::PATH_SEPERATOR;
		}
		sptr2 = includeFile.ConcatTo(sptr);
		if (IO::Path::PATH_SEPERATOR != '/')
			Text::StrReplace(sptr, '/', IO::Path::PATH_SEPERATOR);
		if (IO::Path::GetPathType(CSTRP(buff, sptr2)) == IO::Path::PathType::File)
			return sptr2;
	}
	return 0;
}

void Text::Cpp::CppEnv::InitEnvStatus(NN<Text::Cpp::CppParseStatus> status)
{
	if (this->pt == Text::CodeProject::PROJT_VSPROJECT)
	{
		status->AddGlobalDef(CSTR("_WIN32"), CSTR_NULL);
		if (this->vsv == Text::VSProject::VSV_VS8 || this->vsv == Text::VSProject::VSV_VS9 || this->vsv == Text::VSProject::VSV_VS10)
		{
			status->AddGlobalDef(CSTR("_USE_ATTRIBUTES_FOR_SAL"), CSTR("0")); // /analyze

			status->AddGlobalDef(CSTR("_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES"), CSTR("1"));
			status->AddGlobalDef(CSTR("_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT"), CSTR("1"));
			status->AddGlobalDef(CSTR("_CRT_SECURE_CPP_OVERLOAD_SECURE_NAMES"), CSTR("1"));
		}

		if (this->vsv == Text::VSProject::VSV_VS6)
		{
			status->AddGlobalDef(CSTR("_MSC_VER"), CSTR("1200"));
			status->AddGlobalDef(CSTR("_MSC_FULL_VER"), CSTR("12008804"));
		}
		else if (this->vsv == Text::VSProject::VSV_VS71)
		{
			status->AddGlobalDef(CSTR("_MSC_VER"), CSTR("1310"));
			status->AddGlobalDef(CSTR("_MSC_FULL_VER"), CSTR("13106030"));
		}
		else if (this->vsv == Text::VSProject::VSV_VS8)
		{
			status->AddGlobalDef(CSTR("_MSC_VER"), CSTR("1400"));
			status->AddGlobalDef(CSTR("_MSC_FULL_VER"), CSTR("140050727"));
		}
		else if (this->vsv == Text::VSProject::VSV_VS9)
		{
			status->AddGlobalDef(CSTR("_MSC_VER"), CSTR("1500"));
			status->AddGlobalDef(CSTR("_MSC_FULL_VER"), CSTR("150030729"));
		}
		else if (this->vsv == Text::VSProject::VSV_VS10)
		{
			status->AddGlobalDef(CSTR("_MSC_VER"), CSTR("1600"));
			status->AddGlobalDef(CSTR("_MSC_FULL_VER"), CSTR("160040219"));
		}
		else if (this->vsv == Text::VSProject::VSV_VS11)
		{
			status->AddGlobalDef(CSTR("_MSC_VER"), CSTR("1700"));
			status->AddGlobalDef(CSTR("_MSC_FULL_VER"), CSTR("170051025"));
		}
		else if (this->vsv == Text::VSProject::VSV_VS12)
		{
			status->AddGlobalDef(CSTR("_MSC_VER"), CSTR("1800"));
			status->AddGlobalDef(CSTR("_MSC_FULL_VER"), CSTR("180031101"));
		}
		status->AddGlobalDef(CSTR("__midl"), CSTR("600"));
		status->AddGlobalDef(CSTR("_INTEGRAL_MAX_BITS"), CSTR("64"));
		status->AddGlobalDef(CSTR("_DBG_MEMCPY_INLINE_"), CSTR("0"));
	}
}

Text::Cpp::CppEnv *Text::Cpp::CppEnv::LoadVSEnv(Text::VSProject::VisualStudioVersion vsv)
{
	UTF8Char sbuff[512];
	UTF8Char sbuff2[512];
	UTF8Char sbuff3[512];
	UnsafeArray<UTF8Char> sptr;
	UTF8Char c;
	UnsafeArray<const UTF8Char> csptr;
	Optional<IO::ConfigFile> cfg;
	UOSInt i;
	if (!IsCompilerExist(vsv))
		return 0;
	if (!IO::Path::GetLocAppDataPath(sbuff).SetTo(sptr))
		return 0;
	switch (vsv)
	{
	case Text::VSProject::VSV_VS71:
		sptr = Text::StrConcatC(sptr, UTF8STRC("\\Microsoft\\VisualStudio\\7.1\\VCComponents.dat"));
		cfg = IO::IniFile::Parse(CSTRP(sbuff, sptr), 0);
		break;
	case Text::VSProject::VSV_VS8:
		sptr = Text::StrConcatC(sptr, UTF8STRC("\\Microsoft\\VisualStudio\\8.0\\VCComponents.dat"));
		cfg = IO::IniFile::Parse(CSTRP(sbuff, sptr), 0);
		break;
	case Text::VSProject::VSV_VS9:
		sptr = Text::StrConcatC(sptr, UTF8STRC("\\Microsoft\\VisualStudio\\9.0\\VCComponents.dat"));
		if ((cfg = IO::IniFile::Parse(CSTRP(sbuff, sptr), 0)).IsNull())
		{
			sptr = Text::StrConcatC(sptr, UTF8STRC("\\Microsoft\\VCExpress\\9.0\\VCComponents.dat"));
			cfg = IO::IniFile::Parse(CSTRP(sbuff, sptr), 0);
		}
		break;
	case Text::VSProject::VSV_VS10:
		sptr = Text::StrConcatC(sptr, UTF8STRC("\\Microsoft\\VisualStudio\\10.0\\VCComponents.dat"));
		if ((cfg = IO::IniFile::Parse(CSTRP(sbuff, sptr), 0)).IsNull())
		{
			sptr = Text::StrConcatC(sptr, UTF8STRC("\\Microsoft\\VCExpress\\10.0\\VCComponents.dat"));
			cfg = IO::IniFile::Parse(CSTRP(sbuff, sptr), 0);
		}
		break;
	case Text::VSProject::VSV_VS11:
		sptr = Text::StrConcatC(sptr, UTF8STRC("\\Microsoft\\VisualStudio\\11.0\\VCComponents.dat"));
		if ((cfg = IO::IniFile::Parse(CSTRP(sbuff, sptr), 0)).IsNull())
		{
			sptr = Text::StrConcatC(sptr, UTF8STRC("\\Microsoft\\VCExpress\\11.0\\VCComponents.dat"));
			cfg = IO::IniFile::Parse(CSTRP(sbuff, sptr), 0);
		}
		break;
	case Text::VSProject::VSV_VS12:
		sptr = Text::StrConcatC(sptr, UTF8STRC("\\Microsoft\\VisualStudio\\12.0\\VCComponents.dat"));
		if ((cfg = IO::IniFile::Parse(CSTRP(sbuff, sptr), 0)).IsNull())
		{
			sptr = Text::StrConcatC(sptr, UTF8STRC("\\Microsoft\\VCExpress\\12.0\\VCComponents.dat"));
			cfg = IO::IniFile::Parse(CSTRP(sbuff, sptr), 0);
		}
		break;
	case Text::VSProject::VSV_VS6:
	case Text::VSProject::VSV_UNKNOWN:
	default:
		return 0;
	}
	
	NN<IO::ConfigFile> nncfg;
	if (!cfg.SetTo(nncfg))
		return 0;
	Text::Cpp::CppEnv *env = 0;
	NN<Text::String> paths;
	if (nncfg->GetCateValue(CSTR("VC\\VC_OBJECTS_PLATFORM_INFO\\Win32\\Directories"), CSTR("Include Dirs")).SetTo(paths))
	{
		NEW_CLASS(env, Text::Cpp::CppEnv(vsv));

		csptr = paths->v;
		sptr = sbuff;
		while (true)
		{
			c = *csptr++;
			if (c == 0 || c == ';')
			{
				*sptr = 0;
				while ((i = Text::StrIndexOfC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("$(VCInstallDir)"))) != INVALID_INDEX)
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
					sptr = Text::StrConcatC(sbuff, sbuff3, (UOSInt)(sptr - sbuff3));
				}
				while ((i = Text::StrIndexOfC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("$(WindowsSdkDir)"))) != INVALID_INDEX)
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
					sptr = Text::StrConcatC(sbuff, sbuff3, (UOSInt)(sptr - sbuff3));
				}
				if (Text::StrIndexOfC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("$(")) != INVALID_INDEX)
				{
				}
				else
				{
					env->includePaths.Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
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
	nncfg.Delete();
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

UnsafeArrayOpt<UTF8Char> Text::Cpp::CppEnv::GetVCInstallDir(UnsafeArray<UTF8Char> sbuff, Text::VSProject::VisualStudioVersion vsv)
{
	WChar wbuff[512];
	UnsafeArrayOpt<UTF8Char> sptr = 0;
	UnsafeArray<UTF8Char> nnsptr;
	NN<IO::Registry> reg;
	Optional<IO::Registry> reg2;
	NN<IO::Registry> nnreg2;
#if defined(_WIN64)
	if (IO::Registry::OpenLocalSoftware(L"Wow6432Node\\Microsoft\\VisualStudio").SetTo(reg))
#else
	if (IO::Registry::OpenLocalSoftware(L"Microsoft\\VisualStudio").SetTo(reg))
#endif
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
		if (reg2.SetTo(nnreg2))
		{
			if (nnreg2->GetValueStr(L"InstallDir", wbuff).NotNull())
			{
				nnsptr = Text::StrWChar_UTF8(sbuff, wbuff);
				if (vsv == Text::VSProject::VSV_VS71)
				{
					sptr = IO::Path::AppendPath(sbuff, nnsptr, CSTR("..\\..\\Vc7\\"));
				}
				else
				{
					sptr = IO::Path::AppendPath(sbuff, nnsptr, CSTR("..\\..\\Vc\\"));
				}
			}
			IO::Registry::CloseRegistry(nnreg2);
		}
		IO::Registry::CloseRegistry(reg);
	}
	return sptr;

/*	sptr = IO::Path::GetSystemProgramPath(sbuff);
	switch (vsv)
	{
	case Text::VSProject::VSV_VS71:
		return Text::StrConcatC(sptr, UTF8STRC("\\Microsoft Visual Studio .NET 2003\\VC7\\"));
	case Text::VSProject::VSV_VS8:
		return Text::StrConcatC(sptr, UTF8STRC("\\Microsoft Visual Studio 8\\VC\\"));
	case Text::VSProject::VSV_VS9:
		return Text::StrConcatC(sptr, UTF8STRC("\\Microsoft Visual Studio 9.0\\VC\\"));
	case Text::VSProject::VSV_VS10:
		return Text::StrConcatC(sptr, UTF8STRC("\\Microsoft Visual Studio 10.0\\VC\\"));
	case Text::VSProject::VSV_VS11:
		return Text::StrConcatC(sptr, UTF8STRC("\\Microsoft Visual Studio 11.0\\VC\\"));
	case Text::VSProject::VSV_VS12:
		return Text::StrConcatC(sptr, UTF8STRC("\\Microsoft Visual Studio 12.0\\VC\\"));
	default:
		return 0;
	}*/
}

UnsafeArrayOpt<UTF8Char> Text::Cpp::CppEnv::GetWindowsSdkDir(UnsafeArray<UTF8Char> sbuff)
{
	NN<IO::Registry> reg;
	if (!IO::Registry::OpenLocalSoftware(L"Microsoft\\Microsoft SDKs\\Windows").SetTo(reg))
		return 0;
	WChar wbuff[512];
	UnsafeArrayOpt<WChar> wptr = reg->GetValueStr(L"CurrentInstallFolder", wbuff);
	IO::Registry::CloseRegistry(reg);
	if (wptr.NotNull())
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
	UnsafeArray<UTF8Char> sptr;
	if (!GetVCInstallDir(sbuff, vsv).SetTo(sptr))
		return false;
	sptr = Text::StrConcatC(sptr, UTF8STRC("bin\\ml.exe"));
	return IO::Path::GetPathType(CSTRP(sbuff, sptr)) == IO::Path::PathType::File;
}

/*
Text::Cpp::CppEnv *Text::Cpp::CppEnv::CreateVSEnv()
{
	WChar wbuff[512];
	CompilerType cppVer = GetSystemCompiler(wbuff);
	if (cppVer == Text::Cpp::CppEnv::CT_UNKNOWN)
		return 0;
	Text::Cpp::CppEnv *env;
	NEW_CLASS(env, Text::Cpp::CppEnv());
	env->includePaths->Add(Text::StrCopyNew(wbuff));
	return env;
}

Text::Cpp::CppEnv::CompilerType Text::Cpp::CppEnv::GetSystemCompiler(WChar *includePath)
{
	WChar *wptr;
	wptr = IO::Path::GetSystemProgramPath(includePath);
	Text::StrConcatC(wptr, UTF8STRC("\\Microsoft Visual Studio 10.0\\VC\\bin\\ml.exe"));
	if (IO::Path::GetPathType(includePath) == IO::Path::PathType::File)
	{
		Text::StrConcatC(wptr, UTF8STRC("\\Microsoft Visual Studio 10.0\\VC\\include"));
		return Text::Cpp::CppEnv::CT_VS10;
	}

	Text::StrConcatC(wptr, UTF8STRC("\\Microsoft Visual Studio 9.0\\VC\\bin\\ml.exe"));
	if (IO::Path::GetPathType(includePath) == IO::Path::PathType::File)
	{
		Text::StrConcatC(wptr, UTF8STRC("\\Microsoft Visual Studio 9.0\\VC\\include"));
		return Text::Cpp::CppEnv::CT_VS9;
	}

	Text::StrConcatC(wptr, UTF8STRC("\\Microsoft Visual Studio 8\\VC\\bin\\ml.exe"));
	if (IO::Path::GetPathType(includePath) == IO::Path::PathType::File)
	{
		Text::StrConcatC(wptr, UTF8STRC("\\Microsoft Visual Studio 8\\VC\\include"));
		return Text::Cpp::CppEnv::CT_VS8;
	}

	Text::StrConcatC(wptr, UTF8STRC("\\Microsoft Visual Studio .NET 2003\\VC7\\bin\\ml.exe"));
	if (IO::Path::GetPathType(includePath) == IO::Path::PathType::File)
	{
		Text::StrConcatC(wptr, UTF8STRC("\\Microsoft Visual Studio .NET 2003\\VC7\\include"));
		return Text::Cpp::CppEnv::CT_VS71;
	}
	*includePath = 0;
	return Text::Cpp::CppEnv::CT_UNKNOWN;
}
*/
