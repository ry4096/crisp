#include <stdio.h>
#include <assert.h>
#include <ctype.h>

#include "token.h"

#define LOGGING_OFF
#include "log.h"

#define CASE_WHITESPACE \
   case ' ': case '\t': case '\r':

#define CASE_NEWLINE \
   case '\n':

#define CASE_PUNCT \
   case '(': case ')': case '{': case '}': case '[': case ']':


char* token_state_strings[] = {
   "TOKEN_STATE_START",
   "TOKEN_STATE_COMMENT_START",
   "TOKEN_STATE_COMMENT_LINE",
   "TOKEN_STATE_COMMENT_BLOCK",
   "TOKEN_STATE_COMMENT_BLOCK_END1",
   "TOKEN_STATE_COMMENT_BLOCK_EOF_ERROR",
   "TOKEN_STATE_COMMENT_C_START",
   "TOKEN_STATE_COMMENT_C_BLOCK",
   "TOKEN_STATE_COMMENT_C_BLOCK_END1",
   "TOKEN_STATE_PUNCTUATION",
   "TOKEN_STATE_SYMBOL",
   "TOKEN_STATE_STRING",
   "TOKEN_STATE_STRING_EOF_ERROR",
   "TOKEN_STATE_STRING_ESCAPE",
   "TOKEN_STATE_STRING_ESCAPE_HEX1",
   "TOKEN_STATE_STRING_ESCAPE_HEX2",
   "TOKEN_STATE_STRING_ESCAPE_ERROR",
   "TOKEN_STATE_ONE_NEWLINE",
   "TOKEN_STATE_BLANK_LINE",
   "TOKEN_STATE_EOF"
};



void token_reader_init(struct token_reader* reader, FILE* file, char* filename, char* buffer, int buf_len) {
   reader->file = file;
   reader->filename = filename;
   reader->line = 1;
   reader->column = 0;
   reader->buffer = buffer;
   reader->buf_len = buf_len;
   reader->tok_len = 0;
   reader->state = TOKEN_STATE_START;
   reader->curr = fgetc(file);
   if (reader->curr == '\n') {
      reader->line++;
   }
   reader->token_start_line = 0;
   reader->token_start_column = 0;
   reader->hex_value = '\0';
}

void token_reader_start_token(struct token_reader* reader) {
   reader->token_start_line = reader->line;
   reader->token_start_column = reader->column;
}

void token_reader_next_char(struct token_reader* reader) {
   assert(reader->curr != EOF);
   reader->curr = fgetc(reader->file);
   if (reader->curr == '\n') {
      reader->line++;
      reader->column = -1;
   } else {
      reader->column++;
   }
}

int token_reader_eat_custom_char(struct token_reader* reader, char c) {
   assert(reader->curr != EOF);
   if (reader->tok_len < reader->buf_len) {
      reader->buffer[reader->tok_len++] = c;
      token_reader_next_char(reader);
      return 1;
   }
   else {
      return 0;
   }
}

int token_reader_eat_char(struct token_reader* reader) {
   return token_reader_eat_custom_char(reader, (char)reader->curr);
}

int hex_value(char c) {
   if (c >= '0' && c <= '9') {
      return c - '0';
   }
   else if (c >= 'a' && c <= 'f') {
      return 10 + c - 'a';
   }
   else if (c >= 'A' && c <= 'F') {
      return 10 + c - 'A';
   }
   else {
      return -1;
   }
}


void token_reader_log(struct token_reader* reader) {
   printf("token_reader: %d.%d: curr=%c, tok_len=%d, state=%d, start=%d.%d\n",
             reader->line, reader->column, reader->curr, reader->tok_len, reader->state,
             reader->token_start_line, reader->token_start_column);
}


