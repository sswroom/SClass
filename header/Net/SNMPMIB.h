#ifndef _SM_NET_SNMPMIB
#define _SM_NET_SNMPMIB
#include "Data/StringUTF8Map.h"
#include "Text/StringBuilderUTF.h"
#include "Text/UTF8Reader.h"

namespace Net
{
	class SNMPMIB
	{
	public:
		typedef struct
		{
			const UTF8Char *objectName;
			const UTF8Char *typeName;
			const UTF8Char *typeVal;
			UInt8 oid[32];
			OSInt oidLen;
			Data::ArrayList<const UTF8Char *> *valName;
			Data::ArrayList<const UTF8Char *> *valCont;
		} ObjectInfo;

		typedef struct
		{
			const UTF8Char *moduleName;
			const UTF8Char *moduleFileName;
			Data::ArrayListStrUTF8 *objKeys;
			Data::ArrayList<ObjectInfo*> *objValues;
			Data::ArrayList<ObjectInfo *> *oidList;
		} ModuleInfo;
	private:
		Data::StringUTF8Map<ModuleInfo *> *moduleMap;
		ModuleInfo globalModule;

		static OSInt CalcLineSpace(const UTF8Char *txt);
		static void ModuleAppendOID(ModuleInfo *module, ObjectInfo *obj);
		Bool ParseObjectOID(ModuleInfo *module, ObjectInfo *obj, const UTF8Char *s, Text::StringBuilderUTF *errMessage);
		Bool ParseObjectBegin(Text::UTF8Reader *reader, ObjectInfo *obj, Text::StringBuilderUTF *errMessage);
		Bool ParseModule(Text::UTF8Reader *reader, ModuleInfo *module, Text::StringBuilderUTF *errMessage);
	public:
		SNMPMIB();
		~SNMPMIB();

		ModuleInfo *GetGlobalModule();
		ModuleInfo *GetModuleByFileName(const UTF8Char *fileName);

		void UnloadAll();
		Bool LoadFile(const UTF8Char *fileName, Text::StringBuilderUTF *errMessage);
	};
}
#endif
