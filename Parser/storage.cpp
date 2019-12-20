// SQLParser for storage clauses

#include <stdio.h>
#include <string.h>
#include "sqlparser.h"

// Temporary table options
bool SqlParser::ParseTempTableOptions(Token *table_name, Token **start_out, Token **end_out, bool *no_data)
{
	bool exists = false;

    Token *start = nullptr;

	while(true)
	{
		Token *next = GetNextToken();

        if(next == nullptr)
			break;

        if(start == nullptr)
			start = next;

		// ON COMMIT PRESERVE | DELETE ROWS in Oracle, DB2; ON ROLLBACK PRESERVE | DELETE ROWS in DB2   
		if(next->Compare("ON", L"ON", 2) == true)
		{
			Token *commit = GetNextWordToken("COMMIT", L"COMMIT", 6);
            Token *rollback = nullptr;

            if(commit == nullptr)
				rollback = GetNextWordToken("ROLLBACK", L"ROLLBACK", 8);

            if(commit == nullptr && rollback == nullptr)
				break;

			Token *delete_ = GetNextWordToken("DELETE", L"DELETE", 6);
            Token *preserve = nullptr;

            if(delete_ == nullptr)
				preserve = GetNextWordToken("PRESERVE", L"PRESERVE", 8);

			Token *rows = GetNextWordToken("ROWS", L"ROWS", 4);

			// Oracle does not support ON ROLLBACK, but DELETE ROWS in default on rollback
            if(rollback != nullptr)
			{
                if(delete_ != nullptr)
					Token::Remove(next, rows);
				else
					Comment(next, rows);
			}
			
			exists = true;
			continue;
        }

		// Not a temporary table clause
		PushBack(next);
		break;
	}

	if(exists == true)
	{
        if(start_out != nullptr)
			*start_out = start;

        if(end_out != nullptr)
			*end_out = GetLastToken();
	}

	return exists;
}

// Parse CREATE TABLE storage clause
bool SqlParser::ParseStorageClause()
{
	if(ParseOracleStorageClause() == true)
		return true;
	
	return false;
}

// Parse MySQL CREATE TABLE storage clause
bool SqlParser::ParseMysqlStorageClause(Token *table_name, Token **id_start, Token **comment_out)
{
	bool exists = false;

	// Auto_increment start value
    Token *auto_start = nullptr;

	while(true)
	{
		Token *next = GetNextToken();

        if(next == nullptr)
			break;

		// ENGINE = type
		if(next->Compare("ENGINE", L"ENGINE", 6) == true)
		{
			// Equal sign = is optional in the clause
            /*Token *equal =*/(void) GetNextCharToken('=', L'=');
            /*Token *type =*/(void) GetNextToken();
			exists = true;
			continue;
		}
		else
		// AUTO_INCREMENT = start table option
		if(next->Compare("AUTO_INCREMENT", L"AUTO_INCREMENT", 14) == true)
		{
			// Equal sign = is optional in the clause
            /*Token *equal =*/(void) GetNextCharToken('=', L'=');
			auto_start = GetNextNumberToken();

			exists = true;
			continue;
		}
		else
		// DEFAULT CHARSET
		if(next->Compare("DEFAULT", L"DEFAULT", 7) == true)
		{
			Token *option = GetNextToken();

            if(option == nullptr)
				break;

			// CHARSET
			if(option->Compare("CHARSET", L"CHARSET", 7) == true)
			{
				/*Token *equal */ (void) GetNextCharToken('=', L'=');
                /*Token *value */ (void) GetNextIdentToken();
			}
			else
			// CHARACTER SET
			if(option->Compare("CHARACTER", L"CHARACTER", 9) == true)
			{
				/*Token *set */ (void) GetNextWordToken("SET", L"SET", 3);
				/*Token *equal */ (void) GetNextCharToken('=', L'=');
                /*Token *value */ (void) GetNextIdentToken();

			}

			exists = true;
			continue;
		}
		else
		// COLLATE = value
		if(next->Compare("COLLATE", L"COLLATE", 7) == true)
		{
			/*Token *equal */ (void) GetNextCharToken('=', L'=');
            /*Token *value */ (void) GetNextIdentToken();


			exists = true;
			continue;
		}
		else
		// COMMENT = 'table comment'
		if(next->Compare("COMMENT", L"COMMENT", 7) == true)
		{
			// Equal sign = is optional in the clause
			/*Token *equal */ (void) GetNextCharToken('=', L'=');
			Token *text = GetNextToken();

            if(comment_out != nullptr)
				*comment_out = text;


			exists = true;
			continue;
		}
		else
		// PACK_KEYS = 0 | 1 | DEFAULT
		if(next->Compare("PACK_KEYS", L"PACK_KEYS", 9) == true)
		{
			// Optional = 
			/*Token *equal */ (void) GetNextCharToken('=', L'=');
            /*Token *value */ (void) GetNextToken();


			exists = true;
			continue;
		}
		else
		// ROW_FORMAT = type | DEFAULT
		if(next->Compare("ROW_FORMAT", L"ROW_FORMAT", 10) == true)
		{
			// Optional = 
			/*Token *equal */ (void) GetNextCharToken('=', L'=');
            /*Token *value */ (void) GetNextToken();


			exists = true;
			continue;
		}

		// Not a MySQL stoage clause
		PushBack(next);

		break;
	}

    if(id_start != nullptr)
		*id_start = auto_start;

	return exists;
}

