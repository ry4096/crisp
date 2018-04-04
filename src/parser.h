#ifndef PARSER_H_
#define PARSER_H_

#include <stdio.h>

#include "dlist.h"
#include "token.h"

struct ast_node {
   struct dlist node;
   char* filename;
   int line;
   int column;
   char* text;
   int text_len;
   enum token_state state;
   struct dlist_root children;
};

typedef char* (*string_conv_fn)(void* self, char* str, int length);
typedef void (*string_del_fn)(void* self, char* str, int length);



char* string_conv(void* self, char* data, int length);
void string_del(void* self, char* data, int length);

extern string_conv_fn string_conv_;
extern string_del_fn string_del_;




void ast_node_from_token_reader(struct ast_node* node, struct token_reader* reader, string_conv_fn* str_conv);

enum parser_error_code {
   PARSER_SUCCESS = 0,
   PARSER_ERROR_TOKEN_ERROR,
   PARSER_ERROR_MISMATCHED_BRACKET,
   PARSER_ERROR_EXTRA_CLOSING_BRACKET,
   PARSER_ERROR_BRACKET_EOF
};

struct parser_success {
   enum parser_error_code code;
   struct ast_node* root;
};

struct parser_token_error {
   enum parser_error_code code;
   struct ast_node* root;
   struct ast_node* bad_token;
};

struct parser_mismatched_bracket {
   enum parser_error_code code;
   struct ast_node* root;
   struct ast_node* open_bracket;
   struct ast_node* closing_bracket;
};

struct parser_extra_closing_bracket {
   enum parser_error_code code;
   struct ast_node* node;
};

struct parser_bracket_eof {
   enum parser_error_code code;
   struct ast_node* root;
   struct ast_node* open_bracket;
};

union parser_ret {
   enum parser_error_code code;
   struct parser_success success;
   struct parser_token_error token_error;
   struct parser_mismatched_bracket mismatched_bracket;
   struct parser_extra_closing_bracket extra_closing_bracket;
   struct parser_bracket_eof bracket_eof;
};


char closing_bracket(char open_bracket);

struct ast_node* ast_node_make_child(struct ast_node* parent, struct token_reader* reader, string_conv_fn* str_conv, int struct_size);

int ast_node_rec(struct ast_node* root,
                 struct ast_node* open_bracket,
                 struct token_reader* reader,
                 string_conv_fn* str_conv,
                 int node_struct_size,
                 union parser_ret* parser_ret);

int ast_node_root(struct token_reader* reader,
                  string_conv_fn* str_conv,
                  int node_struct_size,
                  struct ast_node* root,
                  union parser_ret* parser_ret);
 

void ast_node_rec_free(struct ast_node* root, string_del_fn* del_fn);
void ast_node_rec_print(FILE* output, struct ast_node* root, int indent);
void parser_error_print(FILE* output, union parser_ret* ret);

typedef void (*ast_node_call_fn)(struct ast_node* node);
void ast_node_rec_call(struct ast_node* root, ast_node_call_fn callback);
void ast_node_print(FILE* output, struct ast_node* root);

#endif
