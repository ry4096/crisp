#include <stdio.h>

#include "token.h"

#define BUF_LEN 1024
char buffer[BUF_LEN];

int main(int argc, char** argv) {

   struct token_reader reader;

   token_reader_init(&reader, stdin, NULL, buffer, BUF_LEN - 1);

   for(;;) {
      int ret = token_reader_next_token(&reader);
      if (!ret) {
         printf("Buffer full\n");
      }

      buffer[reader.tok_len] = '\0';

      printf("%s: %d, %d: %s\n", token_state_strings[reader.state], reader.token_start_line, reader.token_start_column, buffer);

      if (reader.state == TOKEN_STATE_EOF)
         break;
   }

   return 0;
}
