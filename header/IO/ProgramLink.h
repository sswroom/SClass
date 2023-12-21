#ifndef _SM_IO_PROGRAMLINK
#define _SM_IO_PROGRAMLINK
#include "Text/String.h"

namespace IO
{
	class ProgramLink
	{
	private:
		Optional<Text::String> type;
		Optional<Text::String> name;
		Optional<Text::String> genericName;
		Optional<Text::String> version;
		Optional<Text::String> comment;
		Optional<Text::String> mimeTypes;
		Optional<Text::String> categories;
		Optional<Text::String> keywords;
		Optional<Text::String> cmdLine;
		Bool noDisplay;
		Bool startupNotify;
		Bool terminal;
		Optional<Text::String> icon;
	public:
		ProgramLink();
		~ProgramLink();

		Optional<Text::String> GetType() const;
		void SetType(Optional<Text::String> type);
		void SetType(Text::CString type);
		Optional<Text::String> GetName() const;
		void SetName(Optional<Text::String> name);
		void SetName(Text::CString name);
		Optional<Text::String> GetGenericName() const;
		void SetGenericName(Optional<Text::String> genericName);
		void SetGenericName(Text::CString genericName);
		Optional<Text::String> GetVersion() const;
		void SetVersion(Optional<Text::String> version);
		void SetVersion(Text::CString version);
		Optional<Text::String> GetComment() const;
		void SetComment(Optional<Text::String> comment);
		void SetComment(Text::CString comment);
		Optional<Text::String> GetMimeTypes() const;
		void SetMimeTypes(Optional<Text::String> mimeTypes);
		void SetMimeTypes(Text::CString mimeTypes);
		Optional<Text::String> GetCategories() const;
		void SetCategories(Optional<Text::String> categories);
		void SetCategories(Text::CString categories);
		Optional<Text::String> GetKeywords() const;
		void SetKeywords(Optional<Text::String> keywords);
		void SetKeywords(Text::CString keywords);
		Optional<Text::String> GetCmdLine() const;
		void SetCmdLine(Optional<Text::String> cmdLine);
		void SetCmdLine(Text::CString cmdLine);
		Bool IsNoDisplay() const;
		void SetNoDisplay(Bool noDisplay);
		Bool IsStartupNotify() const;
		void SetStartupNotify(Bool startupNotify);
		Bool HasTerminal() const;
		void SetTerminal(Bool terminal);
		Optional<Text::String> GetIcon() const;
		void SetIcon(Optional<Text::String> icon);
		void SetIcon(Text::CString icon);
	};
}

#endif
