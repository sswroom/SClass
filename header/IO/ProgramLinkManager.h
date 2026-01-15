#ifndef _SM_IO_PROGRAMLINKMANAGER
#define _SM_IO_PROGRAMLINKMANAGER
#include "Data/ArrayListStringNN.h"
#include "IO/ProgramLink.h"

namespace IO
{
	class ProgramLinkManager
	{
	private:
		UIntOS GetLinkNamesDir(NN<Data::ArrayListStringNN> nameList, UnsafeArray<UTF8Char> linkPath, UnsafeArray<UTF8Char> linkPathEnd, UnsafeArray<UTF8Char> filePath, UnsafeArray<UTF8Char> filePathEnd);

	public:
		ProgramLinkManager();
		~ProgramLinkManager();

		UnsafeArray<UTF8Char> GetLinkPath(UnsafeArray<UTF8Char> buff, Bool thisUser);
		UIntOS GetLinkNames(NN<Data::ArrayListStringNN> nameList, Bool allUser, Bool thisUser);
		Bool GetLinkDetail(Text::CStringNN linkName, NN<ProgramLink> link);

		Bool CreateLink(Bool thisUser, Text::CStringNN shortName, Text::CStringNN linkName, Text::CString comment, Text::CString categories, Text::CStringNN cmdLine);
		Bool DeleteLink(Text::CStringNN linkName);
	};
}

#endif
