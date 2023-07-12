#ifndef _SM_IO_PROGRAMLINKMANAGER
#define _SM_IO_PROGRAMLINKMANAGER
#include "Data/ArrayListNN.h"
#include "IO/ProgramLink.h"

namespace IO
{
	class ProgramLinkManager
	{
	private:
		UOSInt GetLinkNamesDir(Data::ArrayListNN<Text::String> *nameList, UTF8Char *linkPath, UTF8Char *linkPathEnd, UTF8Char *filePath, UTF8Char *filePathEnd);

	public:
		ProgramLinkManager();
		~ProgramLinkManager();

		UTF8Char *GetLinkPath(UTF8Char *buff, Bool thisUser);
		UOSInt GetLinkNames(Data::ArrayListNN<Text::String> *nameList, Bool allUser, Bool thisUser);
		Bool GetLinkDetail(Text::CString linkName, ProgramLink *link);

		Bool CreateLink(Bool thisUser, Text::CString shortName, Text::CString linkName, Text::CString comment, Text::CString categories, Text::CString cmdLine);
		Bool DeleteLink(Text::CString linkName);
	};
}

#endif