// Parse Oracle CREATE TABLE, CREATE INDEX, PARTITION definition storage clause
bool SqlParser::ParseOracleStorageClause()
{
	bool exists = false;

	while(true)
	{
		Token *next = GetNextToken();

        if(next == nullptr)
			break;

		// SEGMENT CREATION IMMEDIATE | DEFERRED
		if(next->Compare("SEGMENT", L"SEGMENT", 7) == true)
		{
			Token *creation = GetNextWordToken("CREATION", L"CREATION", 8);
			Token *value = GetNextToken();

            if(creation != nullptr)
				Token::Remove(next, value);

			exists = true;
			continue;
		}
		else
		// PCTFREE num
		if(next->Compare("PCTFREE", L"PCTFREE", 7) == true)
		{
			Token *value = GetNextNumberToken();

            if(value != nullptr)
				Token::Remove(next, value);

			exists = true;
			continue;
		}
		else
		// PCTUSED num
		if(next->Compare("PCTUSED", L"PCTUSED", 7) == true)
		{
			Token *value = GetNextNumberToken();

            if(value != nullptr)
				Token::Remove(next, value);

			exists = true;
			continue;
		}
		else
		// INITRANS num
		if(next->Compare("INITRANS", L"INITRANS", 8) == true)
		{
			Token *value = GetNextNumberToken();

            if(value != nullptr)
				Token::Remove(next, value);

			exists = true;
			continue;
		}
		else
		// MAXTRANS num
		if(next->Compare("MAXTRANS", L"MAXTRANS", 8) == true)
		{
			Token *value = GetNextNumberToken();

            if(value != nullptr)
				Token::Remove(next, value);

			exists = true;
			continue;
		}
		else
		// COMPRESS [BASIC] or COMPRESS num (for index-orginized tables and indexes)
		if(next->Compare("COMPRESS", L"COMPRESS", 8) == true)
		{
			// Optional BASIC keyword
			Token *basic = GetNextWordToken("BASIC", L"BASIC", 5);
            Token *num = nullptr;

			// Check for a number
            if(basic == nullptr)
				num = GetNextNumberToken();

			Token::Remove(next);
			Token::Remove(basic);
			Token::Remove(num);

			exists = true;
			continue;
		}
		else
		// RESULT_CACHE (MODE DEFAULT)
		if (next->Compare("RESULT_CACHE", L"RESULT_CACHE", 12) == true)
		{
			// Optional BASIC keyword
			Token *open =GetNextCharToken('(', L'(');

            /*Token *more */ (void) GetNextWordToken("MODE", L"MODE", 4);
            /*Token *default */ (void) GetNextWordToken("DEFAULT", L"DEFAULT", 7);

			Token *close = GetNextCharToken(')', L')');

            if(open!= nullptr)
				Token::Remove(next, close);
	
			exists = true;
			continue;
		}
		else
		// NOCOMPRESS 
		if(next->Compare("NOCOMPRESS", L"NOCOMPRESS", 10) == true)
		{
			Token::Remove(next);

			exists = true;
			continue;
		}
		else
		// NOCACHE 
		if(next->Compare("NOCACHE", L"NOCACHE", 7) == true)
		{
			Token::Remove(next);

			exists = true;
			continue;
		}
		else
		// LOGGING 
		if(next->Compare("LOGGING", L"LOGGING", 7) == true)
		{
			Token::Remove(next);

			exists = true;
			continue;
		}
		else
		// NOLOGGING 
		if(next->Compare("NOLOGGING", L"NOLOGGING", 9) == true)
		{
			Token::Remove(next);

			exists = true;
			continue;
		}
		else
		// NOPARALLEL 
		if(next->Compare("NOPARALLEL", L"NOPARALLEL", 10) == true)
		{
			Token::Remove(next);

			exists = true;
			continue;
		}
		else
		// PARALLEL num
		if(next->Compare("PARALLEL", L"PARALLEL", 8) == true)
		{
			Token *value = GetNextNumberToken();

            if(value != nullptr)
				Token::Remove(next, value);

			exists = true;
			continue;
		}
		else
		// MONITORING 
		if (next->Compare("MONITORING", L"MONITORING", 10) == true)
		{
			Token::Remove(next);

			exists = true;
			continue;
		}
		else
		// NOMONITORING
		if(next->Compare("NOMONITORING", L"NOMONITORING", 12) == true)
		{
			Token::Remove(next);

			exists = true;
			continue;
		}
		else
		//VISIBLE
		if (next->Compare("VISIBLE", L"VISIBLE", 7) == true)
		{
			Token::Remove(next);

			exists = true;
			continue;
		}
		else
		// TABLESPACE name 
		if(next->Compare("TABLESPACE", L"TABLESPACE", 10) == true)
		{
			Token *name = GetNextIdentToken();
			Token::Remove(next, name);

			exists = true;
			continue;
		}
		else
		// STORAGE () clause 
		if(next->Compare("STORAGE", L"STORAGE", 7) == true)
		{
			exists = ParseOracleStorageClause(next);
			continue;
		}
		else
		// LOB (column) STORE AS (params)
		if(next->Compare("LOB", L"LOB", 3) == true)
		{
			if(ParseOracleLobStorageClause(next) == true)
			{
				exists = true;
				continue;
			}
		}
		//else
		//// DISABLE | ENABLE STORAGE IN ROW
		//if (next->Compare("DISABLE", L"DISABLE", 7) == true || next->Compare("ENABLE", L"ENABLE", 6) == true)
		//{
		//	Token *first = GetNextToken();
		//	if (first->Compare("VALIDATE", L"VALIDATE", 8) == true) {
		//		//Remove ENABLE VALIDATE
		//		Token::Remove(next, first);
		//	}
		//	else {	
		//		/*Token *in*/ (void)GetNextWordToken("IN", L"IN", 2);
		//		/*Token *in*/ (void)GetNextWordToken("ROW", L"ROW", 3);
		//		Token *movement = GetNextWordToken("MOVEMENT", L"MOVEMENT", 8);

		//		//Remove ENABLE IN ROW MOVEMENT
        //		if (movement != nullptr)
		//			Token::Remove(next, movement);
		//	}

		//	exists = true;
		//	continue;
		//}
		else
		// Oracle partitioning clauses
		if(ParseOraclePartitions(next) == true)
		{
			exists = true;
			continue;
		}
		else
		// COMPUTE STATISTICS 
		if(next->Compare("COMPUTE", L"COMPUTE", 7) == true)
		{
			Token *statistics = GetNextWordToken("STATISTICS", L"STATISTICS", 10);

			// Remove if not Oracle
			Token::Remove(next, statistics);

			exists = true;
			continue;
		}
		else
		// ENABLE ROW MOVEMENT 
		if(next->Compare("DISABLE", L"DISABLE", 7) == true || next->Compare("ENABLE", L"ENABLE", 6) == true)
		{
			Token *row = GetNextWordToken("ROW", L"ROW", 3);

            if(row != nullptr)
			{
				Token *movement = GetNextWordToken("MOVEMENT", L"MOVEMENT", 8);
			
				// Remove if not Oracle
				Token::Remove(next, movement);

				exists = true;
				continue;
			}
		}
		else
		// REVERSE index or primary key storage attribute 
		if(next->Compare("REVERSE", L"REVERSE", 7) == true)
		{
			// Remove if not Oracle
			Token::Remove(next);

			exists = true;
			continue;
		}
		
		// Not an Oracle storage clause
		PushBack(next);
		break;
	}

	return exists;
}

// CREATE INDEX storage options
bool SqlParser::ParseCreateIndexOptions()
{
	if(ParseOracleStorageClause() == true)
		return true;

	return false;
}
