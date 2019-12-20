// Various SQL clauses

#include <stdio.h>
#include <string.h>
#include "sqlparser.h"

// ALTER/CREATE SEQUENCE options
bool SqlParser::ParseSequenceOptions(Token **start_with_out, Token **increment_by_out, StatsSummaryItem &ssi)
{
	bool exists = false;

	while(true)
	{
		STATS_DTL_DECL

		Token *option = GetNextToken();

        if(option == nullptr)
			break;

		// START WITH num
		if(TOKEN_CMP(option, "START"))
		{
			/*Token *with */ (void) TOKEN_GETNEXTW("WITH");
			Token *start_with = GetNextNumberToken();

			Token::Remove(option, start_with);

            if(start_with_out != nullptr)
				*start_with_out = start_with;

			STATS_DTL_DESC(SEQUENCE_START_WITH_DESC)
			STATS_DTL_CONV_NO_NEED(Target(SQL_ORACLE, SQL_MARIADB_ORA))

            SEQ_DTL_STATS_V("START WITH num", option)
            SEQ_OPT_DTL_STATS(option, start_with)

			exists = true;
		}
		else
		// INCREMENT BY
		if(TOKEN_CMP(option, "INCREMENT"))
		{
			/*Token *by */ (void) TOKEN_GETNEXTW("BY");
			Token *increment_by = GetNextNumberToken();

			Token::Remove(option, increment_by);

            if(increment_by_out != nullptr)
				*increment_by_out = increment_by;

			STATS_DTL_DESC(SEQUENCE_INCREMENT_BY_DESC)
			STATS_DTL_CONV_NO_NEED(Target(SQL_ORACLE, SQL_MARIADB_ORA))

            SEQ_DTL_STATS_V("INCREMENT BY num", option)
            SEQ_OPT_DTL_STATS(option, increment_by)

			exists = true;
		}
		else
		// MINVALUE num
		if(TOKEN_CMP(option, "MINVALUE"))
		{
			Token *value = GetNextToken();

			Token::Remove(option, value);

			STATS_DTL_DESC(SEQUENCE_MINVALUE_DESC)
			STATS_DTL_CONV_NO_NEED(Target(SQL_ORACLE, SQL_MARIADB_ORA))

            SEQ_DTL_STATS_V("MINVALUE num", option)
            SEQ_OPT_DTL_STATS(option, value)

			exists = true;
		}
		else
		// NOMINVALUE
		if(TOKEN_CMP(option, "NOMINVALUE"))
		{
			Token::Remove(option);

			STATS_DTL_DESC(SEQUENCE_NOMINVALUE_DESC)
			STATS_DTL_CONV_NO_NEED(Target(SQL_ORACLE, SQL_MARIADB_ORA))

            SEQ_DTL_STATS_V("NOMINVALUE", option)

			exists = true;
		}
		else
		// MAXVALUE num
		if(TOKEN_CMP(option, "MAXVALUE"))
		{
			Token *value = GetNextToken();

			Token::Remove(option, value);

			STATS_DTL_DESC(SEQUENCE_MAXVALUE_DESC)
			STATS_DTL_CONV_NO_NEED(Target(SQL_ORACLE, SQL_MARIADB_ORA))

            SEQ_DTL_STATS_V("MAXVALUE num", option)
            SEQ_OPT_DTL_STATS(option, value)

			exists = true;
		}
		else
		// NOMAXVALUE
		if(TOKEN_CMP(option, "NOMAXVALUE"))
		{
			Token::Remove(option);

			STATS_DTL_DESC(SEQUENCE_NOMAXVALUE_DESC)
			STATS_DTL_CONV_NO_NEED(Target(SQL_ORACLE, SQL_MARIADB_ORA))
            SEQ_DTL_STATS_V("NOMAXVALUE", option)

			exists = true;
		}
		else
		// CACHE num
		if(TOKEN_CMP(option, "CACHE"))
		{
			Token *value = GetNextToken();

			Token::Remove(option, value);

			STATS_DTL_DESC(SEQUENCE_CACHE_DESC)
			STATS_DTL_CONV_NO_NEED(Target(SQL_ORACLE, SQL_MARIADB_ORA))

            SEQ_DTL_STATS_V("CACHE num", option)
            SEQ_OPT_DTL_STATS(option, value)

			exists = true;
		}
		else
		// NOCACHE
		if(TOKEN_CMP(option, "NOCACHE"))
		{
			Token::Remove(option);

			STATS_DTL_DESC(SEQUENCE_NOCACHE_DESC)
			STATS_DTL_CONV_NO_NEED(Target(SQL_ORACLE, SQL_MARIADB_ORA))

            SEQ_DTL_STATS_V("NOCACHE", option)

			exists = true;
		}
		else
		// NOCYCLE
		if(TOKEN_CMP(option, "NOCYCLE"))
		{
			Token::Remove(option);

			STATS_DTL_DESC(SEQUENCE_NOCYCLE_DESC)
			STATS_DTL_CONV_NO_NEED(Target(SQL_ORACLE, SQL_MARIADB_ORA))

            SEQ_DTL_STATS_V("NOCYCLE", option)

			exists = true;
		}
		else
		// CYCLE
		if(TOKEN_CMP(option, "CYCLE"))
		{
			Token::Remove(option);

			STATS_DTL_DESC(SEQUENCE_CYCLE_DESC)
			STATS_DTL_CONV_NO_NEED(Target(SQL_ORACLE, SQL_MARIADB_ORA))

            SEQ_DTL_STATS_V("CYCLE", option)

			exists = true;
		}
		else
		// ORDER
		
		STATS_UPDATE_STATUS
	}

	return exists;
}
