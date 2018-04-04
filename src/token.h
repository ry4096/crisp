#ifndef TOKEN_H_
#define TOKEN_H_

#include <stdio.h>

// Tokens shouldn't be longer than this.
#define TOKEN_BUF_LEN 1024

enum token_state {
   TOKEN_STATE_START = 0,
   TOKEN_STATE_COMMENT_START,
   TOKEN_STATE_COMMENT_LINE,
   TOKEN_STATE_COMMENT_BLOCK,
   TOKEN_STATE_COMMENT_BLOCK_END1,
   TOKEN_STATE_COMMENT_BLOCK_EOF_ERROR,
   TOKEN_STATE_COMMENT_C_START,
   TOKEN_STATE_COMMENT_C_BLOCK,
   TOKEN_STATE_COMMENT_C_BLOCK_END1,
   TOKEN_STATE_PUNCTUATION,
   TOKEN_STATE_SYMBOL,
   TOKEN_STATE_STRING,
   TOKEN_STATE_STRING_EOF_ERROR,
   TOKEN_STATE_STRING_ESCAPE,
   TOKEN_STATE_STRING_ESCAPE_HEX1,
   TOKEN_STATE_STRING_ESCAPE_HEX2,
   TOKEN_STATE_STRING_ESCAPE_ERROR,
   TOKEN_STATE_ONE_NEWLINE,
   TOKEN_STATE_BLANK_LINE,
   TOKEN_STATE_EOF
};

extern char* token_state_strings[];


struct token_reader {
   FILE* file;
   char* filename;
   int line;
   int column;
   char* buffer;
   int buf_len;
   int tok_len;
   enum token_state state;
   int curr;
   int token_start_line;
   int token_start_column;
   char hex_value;
};

void token_reader_init(struct token_reader* reader, FILE* file, char* filename, char* buffer, int buf_len);

void token_reader_start_token(struct token_reader* reader);

void token_reader_restart_token(struct token_reader* reader);

void token_reader_next_char(struct token_reader* reader);

int token_reader_eat_custom_char(struct token_reader* reader, char c);

int token_reader_eat_char(struct token_reader* reader);

int hex_value(char c);

int token_reader_next_token(struct token_reader* reader);

char hex_char(char num);
void print_hex(char c, FILE* output);
void print_string_char(char c, FILE* output);
void token_print(FILE* output, char* str, int str_len);


#endif
