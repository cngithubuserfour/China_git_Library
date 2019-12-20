// SQLParser for statements

#include <stdio.h>
#include <string.h>
#include "sqlparser.h"

bool SqlParser::ParseStatement(Token *token, int scope, int *result_sets)
{
    if(token == nullptr)
        return false;

    bool exists = false;
    bool proc = false;

    // ALTER statement
    if(token->Compare("ALTER", L"ALTER", 5) == true)
        exists = ParseAlterStatement(token, result_sets, &proc);
    else
    // COMMENT
    if(token->Compare("COMMENT", L"COMMENT", 7) == true)
        exists = ParseCommentStatement(token);
    else
    // CREATE statement
    if(token->Compare("CREATE", L"CREATE", 6) == true)
        exists = ParseCreateStatement(token, result_sets, &proc);
    else
    // DROP statement
    if(token->Compare("DROP", L"DROP", 4) == true)
        exists = ParseDropStatement(token);
    else
    // INSERT statement
    if(token->Compare("INSERT", L"INSERT", 6) == true)
        exists = ParseInsertStatement(token);


    if(!exists)
        return false;

    // Optional delimiter at the end of the statement
    /*Token *semi */ (void) GetNextCharToken(';', L';');

    return exists;
}

// CREATE statement
bool SqlParser::ParseCreateStatement(Token *create, int *result_sets, bool *proc)
{
    if(create == nullptr)
        return false;

    Token *next = GetNextToken();

    if(next == nullptr)
        return false;

    Token *or_ = nullptr;
    Token *replace = nullptr;
    Token *unique = nullptr;
    Token *materialized = nullptr;

    int prev_object_scope = _obj_scope;

    _obj_scope = 0;

    // OR REPLACE clause in Oracle
    if(next->Compare("OR", L"OR", 2) == true)
    {
        or_ = next;
        replace = GetNextWordToken("REPLACE", L"REPLACE", 7);

        next = GetNextToken();
    }
    else
    // UNIQUE for CREATE INDEX
    if(next->Compare("UNIQUE", L"UNIQUE", 6) == true)
    {
        unique = next;
        next = GetNextToken();
    }
    else
    // GLOBAL TEMPORARY is Oracle
    if(next->Compare("GLOBAL", L"GLOBAL", 6) == true)
    {
        /*Token *temp */ (void) GetNextWordToken("TEMPORARY", L"TEMPORARY", 9);
        next = GetNextToken();

        _obj_scope = SQL_SCOPE_TEMP_TABLE;
    }
    else
    // PUBLIC SYNONYM in Oracle
    if(Token::Compare(next, "PUBLIC", L"PUBLIC", 6) == true)
    {
        next = GetNextToken();
    }

    // FORCE VIEW in Oracle can follow OR REPLACE
    if(Token::Compare(next, "FORCE", L"FORCE", 5) == true)
    {
        Token *force = next;

        next = GetNextToken();

        Token::Remove(force);
    }
    else
    // EDITIONABLE or NONEDITIONABLE for Oracle packages
    if(TOKEN_CMP(next, "EDITIONABLE") || TOKEN_CMP(next, "NONEDITIONABLE"))
    {
        Token *edit = next;

        next = GetNextToken();

        Token::Remove(edit);
    }
    else
    // CREATE MATERIALIZED VIEW in Oracle
    if(TOKEN_CMP(next, "MATERIALIZED"))
    {
        materialized = next;
        next = GetNextToken();
    }

    if(next == nullptr)
        return false;

    bool exists = false;

    // CREATE INDEX
    if(next->Compare("INDEX", L"INDEX", 5) == true)
    {
        _obj_scope = SQL_SCOPE_INDEX;
        exists = ParseCreateIndex(create, unique, next);
    }
    else
    // CREATE TABLE
    if(next->Compare("TABLE", L"TABLE", 5) == true)
    {
        // Scope can be already set to TEMP table i.e.
        if(_obj_scope == 0)
            _obj_scope = SQL_SCOPE_TABLE;

        exists = ParseCreateTable(create, next);
    }
    else
    // CREATE TABLESPACE in Oracle
    if(next->Compare("TABLESPACE", L"TABLESPACE", 10) == true)
        exists = ParseCreateTablespace(create, next);
    else
    // CREATE TRIGGER
    if(next->Compare("TRIGGER", L"TRIGGER", 7) == true)
        exists = ParseCreateTrigger(create, or_, next);
    else
    // CREATE TYPE
    if(next->Compare("TYPE", L"TYPE", 4) == true)
        exists = ParseCreateType(create, next);
    else
    // CREATE VIEW
    if(next->Compare("VIEW", L"VIEW", 4) == true)
        exists = ParseCreateView(create, materialized, next);

    _obj_scope = prev_object_scope;

    return exists;
}

