#ifndef _SM_IO_FILEEXPORTER
#define _SM_IO_FILEEXPORTER
#include "IO/SeekableStream.h"
#include "IO/ParsedObject.h"
#include "Text/EncodingFactory.h"

namespace IO
{
	class FileExporter
	{
	public:
		enum class SupportType
		{
			NotSupported,
			NormalStream,
			MultiFiles,
			PathOnly
		};

		enum class ParamType
		{
			INT32,
			STRINGUTF8,
			SELECTION,
			BOOL
		};

		struct ParamInfo
		{
			Text::CStringNN name;
			ParamType paramType;
			Bool allowNull;
		};

		struct ParamData;

	public:
		virtual ~FileExporter(){};

		virtual Int32 GetName() = 0;
		virtual SupportType IsObjectSupported(NN<IO::ParsedObject> pobj) = 0;
		virtual Bool GetOutputName(UIntOS index, UnsafeArray<UTF8Char> nameBuff, UnsafeArray<UTF8Char> fileNameBuff) = 0;
		virtual Bool ExportFile(NN<IO::SeekableStream> stm, Text::CStringNN fileName, NN<IO::ParsedObject> pobj, Optional<ParamData> param) = 0;
		Bool ExportNewFile(Text::CStringNN fileName, NN<IO::ParsedObject> pobj, Optional<ParamData> param);

		virtual void SetCodePage(UInt32 codePage);
		virtual void SetEncFactory(Optional<Text::EncodingFactory> encFact);

		virtual UIntOS GetParamCnt();
		virtual Optional<ParamData> CreateParam(NN<IO::ParsedObject> pobj);
		virtual void DeleteParam(Optional<ParamData> param);
		virtual Bool GetParamInfo(UIntOS index, NN<ParamInfo> info);
		virtual Bool SetParamStr(Optional<ParamData> aram, UIntOS index, UnsafeArrayOpt<const UTF8Char> val);
		virtual Bool SetParamInt32(Optional<ParamData> param, UIntOS index, Int32 val);
		virtual Bool SetParamSel(Optional<ParamData> param, UIntOS index, UIntOS selCol);
		virtual Bool SetParamBool(Optional<ParamData> param, UIntOS index, Bool val);
		virtual UnsafeArrayOpt<UTF8Char> GetParamStr(Optional<ParamData> param, UIntOS index, UnsafeArray<UTF8Char> buff);
		virtual Int32 GetParamInt32(Optional<ParamData> param, UIntOS index);
		virtual Int32 GetParamSel(Optional<ParamData>param, UIntOS index);
		virtual UnsafeArrayOpt<UTF8Char> GetParamSelItems(Optional<ParamData> param, UIntOS index, UIntOS itemIndex, UnsafeArray<UTF8Char> buff);
		virtual Bool GetParamBool(Optional<ParamData> param, UIntOS index);
	};
}
#endif
