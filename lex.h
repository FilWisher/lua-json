// token type
typedef enum {
	TT_STR,
	TT_NUM,
	
	TT_COMMA,
	
	// open and close array
	TT_AOPEN,
	TT_ACLOSE,
	
	TT_COLON,
	
	// open and close object
	TT_OOPEN,
	TT_OCLOSE,
	
	TT_TRUE,
	TT_FALSE,
	
	TT_NULL,

	TT_END
} tt;

extern char *tt_strings[];

struct token {
	tt typ;
	
	char *str;
	int len;
	
	int line;
	int col;
};

// TODO: add line info for better errors?
struct lexer {
	const char *str;
	char *pos;
	int len;
	
	int line;
	int col;
};

struct lexer *lex_make(const char *str);
struct token *next_token(struct lexer *l);
