// SQLParser Class

#ifndef sqlines_sqlparser_h
#define sqlines_sqlparser_h

#include <stdio.h>
#include <map>
#include "token.h"
#include "stats.h"
#include "report.h"
#include "listt.h"
#include "listw.h"
#include "listwm.h"
#include "doc.h"

// Conversion level
#define LEVEL_APP			1
#define LEVEL_SQL			2
#define LEVEL_STR			3

// SQL object scope
#define SQL_SCOPE_FREE			0
#define SQL_SCOPE_PROC			1
#define SQL_SCOPE_FUNC			2
#define SQL_SCOPE_TRIGGER		3
#define SQL_SCOPE_TABLE			4
#define SQL_SCOPE_TEMP_TABLE	5
#define SQL_SCOPE_INDEX			6
#define SQL_SCOPE_TABLESPACE	7

// Procedural block types
#define SQL_BLOCK_PROC			1
#define SQL_BLOCK_IF			2
#define SQL_BLOCK_WHILE			3
#define SQL_BLOCK_HANDLER		4
#define SQL_BLOCK_CASE			5
#define SQL_BLOCK_FOR			6
#define SQL_BLOCK_LOOP			7
#define SQL_BLOCK_CHECK			8
#define SQL_BLOCK_REPEAT		9
#define SQL_BLOCK_BEGIN			10
#define SQL_BLOCK_EXCEPTION		11
#define SQL_BLOCK_TRY           12
#define SQL_BLOCK_CATCH         13

// Boolean expression scope
#define SQL_BOOL_IF					1
#define SQL_BOOL_IF_EXP				2
#define SQL_BOOL_CASE				3
#define SQL_BOOL_CHECK				4
#define SQL_BOOL_JOIN_ON			5
#define SQL_BOOL_WHERE				6
#define SQL_BOOL_TRIGGER_WHEN		7
#define SQL_BOOL_WHILE				8
#define SQL_BOOL_EXISTS				9
#define SQL_BOOL_UNTIL				10
#define SQL_BOOL_HAVING				11
#define SQL_BOOL_CREATE_RULE		12
#define SQL_BOOL_ASSIGN             13

// SQL clause scope
#define SQL_SCOPE_ASSIGNMENT_RIGHT_SIDE		1
#define SQL_SCOPE_CASE_FUNC					2
#define SQL_SCOPE_CURSOR_PARAMS				3
#define SQL_SCOPE_FUNC_PARAMS				4
#define SQL_SCOPE_INSERT_VALUES				5
#define SQL_SCOPE_PROC_PARAMS				6
#define SQL_SCOPE_SELECT_STMT				7
#define SQL_SCOPE_TAB_COLS					8
#define SQL_SCOPE_TRG_WHEN_CONDITION		9
#define SQL_SCOPE_VAR_DECL					10
#define SQL_SCOPE_XMLSERIALIZE_FUNC			11
#define SQL_SCOPE_SP_ADDTYPE				12
#define SQL_SCOPE_CONVERT_FUNC				13
#define SQL_SCOPE_CAST_FUNC					14
#define SQL_SCOPE_OBJ_TYPE_DECL             15

// SQL SELECT statement scope
#define SQL_SEL_INSERT					1	
#define SQL_SEL_CURSOR					2		
#define SQL_SEL_OPEN					3
#define SQL_SEL_FROM					4
#define SQL_SEL_EXP						5
#define SQL_SEL_SET_UNION				6
#define SQL_SEL_FOR						7
#define SQL_SEL_IN_PREDICATE			8
#define SQL_SEL_EXISTS_PREDICATE		9
#define SQL_SEL_SELECT_LIST				10
#define SQL_SEL_FOREACH					11
#define SQL_SEL_UPDATE_SET				12
#define SQL_SEL_UPDATE_FROM				13
#define SQL_SEL_JOIN					14
#define SQL_SEL_CREATE_TEMP_TABLE_AS	15
#define SQL_SEL_EXPORT					16
#define SQL_SEL_WITH_CTE				17
#define SQL_SEL_VIEW					17

// SQL Statement scope
#define SQL_STMT_ALTER_TABLE            1
#define SQL_STMT_SELECT					2
#define SQL_STMT_INSERT					3
#define SQL_STMT_UPDATE					4
#define SQL_STMT_DELETE					5

// Operators
#define SQL_OPERATOR_CONCAT		1
#define SQL_OPERATOR_PLUS		2

// Expected identifier type
#define SQL_IDENT_OBJECT		    1
#define SQL_IDENT_COLUMN		    2
#define SQL_IDENT_COLUMN_SINGLE	    3
#define SQL_IDENT_COLVAR		    4
#define SQL_IDENT_PARAM			    5
#define SQL_IDENT_VAR			    6

// Bookmark types
#define BOOK_CT_START			1			// CREATE TABLE start
#define BOOK_CT_END				2			// CREATE TABLE end
#define BOOK_CTC_END			3			// CREATE TABLE column definition end
#define BOOK_CTC_ALL_END		4			// CREATE TABLE end of all column definitions, pointer to ) i.e. before storage and other properties
#define BOOK_CI_START			5			// CREATE INDEX start
#define BOOK_CI_END				6			// CREATE INDEX end
#define BOOK_USER_EXCEPTION		7			// User-defined exception

// Cope, Paste and Cut scopes
#define COPY_SCOPE_PROC		1			// Procedure, function, trigger or outer anonymous block