// ALTER statement
bool SqlParser::ParseAlterStatement(Token *alter, int *result_sets, bool *proc)
{
    if(alter == nullptr)
        return false;

    Token *next = GetNextToken();

    if(next == nullptr)
        return false;

    bool exists = false;

    // ALTER TABLE
    if(next->Compare("TABLE", L"TABLE", 5) == true)
    {
        exists = ParseAlterTableStatement(alter, next);
    }
    else
    // ALTER INDEX
    if(next->Compare("INDEX", L"INDEX", 5) == true)
    {
        exists = ParseAlterIndexStatement(alter, next);
    }

    return exists;
}

// ALTER TABLE statement
bool SqlParser::ParseAlterTableStatement(Token *alter, Token *table)
{
    STATS_DECL
    STMS_STATS_V("ALTER TABLE", alter)
    ALTER_TAB_STMS_STATS("ALTER TABLE statements")

    // Get table name
    Token *table_name = GetNextIdentToken();

    if(table_name == nullptr)
        return false;

    Token *next = GetNextToken();

    if(next == nullptr)
        return false;

    int prev_stmt_scope = _stmt_scope;
    _stmt_scope = SQL_STMT_ALTER_TABLE;

    bool gp_comment = false;

    ListW pkcols;

    // ADD constraint
    if(next->Compare("ADD", L"ADD", 3) == true)
    {
        // Try to parse a standalone column constraint
        ParseStandaloneColumnConstraints(alter, pkcols, nullptr);

        gp_comment = true;
    }
    else
    //DROP PRIMARY KEY CASCADE
    if(next->Compare("DROP",L"DROP",4)==true)
    {
        (void)GetNextWordToken("PRIMARY",L"PRIMARY",7);
        (void)GetNextWordToken("KEY", L"KEY", 3);
        Token *cascade = GetNextWordToken("CASCADE", L"CASCADE", 7);

        if (cascade != nullptr) {
            Token::Remove(cascade);
        }
    }


    _stmt_scope = prev_stmt_scope;

    return true;
}

// ALTER INDEX statement
bool SqlParser::ParseAlterIndexStatement(Token *alter, Token * /*index*/)
{
    STATS_DECL
    STMS_STATS_V("ALTER INDEX", alter)

    int options = 0;
    int removed = 0;

    // Index name
    /*Token *name */ (void) GetNextIdentToken();

    while(true)
    {
        Token *next = GetNextToken();

        if(next == nullptr)
            break;

        // Oracle NOPARALLEL
        if(next->Compare("NOPARALLEL", L"NOPARALLEL", 10) == true)
        {
            options++;

            // Mark the option to be removed
            removed++;

            continue;
        }
        else
        // Oracle PARALLEL num
        if(next->Compare("PARALLEL", L"PARALLEL", 8) == true)
        {
            /*Token *num */ (void) GetNextToken();

            options++;

            // Mark the option to be removed
            removed++;

            continue;
        }

        // Unknown option
        PushBack(next);
        break;
    }

    // If there are options and they are all need to removed comment the entire statement
    if(options > 0 && options == removed)
    {
        // Check for closing ;
        Token *semi = GetNextCharToken(';', L';');

        Token *last = (semi != nullptr) ? semi : GetLastToken();

        // Comment the entire ALTER INDEX statement
        Comment(alter, last);
    }

    return true;
}

// Oracle COMMENT statement
bool SqlParser::ParseCommentStatement(Token *comment)
{
    STATS_DECL
    STATS_DTL_DECL

    if(comment == nullptr)
        return false;

    // ON keyword
    Token *on = GetNextWordToken("ON", L"ON", 2);

    if(on == nullptr)
        return false;

    bool index = false;

    // Comment type (TABLE, COLUMN, INDEX i.e.)
    Token *type = GetNextToken();

    if(type == nullptr)
        return false;

    // Object name
    Token *name = GetNextIdentToken();

    Token *is = GetNextWordToken("IS", L"IS", 2);
    Token *text = GetNextToken();

    // COMMENT ON TABLE
    if(type->Compare("TABLE", L"TABLE", 5) == true)
    {
        STMS_STATS_V("COMMENT ON TABLE", comment)

        // ALTER TABLE name COMMENT text in MySQL
        Token::Change(comment, "ALTER", L"ALTER", 5);
        Token::Remove(on);
        Token::Change(is, "COMMENT", L"COMMENT", 7);
    }
    else
    // COMMENT ON COLUMN
    if(type->Compare("COLUMN", L"COLUMN", 6) == true)
    {
        STATS_SET_DESC(SQL_STMT_COMMENT_ON_COLUMN_DESC)
        STATS_DTL_CONV_ERROR(Target(SQL_MARIADB_ORA), STATS_CONV_LOW, "", "")

        STATS_UPDATE_STATUS
        STMS_STATS_V("COMMENT ON COLUMN", comment)

        // Find the end of column in CREATE TABLE
        Book *book = GetBookmark2(BOOK_CTC_END, name);

        // Add the comment to CREATE TABLE
        if(book != nullptr)
        {
            Append(book->book, " COMMENT ", L" COMMENT ", 9);
            AppendCopy(book->book, text);
        }

        Token *last = GetNextCharOrLastToken(';', L';');

        // Comment the statement
        Comment("Moved to CREATE TABLE\n", L"Moved to CREATE TABLE\n", 22, comment, last);
    }
    else
    // COMMENT ON INDEX
    if(type->Compare("INDEX", L"INDEX", 5) == true)
        index = true;

    return true;
}

