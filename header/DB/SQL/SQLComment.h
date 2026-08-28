#ifndef _SM_DB_SQLCOMMENT
#define _SM_DB_SQLCOMMENT
#include "DB/SQL/SQLCommand.h"

namespace DB
{
	namespace SQL
	{
		class SQLComment : public SQLCommand
		{
		public:
			enum class CommentType
			{
				DoubleDash,
				Sharp,
				BlockComment
			};
		private:
			NN<Text::String> commentText;
			CommentType commentType;
		public:
			SQLComment(Text::CStringNN commentText, CommentType commentType);
			virtual ~SQLComment();

			virtual CommandType GetCommandType() const;
			NN<Text::String> GetCommentText() const;
			CommentType GetCommentType() const;
		};
	}
}
#endif
