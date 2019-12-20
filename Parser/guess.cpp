// Analytic functions

#include <stdio.h>
#include <string.h>
#include "sqlparser.h"

// Try to define the data type by the variable or column name (table.column%TYPE)
char SqlParser::GuessType(Token *name)
{
    if(name == nullptr)
		return false;

	char data_type = TOKEN_DT_STRING;
	size_t len = name->len;

	// Ends with DATE (emp.hiredate)
	if(Token::Compare(name, "DATE", L"DATE", len - 4, 4) == true)
		data_type = TOKEN_DT_DATETIME;

	return data_type; 
}

char SqlParser::GuessType(Token *name, TokenStr &type)
{
	char dt = 0;

    if(name == nullptr)
		return dt;

	size_t len = name->len;

	// Integer - ID
	if((Token::Compare(name, "ID", L"ID", 2) == true) ||
		// Ends with _ID (account_id i.e)
		(len >= 3 && Token::Compare(name, "_ID", L"_ID", len - 3, 3) == true))
    {
		dt = TOKEN_DT_NUMBER;
	}
	else
	// Decimal - Ends with BALANCE, AMOUNT (archived_balance i.e)
	if((len >= 7 && Token::Compare(name, "BALANCE", L"BALANCE", len - 7, 7) == true) ||
        (len >= 6 && Token::Compare(name, "AMOUNT", L"AMOUNT", len - 6, 6) == true))
	{
		type.Append("DECIMAL(19,4)", L"DECIMAL(19,4)", 13);

		dt = TOKEN_DT_NUMBER;
	}
	else
	// Datetime - Ends with DATE (emp.hiredate)
	if(len > 4 && Token::Compare(name, "DATE", L"DATE", len - 4, 4) == true)
	{
		type.Append("DATETIME", L"DATETIME", 8);

		dt = TOKEN_DT_DATETIME;
    }

	// Return 0 if no pattern matched, although set the default data type 
	return dt;
}
