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

	TT_END,
	TT_LEN
} tt;

char *tt_string(tt typ);

struct token {
	tt typ;
	char *st;
	int len;
};

struct lexer {
	char *pos;
	int len;
	int i;
	int cap;
	struct token **tks;
};

struct lexer *lex_make();
int lex(struct lexer *lex, char *str);
void lex_delete(struct lexer *lex);