// Cope, Paste and Cut types
#define COPY_EXIT_HANDLER_NOT_FOUND				1
#define COPY_CONTINUE_HANDLER_NOT_FOUND			2
#define COPY_EXIT_HANDLER_SQLEXCEPTION			3
#define COPY_CONTINUE_HANDLER_SQLEXCEPTION		4
#define COPY_EXIT_HANDLER_USER_DEFINED			5
#define COPY_CONTINUE_HANDLER_USER_DEFINED		6
#define COPY_CURSOR_WITH_RETURN					7
#define COPY_EXIT_HANDLER_FOR_SQLSTATE			8
#define COPY_CONTINUE_HANDLER_FOR_SQLSTATE		9

#define TOKEN_GETNEXT(chr)           GetNext(chr, L##chr)
#define TOKEN_GETNEXTP(prev, chr)    GetNext(prev, chr, L##chr)
#define TOKEN_GETNEXTW(string)       GetNext(string, L##string, sizeof(string) - 1)
#define TOKEN_GETNEXTWP(prev, string) GetNext(prev, string, L##string, sizeof(string) - 1)
#define TOKEN_CMPC(token, chr)		 Token::Compare(token, chr, L##chr)
#define TOKEN_CMPCP(token, chr, pos) Token::Compare(token, chr, L##chr, pos)
#define TOKEN_CMP(token, string)     Token::Compare(token, string, L##string, sizeof(string) - 1)
#define TOKEN_CMP_PART0(token, string) Token::Compare(token, string, L##string, 0, sizeof(string) - 1)
#define TOKEN_CHANGE(token, string)  Token::Change(token, string, L##string, sizeof(string) - 1)
#define TOKEN_CHANGE_FMT(token, string, format)  Token::Change(token, string, L##string, sizeof(string) - 1, format)
#define TOKEN_CHANGE_NOFMT(token, string)  Token::ChangeNoFormat(token, string, L##string, sizeof(string) - 1)

#define APPEND(token, string) Append(token, string, L##string, sizeof(string) - 1)
#define APPEND_FMT(token, string, format) Append(token, string, L##string, sizeof(string) - 1, format)
#define APPEND_FIRST_FMT(token, string, format) AppendFirst(token, string, L##string, sizeof(string) - 1, format)
#define APPEND_FIRST_NOFMT(token, string) AppendFirstNoFormat(token, string, L##string, sizeof(string) - 1)
#define APPEND_NOFMT(token, string) AppendNoFormat(token, string, L##string, sizeof(string) - 1)
#define APPENDS(token, tokenstr) Append(token, tokenstr)

#define APPENDSTR(tokenstr, string) tokenstr.Append(string, L##string, sizeof(string) - 1)

#define PREPEND(token, string) Prepend(token, string, L##string, sizeof(string) - 1)
#define PREPEND_FMT(token, string, format) Prepend(token, string, L##string, sizeof(string) - 1, format)
#define PREPEND_NOFMT(token, string) PrependNoFormat(token, string, L##string, sizeof(string) - 1)

#define LOOKNEXT(string) LookNext(string, L##string, sizeof(string) - 1)

#define COMMENT(string, start, end) Comment(string, L##string, sizeof(string) - 1, start, end) 
#define COMMENT_WARN(start, end) COMMENT("Warning: ", start, end) 

typedef std::map<std::string, std::string> StringMap;
typedef std::pair<std::string, std::string> StringMapPair;

class Cobol;

// Bookmark element
struct Book
{
	int type;
	Token *name;
	Token *name2;
	Token *book;

	int data1;

	Book *prev;
	Book *next;

	Book() 
	{ 
		type = 0; name = NULL; name2 = NULL; book = NULL; data1 = 0; prev = NULL; next = NULL; 

	}
};

// Copy, Paste and Cut element
struct CopyPaste
{
	int scope;
	int type;
	Token *name;
	ListT<Token> tokens;

	CopyPaste *next;
	CopyPaste *prev;

	CopyPaste() { scope = 0; type = 0; name = NULL; next = NULL; prev = NULL; }
};

// Metadata information
class Meta
{
public:
    // Table, view, procedure name
    std::string object;
    // Column or parameter
	std::string column;
    // Data type
    std::string dtype;

    Meta(std::string o, std::string c, std::string dt) 
    { 
        object = o; column = c; dtype = dt; prev = next = NULL; 
    }

    Meta *prev;
	Meta *next;
};

class SqlParser
{
	// Source and target SQL dialects
	short _source;
	short _target;

	// Source and target application types
	short _source_app;
	short _target_app;

	// Current conversion level (application, SQL code, dynamic string i.e.)
	int _level;

    // Current object scope (table, view, procedure etc.)
    int _obj_scope;
    // Current statement scope (CREATE TABLE, CREATE VIEW, ALTER TABLE etc.)
    int _stmt_scope;

	// Currently converted input
	const char *_start;
	const char *_next_start;
	int _size;
	int _remain_size;
	int _line;

	// Input tokens
	ListT<Token> _tokens;

	// Bookmarks
	ListT<Book> _bookmarks;
	// Copy, Paste and Cut blocks
	ListT<CopyPaste> _copypaste;

    // Metadata information
    std::map<std::string, ListT<Meta>*> _meta;

	// Scope list
	ListWM _scope;

	// Current procedural scope (procedure, function, trigger i.e.)
	int _spl_scope;

	// Name of current procedure (function, trigger)
	Token *_spl_name;
	// Start token of procedure, function or trigger (usually it is CREATE keyword)
	Token *_spl_start;


	// Current variables and parameters
	ListW _spl_variables;
	ListW _spl_parameters;

	// Outer BEGIN keyword
    Token *_spl_outer_begin;
    // Standalone BEGIN levels in the current procedure (each BEGIN entry is cleared on block exit)
    ListW _spl_begin_blocks;

	// Outer AS or IS keyword
    Token *_spl_outer_as;

    // Last declare statement before the first non-declare statement
	Token *_spl_last_declare;
    // First non-declare statement in procedure or function
	Token *_spl_first_non_declare;

