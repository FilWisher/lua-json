#include <assert.h>
#include <stdio.h>

#include "lex.h" 

int main() {
	struct lexer *l = lex_make("[ 'hell', 1234, true, false, null ]");
	
	struct token *tk = next_token(l);
	for (; tk != NULL && tk->typ != TT_END; tk = next_token(l)) {
		printf("%.*s", tk->len, tk->str);
	}
}