// CREATE TABLE statement
bool SqlParser::ParseCreateTable(Token *create, Token *token)
{
    STATS_DECL
    STMS_STATS_V("CREATE TABLE", create)
    CREATE_TAB_STMS_STATS("CREATE TABLE statements")

    if(token == nullptr)
        return false;

    // Get table name
    Token *table = GetNextIdentToken(SQL_IDENT_OBJECT);

    if(table == nullptr)
        return false;

    // Save bookmark to the start of CREATE TABLE
    Bookmark(BOOK_CT_START, table, create);

    ListW pkcols;
    ListWM inline_indexes;

    // Identity start and increment
    Token *id_col = nullptr;
    Token *id_start = nullptr;
    Token *id_inc = nullptr;
    bool id_default = true;

    Token *last_colname = nullptr;
    Token *last_colend = nullptr;

    // CREATE TABLE AS SELECT
    Token *as = TOKEN_GETNEXTW("AS");

    if(as != nullptr)
    {
        // ( is optional in AS (SELECT ...)
        Token *open = TOKEN_GETNEXT('(');

        if(open != nullptr)
            /*Token *close */ (void) TOKEN_GETNEXT(')');
    }
    else
    {
        // Next token must be (
        if(GetNextCharToken('(', L'(') == nullptr)
            return false;

        ParseCreateTableColumns(create, table, pkcols, &id_col, &id_start, &id_inc, &id_default,
            &inline_indexes, &last_colname);

        last_colend = GetLastToken();

        // Next token must be )
        Token *close = GetNextCharToken(')', L')');

        if(close == nullptr)
            return false;

        // Save bookmark to the end of columns but before storage and other properties
        Book *col_end = Bookmark(BOOK_CTC_ALL_END, table, close);

    }

    // Table comment in MySQL
    Token *comment = nullptr;

    ParseStorageClause();

    /*Token *semi */(void) GetNext(';', L';');

    Token *last = GetLastToken();

    // Save bookmark to the end of CREATE TABLE
    Bookmark(BOOK_CT_END, table, last);

    // Add a separate COMMENT ON TABLE statement
    if(comment != nullptr)
    {
        Append(last, "\n\nCOMMENT ON TABLE ", L"\n\nCOMMENT ON TABLE ", 19, create);
        AppendCopy(last, table);
        Append(last, " IS ", L" IS ", 4, create);
        AppendCopy(last, comment);
    }

    // For Oracle add sequence and trigger to emulate identity column
    if(id_col != nullptr)
    {
        OracleEmulateIdentity(create, table, id_col, last, id_start, id_inc, id_default);
    }

    if(_spl_first_non_declare == nullptr)
        _spl_first_non_declare = create;

    _spl_last_stmt = create;

    return true;
}

// Parse CREATE TABLE column and constraints definitions
bool SqlParser::ParseCreateTableColumns(Token *create, Token *table_name, ListW &pkcols,
                                        Token **id_col, Token **id_start, Token **id_inc, bool *id_default,
                                        ListWM *inline_indexes, Token **last_colname)
{
    bool exists = false;

    while(true)
    {
        // Try to parse a standalone column constraint
        bool cns = ParseStandaloneColumnConstraints(create, pkcols, inline_indexes);

        // Parse a regular column definition
        if(cns == false)
        {
            CREATE_TAB_STMS_STATS("Columns")

            // Column name
            Token *column = GetNextIdentToken(SQL_IDENT_COLUMN_SINGLE);

            if(column == nullptr)
                break;

            // Data type (system or user-defined)
            Token *type = GetNextToken();

            ParseDataType(type);

            if(Token::Compare(type, "SERIAL", L"SERIAL", 0, 6) == true && id_col != nullptr)
                *id_col = column;

            Token *type_end = GetLastToken();

            ParseColumnConstraints(column, type, type_end);

            Token *last =  GetLastToken();

            // Set a bookmark to the column end
            Bookmark(BOOK_CTC_END, table_name, column, last);

            if(last_colname != nullptr)
                *last_colname = column;
        }

        exists = true;

        // If the next token is not comma exit
        if(GetNextCharToken(',', L',') == nullptr)
            break;

        // Most common typo to have comma after last column, exist if next is )
        Token *close = GetNextCharToken(')', L')');

        if(close != nullptr)
        {
            PushBack(close);
            break;
        }
    }

    return exists;
}

// CREATE TABLESPACE
bool SqlParser::ParseCreateTablespace(Token *create, Token *tablespace)
{
    bool exists = false;

    STATS_DECL
    STMS_STATS_V("CREATE TABLESPACE", create)

    return exists;
}

