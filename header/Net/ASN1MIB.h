#ifndef _SM_NET_ASN1MIB
#define _SM_NET_ASN1MIB
#include "Data/ArrayList.h"
#include "Data/ArrayListNN.h"
#include "Data/ArrayListStringNN.h"
#include "Data/FastStringMap.h"
#include "Net/MIBReader.h"
#include "Text/StringBuilderUTF8.h"

namespace Net
{
	class ASN1MIB
	{
	public:
		struct ModuleInfo;

		struct ObjectInfo
		{
			Text::String *objectName;
			Text::String *typeName;
			Text::String *typeVal;
			UInt8 oid[32];
			UOSInt oidLen;
			Data::ArrayListStringNN valName;
			Data::ArrayListStringNN valCont;
			ModuleInfo *impModule;
			Bool parsed;
		};

		struct ModuleInfo
		{
			NotNullPtr<Text::String> moduleName;
			NotNullPtr<Text::String> moduleFileName;
			Data::ArrayListStringNN objKeys;
			Data::ArrayList<ObjectInfo*> objValues;
			Data::ArrayList<ObjectInfo *> oidList;
		};
	private:
		Data::FastStringMap<ModuleInfo *> moduleMap;
		ModuleInfo globalModule;

		static UOSInt CalcLineSpace(const UTF8Char *txt);
		static void ModuleAppendOID(ModuleInfo *module, ObjectInfo *obj);
		Bool ParseObjectOID(ModuleInfo *module, ObjectInfo *obj, Text::String *s, NotNullPtr<Text::StringBuilderUTF8> errMessage);
		Bool ParseObjectBegin(Net::MIBReader *reader, ObjectInfo *obj, NotNullPtr<Text::StringBuilderUTF8> errMessage);
		Bool ParseModule(Net::MIBReader *reader, ModuleInfo *module, NotNullPtr<Text::StringBuilderUTF8> errMessage);
		Bool ApplyModuleOID(ModuleInfo *module, ObjectInfo *obj, NotNullPtr<Text::StringBuilderUTF8> errMessage);
		Bool ApplyModuleOIDs(ModuleInfo *module, NotNullPtr<Text::StringBuilderUTF8> errMessage);
		Bool ApplyOIDs(NotNullPtr<Text::StringBuilderUTF8> errMessage);
		Bool ApplyModuleImports(ModuleInfo *module, NotNullPtr<Text::StringBuilderUTF8> errMessage);
		Bool ApplyImports(NotNullPtr<Text::StringBuilderUTF8> errMessage);
		Bool LoadFileInner(Text::CStringNN fileName, NotNullPtr<Text::StringBuilderUTF8> errMessage, Bool postApply);

		static void RemoveSpace(Text::PString *s);
		static Bool IsType(const UTF8Char *s);
		static Bool IsKnownType(Text::CString s);
		static Bool IsUnknownType(Text::CString s);
		static OSInt BranketEnd(const UTF8Char *s, UTF8Char *brkType);
		static const UTF8Char *SkipWS(const UTF8Char *s);
		static UTF8Char NextChar(const UTF8Char *s);
	public:
		ASN1MIB();
		~ASN1MIB();

		ModuleInfo *GetGlobalModule();
		ModuleInfo *GetModuleByFileName(Text::CString fileName);

		void UnloadAll();
		Bool LoadFile(Text::CStringNN fileName, NotNullPtr<Text::StringBuilderUTF8> errMessage);
	};
}
#endif