	// Last statement in procedure or function
	Token *_spl_last_stmt;

    // Last token of last declared variable (not cursor) in the outer PL/SQL DECLARE block
    Token *_spl_last_outer_declare_var;
	Token *_spl_last_outer_declare_varname;
    
	// Number of result sets returned from procedure
	int _spl_result_sets;
	// Result set cursor names (declared cursor with return)
	ListW _spl_result_set_cursors;
	// Result set generated cursor names (for standalone SELECT returning a result set)
	ListT<TokenStr> _spl_result_set_generated_cursors;
	
	// Delimiter is set at the end of procedure
	bool _spl_delimiter_set;
	// User-defined exceptions and condition handlers
	ListWM _spl_user_exceptions;

	// NEW and OLD correlation name in trigger
	Token *_spl_new_correlation_name;
	Token *_spl_old_correlation_name;
	// Referenced NEW and OLD columns
	ListW _spl_tr_new_columns;
	ListW _spl_tr_old_columns;

	// Outer block label in DB2 procedure
	Token *_spl_outer_label;
	// Oracle PL/SQL cursor parameters
	ListWM _spl_cursor_params;
	// Variables generated for Oracle PL/SQL parameters
	ListWM _spl_cursor_vars;
	// The names and definitions of declared cursors
	ListW _spl_declared_cursors;
	ListW _spl_declared_cursors_using_vars;
	ListWM _spl_declared_cursors_stmts;
	// Current declaring cursor
	Token *_spl_current_declaring_cursor;
	// Current declaring cursor uses procedural variables
	bool _spl_current_declaring_cursor_uses_vars;
	// Cursor for which updates WHERE CURRENT OF is performed
	ListW _spl_updatable_current_of_cursors;
	// Declared cursors and their SELECT statements
	ListWM _spl_declared_cursors_select;
	// Declared cursors and their select list expressions
	ListWM _spl_declared_cursors_select_first_exp;
	ListWM _spl_declared_cursors_select_exp;

	// The current PL/SQL package
	Token *_spl_package;
	// Declared record variables %ROWTYPE
	ListW _spl_rowtype_vars;
	// Referenced %ROWTYPE record fields
	ListWM _spl_rowtype_fields;
	// Fetch into record referenced
	ListW _spl_rowtype_fetches;
	// TYPE name OF TABLE datatype
	ListWM _spl_obj_type_table;

	// Implicit rowtype created by for cursor loops
	ListWM _spl_implicit_rowtype_vars;
	// Implicit rowtype fields
	ListWM _spl_implicit_rowtype_fields;
	// Generated FETCH statements to emulate cursor loops
	ListWM _spl_implicit_rowtype_fetches;

	// Declared local temporary tables, table variables
	ListW _spl_declared_local_tables;
	// Created session temporary tables
	ListW _spl_created_session_tables;


	// Closing parenthesis ) after parameter list
	Token *_spl_param_close;
	
	// RETURNS, RETURN, RETURNING keyword
	Token *_spl_returns;

	// Generated OUT variable names for Informix RETURNING clause
	ListWM _spl_returning_out_names;
	// Function returns integer data type
	bool _spl_return_int;

	// Number of FOREACH statements in the current procedure (function, trigger)
	int _spl_foreach_num;
	// Number of RETURN WITH RESUME in procedure
	int _spl_return_with_resume;
	// Number of RETURN statements in procedure
	int _spl_return_num;

	// Table name from last INSERT statement
	Token *_spl_last_insert_table_name;
	// Cursor name from last OPEN statement
	Token *_spl_last_open_cursor_name;
	// Cursor name from last FETCH statement
	Token *_spl_last_fetch_cursor_name;
	// List of OPEN cursor statemenents 
	ListWM _spl_open_cursors;

	// Prepared statements id
	ListWM _spl_prepared_stmts;
	// Cursors for prepared statements
	ListWM _spl_prepared_stmts_cursors;
	ListWM _spl_prepared_stmts_cursors_with_return;

	// Fields of implicit records created by FOR loops
	ListWM _spl_implicit_record_fields;
	
	// Result set locators associated with procedures
	ListWM _spl_rs_locator_procedures;

    // Loops level in the current procedure (each loop entry is cleared on loop exit)
    ListW _spl_loops;
    // Number of loop labels already generated in the current procedure
    int _spl_loop_labels;

	// Stored procedures calls inside procedural block
	ListWM _spl_sp_calls;

	// Number of SELECT statements moved out of IF boolean condition
	int _spl_moved_if_select;

	// Procedure converted to function
	bool _spl_proc_to_func;
	// Function converted to procedure
	bool _spl_func_to_proc;
    // Handler for NOT FOUND condition
    bool _spl_not_found_handler;
	bool _spl_need_not_found_handler;

	// Monday is 1 day, Sunday is 7 (false if it is unknown from context)
	bool _spl_monday_1;

	// User-defined data types
	ListWM _udt;

	// Number of SELECT statements in the current outer expression
	int _exp_select;

	// Formatting tokens for newly generated code
	Token *_declare_format;

	// Push back token
	Token *_push_back_token;

	// Options
	bool _option_rems;
	std::string _option_oracle_plsql_number_mapping;
	std::string _option_set_explicit_schema;
	std::string _option_cur_file;

	// Mappings
	StringMap _object_map;
	StringMap _schema_map;
	StringMap _func_to_sp_map;

    // Statistics and report
    Stats *_stats;
    Report *_report;

public:
	SqlParser();
	~SqlParser();

	// Set source and target types
	void SetTypes(short source, short target) { _source = source; _target = target; }
    // Set target programming language
    void SetLang(const char *value, bool source);
	// Set option
	void SetOption(const char *option, const char *value);