// CREATE INDEX
bool SqlParser::ParseCreateIndex(Token *create, Token *unique, Token *index)
{
    STATS_DECL
    STMS_STATS_V("CREATE INDEX", create)

    if(index == nullptr)
        return false;

    // Get index name
    _option_rems = true; //remove schema name
    Token *name = GetNextIdentToken(SQL_IDENT_OBJECT, SQL_SCOPE_INDEX);
    _option_rems = false;

    if(name == nullptr)
        return false;

    // Save bookmark to the start of CREATE INDEX
    Bookmark(BOOK_CI_START, name, create);

    /*Token *on */ (void) GetNextWordToken("ON", L"ON", 2);

    // Get table name
    Token *table = GetNextIdentToken(SQL_IDENT_OBJECT, SQL_SCOPE_TABLE);

    if(table == nullptr)
        return false;

    // Open (
    /*Token *open */ (void) GetNextCharToken('(', L'(');


    // Handle index columns
    while(true)
    {
        Token *col = GetNextIdentToken();

        if(col == nullptr)
            break;

        // Parse column as expression as it can be a function-based indes
        ParseExpression(col);

        Token *next = GetNextToken();

        if(next == nullptr)
            break;

        // Optional ASC or DESC keyword
        if(next->Compare("ASC", L"ASC", 3) == true || next->Compare("DESC", L"DESC", 4) == true)
            next = GetNextToken();

        // Leave if not comma
        if(Token::Compare(next, ',', L',') == true)
            continue;

        PushBack(next);
        break;
    }

    // Close )
    /*Token *close */ (void) GetNextCharToken(')', L')');

    ParseCreateIndexOptions();

    Token *last = GetLastToken();

    // Save bookmark to the start of CREATE TABLE
    Bookmark(BOOK_CI_END, name, last);

    return true;
}

// CREATE FUNCTION
bool SqlParser::ParseCreateFunction(Token *create, Token *or_, Token *replace, Token *function)
{
    STATS_DECL
    STMS_STATS_V("CREATE FUNCTION", create);

    if(create == NULL || function == NULL)
        return false;

    ClearSplScope();

    _spl_scope = SQL_SCOPE_FUNC;
    _spl_start = create;

    // Function name
    Token* name = GetNextIdentToken(SQL_IDENT_OBJECT, SQL_SCOPE_FUNC);

    if(name == NULL)
        return false;

    _spl_name = name;

    // Check if it is specified to convert function to procedure
    _spl_func_to_proc = IsFuncToProc(name);

    if(_spl_func_to_proc)
        TOKEN_CHANGE(function, "PROCEDURE");

    // Support OR REPLACE clause (always set it)
    Token::Change(create, "CREATE OR REPLACE", L"CREATE OR REPLACE", 17);

    ParseFunctionParameters(name);

    // RETURN;
    ParseFunctionReturns(function);

    // Second call to parse function options DETERMINISTIC
//    ParseFunctionOptions();

    // IS or AS in Oracle; AS in SQL Server; nothing in MySQL
    Token *as = GetNextWordToken("AS", L"AS", 2);
    Token *is = NULL;

    // Also allows using IS
    if(as == NULL)
    {
        is = GetNextWordToken("IS", L"IS", 2);
    }

//    ParseFunctionBody(create, function, name, Nvl(as, is));

    // Define actual sizes for parameters, cursor parameters and records
    OracleAppendDataTypeSizes();

    // Body can be terminated with /
    Token *pl_sql = GetNextCharToken('/', L'/');

    SplPostActions();

    // Remove Copy/Paste blocks
    ClearCopy(COPY_SCOPE_PROC);

    // In case of parser error can enter to this function recursively, reset variables
    ClearSplScope();

    return true;
}

// CREATE FUNCTION parameters
bool SqlParser::ParseFunctionParameters(Token *function_name)
{
    Token *open = GetNextCharToken('(', L'(');

    // First DEFAULT
    Token *first_default = NULL;

    while(true)
    {
        Token *in = NULL;
        Token *out = NULL;
        Token *inout = NULL;

        Token *name = GetNextIdentToken(SQL_IDENT_PARAM);

        // NULL also returned in case of )
        if(name == NULL)
            break;

        // IN, OUT or IN OUT follows the name

        Token *param_type = GetNextToken();

        if(Token::Compare(param_type, "IN", L"IN", 2) == true)
        {
            in = param_type;

            // Check for IN OUT
            out = GetNextWordToken("OUT", L"OUT", 3);
        }
        else
        if(Token::Compare(param_type, "OUT", L"OUT", 3) == true)
            out = param_type;
        else
            PushBack(param_type);

        Token *data_type = GetNextToken();

        // Check and resolve %TYPE
        bool typed = ParseTypedVariable(name, data_type);

        // Get the parameter data type
        if(typed == false)
        {
            ParseDataType(data_type, SQL_SCOPE_FUNC_PARAMS);
            ParseVarDataTypeAttribute();

            // Propagate data type to variable name
            name->data_type = data_type->data_type;
            name->data_subtype = data_type->data_subtype;
        }

        Token *next = GetNextToken();

        if(next == NULL)
            break;

        bool tsql_default = false;
        bool ora_default = false;

        // DEFAULT keyword
        if(next->Compare("DEFAULT", L"DEFAULT", 7) == true)
            ora_default = true;
        else
            PushBack(next);

        if(tsql_default == true || ora_default == true)
        {
            if(first_default == NULL)
                first_default = next;

            // Default is a constant
            Token *default_exp = GetNextToken();

            // Uses DEFAULT keyword
            if(tsql_default == true)
                Token::Change(next, "DEFAULT", L"DEFAULT", 7);
        }

        // Exit if not comma
        Token *comma = GetNextCharToken(',', L',');

        if(comma == NULL)
            break;
    }

    _spl_param_close =  TOKEN_GETNEXT(')');

    return true;
}

