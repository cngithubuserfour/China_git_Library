// Oracle clauses

#include <stdio.h>
#include <string.h>
#include "sqlparser.h"
#include "listw.h"
#include "str.h"

// Oracle built-in package reference
bool SqlParser::ParseOraclePackage(Token *token)
{
    if(token == nullptr)
		return false;

	STATS_DECL
	STATS_DTL_DECL

	bool exists = false;
	std::string package_name;

	if(TOKEN_CMP_PART0(token, "DBMS_OUTPUT."))
	{
		STATS_SET_DESC(SQL_PKG_DBMS_OUTPUT_DESC)
		package_name = "DBMS_OUTPUT";
		
		exists = true;
	}
	else
	if(TOKEN_CMP_PART0(token, "APEX_") || TOKEN_CMP_PART0(token, "DBMS_") || TOKEN_CMP_PART0(token, "DEBUG_") ||
		TOKEN_CMP_PART0(token, "CTX_") || TOKEN_CMP_PART0(token, "HTF_") || TOKEN_CMP_PART0(token, "HTP_") ||
		TOKEN_CMP_PART0(token, "ORD_") || TOKEN_CMP_PART0(token, "OWA_") || TOKEN_CMP_PART0(token, "SDO_") || 
		TOKEN_CMP_PART0(token, "SEM_") || TOKEN_CMP_PART0(token, "UTL_") || TOKEN_CMP_PART0(token, "WPG_"))
	{
		// Define the package name
		Str::GetSubtringUntil(token->str, '.', (int)token->len, package_name);

		STATS_SET_DESC("Oracle built-in PL/SQL package")
		exists = true;
	}

	if(!exists)
		return exists;

	Token *open = TOKEN_GETNEXT('(');

	// Parse function
    if(open != nullptr)
	{
		PKG_DTL_STATS(token)
	}

	STATS_UPDATE_STATUS
    PKG_STATS_V(package_name.c_str(), token)
	
	return exists;
}

// Oracle STORAGE clause in CREATE TABLE 
bool SqlParser::ParseOracleStorageClause(Token *storage)
{
    if(storage == nullptr)
		return false;

	Token *open = GetNextCharToken('(', L'(');

    if(open == nullptr)
		return false;

	bool exists = false;

	while(true)
	{
		Token *next = GetNextToken();

        if(next == nullptr)
			break;

		// INITIAL num
		if(next->Compare("INITIAL", L"INITIAL", 7) == true)
		{
			Token *value = GetNextToken();

            if(value != nullptr)
				Token::Remove(next, value);

			exists = true;
			continue;
		}
		else
		// NEXT num
		if(next->Compare("NEXT", L"NEXT", 4) == true)
		{
			Token *value = GetNextToken();

            if(value != nullptr)
				Token::Remove(next, value);

			exists = true;
			continue;
		}
		else
		// MINEXTENTS num
		if(next->Compare("MINEXTENTS", L"MINEXTENTS", 10) == true)
		{
			Token *value = GetNextNumberToken();

            if(value != nullptr)
				Token::Remove(next, value);

			exists = true;
			continue;
		}
		else
		// MAXEXTENTS num | UNLIMITED
		if(next->Compare("MAXEXTENTS", L"MAXEXTENTS", 10) == true)
		{
			Token *value = GetNextToken();

            if(value != nullptr)
				Token::Remove(next, value);

			exists = true;
			continue;
		}
		else
		// PCTINCREASE num
		if(next->Compare("PCTINCREASE", L"PCTINCREASE", 11) == true)
		{
			Token *value = GetNextNumberToken();

            if(value != nullptr)
				Token::Remove(next, value);

			exists = true;
			continue;
		}
		else
		// FREELISTS num
		if(next->Compare("FREELISTS", L"FREELISTS", 9) == true)
		{
			Token *num = GetNextToken();

            if(num != nullptr)
				Token::Remove(next, num);

			exists = true;
			continue;
		}
		else
		// FREELIST GROUPS num
		if(next->Compare("FREELIST", L"FREELIST", 8) == true)
		{
			/*Token *groups */ (void) GetNextWordToken("GROUPS", L"GROUPS", 6);
			Token *num = GetNextToken();

            if(num != nullptr)
				Token::Remove(next, num);

			exists = true;
			continue;
		}
		else
		// BUFFER_POOL name 
		if(next->Compare("BUFFER_POOL", L"BUFFER_POOL", 11) == true)
		{
			Token *value = GetNextToken();

            if(value != nullptr)
				Token::Remove(next, value);

			exists = true;
			continue;
		}
		else
		// FLASH_CACHE name 
		if(next->Compare("FLASH_CACHE", L"FLASH_CACHE", 11) == true)
		{
			Token *value = GetNextToken();

            if(value != nullptr)
				Token::Remove(next, value);

			exists = true;
			continue;
		}
		else
		// CELL_FLASH_CACHE name 
		if(next->Compare("CELL_FLASH_CACHE", L"CELL_FLASH_CACHE", 16) == true)
		{
			Token *value = GetNextToken();

            if(value != nullptr)
				Token::Remove(next, value);

			exists = true;
			continue;
		}

		// Not an Oracle storage clause
		PushBack(next);
		break;
	}

	Token *close = GetNextCharToken(')', L')');

	//remove STORAGE ()
	Token::Remove(storage);
	Token::Remove(open);
	Token::Remove(close);

	return exists;
}