	// Perform conversion
	int Convert(const char *input, int size, const char **output, int *out_size, int *lines);

	// Generate output
	void CreateOutputString(const char **output, int *out_size);

	// Post conversion when all tokens processed
	void Post();

	// Get next token from the input
	Token* GetNextToken();
	Token* GetNextToken(Token *prev);
	Token* GetNextIdentToken(int expected_type = 0, int scope = 0);
	Token* GetNextCharToken(const char ch, const wchar_t wch);
	Token* GetNextCharToken(Token *prev, const char ch, const wchar_t wch);
	Token* GetNextCharOrLastToken(const char ch, const wchar_t wch);
	Token* GetNextPlusMinusAsOperatorToken(const char ch, const wchar_t wch);
	Token* GetNextSpecialCharToken(Token *prev, const char *str, const wchar_t *wstr, size_t len);
	Token* GetNextSpecialCharToken(const char *str, const wchar_t *wstr, size_t len);
	Token* GetNextWordToken(const char *str, const wchar_t *wstr, size_t len);
	Token* GetNextWordToken(Token *prev, const char *str, const wchar_t *wstr, size_t len);
	Token* GetNextUntilNewlineToken();
	Token* GetNextNumberToken();
	Token* GetNextNumberToken(Token *prev);
	Token* GetNextStringToken();
	Token* GetPrevToken(Token *token);
	Token* GetLastToken();
	Token* GetLastToken(Token *last);
	Token* GetVariable(Token *name);
	Token* GetParameter(Token *name);
	Token* GetVariableOrParameter(Token *name);
	Token* GetBomToken();

	Token* GetNext() { return GetNextToken(); }
	Token* GetNext(Token *prev) { return GetNextToken(prev); }
	Token* GetNext(const char *str, const wchar_t *wstr, size_t len) { return GetNextWordToken(str, wstr, len); }
	Token* GetNext(Token *prev, const char *str, const wchar_t *wstr, size_t len) { return GetNextWordToken(prev, str, wstr, len); }
	Token* GetNext(const char ch, const wchar_t wch) { return GetNextCharToken(ch, wch); }
	Token* GetNext(Token *prev, const char ch, const wchar_t wch) { return GetNextCharToken(prev, ch, wch); }

	// Handle local block variables
	void EnterLocalVariablesBlock();
	void LeaveLocalVariablesBlock();

	void AddVariable(Token *name) { _spl_variables.Add(name); }
	void AddParameter(Token *name) { _spl_parameters.Add(name); }

	// Return the last fetched token to the input
	void PushBack(Token *token);
	// Check next token for the specific value but do not fecth it from the input
	Token *LookNext(const char *str, const wchar_t *wstr, size_t len);

	// Append the token with the specified value
	Token *Append(Token *token, const char *str, const wchar_t *wstr, size_t len, Token *format = NULL);
    Token *AppendWithSpaceAfter(Token *token, const char *str, const wchar_t *wstr, size_t len, Token *format = NULL);
	void Append(Token *token, int value);
	void Append(Token *token, TokenStr *str, Token *format = NULL);
    void AppendFirst(Token *token, const char *str, const wchar_t *wstr, size_t len, Token *format = NULL);
	Token* AppendNoFormat(Token *token, const char *str, const wchar_t *wstr, size_t len);
	void AppendNoFormat(Token *token, TokenStr *str);
	void AppendFirstNoFormat(Token *token, const char *str, const wchar_t *wstr, size_t len);
	Token* AppendCopy(Token *token, Token *append);
	Token* AppendCopy(Token *token, Token *first, Token *last, bool append_removed = true);
	void AppendSpaceCopy(Token *token, Token *append);
	void AppendSpaceCopy(Token *token, Token *first, Token *last, bool append_removed = true);
    void AppendNewlineCopy(Token *token, Token *first, Token *last, size_t newlines = 1, bool append_removed = true);
	void Append(Token *token, Token *append);
	
	// Prepend the token with the specified value
	Token* Prepend(Token *token, const char *str, const wchar_t *wstr, size_t len, Token *format = NULL);
	Token* PrependNoFormat(Token *token, const char *str, const wchar_t *wstr, size_t len);
	Token* PrependNoFormat(Token *token, TokenStr *str);
	void PrependFirstNoFormat(Token *token, const char *str, const wchar_t *wstr, size_t len);
	Token* PrependCopy(Token *token, Token *prepend);
	Token* PrependCopy(Token *token, Token *first, Token *last, bool prepend_removed = true);
	void PrependSpaceCopy(Token *token, Token *first, Token *last = NULL, bool prepend_removed = true);
	void Prepend(Token *token, Token *prepend);

	void ChangeWithSpacesAround(Token *token, const char *new_str, const wchar_t *new_wstr, size_t len, Token *format = NULL);
	
	// Check for the source and target type
	bool Source(short s1, short s2 = -1, short s3 = -1, short s4 = -1, short s5 = -1, short s6 = -1); 
	bool Target(short t1, short t2 = -1, short t3 = -1, short t4 = -1, short t5 = -1, short t6 = -1); 

	// Return first not NULL
	Token* Nvl(Token *first, Token *second, Token *third = NULL, Token *fourth = NULL);
    Token* NvlLast(Token *first);

	// Enter, leave and check the specified scope
	void Enter(int scope);
	void Leave(int scope);
	bool IsScope(int scope, int scope2 = 0);
	
	// Save and get bookmark
	Book* Bookmark(int type, Token *name, Token *book);
	Book* Bookmark(int type, Token *name, Token *name2, Token *book);

	Book* GetBookmark(int type, Token *name); 
	Book* GetBookmark(int type, Token *name, Token *name2); 
	Book* GetBookmark2(int type, Token *name); 

