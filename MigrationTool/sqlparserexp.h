// Parser exports

#ifndef migrationtool_sqlparserexp_h
#define migrationtool_sqlparserexp_h

// SQL dialect types
#define SQL_ORACLE				2
#define SQL_MYSQL				4

extern void* CreateParserObject();
extern void SetParserTypes(void *parser, short source, short target);
extern int SetParserOption(void *parser, const char *option, const char *value);
extern int ConvertSql(void *parser, const char *input, int size, const char **output, int *out_size, int *lines);
extern void FreeOutput(const char *output);
extern int CreateAssessmentReport(void *parser, const char *summary);

#endif // migrationtool_sqlparserexp_h