// Actions after converting each procedure, function or trigger
void SqlParser::SplPostActions()
{
    // Add declarations for SELECT statement moved out of IF condition
    if(_spl_moved_if_select > 0)
        OracleIfSelectDeclarations();

    // Remove Copy/Paste blocks
    ClearCopy(COPY_SCOPE_PROC);

    // In case of parser error can enter to this function recursively, reset variables
    ClearSplScope();

    return;
}

// CREATE TRIGGER statement
bool SqlParser::ParseCreateTrigger(Token *create, Token *or_, Token *trigger)
{
    STATS_DECL
    STMS_STATS_V("CREATE TRIGGER", create)

    if(trigger == nullptr)
        return false;

    ClearSplScope();

    _spl_scope = SQL_SCOPE_TRIGGER;
    _spl_start = create;



    // Trigger name
    Token *name = GetNextIdentToken();

    if(name == nullptr)
        return false;

    _spl_name = name;

    Token *on = nullptr;
    Token *table = nullptr;

    // BEFORE, AFTER, INSTEAD OF
    Token *when = GetNextWordToken("BEFORE", L"BEFORE", 6);

    if(when == nullptr)
    {
        when = GetNextWordToken("AFTER", L"AFTER", 5);

        if(when == nullptr)
        {
            when = GetNextWordToken("INSTEAD", L"INSTEAD", 7);

            // OF keyword after INSTEAD
            if(when != nullptr)
                /*Token *of */ (void) GetNextWordToken("OF", L"OF", 2);
        }
    }

    // INSERT, UPDATE or DELETE
    Token *insert = nullptr;
    Token *update = nullptr;
    Token *delete_ = nullptr;

    // List of events can be specified
    while(true)
    {
        Token *operation = GetNextToken();

        if(operation == nullptr)
            break;

        if(operation->Compare("INSERT", L"INSERT", 6) == true)
            insert = operation;
        else
        if(operation->Compare("DELETE", L"DELETE", 6) == true)
            delete_ = operation;
        else
        if(operation->Compare("UPDATE", L"UPDATE", 6) == true)
        {
            update = operation;

            // OF col, ... for UPDATE operation
            Token *of = GetNextWordToken("OF", L"OF", 2);

            // Column list
            while(of != nullptr)
            {
                /*Token *col */ (void) GetNextToken();
                Token *comma = GetNextCharToken(',', L',');

                if(comma == nullptr)
                    break;
            }
        }
        else
        {
            PushBack(operation);
            break;
        }

        Token *comma = GetNextCharToken(',', L',');

        if(comma == nullptr)
            break;
    }

    // ON table name
    on = TOKEN_GETNEXTW("ON");
    table = GetNextIdentToken();

    // REFERENCING NEW [AS] name OLD [AS] name
    Token *referencing = GetNextWordToken("REFERENCING", L"REFERENCING", 11);

	// REFERENCING not supported
	if(referencing != NULL)
		Token::Remove(referencing);

	while(referencing != NULL)
	{
		Token *next = GetNextToken();

        if(next == nullptr)
            break;

        // NEW [AS] name
        if(next->Compare("NEW", L"NEW", 3) == true)
        {
            // AS is optional
            /*Token *as */ (void) GetNextWordToken("AS", L"AS", 2);

            _spl_new_correlation_name = GetNextToken();

				Token::Remove(next, _spl_new_correlation_name);
            continue;
        }
        else
        // OLD [AS] name
        if(next->Compare("OLD", L"OLD", 3) == true)
        {
            // AS is optional
            /*Token *as */ (void) GetNextWordToken("AS", L"AS", 2);

            _spl_old_correlation_name = GetNextToken();

				Token::Remove(next, _spl_old_correlation_name);
            continue;
        }



        PushBack(next);
        break;
    }

    // FOR EACH ROW or FOR EACH STATEMENT
    Token *for_ = GetNextWordToken("FOR", L"FOR", 3);
    Token *each = nullptr;
    Token *row = nullptr;
    Token *statement = nullptr;

    if(for_ != nullptr)
        each = GetNextWordToken("EACH", L"EACH", 4);

    if(each != nullptr)
        row = GetNextWordToken("ROW", L"ROW", 3);

    if(row == nullptr)        
		statement = GetNextWordToken("STATEMENT", L"STATEMENT", 9);

	if (row == nullptr && statement == nullptr) {
		Append(table, " FOR EACH ROW", L" FOR EACH ROW", 13);
	}

    // WHEN (condition)
    Token *when2 = GetNextWordToken("WHEN", L"WHEN", 4);

    if(when2 != nullptr)
    {
        Enter(SQL_SCOPE_TRG_WHEN_CONDITION);

        // Parse trigger condition
        ParseBooleanExpression(SQL_BOOL_TRIGGER_WHEN);

        Leave(SQL_SCOPE_TRG_WHEN_CONDITION);
    }

    Token *end = nullptr;

    ParseCreateTriggerBody(create, name, table, when, insert, update, delete_, &end);

    // Remove Copy/Paste blocks
    ClearCopy(COPY_SCOPE_PROC);

    // In case of parser error can enter to this function recursively, reset variables
    ClearSplScope();

    return true;
}
// Body of CREATE TRIGGER statement
bool SqlParser::ParseCreateTriggerBody(Token *create, Token *name, Token *table, Token *when,
                                            Token *insert, Token * /*update*/, Token * /*delete_*/, Token **end_out)
{
    Token *begin = GetNextWordToken("BEGIN", L"BEGIN", 5);


    // Try to recognize a pattern in the trigger body
    bool pattern = ParseCreateTriggerOraclePattern(create, table, when, insert);

    // If pattern matched, remove the trigger
    if(pattern == true)
    {
        Token *end = GetNextWordToken("END", L"END", 3);
        Token *semi = GetNextCharToken(';', L';');

        Token *last = (semi == nullptr) ? end : semi;

        Token::Remove(create, last);
    }

    bool frontier = (begin != nullptr) ? true : false;

    ParseBlock(SQL_BLOCK_PROC, frontier, SQL_SCOPE_TRIGGER, nullptr);

    Token *end = GetNextWordToken("END", L"END", 3);

    // END required for trigger
    if(begin == nullptr && end == nullptr)
        Append(GetLastToken(), "\nEND;", L"\nEND;", 5, create);

    // Must follow END name
    Token *semi = GetNextCharToken(';', L';');

    //Procedure name can be specified after END
    if(semi == nullptr)
    {
        if(ParseSplEndName(name, end) == true)
            semi = GetNextCharToken(';', L';');

        // Append ; after END
        if(end != nullptr && semi == nullptr)
            AppendNoFormat(end, ";", L";", 1);
    }

    if(end_out != nullptr)
        *end_out = end;

    return true;
}
// DROP statement
bool SqlParser::ParseDropStatement(Token *drop)
{
    if(drop == nullptr)
        return false;

    bool exists = false;

    Token *next = GetNextToken();

    if(next == nullptr)
        return false;

    // DROP TABLE
    if(next->Compare("TABLE", L"TABLE", 5) == true)
        exists = ParseDropTableStatement(drop, next);
    else
    // DROP TRIGGER
    if(next->Compare("TRIGGER", L"TRIGGER", 7) == true)
        exists = ParseDropTriggerStatement(drop, next);

    return exists;
}
// DROP TABLE statement
bool SqlParser::ParseDropTableStatement(Token *drop, Token *table)
{
    if(drop == nullptr || table == nullptr)
        return false;

    STATS_DECL
    STMS_STATS_V("DROP TABLE", drop)

    Token *table_name = GetNextIdentToken(SQL_IDENT_OBJECT);

    if(table_name == nullptr)
        return false;

    bool drop_if_exists = false;

    Token *next = GetNextToken();

    // IF
    if(next->Compare("IF", L"IF", 2) == true)
    {
        drop_if_exists = true;
    }
    else
    // CASCADE CONSTRAINTS after table name
    if (next->Compare("CASCADE", L"CASCADE", 7) == true) {
        Token *constranits=GetNextWordToken("CONSTRAINTS", L"CONSTRAINTS", 11);
        if (constranits != nullptr) {
            Token::Remove(next, constranits);
        }
    }

    // Add PL/SQL block to implement IF EXISTS
    if(drop_if_exists == true)
    {
        Prepend(drop, "BEGIN\n   EXECUTE IMMEDIATE '", L"BEGIN\n   EXECUTE IMMEDIATE '", 28);
        Append(table_name, "';\nEXCEPTION\n   WHEN OTHERS THEN NULL;\nEND;\n/",
                          L"';\nEXCEPTION\n   WHEN OTHERS THEN NULL;\nEND;\n/", 45, drop);

        // If ; follows the DROP remove it as we already set all delimiters
        Token *semi = GetNextCharToken(';', L';');

        if(semi != nullptr)
            Token::Remove(semi);
    }
    else
    {
        Prepend(table_name, "IF EXISTS ", L"IF EXISTS ", 10);
    }

    // For Oracle, check if a temporary table is removed

    for(ListwItem *i = _spl_created_session_tables.GetFirst(); i != nullptr; i = i->next)
    {
        Token *temp = (Token*)i->value;

        if(Token::Compare(table_name, temp) == true)
        {
            // Use TRUNCATE as it is a permanent object in Oracle
            Token::Change(drop, "TRUNCATE", L"TRUNCATE", 8);

            // If inside a procedure block surround with EXECUTE IMMEDIATE
            if(_spl_scope == SQL_SCOPE_PROC || _spl_scope == SQL_SCOPE_FUNC || _spl_scope == SQL_SCOPE_TRIGGER)
            {
                Prepend(drop, "EXECUTE IMMEDIATE '", L"EXECUTE IMMEDIATE '", 19);
                AppendNoFormat(table_name, "'", L"'", 1);
            }

            break;
        }
    }

    return true;
}

