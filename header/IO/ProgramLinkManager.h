#ifndef _SM_IO_PROGRAMLINKMANAGER
#define _SM_IO_PROGRAMLINKMANAGER
#include "Data/ArrayList.h"
#include "IO/ProgramLink.h"

namespace IO
{
	class ProgramLinkManager
	{
	private:
		UOSInt GetLinkNamesDir(Data::ArrayList<Text::String*> *nameList, UTF8Char *linkPath, UTF8Char *linkPathEnd, UTF8Char *filePath, UTF8Char *filePathEnd);

	public:
		ProgramLinkManager();
		~ProgramLinkManager();

		UOSInt GetLinkNames(Data::ArrayList<Text::String*> *nameList, Bool allUser, Bool thisUser);
		Bool GetLinkDetail(Text::CString linkName, ProgramLink *link);
	};
}

#endif
