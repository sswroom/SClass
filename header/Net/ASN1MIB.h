#ifndef _SM_NET_ASN1MIB
#define _SM_NET_ASN1MIB
#include "Data/ArrayList.h"
#include "Data/ArrayListNN.h"
#include "Data/ArrayListStringNN.h"
#include "Data/FastStringMapNN.h"
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
			Optional<ModuleInfo> impModule;
			Bool parsed;
		};

		struct ModuleInfo
		{
			NN<Text::String> moduleName;
			NN<Text::String> moduleFileName;
			Data::ArrayListStringNN objKeys;
			Data::ArrayListNN<ObjectInfo> objValues;
			Data::ArrayListNN<ObjectInfo> oidList;
		};
	private:
		Data::FastStringMapNN<ModuleInfo> moduleMap;
		ModuleInfo globalModule;

		static UOSInt CalcLineSpace(const UTF8Char *txt);
		static void ModuleAppendOID(NN<ModuleInfo> module, NN<ObjectInfo> obj);
		Bool ParseObjectOID(NN<ModuleInfo> module, NN<ObjectInfo> obj, Text::String *s, NN<Text::StringBuilderUTF8> errMessage);
		Bool ParseObjectBegin(NN<Net::MIBReader> reader, Optional<ObjectInfo> obj, NN<Text::StringBuilderUTF8> errMessage);
		Bool ParseModule(NN<Net::MIBReader> reader, NN<ModuleInfo> module, NN<Text::StringBuilderUTF8> errMessage);
		Bool ApplyModuleOID(NN<ModuleInfo> module, NN<ObjectInfo> obj, NN<Text::StringBuilderUTF8> errMessage);
		Bool ApplyModuleOIDs(NN<ModuleInfo> module, NN<Text::StringBuilderUTF8> errMessage);
		Bool ApplyOIDs(NN<Text::StringBuilderUTF8> errMessage);
		Bool ApplyModuleImports(ModuleInfo *module, NN<Text::StringBuilderUTF8> errMessage);
		Bool ApplyImports(NN<Text::StringBuilderUTF8> errMessage);
		Bool LoadFileInner(Text::CStringNN fileName, NN<Text::StringBuilderUTF8> errMessage, Bool postApply);

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

		NN<ModuleInfo> GetGlobalModule();
		Optional<ModuleInfo> GetModuleByFileName(Text::CStringNN fileName);

		void UnloadAll();
		Bool LoadFile(Text::CStringNN fileName, NN<Text::StringBuilderUTF8> errMessage);
	};
}
#endif