// DROP TRIGGER statement
bool SqlParser::ParseDropTriggerStatement(Token *drop, Token *trigger)
{
    if(drop == nullptr || trigger == nullptr)
        return false;

    STATS_DECL
    STMS_STATS_V("DROP TRIGGER", drop)

    Token *trigger_name = GetNextIdentToken();

    if(trigger_name == nullptr)
        return false;

    bool drop_if_exists = false;

    // Add PL/SQL block to implement IF EXISTS
    if(drop_if_exists == true)
    {
        Prepend(drop, "BEGIN\n   EXECUTE IMMEDIATE '", L"BEGIN\n   EXECUTE IMMEDIATE '", 28);
        Append(trigger_name, "';\nEXCEPTION\n   WHEN OTHERS THEN NULL;\nEND;\n/",
                          L"';\nEXCEPTION\n   WHEN OTHERS THEN NULL;\nEND;\n/", 45, drop);

        // If ; follows the DROP remove it as we already set all delimiters
        Token *semi = GetNextCharToken(';', L';');

        if(semi != nullptr)
            Token::Remove(semi);
    }

    return true;
}
// CREATE TYPE
bool SqlParser::ParseCreateType(Token *create, Token *type)
{
    if(create == nullptr || type == nullptr)
        return false;

    STATS_DECL
    STMS_STATS_V("CREATE TYPE", create)

    // User-defined type name
    Token *name = GetNextToken();

    if(name == nullptr)
        return false;

    // System data type
    Token *datatype = GetNextToken();

    ParseDataType(datatype);

    Token *type_end = GetLastToken();

    // Optional constraint
    ParseColumnConstraints(name, datatype, type_end);

    Token *last = GetLastToken();

    _udt.Add(name, datatype, type_end);

    // CREATE TYPE name AS OBJECT (name type) in Oracle

    Append(create, " OR REPLACE", L" OR REPLACE", 11);

    Token *semi = GetNextCharToken(';', L';');

    // In Oracle CREATE TYPE requires / for execution
    Append(Nvl(semi, last), "\n/", L"\n/", 2);


    return true;
}

