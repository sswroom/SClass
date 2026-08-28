#include "Stdafx.h"
#include "DB/SQL/SQLComment.h"

DB::SQL::SQLComment::SQLComment(Text::CStringNN commentText, CommentType commentType)
{
	this->commentText = Text::String::New(commentText);
	this->commentType = commentType;
}

DB::SQL::SQLComment::~SQLComment()
{
	this->commentText->Release();
}

DB::SQL::CommandType DB::SQL::SQLComment::GetCommandType() const
{
	return CommandType::Comment;
}

NN<Text::String> DB::SQL::SQLComment::GetCommentText() const
{
	return this->commentText;
}

DB::SQL::SQLComment::CommentType DB::SQL::SQLComment::GetCommentType() const
{
	return this->commentType;
}
