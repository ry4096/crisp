#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <malloc.h>

#include "parser.h"
#include "token.h"
#include "dlist.h"

#define BUF_LEN 1024
char buffer[BUF_LEN];


/*
char* string_conv(void* self, char* data, int length) {
   data[length] = '\0';
   return strdup(data);
}

void string_del(void* self, char* data, int length) {
   if (data) {
      free(data);
   }
}

string_conv_fn string_conv_ = (string_conv_fn)string_conv;
string_del_fn string_del_ = (string_del_fn)string_del;
*/

int main(int argc, char** argv) {

   struct token_reader reader;


   token_reader_init(&reader, stdin, NULL, buffer, BUF_LEN - 1);


   struct ast_node root;
   root.line = -1;
   root.column = -1;
   root.text = NULL;
   root.text_len = 0;
   root.state = TOKEN_STATE_PUNCTUATION;
   dlist_root_init(&root.children);

   union parser_ret parser_ret;

   for(;;) {
      int ret = ast_node_rec(&root, &root, &reader, &string_conv_, sizeof(struct ast_node), &parser_ret);

      ast_node_rec_print(stdout, &root, 0);
      parser_error_print(stdout, &parser_ret);

      ast_node_free_children(&root, &string_del_);

      if (reader.curr == EOF) break;
   }

   return 0;
}