// CREATE VIEW statement
bool SqlParser::ParseCreateView(Token *create, Token *materialized, Token *view)
{
    STATS_DECL
    STATS_DTL_DECL

    if(create == nullptr || view == nullptr)
        return false;

    Token *name = GetNextIdentToken(SQL_IDENT_OBJECT);

    if(name == nullptr)
        return false;

    Token *open = GetNext('(', L'(');

    // Optional column list
    while(true)
    {
        if(open == nullptr)
            break;

        // Column name
        Token *column = GetNextIdentToken();

        if(column == nullptr)
            break;

        Token *comma = GetNext(',', L',');

        if(comma == nullptr)
            break;
    }

    /*Token *close */ (void) GetNext(open, ')', L')');

    // AS keyword
    /*Token *as */ (void) GetNext("AS", L"AS", 2);


    // View options
    while(true)
    {
        bool exists = false;

        Token *option = GetNextToken();

        if(option == nullptr)
            break;

        // WITH READ ONLY
        if(option->Compare("WITH", L"WITH", 4) == true)
        {
            Token *read = GetNext("READ", L"READ", 4);
            Token *only = GetNext(read, "ONLY", L"ONLY", 4);

            if(read != nullptr && only != nullptr)
            {
                // Remove for MySQL
                Token::Remove(option, only);

                exists = true;
                continue;
            }
            else
                PushBack(option);
        }

        if(exists == false)
            break;
    }

    // Regular view
    if(materialized == nullptr)
    {
        STATS_SET_DESC(SQL_STMT_CREATE_VIEW_DESC)
        STMS_STATS_V("CREATE VIEW", create)
    }
    // Materialized view
    else
    {
        STATS_SET_DESC(SQL_STMT_CREATE_MATERIALIZED_VIEW_DESC)
        STATS_DTL_DESC(SQL_STMT_CREATE_MATERIALIZED_VIEW_DESC)
        STATS_DTL_CONV_ERROR(Target(SQL_MARIADB_ORA), STATS_CONV_HIGH, "", "")

        STATS_UPDATE_STATUS
        STMS_STATS_V("CREATE MATERIALIZED VIEW", create)
    }

    return true;
}

