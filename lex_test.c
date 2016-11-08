#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "lex.h" 

void test_oneline() {
	struct lexer *l = lex_make("[ 'hell', 1234, true, false, null ]");
	
	struct token *tk = next_token(l);
	for (; tk != NULL && tk->typ != TT_END; tk = next_token(l)) {
		printf("%.*s (%d:%d); ", tk->len, tk->str, tk->line, tk->col);
		free(tk);
	}
	printf("\n");
	free(l);
}

void test_multiline() {
	struct lexer *l = lex_make("[ 'hell',\n1234,\ntrue,\nfalse,\nnull\n]");
	
	struct token *tk = next_token(l);
	for (; tk != NULL && tk->typ != TT_END; tk = next_token(l)) {
		printf("%.*s (%d:%d); ", tk->len, tk->str, tk->line, tk->col);
		free(tk);
	}
	printf("\n");
	free(l);
}

int main() {
	test_oneline();
	test_multiline();
}
