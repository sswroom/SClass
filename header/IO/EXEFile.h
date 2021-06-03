#ifndef _SM_IO_EXEFILE
#define _SM_IO_EXEFILE
#include "Data/DateTime.h"
#include "IO/ParsedObject.h"
#include "Manage/DasmBase.h"
#include "Text/StringBuilderUTF.h"

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
			const UTF8Char *moduleName;
			Data::ArrayList<const UTF8Char *> *funcs;
		} ImportInfo;

		typedef struct
		{
			const UTF8Char *funcName;
		} ExportInfo;

		typedef struct
		{
			ResourceType rt;
			const UTF8Char *name;
			UInt32 codePage;
			const UInt8 *data;
			UOSInt dataSize;
		} ResourceInfo;
	private:
		Data::ArrayList<const UTF8Char *> *propNames;
		Data::ArrayList<const UTF8Char *> *propValues;
		Data::ArrayList<ImportInfo*> *importList;
		Data::ArrayList<ExportInfo*> *exportList;
		Data::ArrayList<ResourceInfo*> *resList;

		ProgramEnvDOS *envDOS;

	public:
		EXEFile(const UTF8Char *fileName);
		virtual ~EXEFile();

		virtual IO::ParsedObject::ParserType GetParserType();

		void AddProp(const UTF8Char *name, const UTF8Char *value);
		UOSInt GetPropCount();
		const UTF8Char *GetPropName(UOSInt index);
		const UTF8Char *GetPropValue(UOSInt index);

		UOSInt AddImportModule(const UTF8Char *moduleName);
		void AddImportFunc(UOSInt modIndex, const UTF8Char *funcName);
		UOSInt GetImportCount();
		const UTF8Char *GetImportName(UOSInt modIndex);
		UOSInt GetImportFuncCount(UOSInt modIndex);
		const UTF8Char *GetImportFunc(UOSInt modIndex, UOSInt funcIndex);

		void AddExportFunc(const UTF8Char *funcName);
		UOSInt GetExportCount();
		const UTF8Char *GetExportName(UOSInt index);

		Bool HasDOS();
		void AddDOSEnv(UOSInt b16CodeLen, Manage::Dasm::DasmX86_16_Regs *b16Regs, UInt16 b16CodeSegm);
		UInt8 *GetDOSCodePtr(UOSInt *codeLen);
		void SetDOSHasPSP(Bool hasPSP);
		void GetDOSInitRegs(Manage::Dasm::DasmX86_16_Regs *regs);
		UInt16 GetDOSCodeSegm();

		void AddResource(const UTF8Char *name, const UInt8 *data, UOSInt dataSize, UInt32 codePage, ResourceType rt);
		UOSInt GetResourceCount();
		const ResourceInfo *GetResource(UOSInt index);
	public:
		static Bool GetFileTime(const UTF8Char *fileName, Data::DateTime *fileTimeOut);
		static const UTF8Char *GetResourceTypeName(ResourceType rt);
		static void GetResourceDesc(const ResourceInfo *res, Text::StringBuilderUTF *sb);
	};
}

#endif
