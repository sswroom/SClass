#ifndef _SM_IO_EXEFILE
#define _SM_IO_EXEFILE
#include "Data/ArrayListNN.h"
#include "Data/DateTime.h"
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
			NotNullPtr<Text::String> moduleName;
			Data::ArrayListNN<Text::String> *funcs;
		} ImportInfo;

		typedef struct
		{
			NotNullPtr<Text::String> funcName;
		} ExportInfo;

		typedef struct
		{
			ResourceType rt;
			NotNullPtr<Text::String> name;
			UInt32 codePage;
			const UInt8 *data;
			UOSInt dataSize;
		} ResourceInfo;
	private:
		Data::ArrayListNN<Text::String> propNames;
		Data::ArrayListNN<Text::String> propValues;
		Data::ArrayList<ImportInfo*> importList;
		Data::ArrayList<ExportInfo*> exportList;
		Data::ArrayList<ResourceInfo*> resList;

		ProgramEnvDOS *envDOS;

	public:
		EXEFile(NotNullPtr<Text::String> fileName);
		virtual ~EXEFile();

		virtual IO::ParserType GetParserType() const;

		void AddProp(Text::CString name, Text::CString value);
		UOSInt GetPropCount() const;
		Text::String *GetPropName(UOSInt index) const;
		Text::String *GetPropValue(UOSInt index) const;
		

		UOSInt AddImportModule(Text::CString moduleName);
		void AddImportFunc(UOSInt modIndex, Text::CString funcName);
		UOSInt GetImportCount() const;
		Text::String *GetImportName(UOSInt modIndex) const;
		UOSInt GetImportFuncCount(UOSInt modIndex) const;
		Text::String *GetImportFunc(UOSInt modIndex, UOSInt funcIndex) const;

		void AddExportFunc(Text::CString funcName);
		UOSInt GetExportCount() const;
		Text::String *GetExportName(UOSInt index) const;

		Bool HasDOS() const;
		void AddDOSEnv(UOSInt b16CodeLen, Manage::Dasm::DasmX86_16_Regs *b16Regs, UInt16 b16CodeSegm);
		UInt8 *GetDOSCodePtr(UOSInt *codeLen) const;
		void SetDOSHasPSP(Bool hasPSP);
		void GetDOSInitRegs(Manage::Dasm::DasmX86_16_Regs *regs) const;
		UInt16 GetDOSCodeSegm() const;

		void AddResource(Text::CString name, const UInt8 *data, UOSInt dataSize, UInt32 codePage, ResourceType rt);
		UOSInt GetResourceCount() const;
		const ResourceInfo *GetResource(UOSInt index) const;
	public:
		static Bool GetFileTime(Text::CString fileName, Data::DateTime *fileTimeOut);
		static Text::CString GetResourceTypeName(ResourceType rt);
		static void GetResourceDesc(const ResourceInfo *res, Text::StringBuilderUTF8 *sb);
	};
}

#endif