	// Copy, Paste and Cut operations
	void Cut(int scope, int type, Token *name, Token *start, Token *end);
	void ClearCopy(int scope);

	// Actions after converting each procedure, function or trigger
	void SplPostActions();
	// Clear all procedural lists, statuses
	void ClearSplScope();

	// Get tokens
	void SkipSpaceTokens();
	bool GetSingleCharToken(Token *token);
	bool GetWordToken(Token *token);
	bool GetQuotedIdentifier(Token *token, bool starts_as_unquoted = false);
	bool GetStringLiteral(Token *token);
	
	// Parser functions
	void Parse(Token *token, int scope, int *result_sets);
	bool ParseStatement(Token *token, int scope, int *result_sets);
	bool ParseDataType(Token *type, int clause_scope = SQL_SCOPE_TAB_COLS);
	bool ParseTypedVariable(Token *var, Token *ref_type);
	bool ParseVarDataTypeAttribute();
    Token* ParseExpression();
	bool ParseExpression(Token *token, int prev_operator = 0);
	bool ParseBooleanExpression(int scope, Token *stmt_start = NULL, int *conditions_count = NULL, int *rowlimit = NULL, Token *prev_open = NULL, 
		bool *bool_operator_not_exists = NULL);
	bool ParseBooleanAndOr(int scope, Token *stmt_start, int *conditions_count, int *rowlimit);
	bool ParseInPredicate(Token *in);
	bool ParseOraclePackage(Token *token);
	bool ParseFunction(Token *token);
	bool ParseFunctionWithoutParameters(Token *token);
	bool ParseDatetimeLiteral(Token *token);
    bool ParseNamedVarExpression(Token *token);
	bool ParseBlock(int type, bool frontier, int scope, int *result_sets);
	bool ParseComment();
	bool ParseCaseExpression(Token *first);
	bool ParseAnalyticFunctionOverClause(Token *over);
//	bool ParseSystemProcedure(Token *execute, Token *name);

	// Operators
    bool ParseAdditionOperator(Token *first);
	bool ParseDivisionOperator(Token *first);
	bool ParseMultiplicationOperator(Token *first);
	bool ParseStringConcatenation(Token *first, int prev_operator);
	bool ParseSubtractionOperator(Token *first);
	bool ParsePercentOperator(Token *first);
	bool ParseAddSubIntervalChain(Token *first, int prev_operator);

	// Data types
	bool ParseBfileType(Token *name);
	bool ParseBinaryDoubleType(Token *name);
	bool ParseBinaryFloatType(Token *name);
	bool ParseBlobType(Token *name);
	bool ParseCharacterType(Token *name);
	bool ParseCharType(Token *name);
	bool ParseClobType(Token *name);
	bool ParseDateType(Token *name);
	bool ParseDecimalType(Token *name);
	bool ParseDoubleType(Token *name);
	bool ParseFloatType(Token *name);
	bool ParseIntervalType(Token *name);
	bool ParseIntType(Token *name);
	bool ParseLongType(Token *name);
	bool ParseNcharType(Token *name);
	bool ParseNclobType(Token *name);
	bool ParseNumberType(Token *name, int clause_scope);
	bool ParseNumericType(Token *name);
	bool ParseNvarchar2Type(Token *name, int clause_scope);
	bool ParseRawType(Token *name);
	bool ParseRealType(Token *name);
	bool ParseRefcursor(Token *name);
	bool ParseRowidType(Token *name);
	bool ParseSmallintType(Token *name);
	bool ParseTimestampType(Token *name);
	bool ParseUrowidType(Token *name);
	bool ParseVarcharType(Token *name, int clause_scope);
	bool ParseVarchar2Type(Token *name, int clause_scope);
	bool ParseXmltypeType(Token *name);

	// Statements
	bool ParseAlterStatement(Token *token, int *result_sets, bool *proc);
	bool ParseAlterTableStatement(Token *alter, Token *table);
	bool ParseAlterIndexStatement(Token *alter, Token *index);
	bool ParseAssignmentStatement(Token *variable);
	bool ParseCommentStatement(Token *comment);
	bool ParseCreateFunction(Token *create, Token *or_, Token *replace, Token *function);
	bool ParseCreateStatement(Token *token, int *result_sets, bool *proc);
	bool ParseCreateTable(Token *create, Token *table);
	bool ParseCreateTablespace(Token *create, Token *tablespace);
	bool ParseCreateIndex(Token *create, Token *unique, Token *index);
	bool ParseCreateTrigger(Token *create, Token *or_, Token *trigger);
	bool ParseCreateTriggerBody(Token *create, Token *name, Token *table, Token *when, Token *insert, Token *update, Token *delete_, Token **end);
	bool ParseCreateTriggerOraclePattern(Token *create, Token *table, Token *when, Token *insert);
	bool ParseCreateType(Token *create, Token *type);
	bool ParseSequenceOptions(Token **start_with, Token **increment_by, StatsSummaryItem &sti);
	bool ParseCreateView(Token *create, Token *materialized, Token *view);
	bool ParseDropStatement(Token *drop);
	bool ParseDropTableStatement(Token *drop, Token *table);
	bool ParseDropTriggerStatement(Token *drop, Token *trigger);
	bool ParseInsertStatement(Token *insert);
	bool ParseSelectStatement(Token *token, int block_scope, int select_scope, int *result_sets, Token **list_end, ListW *exp_starts, ListW *out_cols, ListW *into_cols, int *appended_subquery_aliases, Token **from_end, Token **where_end);
	bool ParseSelectExpressionPattern(Token *open, Token *select); 
	
