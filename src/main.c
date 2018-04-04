#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <malloc.h>

#include "inter.h"
#include "inter_type.h"
#include "inter_func.h"
#include "inter_dl.h"
#include "parser.h"
#include "token.h"
#include "dlist.h"
#include "refc.h"

#define LOGGING_OFF
#include "log.h"

#define MAIN_CLEAN_UP

char buffer[TOKEN_BUF_LEN];

//#define STACK_SIZE 10240
#define STACK_SIZE 102400

//void debug_symbol(struct inter* inter);


void print_refc_string(FILE* output, char* obj) {
   if (obj) {
      fprintf(output, "%s : %d", obj, refc_count(obj));
   } else {
      fprintf(output, "<NULL>");
   }
}




void usage(char* progname) {
   fprintf(stderr, "usage: %s [-s] [-i] [script]\n"
                   "   -s: force script mode\n"
                   "   -i: force interactive mode\n", progname);
}

int main(int argc, char** argv) {

   struct inter inter;

   int i = 1;
   FILE* input = stdin;
   char* filename = "-";
   int interactive = 1;

   int opt_s = 0;
   int opt_i = 0;

   i = 1;
   while (i < argc) {
      if (argv[i][0] == '-') {
         char* opt = argv[i] + 1;
         while(*opt) {
            switch(*opt) {
               case 'i':
                  opt_i = 1;
                  break;
               case 's':
                  opt_s = 1;
                  break;
               default:
                  usage(argv[0]);
                  return 1;
            }
            opt++;
         }
         i++;
      }
      else {
         if (input == stdin) {
            filename = argv[i];
            interactive = 0;
            //printf("filename = %s\n", filename);
            input = fopen(filename, "r");
            if (!input) {
               fprintf(stderr, "Could not open file: %s\n", filename);
               return 2;
            }
            i++;
         }
         else {
            usage(argv[0]);
            return 1;
         }
      }
   }
   
   if (opt_s) {
      interactive = 0;
   }
   else if (opt_i) {
      interactive = 1;
   }

   char stack_buffer[STACK_SIZE];
   struct token_reader reader;

   inter_init(&inter, stack_buffer, STACK_SIZE);
   inter_def_builtin_types(&inter);
   inter_def_std_funcs(&inter);
   inter_def_dl_funcs(&inter);

   char* filename_str = inter_intern_sym(&inter, filename, strlen(filename));
   refc_inc(filename_str);

   inter_push_abs_path_str(&inter, filename_str, strlen(filename_str));

   error err;

   token_reader_init(&reader, input, filename_str, buffer, TOKEN_BUF_LEN - 1);

   struct ast_node_with_type root;

   err = inter_do_file(&inter, &root, &reader, interactive);
   if (err) {
      (*err)->error_print(stdout, err);
      (*err)->error_free(err);
   }

   /*
   inter_undef_dl_funcs(&inter);
   inter_undef_std_funcs(&inter);
   inter_undef_builtin_types(&inter);
   */

   //debug_symbol(&inter);

#ifdef MAIN_CLEAN_UP
   //hash_set_dump(stderr, &inter.sym_table, (hash_value2_fn)print_refc_string);

   struct filepath_node* node = (struct filepath_node*)dlist_root_pop(&inter.script_path_stack);
   filepath_node_free(node, &inter);

   inter_release_sym(&inter, filename_str);

   inter_destruct(&inter);
#endif

   if (input != stdin) {
      fclose(input);
   }

   return 0;
}