int token_reader_next_token(struct token_reader* reader) {
   reader->state = TOKEN_STATE_START;
   reader->tok_len = 0;
   int hex_ret;

   for(;;) {
#ifdef LOGGING
      token_reader_log(reader);
#endif
      switch(reader->state) {
         case TOKEN_STATE_START:
            switch(reader->curr) {
               CASE_WHITESPACE
                  token_reader_next_char(reader);
                  break;
               CASE_NEWLINE
                  reader->state = TOKEN_STATE_ONE_NEWLINE;
                  token_reader_start_token(reader);
                  token_reader_next_char(reader);
                  break;
               CASE_PUNCT
                  reader->state = TOKEN_STATE_PUNCTUATION;
                  token_reader_start_token(reader);
                  token_reader_eat_char(reader);
                  return 1;
               case '"':
                  reader->state = TOKEN_STATE_STRING;
                  token_reader_start_token(reader);
                  token_reader_next_char(reader);
                  break;
               case '#':
                  reader->state = TOKEN_STATE_COMMENT_START;
                  token_reader_start_token(reader);
                  token_reader_next_char(reader);
                  break;
               case '/':
                  reader->state = TOKEN_STATE_COMMENT_C_START;
                  token_reader_start_token(reader);
                  token_reader_next_char(reader);
                  break;
               case EOF:
                  reader->state = TOKEN_STATE_EOF;
                  token_reader_start_token(reader);
                  return 1;
               default:
                  reader->state = TOKEN_STATE_SYMBOL;
                  token_reader_start_token(reader);
                  if (!token_reader_eat_char(reader)) return 0;
                  break;
            }
            break;

         case TOKEN_STATE_COMMENT_START:
            switch(reader->curr) {
               CASE_NEWLINE
                  reader->state = TOKEN_STATE_START;
                  token_reader_next_char(reader);
                  break;
               case '-':
                  reader->state = TOKEN_STATE_COMMENT_BLOCK;
                  token_reader_next_char(reader);
                  break;
               case EOF:
                  reader->state = TOKEN_STATE_EOF;
                  return 1;
               default:
                  reader->state = TOKEN_STATE_COMMENT_LINE;
                  token_reader_next_char(reader);
                  break;
            }
            break;

         case TOKEN_STATE_COMMENT_LINE:
            switch(reader->curr) {
               CASE_NEWLINE
                  reader->state = TOKEN_STATE_START;
                  token_reader_next_char(reader);
                  break;
               case EOF:
                  reader->state = TOKEN_STATE_EOF;
                  return 1;
               default:
                  token_reader_next_char(reader);
                  break;
            }
            break;

         case TOKEN_STATE_COMMENT_BLOCK:
            switch(reader->curr) {
               case '-':
                  reader->state = TOKEN_STATE_COMMENT_BLOCK_END1;
                  token_reader_next_char(reader);
                  break;
               case EOF:
                  reader->state = TOKEN_STATE_COMMENT_BLOCK_EOF_ERROR;
                  return 1;
               default:
                  token_reader_next_char(reader);
                  break;
            }
            break;

         case TOKEN_STATE_COMMENT_BLOCK_END1:
            switch(reader->curr) {
               case '#':
                  reader->state = TOKEN_STATE_START;
                  token_reader_next_char(reader);
                  break;
               case EOF:
                  reader->state = TOKEN_STATE_COMMENT_BLOCK_EOF_ERROR;
                  return 1;
               default:
                  reader->state = TOKEN_STATE_COMMENT_BLOCK;
                  token_reader_next_char(reader);
                  break;
            }
            break;

         case TOKEN_STATE_COMMENT_BLOCK_EOF_ERROR:
            assert(0);
            return 1;

         case TOKEN_STATE_COMMENT_C_START:
            switch(reader->curr) {
               CASE_NEWLINE CASE_WHITESPACE CASE_PUNCT case EOF:
                  reader->state = TOKEN_STATE_SYMBOL;
                  return 1;
               case '"':
                  reader->state = TOKEN_STATE_STRING;
                  token_reader_next_char(reader);
                  break;
               case '/':
                  reader->state = TOKEN_STATE_COMMENT_LINE;
                  token_reader_next_char(reader);
                  break;
               case '*':
                  reader->state = TOKEN_STATE_COMMENT_C_BLOCK;
                  token_reader_next_char(reader);
                  break;
               default:
                  reader->state = TOKEN_STATE_SYMBOL;
                  if (!token_reader_eat_char(reader)) return 0;
                  break;
            }
            break;

         case TOKEN_STATE_COMMENT_C_BLOCK:
            switch(reader->curr) {
               case '*':
                  reader->state = TOKEN_STATE_COMMENT_C_BLOCK_END1;
                  token_reader_next_char(reader);
                  break;
               case EOF:
                  reader->state = TOKEN_STATE_COMMENT_BLOCK_EOF_ERROR;
                  return 1;
               default:
                  token_reader_next_char(reader);
                  break;
            }
            break;

         case TOKEN_STATE_COMMENT_C_BLOCK_END1:
            switch(reader->curr) {
               case '/':
                  reader->state = TOKEN_STATE_START;
                  token_reader_next_char(reader);
                  break;
               case EOF:
                  reader->state = TOKEN_STATE_COMMENT_BLOCK_EOF_ERROR;
                  return 1;
               default:
                  reader->state = TOKEN_STATE_COMMENT_C_BLOCK;
                  token_reader_next_char(reader);
                  break;
            }
            break;

         case TOKEN_STATE_PUNCTUATION:
            assert(0);
            return 1;

         case TOKEN_STATE_SYMBOL:
            switch(reader->curr) {
               CASE_NEWLINE CASE_WHITESPACE CASE_PUNCT case EOF:
                  return 1;
               case '"':
                  reader->state = TOKEN_STATE_STRING;
                  token_reader_next_char(reader);
                  break;
               default:
                  if (!token_reader_eat_char(reader)) return 0;
                  break;
            }
            break;

         case TOKEN_STATE_STRING:
            switch(reader->curr) {
               case '"':
                  reader->state = TOKEN_STATE_SYMBOL;
                  token_reader_next_char(reader);
                  break;
               case EOF:
                  reader->state = TOKEN_STATE_STRING_EOF_ERROR;
                  return 1;
               case '\\':
                  reader->state = TOKEN_STATE_STRING_ESCAPE;
                  token_reader_next_char(reader);
                  break;
               default:
                  if (!token_reader_eat_char(reader)) return 0;
                  break;
            }
            break;

         case TOKEN_STATE_STRING_EOF_ERROR:
            assert(0);
            return 1;

         case TOKEN_STATE_STRING_ESCAPE:
            switch(reader->curr) {
               case 'n':
                  reader->state = TOKEN_STATE_STRING;
                  if (!token_reader_eat_custom_char(reader, '\n')) return 0;
                  break;
               case 't':
                  reader->state = TOKEN_STATE_STRING;
                  if (!token_reader_eat_custom_char(reader, '\t')) return 0;
                  break;
               case 'a':
                  reader->state = TOKEN_STATE_STRING;
                  if (!token_reader_eat_custom_char(reader, '\a')) return 0;
                  break;
               case 'r':
                  reader->state = TOKEN_STATE_STRING;
                  if (!token_reader_eat_custom_char(reader, '\r')) return 0;
                  break;
               case EOF:
                  reader->state = TOKEN_STATE_STRING_EOF_ERROR;
                  return 1;
               case 'x':
                  reader->state = TOKEN_STATE_STRING_ESCAPE_HEX1;
                  break;
               default:
                  if (!token_reader_eat_char(reader)) return 0;
                  reader->state = TOKEN_STATE_STRING;
                  break;
            }
            break;

         case TOKEN_STATE_STRING_ESCAPE_HEX1:
            if (reader->curr == EOF) {
               reader->state = TOKEN_STATE_STRING_EOF_ERROR;
               return 1;
            }
            else {
               hex_ret = hex_value(reader->curr);
               if (hex_ret == -1) {
                  reader->state = TOKEN_STATE_STRING_ESCAPE_ERROR;
                  return 1;
               }
               else {
                  reader->hex_value = hex_ret << 4;
                  reader->state = TOKEN_STATE_STRING_ESCAPE_HEX2;
                  token_reader_next_char(reader);
               }
            }
            break;

         case TOKEN_STATE_STRING_ESCAPE_HEX2:
            if (reader->curr == EOF) {
               reader->state = TOKEN_STATE_STRING_EOF_ERROR;
               return 1;
            }
            else {
               hex_ret = hex_value(reader->curr);
               if (hex_ret == -1) {
                  reader->state = TOKEN_STATE_STRING_ESCAPE_ERROR;
                  return 1;
               }
               else {
                  reader->hex_value = reader->hex_value | hex_ret;
                  reader->state = TOKEN_STATE_STRING;
                  if (!token_reader_eat_custom_char(reader, reader->hex_value)) return 0;
               }
            }
            break;

         case TOKEN_STATE_STRING_ESCAPE_ERROR:
            assert(0);
            return 1;

         case TOKEN_STATE_ONE_NEWLINE:
            switch(reader->curr) {
               CASE_WHITESPACE
                  token_reader_next_char(reader);
                  break;

               CASE_NEWLINE
                  reader->state = TOKEN_STATE_BLANK_LINE;
                  return 1;

               default:
                  reader->state = TOKEN_STATE_START;
                  token_reader_start_token(reader);
                  break;
            }
            break;

         case TOKEN_STATE_BLANK_LINE:
            assert(0);
            return 1;

         case TOKEN_STATE_EOF:
            assert(0);
            return 1;
      }
   }
}



// For printing tokens
char hex_char(char num) {
   if (num < 10) {
      return '0' + num;
   }
   else {
      return 'a' + num;
   }
}

void print_hex(char c, FILE* output) {
   fputc(hex_char(c >> 4), output);
   fputc(hex_char(c & 0x0F), output);
}

void print_string_char(char c, FILE* output) {
   if (isprint(c)) {
      fputc(c, output);
   }
   else {
      fputc('\\', output);

      switch(c) {
         case '\n':
            fputc('n', output);
            break;
         case '\t':
            fputc('t', output);
            break;
         case '\a':
            fputc('a', output);
            break;
         case '\r':
            fputc('r', output);
            break;
         default:
            fputc('x', output);
            print_hex(c, output);
      }
   }
}


void token_print(FILE* output, char* str, int str_len) {
   int quotes_needed = 0;

   int i;
   for (i = 0; i < str_len; i++) {
      if (!isgraph(str[i])) {
         quotes_needed = 1;
         break;
      }
   }

   if (!quotes_needed) {
      fwrite(str, str_len, 1, output);
   }
   else {
      fputc('"', output);

      for (i = 0; i < str_len; i++) {
         print_string_char(str[i], output);
      }

      fputc('"', output);
   }
}


