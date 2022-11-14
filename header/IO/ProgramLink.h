#ifndef _SM_IO_PROGRAMLINK
#define _SM_IO_PROGRAMLINK
#include "Text/String.h"

namespace IO
{
	class ProgramLink
	{
	private:
		Text::String *type;
		Text::String *name;
		Text::String *genericName;
		Text::String *version;
		Text::String *comment;
		Text::String *mimeTypes;
		Text::String *categories;
		Text::String *keywords;
		Text::String *cmdLine;
		Bool noDisplay;
		Bool startupNotify;
		Bool terminal;
		Text::String *icon;
	public:
		ProgramLink();
		~ProgramLink();

		Text::String *GetType() const;
		void SetType(Text::String *type);
		void SetType(Text::CString type);
		Text::String *GetName() const;
		void SetName(Text::String *name);
		void SetName(Text::CString name);
		Text::String *GetGenericName() const;
		void SetGenericName(Text::String *genericName);
		void SetGenericName(Text::CString genericName);
		Text::String *GetVersion() const;
		void SetVersion(Text::String *version);
		void SetVersion(Text::CString version);
		Text::String *GetComment() const;
		void SetComment(Text::String *comment);
		void SetComment(Text::CString comment);
		Text::String *GetMimeTypes() const;
		void SetMimeTypes(Text::String *mimeTypes);
		void SetMimeTypes(Text::CString mimeTypes);
		Text::String *GetCategories() const;
		void SetCategories(Text::String *categories);
		void SetCategories(Text::CString categories);
		Text::String *GetKeywords() const;
		void SetKeywords(Text::String *keywords);
		void SetKeywords(Text::CString keywords);
		Text::String *GetCmdLine() const;
		void SetCmdLine(Text::String *cmdLine);
		void SetCmdLine(Text::CString cmdLine);
		Bool IsNoDisplay() const;
		void SetNoDisplay(Bool noDisplay);
		Bool IsStartupNotify() const;
		void SetStartupNotify(Bool startupNotify);
		Bool HasTerminal() const;
		void SetTerminal(Bool terminal);
		Text::String *GetIcon() const;
		void SetIcon(Text::String *icon);
		void SetIcon(Text::CString icon);
	};
}

#endif