	// Functions
	bool ParseFunctionAbs(Token *name, Token *open);
	bool ParseFunctionAcos(Token *name, Token *open);
	bool ParseFunctionAddMonths(Token *name);
	bool ParseFunctionAscii(Token *name, Token *open);
	bool ParseFunctionAsciistr(Token *name, Token *open);
	bool ParseFunctionAsin(Token *name, Token *open);
	bool ParseFunctionAtan(Token *name, Token *open);
	bool ParseFunctionAtan2(Token *name);
	bool ParseFunctionAvg(Token *name, Token *open);
	bool ParseFunctionBinToNum(Token *name, Token *open);
	bool ParseFunctionBitand(Token *name, Token *open);
	bool ParseFunctionBitnot(Token *name, Token *open);
	bool ParseFunctionCast(Token *name);
	bool ParseFunctionCeil(Token *name, Token *open);
	bool ParseFunctionChr(Token *name, Token *open);
	bool ParseFunctionCoalesce(Token *name, Token *open);
	bool ParseFunctionConcat(Token *name, Token *open);
	bool ParseFunctionConvert(Token *name);
	bool ParseFunctionCos(Token *name, Token *open);
	bool ParseFunctionCosh(Token *name, Token *open);
	bool ParseFunctionCount(Token *name, Token *open);
	bool ParseFunctionDenseRank(Token *name, Token *open);
	bool ParseFunctionDecode(Token *name, Token *open);
	bool ParseFunctionDeref(Token *name, Token *open);
	bool ParseFunctionEmptyBlob(Token *name, Token *open);
	bool ParseFunctionEmptyClob(Token *name, Token *open);
	bool ParseFunctionEmptyDbclob(Token *name, Token *open);
	bool ParseFunctionEmptyNclob(Token *name, Token *open);
	bool ParseFunctionExp(Token *name, Token *open);
	bool ParseFunctionExtract(Token *name, Token *open);
	bool ParseFunctionFloor(Token *name, Token *open);
	bool ParseFunctionGreatest(Token *name, Token *open);
	bool ParseFunctionHextoraw(Token *name, Token *open);
	bool ParseFunctionInitcap(Token *name, Token *open);
	bool ParseFunctionInstr(Token *name, Token *open);
	bool ParseFunctionInstrb(Token *name, Token *open);
	bool ParseFunctionInteger(Token *name, Token *open);
	bool ParseFunctionLastDay(Token *name, Token *open);
	bool ParseFunctionLeast(Token *name, Token *open);
	bool ParseFunctionLength(Token *name, Token *open);
	bool ParseFunctionLengthb(Token *name, Token *open);
	bool ParseFunctionLn(Token *name, Token *open);
	bool ParseFunctionLog(Token *name, Token *open);
	bool ParseFunctionLower(Token *name, Token *open);
	bool ParseFunctionLpad(Token *name, Token *open);
	bool ParseFunctionLtrim(Token *name, Token *open);
	bool ParseFunctionMod(Token *name, Token *open);
	bool ParseFunctionMonthsBetween(Token *name, Token *open);
	bool ParseFunctionNextDay(Token *name, Token *open);
	bool ParseFunctionNullif(Token *name, Token *open);
	bool ParseFunctionNvl(Token *name, Token *open);
	bool ParseFunctionNvl2(Token *name, Token *open);
	bool ParseFunctionPower(Token *name, Token *open);
	bool ParseFunctionRank(Token *name, Token *open);
	bool ParseFunctionRegexpSubstr(Token *name, Token *open);
	bool ParseFunctionRemainder(Token *name, Token *open);
	bool ParseFunctionReplace(Token *name, Token *open);
	bool ParseFunctionReverse(Token *name, Token *open);
	bool ParseFunctionRound(Token *name, Token *open);
    bool ParseFunctionRowNumber(Token *name, Token *open);
	bool ParseFunctionRpad(Token *name, Token *open);
	bool ParseFunctionRtrim(Token *name, Token *open);
	bool ParseFunctionSign(Token *name, Token *open);
	bool ParseFunctionSin(Token *name, Token *open);
	bool ParseFunctionSinh(Token *name, Token *open);
	bool ParseFunctionSoundex(Token *name, Token *open);
	bool ParseFunctionSqrt(Token *name, Token *open);
	bool ParseFunctionSubstr(Token *name, Token *open);
	bool ParseFunctionSubstr2(Token *name, Token *open);
	bool ParseFunctionSubstrb(Token *name, Token *open);
	bool ParseFunctionSum(Token *name, Token *open);
	bool ParseFunctionSysGuid(Token *name, Token *open);
	bool ParseFunctionTan(Token *name, Token *open);
	bool ParseFunctionTanh(Token *name, Token *open);
	bool ParseFunctionToChar(Token *name, Token *open);
	bool ParseFunctionToClob(Token *name, Token *open);
	bool ParseFunctionToDate(Token *name);
	bool ParseFunctionToLob(Token *name, Token *open);
	bool ParseFunctionToNchar(Token *name);
	bool ParseFunctionToNumber(Token *name, Token *open);
	bool ParseFunctionToSingleByte(Token *name, Token *open);
	bool ParseFunctionToTimestamp(Token *name, Token *open);
	bool ParseFunctionTranslate(Token *name, Token *open);
	bool ParseFunctionTrim(Token *name, Token *open);
	bool ParseFunctionTrunc(Token *name);
	bool ParseFunctionUnistr(Token *name, Token *open);
	bool ParseFunctionUpper(Token *name, Token *open);
	bool ParseFunctionUserenv(Token *name, Token *open);
	bool ParseFunctionVsize(Token *name, Token *open);
	bool ParseFunctionXmlagg(Token *name, Token *open);
	bool ParseFunctionXmlattributes(Token *name, Token *open);
	bool ParseFunctionXmlcast(Token *name, Token *open);
	bool ParseFunctionXmlcdata(Token *name, Token *open);
	bool ParseFunctionXmlcomment(Token *name, Token *open);
	bool ParseFunctionXmlconcat(Token *name, Token *open);
	bool ParseFunctionXmldiff(Token *name, Token *open);
	bool ParseFunctionXmlelement(Token *name, Token *open);
	bool ParseFunctionXmlforest(Token *name, Token *open);
	bool ParseFunctionXmlisvalid(Token *name, Token *open);
	bool ParseFunctionXmlnamespaces(Token *name, Token *open);
	bool ParseFunctionXmlparse(Token *name, Token *open);
	bool ParseFunctionXmlpatch(Token *name, Token *open);
	bool ParseFunctionXmlpi(Token *name, Token *open);
	bool ParseFunctionXmlquery(Token *name, Token *open);
	bool ParseFunctionXmlroot(Token *name, Token *open);
	bool ParseFunctionXmlsequence(Token *name, Token *open);
	bool ParseFunctionXmlserialize(Token *name, Token *open);
	bool ParseFunctionXmltransform(Token *name, Token *open);
	bool ParseUnknownFunction(Token *name, Token *open);
	bool ParseFunctionCurrent(Token *name);
	bool ParseFunctionCurrentDate(Token *name);
	bool ParseFunctionCurrentSchema(Token *name);
	bool ParseFunctionCurrentTimestamp(Token *name);
	bool ParseFunctionInterval(Token *name);
	bool ParseFunctionLocaltimestamp(Token *name);
	bool ParseFunctionNull(Token *name);
	bool ParseFunctionSqlcode(Token *name);
	bool ParseFunctionSqlPercent(Token *name);
	bool ParseFunctionSysdate(Token *name);
	bool ParseFunctionSystimestamp(Token *name);
	bool ParseFunctionUser(Token *name);
	    // Read the data type from available meta information
    const char *GetMetaType(Token *object, Token *column = NULL);

