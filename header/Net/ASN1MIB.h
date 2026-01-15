#ifndef _SM_NET_ASN1MIB
#define _SM_NET_ASN1MIB
#include "Data/ArrayListNN.hpp"
#include "Data/ArrayListStringNN.h"
#include "Data/FastStringMapNN.hpp"
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
			UIntOS oidLen;
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

		static UIntOS CalcLineSpace(UnsafeArray<const UTF8Char> txt);
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

		static void RemoveSpace(NN<Text::PString> s);
		static Bool IsType(UnsafeArray<const UTF8Char> s);
		static Bool IsKnownType(Text::CStringNN s);
		static Bool IsUnknownType(Text::CStringNN s);
		static IntOS BranketEnd(UnsafeArray<const UTF8Char> s, OptOut<UTF8Char> brkType);
		static UnsafeArray<const UTF8Char> SkipWS(UnsafeArray<const UTF8Char> s);
		static UTF8Char NextChar(UnsafeArray<const UTF8Char> s);
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