// INSERT statement
bool SqlParser::ParseInsertStatement(Token *insert)
{
    if(insert == nullptr)
        return false;

    STATS_DECL
    STMS_STATS(insert)

    Token *into = GetNextWordToken("INTO", L"INTO", 4);

    if(into == nullptr)
        return false;

    Token *table_name = GetNextIdentToken(SQL_IDENT_OBJECT);

    if(table_name == nullptr)
        return false;

    _spl_last_insert_table_name = table_name;

    // Optional column list
    Token *open1 = GetNextCharToken('(', L'(');

    ListWM cols;

    if(open1 != nullptr)
    {
        bool column_list = true;

        // Make sure it is not (SELECT ...)
        Token *select = GetNextWordToken("SELECT", L"SELECT", 6);
        Token *comma = (select != nullptr) ? GetNextCharToken('(', L'(') : nullptr;

        if(select != nullptr && comma == nullptr)
        {
            PushBack(open1);
            column_list = false;
        }

        // Get columns
        while(column_list)
        {
            // Column name
            Token *col = GetNextIdentToken(SQL_IDENT_COLUMN_SINGLE);

            if(col == nullptr)
                break;

            // Comma or )
            Token *del = GetNextToken();

            cols.Add(col, del);

            if(Token::Compare(del, ',', L','))
                continue;

            break;
        }
    }

    // VALUES or SELECT can go next
    Token *values = GetNextWordToken("VALUES", L"VALUES", 6);
    Token *select = nullptr;
    Token *open2 = nullptr;

    if(values == nullptr)
    {
        // SELECT can be optionaly enclosed with ()
        open2 = GetNextCharToken('(', L'(');

    }

    if(values == nullptr && select == nullptr)
        return false;

    // VALUES clause
    if(values != nullptr)
    {
        int rows = 0;
        Token *close2 = nullptr;

        Enter(SQL_SCOPE_INSERT_VALUES);

        // For MySQL multiple comma-separated rows can be specified
        while(true)
        {
            Token *open2 = GetNextCharToken('(', L'(');
            close2 = nullptr;

            if(open2 == nullptr)
                return false;

            int num = 0;

            // Get list of values
            while(true)
            {
                // Insert expression
                Token *exp = GetNextToken();

                if(exp == nullptr)
                    break;

                ParseExpression(exp);

                // Comma or )
                close2 = GetNextToken();

                bool val_removed = false;

                if(Token::Compare(close2, ',', L','))
                {
                    if(val_removed)
                        Token::Remove(close2);

                    num++;
                    continue;
                }

                break;
            }

            // Check if another row specified
            Token *comma = GetNextCharToken(',', L',');

            rows++;

            // For Oracle, convert multiple rows to SELECT UNION ALL
            if(comma != nullptr || rows > 1)
            {
                if(rows == 1)
                    Token::Remove(values);

                Prepend(open2, " SELECT ", L" SELECT ", 8, values);
                Append(close2, " FROM ", L" FROM ", 6, values);
                AppendNoFormat(close2, "dual", L"dual", 4);

                if(comma != nullptr)
                    Append(close2, " UNION ALL ", L" UNION ALL ", 11, values);

                Token::Remove(open2);
                Token::Remove(close2);
                Token::Remove(comma);
            }

            if(comma == nullptr)
                break;
        }

        Leave(SQL_SCOPE_INSERT_VALUES);

        // RETURNING clause in Oracle
        Token *returning = TOKEN_GETNEXTW("RETURNING");

        if(returning != nullptr)
        {
            Token *col = GetNextIdentToken(SQL_IDENT_COLUMN_SINGLE);
            /*Token *into */(void) TOKEN_GETNEXTWP(col, "INTO");
            /*Token *var */(void) GetNextIdentToken();
        }
    }

    // Implement CONTINUE handler in Oracle
    OracleContinueHandlerForInsert(insert);

    return true;
}
// Procedure name can be specified after outer END
bool SqlParser::ParseSplEndName(Token *name, Token * /*end*/)
{
    if(name == nullptr)
        return false;

    bool exists = false;

    Token *next = GetNextToken();

    if(next == nullptr)
        return false;

    // Compare with procedure name, here it can be specified without schema name, or one can be quoted
    // another unquoted
    if(CompareIdentifiersExistingParts(name, next) == true)
    {
        exists = true;
    }
    else
    // Compare with label name, label is without : here
    if(_spl_outer_label != nullptr && Token::Compare(_spl_outer_label, next, _spl_outer_label->len-1) == true)
    {
        // Remove for other databases
        Token::Remove(next);

        exists = true;
    }
    else
        PushBack(next);

    return exists;
}

// Function RETURNS clause
bool SqlParser::ParseFunctionReturns(Token *function)
{
    Token *returns = GetNextToken();

    if(returns == NULL)
        return false;

    // RETURN
    if(Token::Compare(returns, "RETURN", L"RETURN", 6) == true)
    {
        // Return data type
        Token *data_type = GetNextToken();

        ParseDataType(data_type, SQL_SCOPE_FUNC_PARAMS);
    }

    return true;
}