	// Guess functions
	char GuessType(Token *name);
	char GuessType(Token *name, TokenStr &type);

	// Clauses
	bool ParseCreateTableColumns(Token *create, Token *table_name, ListW &pkcols, Token **id_col, Token **id_start, Token **id_inc, bool *id_default, ListWM *inline_indexes, Token **last_colname);
	bool ParseColumnConstraints(Token *column, Token *type, Token *type_end);
	bool ParseDefaultExpression(Token *type, Token *token);
	bool FormatDefaultExpression(Token *datatype, Token *exp, Token *default_);
    bool ParseStandaloneColumnConstraints(Token *alter, ListW &pkcols, ListWM *inline_indexes);
	bool ParseInlineColumnConstraint(Token *type, Token *type_end, Token *constraint, int num);
	bool ParseKeyConstraint(Token *alter, Token *primary, ListW &pkcols, ListWM *inline_indexes);
	bool ParseKeyIndexOptions();
	bool ParseForeignKey(Token *foreign);
	bool ParseCheckConstraint(Token *check);
	bool ParseConstraintOption();
	bool ParseSplEndName(Token *name, Token *end);
	bool ParseOracleVariableDeclarationBlock(Token *declare);
	bool ParseOracleCursorDeclaration(Token *cursor, ListWM *cursors);
	bool ParseOracleObjectType(Token *type);
	bool ParseOracleObjectTypeAssignment(Token *name);
	bool ParseOraclePragma(Token *pragma);
	bool ParseOracleException(Token *name);
	bool ParseFunctionParameters(Token *function_name);
	bool ParseFunctionReturns(Token *function);
	bool ParseSubSelect(Token *open, int select_scope);
	Token* GetNextSelectStartKeyword();
	bool ParseSelectCteClause(Token *with);
	bool ParseSelectList(Token *select, int select_scope, bool *into, bool *dummy_not_required, bool *agg_func, bool *agg_list_func, ListW *exp_starts, ListW *out_cols, ListW *into_cols, Token **rowlimit_slist, bool *rowlimit_percent);
	bool ParseSelectListPredicate(Token **rowlimit_slist, bool *rowlimit_percent);
	bool ParseSelectFromClause(Token *select, bool nested_from, Token **from, Token **from_end, int *appended_subquery_aliases, bool dummy_not_required, ListWM *from_table_end);
	bool ParseJoinClause(Token *first, Token *second, bool first_is_subquery, ListWM *from_table_end);
	bool GetJoinKeywords(Token *token, Token **left_right_full, Token **outer_inner, Token **join);
	bool ParseWhereClause(int stmt_scope, Token **where_, Token **where_end, int *rowlimit = NULL);
	bool ParseWhereCurrentOfCursor(int stmt_scope);
	bool ParseSelectGroupBy();
	bool ParseSelectHaving();
	bool ParseSelectOrderBy(Token **order);
	bool ParseSelectSetOperator(int block_scope, int select_scope);
	bool ParseSelectOptions(Token *select, Token *from_end, Token *where_, Token *order, Token **rowlimit_soptions, int *rowlimit);
	void SelectSetOutColsDataTypes(ListW *out_cols, ListWM *from_table_end);
	void SelectConvertRowlimit(Token *select, Token *from, Token *from_end, Token *where_, Token *where_end, Token *pre_order, Token *order, Token *rowlimit_slist, Token *rowlimit_soptions, int rowlimit, bool rowlimit_percent);
	
	bool ParseTempTableOptions(Token *table_name, Token **start, Token **end, bool *no_data);
	bool ParseStorageClause();
	bool ParseCreateIndexOptions();
	bool OpenWithReturnCursor(Token *name);

