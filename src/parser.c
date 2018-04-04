#include <stdio.h>
#include <assert.h>
#include <malloc.h>
#include <string.h>

#include "dlist.h"
#include "token.h"
#include "parser.h"
#include "refc.h"


#define LOGGING_OFF
#include "log.h"


char* string_conv(void* self, char* data, int length) {
   //data[length] = '\0';
   //return strdup(data);
   char* copy = (char*)refc_alloc(length + 1);
   assert(copy);
   copy[length] = '\0';
   memcpy(copy, data, length);
   return copy;
}

void string_del(void* self, char* data, int length) {
   //if (data) {
   //   free(data);
   //}
   if (data) {
      refc_dec_free(data);
   }
}

string_conv_fn string_conv_ = (string_conv_fn)string_conv;
string_del_fn string_del_ = (string_del_fn)string_del;


void ast_node_from_token_reader(struct ast_node* node, struct token_reader* reader, string_conv_fn* str_conv) {
   node->filename = reader->filename != NULL
      ? (char*)refc_copy(reader->filename)
      : NULL;
   node->line = reader->token_start_line;
   node->column = reader->token_start_column;
   node->text = (*str_conv)(str_conv, reader->buffer, reader->tok_len);
   node->text_len = reader->tok_len;
   node->state = reader->state;
   dlist_root_init(&node->children);
   dlist_init(&node->node);
}

char closing_bracket(char open_bracket) {
   assert(open_bracket == '(' || open_bracket == '[' || open_bracket == '{');
   switch(open_bracket) {
      case '(': return ')';
      case '[': return ']';
      case '{': return '}';
   }
}


struct ast_node* ast_node_make_child(struct ast_node* parent, struct token_reader* reader, string_conv_fn* str_conv, int struct_size) {
   struct ast_node* node = (struct ast_node*)refc_alloc(struct_size);
   assert(node);
   memset(node, 0, struct_size);
   ast_node_from_token_reader(node, reader, str_conv);
   dlist_root_append(&parent->children, &node->node);
   return node;
}

int ast_node_rec(struct ast_node* root,
                 struct ast_node* open_bracket,
                 struct token_reader* reader,
                 string_conv_fn* str_conv,
                 int node_struct_size,
                 union parser_ret* parser_ret) {
   for(;;) {
      int ret = token_reader_next_token(reader);

      if (reader->state == TOKEN_STATE_EOF) {
         if (open_bracket->text == NULL) {
            parser_ret->code = PARSER_SUCCESS;
            parser_ret->success.root = root;
            return 0;
         }
         else {
            parser_ret->code = PARSER_ERROR_BRACKET_EOF;
            parser_ret->bracket_eof.root = root;
            parser_ret->bracket_eof.open_bracket = open_bracket;
            return 0;
         }
      }
      else if (reader->state == TOKEN_STATE_COMMENT_BLOCK_EOF_ERROR ||
               reader->state == TOKEN_STATE_STRING_EOF_ERROR ||
               reader->state == TOKEN_STATE_STRING_ESCAPE_ERROR ||
               !ret) {
         parser_ret->code = PARSER_ERROR_TOKEN_ERROR;
         parser_ret->token_error.root = root;
         parser_ret->token_error.bad_token = ast_node_make_child(open_bracket, reader, str_conv, node_struct_size);
         return 0;
      }
      else if (reader->state == TOKEN_STATE_PUNCTUATION) {
         assert(reader->tok_len > 0);
         if (*(reader->buffer) == '(' ||
             *(reader->buffer) == '[' ||
             *(reader->buffer) == '{') { 
            ret = ast_node_rec(
                  root,
                  ast_node_make_child(open_bracket, reader, str_conv, node_struct_size),
                  reader,
                  str_conv,
                  node_struct_size,
                  parser_ret);

            if (!ret)
               return 0;
         }
         else {
            int brackets_match = 1;
            if (open_bracket->text == NULL) {
               brackets_match = 0;
            }
            else {
               char expected_closing_bracket = closing_bracket(*(open_bracket->text));
               brackets_match = (expected_closing_bracket == *(reader->buffer));
            }

            if (!brackets_match) {
               parser_ret->code = PARSER_ERROR_MISMATCHED_BRACKET;
               parser_ret->mismatched_bracket.root = root;
               parser_ret->mismatched_bracket.open_bracket = open_bracket;
               parser_ret->mismatched_bracket.closing_bracket = ast_node_make_child(open_bracket, reader, str_conv, node_struct_size);
               return 0;
            }

            parser_ret->code = PARSER_SUCCESS;
            parser_ret->success.root = root;
            return 1;
         }
      }
      else if (reader->state == TOKEN_STATE_BLANK_LINE) {
         if (open_bracket->text == NULL) {
            // Terminate null brackets with blank lines.
            parser_ret->code = PARSER_SUCCESS;
            parser_ret->success.root = root;
            return 1;
         }
      }
      else {
         ast_node_make_child(open_bracket, reader, str_conv, node_struct_size);
      }
   }
}


