// SQLParser for SQL patterns

#include <stdio.h>
#include <string.h>
#include "sqlparser.h"

// Try to recognize well-known patterns in Oracle trigger
bool SqlParser::ParseCreateTriggerOraclePattern(Token *create, Token *table, Token *when, Token *insert)
{
	bool exists = false;

	return exists;
}

// SELECT seq.NEXTVAL INTO :new.col FROM dual in Oracle
bool SqlParser::SelectNextvalFromDual(Token **sequence, Token **column)
{
	Token *select = GetNextWordToken("SELECT", L"SELECT", 6);

    if(select == nullptr)
		return false;

	// Sequence name with NEXTVAL pseudocolumn
	Token *seq = GetNextIdentToken();

	// INTO clause
	Token *into = GetNextWordToken("INTO", L"INTO", 4);

    if(into == nullptr)
		return false;

	// Column name possible with :NEW
	Token *col = GetNextIdentToken();

	// FROM dual
	/*Token *from */ (void) GetNextWordToken("FROM", L"FROM", 4);
	Token *dual = GetNextWordToken("dual", L"dual", 4);
	/*Token *semi1 */ (void) GetNextCharToken(';', L';');

    if(dual == nullptr)
		return false;

	// END must follow SELECT
	Token *end = GetNextWordToken("END", L"END", 3);
	
	bool exists = false;

	// SELECT NEXTVAL FROM dual pattern matched
    if(end != nullptr)
	{
		PushBack(end);

        if(sequence != nullptr)
			*sequence = seq;

        if(column != nullptr)
			*column = col;

		exists = true;
	}

	return exists;
}

// Patterns in (SELECT ...) expression
bool SqlParser::ParseSelectExpressionPattern(Token *open, Token *select)
{
    if(open == nullptr || select == nullptr)
		return false;

	bool exists = false;

	return exists;
}

