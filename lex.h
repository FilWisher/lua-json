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

char *tt_string(tt typ);

struct token {
	tt typ;
	char *str;
	int len;
};

// TODO: add line info for better errors?
struct lexer {
	const char *str;
	char *pos;
	int len;
};

struct lexer *lex_make(const char *str);
struct token *next_token(struct lexer *l);
