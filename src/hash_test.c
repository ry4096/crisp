#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <assert.h>

#include "hash_table.h"

char* get_string(struct hash_table* hash, char* buffer) {
   char** slot = (char**)hash_set_slot(hash, buffer, NULL);
   if (!*slot) {
      *slot = strdup(buffer);
      assert(*slot);
   }

   return *slot;
}

#define BUF_SIZE 1024

int main(int argc, char** argv) {
   char buffer[BUF_SIZE];
   struct hash_table sym_table;
   struct hash_table map;

   hash_table_init(&sym_table, &hash_string_obj, 0);
   hash_table_init(&map, &hash_pointer_obj, 0);

   char* get = get_string(&sym_table, "get");
   char* set = get_string(&sym_table, "set");
   char* delete = get_string(&sym_table, "delete");

   while(fgets(buffer, BUF_SIZE, stdin) != NULL) {

      /*
      printf("sym_table:\n");
      hash_set_dump(stdout, &sym_table, (hash_value2_fn)print_string);
      printf("map:\n");
      hash_map_dump(stdout, &map, (hash_value2_fn)print_string, (hash_value2_fn)print_string);
      */

      char* newline = strchr(buffer, '\n');
      if (newline) {
         *newline = '\0';
      }

      char* delim = " \t";
      char* tok = strtok(buffer, delim);

      if (tok != NULL) {
         char* command = hash_set_find(&sym_table, tok, NULL);
         if (command == get) {
            tok = strtok(NULL, delim);
            if (!tok) {
               fprintf(stderr, "Expected Key\n");
            } else {
               char* key = get_string(&sym_table, tok);
               char* value = (char*)hash_map_find_value(&map, key, NULL);
               if (value) {
                  printf("%s = %s\n", key, value);
               } else {
                  printf("%s is undefined\n", key);
               }
            }
         } else if (command == set) {
            tok = strtok(NULL, delim);
            if (!tok) {
               fprintf(stderr, "Expected Key\n");
            } else {
               char* key = get_string(&sym_table, tok);
               tok = strtok(NULL, delim);
               if (!tok) {
                  fprintf(stderr, "Expected Value\n");
               } else {
                  char* value = get_string(&sym_table, tok);
                  char* old = hash_map_set(&map, key, value);
               }
            }
         } else if (command == delete) {
            tok = strtok(NULL, delim);
            if (!tok) {
               fprintf(stderr, "Expected Key\n");
            } else {
               char* key = get_string(&sym_table, tok);
               struct key_value* old = hash_map_remove(&map, key, NULL);
               if (old) {
                  //printf("Deleted: %s = %s\n", old->key, old->value);
                  free(old);
               } else {
                  printf("%s is undefined\n", key);
               }
            }
         } else {
            fprintf(stderr, "Command unknown: %s\n", tok);
         }
      }
   }


   hash_set_for_each(&map, (hash_value_fn)free);
   hash_table_destruct(&map);

   hash_set_for_each(&sym_table, (hash_value_fn)free);
   hash_table_destruct(&sym_table);

   return 0;
}
