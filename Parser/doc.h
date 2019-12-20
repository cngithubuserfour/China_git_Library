// Conversion notes and documentation links

#ifndef sqlexec_doc_h
#define sqlexec_doc_h

#define SQL_STMT_COMMENT_ON_COLUMN_DESC         "Comment on column"
#define SQL_STMT_CREATE_SEQUENCE_DESC           "Create sequence"
#define SQL_STMT_CREATE_MATERIALIZED_VIEW_DESC  "Create materialized view"
#define SQL_STMT_CREATE_VIEW_DESC               "Create view"
#define SQL_STMT_CREATE_PROC_DESC               "Create stored procedure"
#define SQL_STMT_SELECT_DESC                    "Execute a query"

#define SQL_STMT_SELECT_ORAJOIN                 "Oracle outer join (+) syntax"

#define PLSQL_STMT_EXCEPTION_BLOCK_DESC    "Exception block"
#define PLSQL_STMT_EXECUTE_IMMEDIATE_DESC  "Execute a dynamic SQL"
#define PLSQL_STMT_EXIT_DESC               "Exit from loop"
#define PLSQL_STMT_FOR_LOOP_DESC           "FOR loop statement"
#define PLSQL_STMT_IF_DESC                 "IF statement"
#define PLSQL_STMT_NULL_DESC               "No operation statement"
#define PLSQL_STMT_OPEN_DESC               "Open a cursor"
#define PLSQL_STMT_PREPARE_DESC            "Prepare a dynamic SQL"
#define PLSQL_STMT_RAISE_DESC              "Raise an exception" 
#define PLSQL_STMT_RETURN_DESC             "Return from procedure or function"
#define PLSQL_STMT_WHILE_DESC              "WHILE loop statement"

#define PLSQL_STMT_EXCEPTION_ACCESS_INTO_NULL_DESC        "Assign attribute of NULL object" 
#define PLSQL_STMT_EXCEPTION_CASE_NOT_FOUND_DESC          "No condition met in CASE"
#define PLSQL_STMT_EXCEPTION_COLLECTION_IS_NULL_DESC      "Assigning to NULL collection"
#define PLSQL_STMT_EXCEPTION_CURSOR_ALREADY_OPEN_DESC     "Cursor already open"
#define PLSQL_STMT_EXCEPTION_DUP_VAL_ON_INDEX_DESC 	      "Insert duplicate values"
#define PLSQL_STMT_EXCEPTION_INVALID_CURSOR_DESC 	      "Operation on non-open cursor"
#define PLSQL_STMT_EXCEPTION_INVALID_NUMBER_DESC 	      "String does not represent valid number"
#define PLSQL_STMT_EXCEPTION_LOGIN_DENIED_DESC 		      "Invalid user name or password"
#define PLSQL_STMT_EXCEPTION_NO_DATA_FOUND_DESC 	      "SELECT INTO returns no rows"
#define PLSQL_STMT_EXCEPTION_NO_DATA_NEEDED_DESC 	      "Early exit from pipelined function"
#define PLSQL_STMT_EXCEPTION_NOT_LOGGED_ON_DESC 	      "Not connected"
#define PLSQL_STMT_EXCEPTION_OTHERS_DESC			      "All other conditions"
#define PLSQL_STMT_EXCEPTION_PROGRAM_ERROR_DESC 	      "Internal error"
#define PLSQL_STMT_EXCEPTION_ROWTYPE_MISMATCH_DESC 	      "Incompatible types"
#define PLSQL_STMT_EXCEPTION_SELF_IS_NULL_DESC 		      "Invoke method of NULL instance"
#define PLSQL_STMT_EXCEPTION_STORAGE_ERROR_DESC 	      "Out of memory"
#define PLSQL_STMT_EXCEPTION_SUBSCRIPT_BEYOND_COUNT_DESC  "Index number too large"
#define PLSQL_STMT_EXCEPTION_SUBSCRIPT_OUTSIDE_LIMIT_DESC "Index number is out of range"
#define PLSQL_STMT_EXCEPTION_SYS_INVALID_ROWID_DESC       "Invalid rowid"
#define PLSQL_STMT_EXCEPTION_TIMEOUT_ON_RESOURCE_DESC     "Timeout"
#define PLSQL_STMT_EXCEPTION_TOO_MANY_ROWS_DESC 	      "SELECT INTO returns more than one row"
#define PLSQL_STMT_EXCEPTION_VALUE_ERROR_DESC 		      "Expression error"
#define PLSQL_STMT_EXCEPTION_ZERO_DIVIDE_DESC             "Divide a number by zero"

#define SEQUENCE_CACHE_DESC         "Number of sequence values to cache"
#define SEQUENCE_CURRVAL_DESC       "The current value of sequence"
#define SEQUENCE_CYCLE_DESC         "Reuse values after reaching the limit"
#define SEQUENCE_INCREMENT_BY_DESC  "Positive or negative increment"
#define SEQUENCE_MINVALUE_DESC      "Minimum value"
#define SEQUENCE_MAXVALUE_DESC      "Maximum value"
#define SEQUENCE_NEXTVAL_DESC       "The next value of sequence"
#define SEQUENCE_NOCACHE_DESC       "Do not preallocate sequence values"
#define SEQUENCE_NOCYCLE_DESC       "Do not reuse values after reaching the limit"
#define SEQUENCE_NOMINVALUE_DESC    "No minimum value"
#define SEQUENCE_NOMAXVALUE_DESC    "No maximum value"

#define SEQUENCE_NOORDER_DESC    "No need to guarantee sequence numbers in order of requests" 
#define SEQUENCE_NOORDER_CONV    "Option is not supported, removed as it is default" 

#define SEQUENCE_ORDER_DESC    "Guarantee sequence numbers in order of requests" 
#define SEQUENCE_ORDER_CONV    "Option is not supported, commented" 
#define SEQUENCE_ORDER_URL     "sequence_order"

#define SEQUENCE_START_WITH_DESC    "Initial value" 

#define SQL_PKG_DBMS_OUTPUT_DESC    "Send messages and print debug information"

#define SQL_PKG_DBMS_OUTPUT_PUT_LINE_DESC    "Place line in the buffer or print line"

#endif // sqlexec_doc_h
