#ifndef _SM_IO_EXEFILE
#define _SM_IO_EXEFILE
#include "Data/ArrayListStringNN.h"
#include "Data/DateTime.h"
#include "Data/Timestamp.h"
#include "IO/ParsedObject.h"
#include "Manage/DasmBase.h"
#include "Text/CString.h"
#include "Text/String.h"
#include "Text/StringBuilderUTF8.h"

namespace IO
{
	class EXEFile : public IO::ParsedObject
	{
	public:
		typedef enum
		{
			EXEO_UNKNOWN,
			EXEO_DOS,
			EXEO_WIN32,
			EXEO_WIN64,
			EXEO_ALPHA,
			EXEO_ARM,
			EXEO_ALPHA64,
			EXEO_M68K,
			EXEO_MIPS16,
			EXEO_MIPSFPU,
			EXEO_MIPSFPU16,
			EXEO_POWERPC,
			EXEO_R3000,
			EXEO_R4000,
			EXEO_R10000,
			EXEO_SH3,
			EXEO_SH4,
			EXEO_THUMB
		} EXEOS;

		typedef enum
		{
			EXESUBSYS_UNKNOWN = 0,
			EXESUBSYS_NATIVE = 1,
			EXESUBSYS_WINGUI = 2,
			EXESUBSYS_WINCUI = 3,
			EXESUBSYS_POSIX = 7,
			EXESUBSYS_CEGUI = 9,
			EXESUBSYS_EFIAPP = 10,
			EXESUBSYS_EFIDRIVERBOOT = 11,
			EXESUBSYS_EFIDRIVERRUNTIME = 12
		} EXESubsys;

		typedef enum
		{
			RT_UNKNOWN,
			RT_VERSIONINFO,
			RT_BITMAP,
			RT_MENU,
			RT_DIALOG,
			RT_STRINGTABLE,
			RT_ACCELERATOR,
			RT_CURSOR,
			RT_ICON,
			RT_FONT,
			RT_FONTDIR,
			RT_RAW_DATA,
			RT_MESSAGETABLE,
			RT_DLGINCLUDE,
			RT_PLUGPLAY,
			RT_VXD,
			RT_ANICURSOR,
			RT_ANIICON,
			RT_HTML,
			RT_MANIFEST
		} ResourceType;

		typedef struct
		{
			EXEOS machineType;
			Data::DateTime *fileTime;
			Int32 linkerVersion;
			Int32 osVersion;
			Int32 imageVersion;
			Int32 subsysVersion;
			EXESubsys subsysType;
		} ExtraProperties;

		typedef struct
		{
			UInt8 *b16Codes;
			UOSInt b16CodeLen;
			Manage::Dasm::DasmX86_16_Regs *b16Regs;
			Bool b16HasPSP;
			UInt16 b16CodeSegm;
		} ProgramEnvDOS;

		typedef struct
		{
			NN<Text::String> moduleName;
			Data::ArrayListStringNN *funcs;
		} ImportInfo;

		typedef struct
		{
			NN<Text::String> funcName;
		} ExportInfo;

		typedef struct
		{
			ResourceType rt;
			NN<Text::String> name;
			UInt32 codePage;
			const UInt8 *data;
			UOSInt dataSize;
		} ResourceInfo;
	private:
		Data::ArrayListStringNN propNames;
		Data::ArrayListStringNN propValues;
		Data::ArrayListNN<ImportInfo> importList;
		Data::ArrayListNN<ExportInfo> exportList;
		Data::ArrayListNN<ResourceInfo> resList;

		ProgramEnvDOS *envDOS;

	public:
		EXEFile(NN<Text::String> fileName);
		virtual ~EXEFile();

		virtual IO::ParserType GetParserType() const;

		void AddProp(Text::CString name, Text::CString value);
		UOSInt GetPropCount() const;
		Optional<Text::String> GetPropName(UOSInt index) const;
		Optional<Text::String> GetPropValue(UOSInt index) const;
		

		UOSInt AddImportModule(Text::CString moduleName);
		void AddImportFunc(UOSInt modIndex, Text::CString funcName);
		UOSInt GetImportCount() const;
		Optional<Text::String> GetImportName(UOSInt modIndex) const;
		UOSInt GetImportFuncCount(UOSInt modIndex) const;
		Optional<Text::String> GetImportFunc(UOSInt modIndex, UOSInt funcIndex) const;

		void AddExportFunc(Text::CStringNN funcName);
		UOSInt GetExportCount() const;
		Optional<Text::String> GetExportName(UOSInt index) const;

		Bool HasDOS() const;
		void AddDOSEnv(UOSInt b16CodeLen, Manage::Dasm::DasmX86_16_Regs *b16Regs, UInt16 b16CodeSegm);
		UInt8 *GetDOSCodePtr(OutParam<UOSInt> codeLen) const;
		void SetDOSHasPSP(Bool hasPSP);
		void GetDOSInitRegs(Manage::Dasm::DasmX86_16_Regs *regs) const;
		UInt16 GetDOSCodeSegm() const;

		void AddResource(Text::CString name, const UInt8 *data, UOSInt dataSize, UInt32 codePage, ResourceType rt);
		UOSInt GetResourceCount() const;
		Optional<const ResourceInfo> GetResource(UOSInt index) const;
	public:
		static Bool GetFileTime(Text::CStringNN fileName, Data::DateTime *fileTimeOut);
		static Data::Timestamp GetFileTime(Text::CStringNN fileName);
		static Text::CStringNN GetResourceTypeName(ResourceType rt);
		static void GetResourceDesc(NN<const ResourceInfo> res, NN<Text::StringBuilderUTF8> sb);
	};
}

#endif