	bool ParseOracleStorageClause();
	bool ParseOracleStorageClause(Token *storage);
	bool ParseOracleLobStorageClause(Token *lob);
	bool ParseOraclePartitions(Token *token);
	bool ParseOraclePartitionsBy(Token *token);
	bool ParseOraclePartition(Token *partition, Token *subpartition);
	bool ParseOracleOuterJoin(Token *exp_start, Token *column);
	bool ParseOracleRownumCondition(Token *first, Token *op, Token *second, int *rowlimit);
	bool RecognizeOracleDateFormat(Token *str, TokenStr &format);
	void OracleEmulateIdentity(Token *create, Token *table, Token *column, Token *last, Token *id_start, Token *id_inc, bool id_default);
	void OracleContinueHandlerForFetch(Token *fetch, Token *cursor);
	void OracleContinueHandlerForSelectInto(Token *select);
	void OracleContinueHandlerForUpdate(Token *update);
	void OracleContinueHandlerForInsert(Token *insert);
	void OracleMoveBeginAfterDeclare(Token *create, Token *as, Token *begin, Token *body_start);
	void OracleAddOutRefcursor(Token *create, Token *name, Token *last_param);
	void OracleAppendDataTypeSizes();
	void OracleAppendDataTypeSize(Token *data_type);
	void OracleRemoveDataTypeSize(Token *data_type);
	void OracleTruncateTemporaryTablesWithReplace();
	void OracleMoveSelectOutofIf(Token *stmt_start, Token *select, Token *select_list_end, Token *open, Token *close);
	void OracleIfSelectDeclarations();
	void OracleChangeEqualToDefault(Token *equal);
	bool ParseOracleSetOptions(Token *set);

	bool ParseMysqlStorageClause(Token *table_name, Token **id_start, Token **comment);

	// Add variable declarations generated in the procedural block
	void AddGeneratedVariables();

	// Patterns
	bool SelectNextvalFromDual(Token **sequence, Token **column);

	// Identifiers
	void ConvertIdentifier(Token *token, int expected_type = 0, int scope = 0);
	void ConvertObjectIdentifier(Token *token, int scope = 0);
	void ConvertColumnIdentifier(Token *token, int scope = 0);
    void ConvertColumnIdentifierSingle(Token *token, int scope = 0);
	void ConvertParameterIdentifier(Token *param);
	void ConvertParameterIdentifier(Token *ref, Token *decl);
	void ConvertVariableIdentifier(Token *token);
	void ConvertVariableIdentifier(Token *ref, Token *decl);
	void ConvertSchemaName(Token *token, TokenStr &ident, size_t *len);
	void ConvertObjectName(Token *token, TokenStr &ident, size_t *len);
	bool ConvertCursorParameter(Token *token);
	bool ConvertRecordVariable(Token *token);
	bool ConvertImplicitRecordVariable(Token *token);
	bool ConvertImplicitRecordVariable(Token *token, Token *rec);
	bool ConvertImplicitForRecordVariable(Token *token);
	bool ConvertTriggerNewOldColumn(Token *token);
	bool ConvertTableVariable(Token *token);
	bool ConvertLocalTableColumn(Token *token);
	bool ConvertSessionTemporaryTable(Token *token);
	int GetIdentPartsCount(Token *token);
	void GetNextIdentItem(Token *token, TokenStr &ident, size_t *len);
	void SplitIdentifierByLastPart(Token *token, TokenStr &lead, TokenStr &trail, int parts = -1);
	bool ConvertOraclePseudoColumn(Token *token);
	void ConvertIdentRemoveLeadingPart(Token *token);
	void PrefixPackageName(TokenStr &ident);
	void DiscloseRecordVariables(Token *format);
	void DiscloseImplicitRecordVariables(Token *format);
	bool IsFuncToProc(Token *name);

	// Get position to append new generated declarations
	Token* GetDeclarationAppend(); 
	// Generate cursor name for standalone SELECT returning a result set
	void GenerateResultSetCursorName(TokenStr *name);
	
	// Find a token in list
	Token* Find(ListW &list, Token *what);
	ListwmItem* Find(ListWM &list, Token *what, Token *what2 = NULL);

	// Create a new identifier by appending the word
	Token *AppendIdentifier(Token *source, const char *word, const wchar_t *w_word, size_t len);
	void AppendIdentifier(TokenStr &source, const char *word, const wchar_t *w_word, size_t len);
	void AppendIdentifier(TokenStr &source, TokenStr &append);
	// Split identifier to 2 parts
	void SplitIdentifier(Token *source, Token *first, Token *second);

	// Add /* */ comments around the tokens
	void Comment(Token *first, Token *last = NULL);
	void Comment(const char *word, const wchar_t *w_word, size_t len, Token *first, Token *last);
	void CommentNoSpaces(Token *first, Token *last = NULL);

	// Check whether the token is valid column/table alias
	bool IsValidAlias(Token *token);

	// Check for an aggregate function
	bool IsAggregateFunction(Token *name);
	// Check for LIST aggregate function
	bool IsListAggregateFunction(Token *name);			

	// Define database object name mappings, meta information etc.
	void SetObjectMappingFromFile(const char *file);
	void SetSchemaMapping(const char *mapping);
    void SetMetaFromFile(const char *file);
	void SetFuncToSpMappingFromFile(const char *file);

	// Map object name for identifier
	bool MapObjectName(Token *token);

	// Compare identifiers
	bool CompareIdentifiers(Token *token, const char *str);
	bool CompareIdentifiersExistingParts(Token *first, Token *second);
	bool CompareIdentifierPart(TokenStr &first, TokenStr &second);

	// Set parser level (application, SQL, string i.e.)
	void SetLevel(int level) { _level = level; }

    // Create report file
    int CreateReport(const char *summary); 
};

#endif // sqlines_sqlparser_h
