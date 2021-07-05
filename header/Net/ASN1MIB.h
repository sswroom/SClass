#ifndef _SM_NET_ASN1MIB
#define _SM_NET_ASN1MIB
#include "Data/StringUTF8Map.h"
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
			const UTF8Char *objectName;
			const UTF8Char *typeName;
			const UTF8Char *typeVal;
			UInt8 oid[32];
			UOSInt oidLen;
			Data::ArrayList<const UTF8Char *> *valName;
			Data::ArrayList<const UTF8Char *> *valCont;
			ModuleInfo *impModule;
			Bool parsed;
		};

		struct ModuleInfo
		{
			const UTF8Char *moduleName;
			const UTF8Char *moduleFileName;
			Data::ArrayListStrUTF8 *objKeys;
			Data::ArrayList<ObjectInfo*> *objValues;
			Data::ArrayList<ObjectInfo *> *oidList;
		};
	private:
		Data::StringUTF8Map<ModuleInfo *> *moduleMap;
		ModuleInfo globalModule;

		static UOSInt CalcLineSpace(const UTF8Char *txt);
		static void ModuleAppendOID(ModuleInfo *module, ObjectInfo *obj);
		Bool ParseObjectOID(ModuleInfo *module, ObjectInfo *obj, const UTF8Char *s, Text::StringBuilderUTF *errMessage);
		Bool ParseObjectBegin(Net::MIBReader *reader, ObjectInfo *obj, Text::StringBuilderUTF *errMessage);
		Bool ParseModule(Net::MIBReader *reader, ModuleInfo *module, Text::StringBuilderUTF *errMessage);
		Bool ApplyModuleOID(ModuleInfo *module, ObjectInfo *obj, Text::StringBuilderUTF *errMessage);
		Bool ApplyModuleOIDs(ModuleInfo *module, Text::StringBuilderUTF *errMessage);
		Bool ApplyOIDs(Text::StringBuilderUTF *errMessage);
		Bool ApplyModuleImports(ModuleInfo *module, Text::StringBuilderUTF *errMessage);
		Bool ApplyImports(Text::StringBuilderUTF *errMessage);
		Bool LoadFileInner(const UTF8Char *fileName, Text::StringBuilderUTF *errMessage, Bool postApply);

		static void RemoveSpace(UTF8Char *s);
		static Bool IsType(const UTF8Char *s);
		static Bool IsKnownType(const UTF8Char *s);
		static Bool IsUnknownType(const UTF8Char *s);
	public:
		ASN1MIB();
		~ASN1MIB();

		ModuleInfo *GetGlobalModule();
		ModuleInfo *GetModuleByFileName(const UTF8Char *fileName);

		void UnloadAll();
		Bool LoadFile(const UTF8Char *fileName, Text::StringBuilderUTF *errMessage);
	};
}
#endif
