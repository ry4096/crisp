#ifndef INTER_DL_H_
#define INTER_DL_H_


#include "error.h"
#include "inter.h"

error inter_dl_open_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_dl_open_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_dl_error_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_dl_error_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_dl_def_group_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_dl_def_group_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_dl_sym_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_dl_sym_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_dl_close_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_dl_close_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);


void inter_def_dl_funcs(struct inter* inter);
void inter_undef_dl_funcs(struct inter* inter);





#endif // INTER_DL_H_