int ast_node_root(struct token_reader* reader,
                  string_conv_fn* str_conv,
                  int node_struct_size,
                  struct ast_node* root,
                  union parser_ret* parser_ret) {
   int ret = token_reader_next_token(reader);

   ast_node_from_token_reader(root, reader, str_conv);

   if (reader->state == TOKEN_STATE_EOF) {
      parser_ret->code = PARSER_SUCCESS;
      parser_ret->success.root = root;
      return 0;
   }
   else if (reader->state == TOKEN_STATE_COMMENT_BLOCK_EOF_ERROR ||
            reader->state == TOKEN_STATE_STRING_EOF_ERROR ||
            reader->state == TOKEN_STATE_STRING_ESCAPE_ERROR ||
            !ret) {
      parser_ret->code = PARSER_ERROR_TOKEN_ERROR;
      parser_ret->token_error.root = NULL;
      parser_ret->token_error.bad_token = root;
      return 0;
   }
   else if (reader->state == TOKEN_STATE_PUNCTUATION) {
      assert(reader->tok_len > 0);
      if (*(reader->buffer) == '(' ||
          *(reader->buffer) == '[' ||
          *(reader->buffer) == '{') { 
         ret = ast_node_rec(
               root,
               root,
               reader,
               str_conv,
               node_struct_size,
               parser_ret);

         if (!ret)
            return 0;
      }
      else {
         assert(*(reader->buffer) == ')' ||
                *(reader->buffer) == ']' ||
                *(reader->buffer) == '}');

         parser_ret->code = PARSER_ERROR_MISMATCHED_BRACKET;
         parser_ret->extra_closing_bracket.node = root;
         return 0;
      }
   }
   else {
      parser_ret->code = PARSER_SUCCESS;
      parser_ret->success.root = root;
      return 1;
   }
}
 


void ast_node_rec_free(struct ast_node* root, string_del_fn* del_fn) {
   if (refc_dec(root))
      return;

   if (del_fn) {
      if (root->text)
         (*del_fn)(del_fn, root->text, root->text_len);

      if (root->filename)
         (*del_fn)(del_fn, root->filename, strlen(root->filename));
   }

   while(!dlist_root_empty(&root->children)) {
      struct dlist* curr = dlist_root_pop(&root->children);

      struct ast_node* ast_node = (struct ast_node*)curr;
      ast_node_rec_free(ast_node, del_fn);
   }

   refc_free(root);
}

void ast_node_free_children(struct ast_node* root, string_del_fn* del_fn) {
   while(!dlist_root_empty(&root->children)) {
      struct dlist* curr = dlist_root_pop(&root->children);

      struct ast_node* ast_node = (struct ast_node*)curr;
      ast_node_rec_free(ast_node, del_fn);
   }
}

// This assumes root->text is null terminated.
void ast_node_rec_print(FILE* output, struct ast_node* root, int indent) {
   int index;
   for(index = 0; index < indent; index++) {
      fprintf(output, "  ");
   }

   fprintf(output, "%s: %d, %d: %s\n",
         token_state_strings[root->state],
         root->line,
         root->column,
         root->text);

   struct ast_node* start = (struct ast_node*)(root->children.entry_node);
   if (start) {
      struct ast_node* curr = start;
      do {
         ast_node_rec_print(output, curr, indent + 1);
         curr = (struct ast_node*)(curr->node.next);
      } while(curr != start);
   }
}

void parser_error_print(FILE* output, union parser_ret* parser_ret) {
   switch(parser_ret->code) {
      case PARSER_SUCCESS:
         fprintf(output, "Success\n");
         break;

      case PARSER_ERROR_TOKEN_ERROR:
         fprintf(output, "%d,%d: Token_error: %s: %s\n",
               parser_ret->token_error.bad_token->line,
               parser_ret->token_error.bad_token->column,
               token_state_strings[parser_ret->token_error.bad_token->state],
               parser_ret->token_error.bad_token->text);
         break;

      case PARSER_ERROR_MISMATCHED_BRACKET:
         fprintf(output, "Mismatched Brackets: Open: %d,%d: %s. Closing: %d,%d, %s\n",
               parser_ret->mismatched_bracket.open_bracket->line,
               parser_ret->mismatched_bracket.open_bracket->column,
               parser_ret->mismatched_bracket.open_bracket->text,
               parser_ret->mismatched_bracket.closing_bracket->line,
               parser_ret->mismatched_bracket.closing_bracket->column,
               parser_ret->mismatched_bracket.closing_bracket->text);
         break;

      case PARSER_ERROR_EXTRA_CLOSING_BRACKET:
         fprintf(output, "Extra Closing Bracket: %d,%d, %s\n",
               parser_ret->extra_closing_bracket.node->line,
               parser_ret->extra_closing_bracket.node->column,
               parser_ret->extra_closing_bracket.node->text);
         break;

      case PARSER_ERROR_BRACKET_EOF:
         fprintf(output, "%d,%d: Bracket Unmatched: %s\n",
               parser_ret->bracket_eof.open_bracket->line,
               parser_ret->bracket_eof.open_bracket->column,
               parser_ret->bracket_eof.open_bracket->text);
         break;
   }
}


void ast_node_rec_call(struct ast_node* root, ast_node_call_fn callback) {
   callback(root);

   struct ast_node* first = (struct ast_node*)root->children.entry_node;

   if (first) {
      struct ast_node* curr = first;
      do {
         ast_node_rec_call(curr, callback);
         curr = (struct ast_node*)curr->node.next;
      } while(curr != first);
   }
}

void ast_node_print(FILE* output, struct ast_node* root) {
   if (root) {
      token_print(output, root->text, root->text_len);

      struct ast_node* first = (struct ast_node*)root->children.entry_node;
      if (root->state == TOKEN_STATE_PUNCTUATION) {
         if (first) {
            ast_node_print(output, first);

            struct ast_node* curr = (struct ast_node*)first->node.next;
            while (curr != first) {
               fputc(' ', output);
               ast_node_print(output, curr);
               curr = (struct ast_node*)curr->node.next;
            }
         }

         if (root->text_len == 0) {
            // The root was a dummy node.
            // Assume newline delimitation.
            fputc('\n', output);
         }
         else {
            assert(root->text_len == 1);
            char closing = closing_bracket(root->text[0]);
            fputc(closing, output);
         }
      }
   } else {
      fprintf(output, "NULL");
   }
}