// Oracle LOB (column) STORE AS (params) 
bool SqlParser::ParseOracleLobStorageClause(Token *lob)
{
    if(lob == nullptr)
		return false;

	Token *open = GetNextCharToken('(', L'(');

    if(open == nullptr)
		return false;

	// LOB column
	/*Token *col */ (void) GetNextIdentToken();

	/*Token *close*/ (void) GetNextCharToken(')', L')');

	/*Token *store*/ (void) GetNextWordToken("STORE", L"STORE", 5);
	Token *as = GetNextWordToken("AS", L"AS", 2);

	// Optional BASICFILE or SECUREFILE
	Token *type = GetNextWordToken("BASICFILE", L"BASICFILE", 9);

    if(type == nullptr)
		type = GetNextWordToken("SECUREFILE", L"SECUREFILE", 10);

	// Optional LOB segment name
	Token *segment_name = GetNextIdentToken();

	Token::Remove(lob, as);
	Token::Remove(type);
	Token::Remove(segment_name);

	// Start with open (
	Token *open2 = GetNextCharToken('(', L'(');

    if(open2 == nullptr)
		return false;

	bool exists = false;

	while(true)
	{
		Token *next = GetNextToken();

        if(next == nullptr)
			break;

		// TABLESPACE name
		if(next->Compare("TABLESPACE", L"TABLESPACE", 10) == true)
		{
			Token *name = GetNextToken();

            if(name != nullptr)
				Token::Remove(next, name);

			exists = true;
			continue;
		}
		else
		// DISABLE | ENABLE STORAGE IN ROW
		if(next->Compare("DISABLE", L"DISABLE", 7) == true || next->Compare("ENABLE", L"ENABLE", 6) == true)
		{
			/*Token *storage*/ (void) GetNextWordToken("STORAGE", L"STORAGE", 7);
			/*Token *in*/ (void) GetNextWordToken("IN", L"IN", 2);
			Token *row = GetNextWordToken("ROW", L"ROW", 3);

            if(row != nullptr)
				Token::Remove(next, row);

			exists = true;
			continue;
		}
		else
		// CHUNK num
		if(next->Compare("CHUNK", L"CHUNK", 5) == true)
		{
			Token *value = GetNextNumberToken();

            if(value != nullptr)
				Token::Remove(next, value);

			exists = true;
			continue;
		}
		else
		// PCTVERSION for LOB storage 
		if(next->Compare("PCTVERSION", L"PCTVERSION", 10) == true)
		{
			Token *num = GetNextNumberToken();

            if(num != nullptr)
			{
				Token::Remove(next, num);

				exists = true;
				continue;
			}
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
		// NOCACHE 
		if(next->Compare("NOCACHE", L"NOCACHE", 7) == true)
		{
			Token::Remove(next);

			exists = true;
			continue;
		}
		else
		// RETENTION
		if (next->Compare("RETENTION", L"RETENTION", 9) == true)
		{
			Token::Remove(next);
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

		// Not an Oracle storage clause
		PushBack(next);
		break;
	}

	Token *close2 = GetNextCharToken(')', L')');

	Token::Remove(open2);
	Token::Remove(close2);

	return exists;
}

// Oracle partitions definitions
bool SqlParser::ParseOraclePartitions(Token *token)
{
    if(token == nullptr)
		return false;

	// LOCAL or GLOBAL can be specified for index partition
    Token *local = token->Compare("LOCAL", L"LOCAL", 5) ? token : nullptr;
    Token *global = nullptr;
	
    if(local == nullptr && token->Compare("GLOBAL", L"GLOBAL", 6))
		global = token;

    if(local != nullptr || global != nullptr)
	{
		Token::Remove(token);

        if(global != nullptr)
			token = GetNextToken();
	}

    Token *subpartition = nullptr;

	// PARTITION BY is not specified for LOCAL partitioned indexes
    if(local == nullptr)
	{
		// PARTITION BY clause
		if(ParseOraclePartitionsBy(token) == false)
			return false;

		subpartition = GetNextWordToken("SUBPARTITION", L"SUBPARTITION", 12);

		// SUBPARTITION clause can follow
        if(subpartition != nullptr)
		{
			ParseOraclePartitionsBy(subpartition);

			// SUBPARTITION TEMPLATE
			Token *subpartition2 = GetNextWordToken("SUBPARTITION", L"SUBPARTITION", 12);
            Token *template_ = nullptr;

            if(subpartition2 != nullptr)
			{
				template_ = GetNextWordToken("TEMPLATE", L"TEMPLATE", 8);

				// Subpartition template definition
                ParseOraclePartition(nullptr, subpartition2);
			}
		}

		// PARTITION definition including subpartitions
		ParseOraclePartition(token, subpartition);
	}
	else
	{
		// A dummy tokens to parse PARTITION and SUBPARTITION definitions for index
		Token part, subpart;

		// PARTITION definition including subpartitions for LOCAL index
		ParseOraclePartition(&part, &subpart);
	}

	return true;
}

// Oracle partition or subpartition type and columns 
bool SqlParser::ParseOraclePartitionsBy(Token *token)
{
    if(token == nullptr)
		return false;
		
    Token *partition = token->Compare("PARTITION", L"PARTITION", 9) ? token : nullptr;
    Token *subpartition = nullptr;

    if(partition == nullptr)
        subpartition = token->Compare("SUBPARTITION", L"SUBPARTITION", 12) ? token : nullptr;
	
    if(partition == nullptr && subpartition == nullptr)
		return false;

	/*Token *by */ (void) GetNextWordToken("BY", L"BY", 2);

	// RANGE, HASH or LIST
	Token *range = GetNextWordToken("RANGE", L"RANGE", 5);
    Token *hash = nullptr;
    Token *list = nullptr;
	
    if(range == nullptr)
		hash = GetNextWordToken("HASH", L"HASH", 4);

    if(range == nullptr && hash == nullptr)
		list = GetNextWordToken("LIST", L"LIST", 4);

	Token *open = GetNextCharToken('(', L'(');

    if(open == nullptr)
		return false;

	// Multiple columns can be specified
	while(true)
	{
		Token *column = GetNextIdentToken();

        if(column == nullptr)
			break;

		Token *comma = GetNextCharToken(',', ',');

        if(comma == nullptr)
			break;
	}

    /*Token *close */ GetNextCharToken(')', L')');

	// SUBPARTITIONS num
    if(subpartition != nullptr)
	{
		Token *subpartitions = GetNextWordToken("SUBPARTITIONS", L"SUBPARTITIONS", 13);
        Token *num = nullptr;

        if(subpartitions != nullptr)
		{
			num = GetNextToken();

			Token::Remove(subpartitions, num);
		}
	}

	return true;
}

// Oracle PARTITION or SUBPARTITION definition clause 
bool SqlParser::ParseOraclePartition(Token *partition, Token *subpartition)
{
	// Both partition and subpartition parameters can be NULL for LOCAL partitioned indexes
	Token *open = GetNextCharToken('(', L'(');

    if(open == nullptr)
		return false;

	bool exists = false;

	// List of partition/subpartition definitions can be specified
	while(true)
	{
        Token *type = nullptr;

        if(partition != nullptr)
			type = GetNextWordToken("PARTITION", L"PARTITION", 9);
		else
			type = GetNextWordToken("SUBPARTITION", L"SUBPARTITION", 12);

        if(type == nullptr)
			break;

		Token *name = GetNextIdentToken();

        if(name == nullptr)
			break;

		exists = true;

		// VALUES clause (optional for example for index)
		Token *values = GetNextWordToken("VALUES", L"VALUES", 6);

        if(values != nullptr)
		{
			// LESS THAN
            /*Token *less */(void) GetNextWordToken("LESS", L"LESS", 4);
            /*Token *than */(void) GetNextWordToken("THAN", L"THAN", 4);

			/*Token *open_range */ (void) GetNextCharToken('(', L'(');
			Token *exp = GetNextToken();

			// Expression to define the partition range			
            ParseExpression(exp);

			Token *close_range = GetNextCharToken(')', L')');

            if(close_range == nullptr)
				break;
		}

		// Storage for the partition 
		ParseOracleStorageClause();

		// Now parse SUBPARTITION definitions for the current PARTITION recursively
        if(partition != nullptr && subpartition != nullptr)
            ParseOraclePartition(nullptr, subpartition);

		// Partition definitions are comma separated
		Token *comma = GetNextCharToken(',', L',');

        if(comma == nullptr)
			break;
	}

	Token *close = GetNextCharToken(')', L')');

	// PARTITION BY was removed, so remove partition definitions as well
    if(partition != nullptr && partition->IsRemoved())
		Token::Remove(open, close);
	else
	// SUBPARTITION BY was removed
    if(partition == nullptr && subpartition != nullptr && subpartition->IsRemoved())
		Token::Remove(open, close);

	return exists;
}

// Oracle variable declaration block
bool SqlParser::ParseOracleVariableDeclarationBlock(Token *declare)
{
	bool exists = false;

	// declare points to AS keyword for outer procedural block

	// Remove DECLARE for MySQL as it will be specified before each variable
	if(Token::Compare(declare, "DECLARE", L"DECLARE", 7) == true)
	{
		Token::Remove(declare);
    }

    // Last token of last declared variable (not cursor) in the current PL/SQL DECLARE block
    Token *last_declare_var = nullptr;

    // Cursor definitions
	ListWM cursors;

	// Process declaration statements until BEGIN
	while(true)
	{
		// Variable name
		Token *name = GetNextIdentToken();

        if(name == nullptr)
			break;

		// Exit if BEGIN matched, or PROCEDURE/FUNCTION in package body
        if(TOKEN_CMP(name, "BEGIN") || (_spl_package != nullptr && (TOKEN_CMP(name, "PROCEDURE") || TOKEN_CMP(name, "FUNCTION"))))
		{
			PushBack(name);
			break;
		}

		// CURSOR cur IS SELECT definition, PRAGMA or forward declaration for exception
		if(ParseOracleCursorDeclaration(name, &cursors) || ParseOracleObjectType(name) ||
			ParseOraclePragma(name) || ParseOracleException(name))
		{
			exists = true;
			continue;
		}

		_spl_variables.Add(name);
		exists = true;

		// Add DECLARE before name in SQL Server, Sybase and MySQL
		Prepend(name, "DECLARE ", L"DECLARE ", 8, declare);

		// Optional CONSTANT
        /*Token *constant */(void) TOKEN_GETNEXTW("CONSTANT");

		Token *data_type = GetNextToken();

		// Check and resolve Oracle %TYPE and %ROWTYPE variable
		bool typed = ParseTypedVariable(name, data_type);

		// Get the parameter data type
		if(typed == false)
			ParseDataType(data_type, SQL_SCOPE_VAR_DECL);

		// Propagate data type to variable
		name->data_type = data_type->data_type;
		name->data_subtype = data_type->data_subtype;

        /*Token *data_type_end*/(void) GetLastToken();

		// Optional initialization value := value in Oracle and = value in PostgreSQL
		Token *colon = GetNextCharToken(':', L':');
		Token *equal = GetNextCharToken('=', L'=');
        Token *default_ = nullptr;

		// DEFAULT keyword can be also specified
        if(equal == nullptr)
			default_ = GetNext("DEFAULT", L"DEFAULT", 7);

        if(colon != nullptr || equal != nullptr || default_ != nullptr)
		{
			Token *exp = GetNextToken();

			// Parse initialization expression
			ParseExpression(exp);

			// In MySQL use DEFAULT keyword
            if(equal != nullptr)
			{
				Token::Change(equal, " DEFAULT ", L" DEFAULT ", 9);
				Token::Remove(colon);
			}
		}

		// ; after each declaration
		Token *semi = GetNextCharToken(';', L';');

        last_declare_var = Nvl(semi, GetLastToken());

        // Check if we are in the outer BEGIN block
        if(_spl_begin_blocks.GetCount() == 0)
		{
            _spl_last_outer_declare_var = last_declare_var;
			_spl_last_outer_declare_varname = name;
		}

        if(semi == nullptr)
			break;
	}

	// Generate variables for cursor parameters 
    if(exists == true && _spl_cursor_params.GetCount() > 0)
	{
        for(ListwmItem *i = _spl_cursor_params.GetFirst(); i != nullptr; i = i->next)
		{
			Token *cursor = (Token*)i->value;
			Token *name = (Token*)i->value2;
			Token *param = (Token*)i->value3;
			Token *data_type = (Token*)i->value4;

			Prepend(cursor, "DECLARE ", L"DECLARE ", 8);

			// The target value of parameter already contains variable name
			PrependCopy(cursor, param);
			Prepend(cursor, " ", L" ", 1);
			
			Token *last_added = PrependCopy(cursor, data_type);

			// The lengths for some cursor variables can be set later
			_spl_cursor_vars.Add(name, param, last_added);

            if(i->next != nullptr)
				Prepend(cursor, ";\n", L";\n", 2);
			else
				Prepend(cursor, ";\n\n", L";\n\n", 3);
		}
	}

    // Cursors must be after all variables in MySQL and MariaDB, so move them if required
    if(exists == true && last_declare_var != nullptr &&
        cursors.GetCount() > 0)
	{
        for(ListwmItem *i = cursors.GetFirst(); i != nullptr; i = i->next)
		{
            Token *cursor = (Token*)i->value;
			Token *semi = (Token*)i->value2;

            // Cursor goes before the last variable
            if(cursor->remain_size > last_declare_var->remain_size)
            {
                AppendNewlineCopy(last_declare_var, cursor, semi, 2, false);
                Token::Remove(cursor, semi);
            }
        }
    }

	_declare_format = declare;
	_spl_last_declare = GetLastToken();

	return exists;
}

// CURSOR cur(params) IS SELECT definition
bool SqlParser::ParseOracleCursorDeclaration(Token *cursor, ListWM *cursors)
{
    if(cursor == nullptr)
		return false;

	if(cursor->Compare("CURSOR", L"CURSOR", 6) == false)
		return false;

	Token *name = GetNextIdentToken();

    if(name == nullptr)
		return false;

	_spl_declared_cursors.Add(name);
	_spl_current_declaring_cursor = name;
	_spl_current_declaring_cursor_uses_vars = false;

	// Optional cursor parameters
	Token *open = GetNextCharToken('(', L'(');
    Token *close = nullptr;

	bool params_exist = false;

    if(open != nullptr)
	{
		// Cursor parameters are used as local variable in the following SELECT
		EnterLocalVariablesBlock();

		params_exist = true;

		while(true)
		{
			// Parameter name
			Token *param = GetNextIdentToken();

            if(param == nullptr)
				break;

			// Optional IN keyword
			/*Token *in */ (void) GetNextWordToken("IN", L"IN", 2);

			Token *data_type = GetNextToken();

			// Check and resolve Oracle %TYPE variable
			bool typed = ParseTypedVariable(param, data_type);

			// Get the parameter data type
			if(!typed)
				ParseDataType(data_type, SQL_SCOPE_CURSOR_PARAMS);

			// SQL Server does support parameters, and cursor get all variable values during declaration,
			// not during OPEN cursor, so DECLARE is moved before OPEN 
			// MySQL also does not support cursor parameters

			// Generate variable not substitute with actual values (especially if they are constants)
			// to increase chance that SQL execution plan is re-used

			// Define variable name to use instead of parameter 
			TokenStr str;

			str.Append(name);
			str.Append("_", L"_", 1);
			str.Append(param);
				
			// Save variable name
			Token::ChangeNoFormat(param, str);
			Token::Remove(param, data_type);

			// Save information about cursor parameters (variables will be generated for MySQL i.e.)
			_spl_cursor_params.Add(cursor, name, param, data_type);

			_spl_variables.Add(param);

			// Next must be comma if list continues
			Token *comma = GetNextCharToken(',', L',');

            if(comma == nullptr)
				break;

			Token::Remove(comma);
		}

		close = GetNextCharToken(')', L')');

		Token::Remove(open);
		Token::Remove(close);
	}

	Token *is = GetNextWordToken("IS", L"IS", 2);

	// FOR in SQL Server, MySQL
	Token::Change(is, "FOR", L"FOR", 3);

	// For SQL Server, MySQL change to DECLARE name CURSOR
	Token::Change(cursor, "DECLARE", L"DECLARE", 7);
	Append(name, " CURSOR", L"CURSOR", 7, cursor); 
	

	// SELECT can be optionally enclosed with ()
	Token *open_sel = TOKEN_GETNEXT('(');
	//Token *select = GetNextSelectStartKeyword();

    //if(select != nullptr)
	//{
    //	ParseSelectStatement(select, 0, SQL_SEL_CURSOR, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);
	//
	//	_spl_declared_cursors_select.Add(name, select, GetLastToken());
	//}

	/*Token *close_sel */ TOKEN_GETNEXTP(open_sel, ')');

	// ; after each declaration
	Token *semi = GetNextCharToken(';', L';');

    // Save cursor definitions for the current declaration block
    if(cursors != nullptr)
        cursors->Add(cursor, GetLastToken(semi));


	// End of scope for cursor parameters
	if(params_exist == true)
		LeaveLocalVariablesBlock();

    _spl_current_declaring_cursor = nullptr;

	return true;
}

// Oracle object type declaration
bool SqlParser::ParseOracleObjectType(Token *type)
{
    if(type == nullptr)
		return false;

	if(!TOKEN_CMP(type, "TYPE"))
		return false;

	Token *name = GetNextToken();

    if(name == nullptr)
		return false;

	Token *is = TOKEN_GETNEXTW("IS");
	Token *table = TOKEN_GETNEXTWP(is, "TABLE");
	Token *of = TOKEN_GETNEXTWP(table, "OF");

	// TYPE typeTab IS TABLE OF data_type INDEX BY BINARY_INTEGER;
    if(table != nullptr && of != nullptr)
	{
		Token *data_type = GetNextToken();
		ParseDataType(data_type, SQL_SCOPE_OBJ_TYPE_DECL);

		_spl_obj_type_table.Add(name, data_type);

		Token *index = TOKEN_GETNEXTW("INDEX");
		Token *by = TOKEN_GETNEXTWP(index, "BY");
        /*Token *binary_integer */(void) TOKEN_GETNEXTWP(by, "BINARY_INTEGER");

        /*Token *semi */(void) TOKEN_GETNEXT(';');

	}

	return true;
}

// Oracle object type assignment statement - tab_type_var(index) := expression
bool SqlParser::ParseOracleObjectTypeAssignment(Token *name)
{
    if(name == nullptr)
		return false;

	Token *var = GetVariableOrParameter(name);

    if(var == nullptr || var->data_subtype != TOKEN_DT2_UDT_TAB_SCALAR)
		return false;

	Token *open = TOKEN_GETNEXT('(');
	Token *index = GetNextToken(open);

    if(index == nullptr)
		return false;

	Token *close = TOKEN_GETNEXT(')');
	Token *colon = TOKEN_GETNEXTP(close, ':');
	Token *equal = TOKEN_GETNEXTP(colon, '=');

    if(equal == nullptr)
		return false;

	Token *exp = GetNext();
	ParseExpression(exp);
    /*Token *exp_end */(void) GetLastToken();

	return true;
}

// Oracle PRAGMA clause
bool SqlParser::ParseOraclePragma(Token *pragma)
{
    if(pragma == nullptr)
		return false;

	if(!TOKEN_CMP(pragma, "PRAGMA"))
		return false;

	Token *name = GetNextToken();

    if(name == nullptr)
		return false;

	Token *semi = TOKEN_GETNEXT(';');

	Token::Remove(pragma, semi);

	return true;
}

// Forward declaration for exception
bool SqlParser::ParseOracleException(Token *name)
{
    if(name == nullptr)
		return false;

	Token *exception = TOKEN_GETNEXTW("EXCEPTION");

    if(exception == nullptr)
		return false;

	Token *semi = TOKEN_GETNEXT(';');

	Token::Remove(name, semi);

	return true;
}

// Define actual sizes for parameters, cursor parameters and records
void SqlParser::OracleAppendDataTypeSizes()
{
	// Variables generated for cursor parameters
	if(_spl_cursor_vars.GetCount() > 0)
	{
		ListwmItem *item = _spl_cursor_vars.GetFirst();

        while(item != nullptr)
		{
			Token *data_type = (Token*)item->value3;

			// Define the size based on the data type
			OracleAppendDataTypeSize(data_type);

			item = item->next;
		}
	}
}

// Append size for the specified data type token
void SqlParser::OracleAppendDataTypeSize(Token *data_type)
{
    if(data_type == nullptr)
		return;

	// VARCHAR2
	if(data_type->Compare("VARCHAR2", L"VARCHAR2", 8) == true)
		AppendFirstNoFormat(data_type, "(4000)", L"(4000)", 6);
}

void SqlParser::OracleRemoveDataTypeSize(Token *data_type)
{
    if(data_type == nullptr)
		return;

	Token *cur = Token::SkipSpaces(data_type->next);

	// Datatype without size specifier; target (size) can be added when INT converted to NUMBER(10) i.e.
	if(Token::Compare(cur, '(', L'(') == false && Token::CompareTarget(cur, "(", L"(", 1) == false)
		return;

	// Remove all tokens until ) inclusive
    while(cur != nullptr)
	{
		Token::Remove(cur);

		if(Token::Compare(cur, ')', L')') == true || Token::CompareTarget(cur, ")", L")", 1) == true)
			break;

		cur = cur->next;
	}
}

// Check for (+) join condition
bool SqlParser::ParseOracleOuterJoin(Token *exp_start, Token * /*column*/)
{
	bool exists = false;
	Token *open = TOKEN_GETNEXT('(');

    if(open != nullptr)
	{
		Token *plus = TOKEN_GETNEXT('+');

        if(plus != nullptr)
		{
			/*Token *close */ TOKEN_GETNEXT(')');

			STATS_DTL_DECL
			STATS_DTL_DESC(SQL_STMT_SELECT_ORAJOIN)
			STATS_DTL_CONV_ERROR(Target(SQL_MARIADB_ORA), STATS_CONV_MEDIUM, "", "")
			SELECT_DTL_STATS_V("Outer join (+)", exp_start)

			exists = true;
		}
		else
			PushBack(open);
	}

	return exists;
}

// Oracle 'rownum = <= < num' condition in WHERE clause 
bool SqlParser::ParseOracleRownumCondition(Token *first, Token *op, Token *second, int *rowlimit)
{
    if(first == nullptr || op == nullptr || second == nullptr)
		return false;

	// rownum can be at any side of the expression
	bool rownum1 = first->Compare("rownum", L"rownum", 6);
    bool rownum2 = (first == nullptr) ? second->Compare("rownum", L"rownum", 6) : false;

	if(rownum1 == false && rownum2 == false)
		return false;

	int limit = -1;

	// Get row limit value
	if(rownum1 == true)
		limit = second->GetInt();
	else
		limit = first->GetInt();

	// If target not Oracle remove condition
	Token::Remove(first, second);

    if(rowlimit != nullptr)
		*rowlimit = limit;

	return true;
}

// Get Oracle format string for specified string literal containing date or timestamp value
bool SqlParser::RecognizeOracleDateFormat(Token *str, TokenStr &format)
{
    if(str == nullptr || str->type != TOKEN_STRING || str->len == 0)
		return false;

	bool exists = false;

	// YYYYMMDD format
	if(str->len == 10)
	{
		format.Append("'YYYYMMDD'", L"'YYYYMMDD'", 10);
		exists = true;
	}
	else
	// Full DATE with 2 delimiters
	if(str->len == 12)
	{
		// 'xx/xx/yyyy', 'xx-xx-yyyy', 'xx.xx.yyyy'
		if((Token::Compare(str, '/', L'/', 3) == true && Token::Compare(str, '/', L'/', 6) == true) ||
			(Token::Compare(str, '-', L'-', 3) == true && Token::Compare(str, '-', L'-', 6) == true) ||
			(Token::Compare(str, '.', L'.', 3) == true && Token::Compare(str, '.', L'.', 6) == true))
		{
			// Month or day can be at the first position
			int n1 = str->GetInt(1, 2);
			int n2 = str->GetInt(4, 2);

			if(n1 != -1 && n2 != -1)
			{
				bool mdy = false;
				bool dmy = false;

				// Day and month are equal in string literal
				if(n1 == n2)
				{
					// If delimiter is / use US notation by default MM/DD/YYYY
					if(Token::Compare(str, '/', L'/', 3) == true)
						mdy = true;
					else
						dmy = true;
				}

				// Month, day, year
				if(mdy == true)
				{
					if(Token::Compare(str, '/', L'/', 3) == true)
						format.Append("'MM/DD/YYYY'", L"'MM/DD/YYYY'", 12);
					else
					if(Token::Compare(str, '-', L'-', 3) == true)
						format.Append("'MM-DD-YYYY'", L"'MM-DD-YYYY'", 12);
					else
					if(Token::Compare(str, '.', L'.', 3) == true)
						format.Append("'MM.DD.YYYY'", L"'MM.DD.YYYY'", 12);
				
					exists = true;
				}
			}
		}
	}
	else
	// ISO format YYYY-MM-DD HH24:MI:SS.FF 
	if(str->len >= 19)
	{
		// Month < 12
		int month = str->GetInt(6, 2);

		// Check delimiters in 'YYYY-MM-DD HH:MI:SS.FF'
		if(Token::Compare(str, '-', L'-', 5) == true && Token::Compare(str, '-', L'-', 8) == true &&
			Token::Compare(str, ':', L':', 14) == true && Token::Compare(str, ':', L':', 17) == true &&
			month <= 12)
		{
			format.Append("'YYYY-MM-DD HH24:MI:SS", L"'YYYY-MM-DD HH24:MI:SS", 22);

			// Check if fraction is set, FF recognizes any number of digits
			if(str->len > 19)
				format.Append(".FF", L".FF", 3);

			format.Append("'", L"'", 1);
		
			exists = true;
		}		
	}

	return exists;
}

// Add sequence and trigger to emulate identity column
void SqlParser::OracleEmulateIdentity(Token *create, Token *table, Token *column, Token *last, 
										Token *id_start, Token *id_inc, bool id_default)
{
    if(last == nullptr)
		return;

	TokenStr seq_name(table);
	TokenStr tr_name(table);

	AppendIdentifier(seq_name, "_seq", L"_seq", 4);
	AppendIdentifier(tr_name, "_seq_tr", L"_seq_tr", 7);
		
	// Generate CREATE SEQUENCE after CREATE TABLE
	Append(last, "\n\n", L"\n\n", 2);
	AppendNoFormat(last, "-- Generate ID using sequence and trigger", L"-- Generate ID using sequence and trigger", 41);
	Append(last, "\n", L"\n", 1);

	Append(last, "CREATE SEQUENCE ", L"CREATE SEQUENCE ", 16, create);
	AppendNoFormat(last, &seq_name);
	Append(last, " START WITH ", L" START WITH ", 12, create);

    if(id_start != nullptr)
		AppendCopy(last, id_start);
	else
		AppendNoFormat(last, "1", L"1", 1);

	Append(last, " INCREMENT BY ", L" INCREMENT BY ", 14, create);

    if(id_inc != nullptr)
		AppendCopy(last, id_inc);
	else
		AppendNoFormat(last, "1", L"1", 1);

	Append(last, ";\n\n", L";\n\n", 3);

	Append(last, "CREATE OR REPLACE TRIGGER ", L"CREATE OR REPLACE TRIGGER ", 26, create);
	AppendNoFormat(last, &tr_name);
	Append(last, "\n BEFORE INSERT ON ", L"\n BEFORE INSERT ON ", 19, create);
	AppendCopy(last, table);
	Append(last, " FOR EACH ROW", L" FOR EACH ROW", 13, create);

	// GENERATED ALWAYS or BY DEFAULT can be used in DB2
	if(id_default == true)
	{
		Append(last, "\n WHEN (NEW.", L"\n  WHEN (NEW.", 12, create);
		AppendCopy(last, column);
		Append(last, " IS NULL)", L" IS NULL)", 9, create);
	}
	
	Append(last, "\nBEGIN\n SELECT ", L"\nBEGIN\n SELECT ", 15, create);
	AppendNoFormat(last, &seq_name);
	Append(last, ".NEXTVAL INTO :NEW.", L".NEXTVAL INTO :NEW.", 19, create);
	AppendCopy(last, column);
	Append(last, " FROM dual;\nEND;\n/", L" FROM dual;\nEND;\n/", 18, create);
}


// Implement CONTINUE handler for NOT FOUND after FETCH statement in Oracle
void SqlParser::OracleContinueHandlerForFetch(Token *fetch, Token *cursor)
{
	CopyPaste *cur = _copypaste.GetFirstNoCurrent();

    Token *end = nullptr;
	bool exists = false;

	// Find CONTINUE handler for NOT FOUND
    while(cur != nullptr)
	{
		bool found = false;

		if(cur->type == COPY_CONTINUE_HANDLER_NOT_FOUND)
			found = true;

		if(found == false)
		{
			cur = cur->next;
			continue;
		}

		// Define the end of FETCH statement
		if(exists == false)
		{
			end = GetLastToken(GetNextCharToken(';', L';'));
			exists = true;
		}

		Append(end, "\nIF ", L"\nIF ", 4, fetch);
		AppendCopy(end, cursor);
		Append(end, "%NOTFOUND THEN", L"%NOTFOUND THEN", 14, fetch);

		Token *token = cur->tokens.GetFirstNoCurrent();

		// Copy tokens
        while(token != nullptr)
		{
			AppendCopy(end, token);
			token = token->next;
		}

		Append(end, "\nEND IF;", L"\nEND IF;", 8, fetch);

		cur = cur->next;
	}
}

// Implement CONTINUE handler for NOT FOUND and SQLEXCEPTION after SELECT INTO in Oracle
void SqlParser::OracleContinueHandlerForSelectInto(Token *select)
{
	CopyPaste *cur = _copypaste.GetFirstNoCurrent();

    Token *end = nullptr;
	bool exists = false;

	// Find CONTINUE handler for NOT FOUND
    while(cur != nullptr)
	{
		if(cur->type != COPY_CONTINUE_HANDLER_NOT_FOUND && cur->type != COPY_CONTINUE_HANDLER_SQLEXCEPTION)
		{
			cur = cur->next;
			continue;
		}

		// Define the end of SELECT INTO statement
		if(exists == false)
		{
			end = GetLastToken(GetNextCharToken(';', L';'));
			exists = true;
		}

		Prepend(select, "BEGIN\n", L"BEGIN\n", 6);
		Append(end, "\nEXCEPTION WHEN ", L"\nEXCEPTION WHEN ", 16, select);
		
		if(cur->type == COPY_CONTINUE_HANDLER_NOT_FOUND)
			Append(end, "NO_DATA_FOUND THEN ", L"NO_DATA_FOUND THEN ", 19, select);
		else
			Append(end, "OTHERS THEN ", L"OTHERS THEN ", 12, select);

		Token *token = cur->tokens.GetFirstNoCurrent();

		// Copy tokens
        while(token != nullptr)
		{
			AppendCopy(end, token);
			token = token->next;
		}

		Append(end, "\nEND;", L"\nEND;", 5, select);

		cur = cur->next;
	}
}

// Implement CONTINUE handler after INSERT in Oracle
void SqlParser::OracleContinueHandlerForInsert(Token *insert)
{
	CopyPaste *cur = _copypaste.GetFirstNoCurrent();

    Token *end = nullptr;
	bool exists = false;

	// Find user-defined handler 
    while(cur != nullptr)
	{
        if(cur->type != COPY_CONTINUE_HANDLER_USER_DEFINED || cur->name == nullptr)
		{
			cur = cur->next;
			continue;
		}

        Token *error = nullptr;

		// Find native error code by condition name
		ListwmItem *i = Find(_spl_user_exceptions, cur->name);

        if(i != nullptr)
			error = (Token*)i->value2;

		// No error code found
        if(error == nullptr)
		{
			cur = cur->next;
			continue;
		}

		// Define the end of INSERT statement
		if(exists == false)
		{
			end = GetLastToken(GetNextCharToken(';', L';'));
			exists = true;
		}

		Prepend(insert, "BEGIN\n", L"BEGIN\n", 6);
		Append(end, "\nEXCEPTION WHEN ", L"\nEXCEPTION WHEN ", 16, insert);

		Token *token = cur->tokens.GetFirstNoCurrent();

		// Copy tokens
        while(token != nullptr)
		{
			AppendCopy(end, token);
			token = token->next;
		}

		Append(end, "\nEND;", L"\nEND;", 5, insert);

		cur = cur->next;
	}
}

// Implement CONTINUE handler for NOT FOUND after UPDATE statement in Oracle
void SqlParser::OracleContinueHandlerForUpdate(Token *update)
{
	CopyPaste *cur = _copypaste.GetFirstNoCurrent();

    Token *end = nullptr;
	bool exists = false;

	// Find CONTINUE handler for NOT FOUND
    while(cur != nullptr)
	{
		if(cur->type != COPY_CONTINUE_HANDLER_NOT_FOUND)
		{
			cur = cur->next;
			continue;
		}

		// Define the end of UPDATE statement
		if(exists == false)
		{
			end = GetLastToken(GetNextCharToken(';', L';'));
			exists = true;
		}

		Append(end, "\nIF SQL%ROWCOUNT = 0 THEN ", L"\nIF SQL%ROWCOUNT = 0 THEN ", 26, update);

		Token *token = cur->tokens.GetFirstNoCurrent();

		// Copy tokens
        while(token != nullptr)
		{
			AppendCopy(end, token);
			token = token->next;
		}

		Append(end, "\nEND IF;", L"\nEND IF;", 8, update);

		cur = cur->next;
	}
}

// If there declarations in the body move BEGIN after last DECLARE (used for PostgreSQL as well)
void SqlParser::OracleMoveBeginAfterDeclare(Token *create, Token *as, Token *begin, Token *body_start)
{
	// No declaration statements
    if(_spl_last_declare == nullptr)
	{
		// Oracle requires IS keyword
        if(as == nullptr)
		{
            if(begin != nullptr)
				Prepend(begin, "IS\n", L"IS\n", 3);
			else
				Append(body_start, " IS ", L" IS ", 4, create);
		}

		// No BEGIN in source, but it is required for Oracle
        if(begin == nullptr)
			Append(body_start, "\nBEGIN", L"\nBEGIN", 6, create);
	}
	// There are declarations
	else
	{
		Token *append = GetDeclarationAppend();

        if(append != nullptr)
		{
            if(append->next != nullptr && append->next->IsBlank() == true)
				Append(append, "\nBEGIN", L"\nBEGIN", 6, Nvl(begin, create));
			else
            if(append->prev != nullptr && append->prev->IsBlank() == true)
				Append(append, "BEGIN\n", L"BEGIN\n", 6, Nvl(begin, create));
			else
				Append(append, "\nBEGIN\n", L"\nBEGIN\n", 7, Nvl(begin, create));
		}

		// Oracle requires IS keyword
        if(as == nullptr)
		{
            if(begin != nullptr)
				Token::Change(begin, "IS", L"IS", 2);
			else
				Append(body_start, " IS ", L" IS ", 4, create);
		}
		// Remove BEGIN in the original position
		else
			Token::Remove(begin);
	}
}

// Add cursor parameters if the procedure returns a result set
void SqlParser::OracleAddOutRefcursor(Token *create, Token *name, Token *last_param)
{
	// No result sets
	if(_spl_result_sets == 0)
		return;

	CopyPaste *cur = _copypaste.GetFirstNoCurrent();

	int num = 0;

    Token *append = nullptr;

	// Procedure has parameters
    if(last_param != nullptr)
	{
		AppendNoFormat(last_param, ", ", L", ", 2);
		append = last_param;
	}
	else
	{
		AppendNoFormat(name, " (", L" (", 2);
		append = name;
	}

	// Find cursor names
    while(cur != nullptr)
	{
		if(cur->type != COPY_CURSOR_WITH_RETURN)
		{
			cur = cur->next;
			continue;
		}

		if(num > 0)
			AppendNoFormat(append, ", ", L", ", 2);

		AppendCopy(append, cur->name);
		Append(append, " OUT SYS_REFCURSOR", L" OUT SYS_REFCURSOR", 18, create);

		cur = cur->next;
		num++;
	}

	// Find generated cursor names for standalone SELECT statements returning a result set
    for(TokenStr *t = _spl_result_set_generated_cursors.GetFirst(); t != nullptr; t = t->next)
	{
		if(num > 0)
			AppendNoFormat(append, ", ", L", ", 2);

		AppendNoFormat(append, t);
		Append(append, " OUT SYS_REFCURSOR", L" OUT SYS_REFCURSOR", 18, create);

		num++;
	}

	// Find cursor name for prepared statements with return
    for(ListwmItem *i = _spl_prepared_stmts_cursors_with_return.GetFirst(); i != nullptr; i = i->next)
	{
		Token *cursor = (Token*)i->value;

		if(num > 0)
			AppendNoFormat(append, ", ", L", ", 2);

		AppendCopy(append, cursor);
		Append(append, " OUT SYS_REFCURSOR", L" OUT SYS_REFCURSOR", 18, create);

		num++;
	}

    if(last_param == nullptr && num > 0)
		AppendNoFormat(append, ")", L")", 1);
}

// Check if temporary tables need to be truncated at the beginning of block in Oracle
void SqlParser::OracleTruncateTemporaryTablesWithReplace()
{
 //   for(ListwItem *i = _spl_declared_tables_with_replace.GetFirst(); i != nullptr; i = i->next)
	//{
	//	Token *table_name = (Token*)i->value;

 //       if(i->prev == nullptr)
	//		Prepend(_spl_first_non_declare, "\n", L"\n", 1);

	//	Prepend(_spl_first_non_declare, "EXECUTE IMMEDIATE 'TRUNCATE TABLE ", L"EXECUTE IMMEDIATE 'TRUNCATE TABLE ", 34);
	//	PrependCopy(_spl_first_non_declare, table_name);
	//	Prepend(_spl_first_non_declare, "';\n", L"';\n", 3);

 //       if(i->next == nullptr)
	//		Prepend(_spl_first_non_declare, "\n", L"\n", 1);
	//}
}

// Move SELECT statement inside IF to a standalone SELECT INTO before IF
void SqlParser::OracleMoveSelectOutofIf(Token *stmt_start, Token *select, Token *select_list_end, 
										Token *open, Token *close)
{
    if(stmt_start == nullptr || open == nullptr || close == nullptr)
		return;

	TokenStr var("v_cnt", L"v_cnt", 5);

	// ( and ) are not required in the standalone SELECT INTO
	Token::ChangeNoFormat(open, var);
	Token::Change(close, ";", L";", 1);

	Append(select_list_end, " INTO ", L" INTO ", 6, select);
	AppendNoFormat(select_list_end, &var);

	PrependCopy(stmt_start, open->next, close, false);
	Prepend(stmt_start, "\n\n", L"\n\n", 2);

	// Remove SELECT in IF
	Token::Remove(open->next, close);

	_spl_moved_if_select++;
}

// Add declarations for SELECT statement moved out of IF condition
void SqlParser::OracleIfSelectDeclarations()
{
	if(_spl_moved_if_select == 0)
		return;

	//Token *append = GetDeclarationAppend();

	//Append(append, "v_cnt NUMBER(10);", L"v_cnt NUMBER(10);", 17); 
}

// Change = to DEFAULT keyword in procedure parameters 
void SqlParser::OracleChangeEqualToDefault(Token *equal)
{
    if(equal == nullptr)
		return;

	bool space_before = false;
	bool space_after = false;

	// INT=0 without spaces is possible, so check if we need to add spaces arounf DEFAULT keyword
    if(equal->prev != nullptr && equal->prev->IsBlank() == true)
		space_before = true;

    if(equal->next != nullptr && equal->next->IsBlank() == true)
		space_after = true;

	if(space_before == true && space_after == true)
		Token::Change(equal, "DEFAULT", L"DEFAULT", 7);
	else
	if(space_before == true && space_after == false)
		Token::Change(equal, "DEFAULT ", L"DEFAULT ", 8);
	else
	if(space_before == false && space_after == true)
		Token::Change(equal, " DEFAULT", L" DEFAULT", 8);
	else
		Token::Change(equal, " DEFAULT ", L" DEFAULT ", 9);
}

// Oracle SQL*Plus SET options, SET DEFINE ON/OFF i.e
bool SqlParser::ParseOracleSetOptions(Token *set)
{
    if(set == nullptr)
		return false;

	bool exists = false;

	bool comment = false;
	bool remove = false;

	Token *option = GetNextToken();

    if(option == nullptr)
		return false;

	// SET DEFINE ON | OFF
	if(option->Compare("DEFINE", L"DEFINE", 6) == true)
	{
		Token *on = GetNext("ON", L"ON", 2);
        Token *off = nullptr;

        if(on == nullptr)
			off = GetNext("OFF", L"OFF", 3);

        if(on != nullptr || off != nullptr)
		{
			comment = true;

			exists = true;
		}
		else
			PushBack(option);
	}
	// Not a SET option
	else
		PushBack(option);

	if(exists == true)
	{
		Token *last = Nvl(GetNextCharToken(';', L';'), GetLastToken());

		if(comment == true)
			Comment(set, last);
		else
		if(remove == true)
			Token::Remove(set, last);
	}

	return exists;
}
