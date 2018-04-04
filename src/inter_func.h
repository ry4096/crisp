#ifndef INTER_FUNC_H_
#define INTER_FUNC_H_

#include "inter.h"
#include "error.h"


error inter_func_check_args(struct inter* inter, type expected_type, struct ast_node_with_type* root);

error inter_check_alloc_node(struct ast_node_with_type* alloc_node);
void* inter_eval_alloc_node(struct inter* inter, struct ast_node_with_type* alloc_node, int alloc_size, error* err);
void inter_do_free_alloc_node(struct inter* inter, struct ast_node_with_type* alloc_node, struct typed_value* alloc);

error inter_func_print_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_char_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_func_char_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_uchar_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_func_uchar_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_string_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_func_string_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_if_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_func_if_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_if_2_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_if_1_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_defined_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_func_defined_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_def_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_func_def_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_undef_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_func_undef_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_set_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_func_set_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);



#if 0
error inter_func_num_p_op_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_func_num_op_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_func_int_op_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
#endif



error inter_func_sum_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_func_sub_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_func_mul_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_func_div_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_func_mod_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_func_bit_and_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_func_bit_or_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_func_bit_not_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_func_shift_left_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_func_shift_right_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
 



//error inter_func_cmp_op_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_func_gt_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_func_g_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_func_lt_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_func_l_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);



error inter_func_sum_eval_char(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_sum_eval_uchar(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_sum_eval_short(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_sum_eval_ushort(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_sum_eval_int(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_sum_eval_uint(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_sum_eval_long(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_sum_eval_ulong(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_sum_eval_long_long(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_sum_eval_ulong_long(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_sum_eval_float(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_sum_eval_double(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_sum_eval_long_double(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_sum_eval_p_char(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_sum_eval_p_uchar(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_sum_eval_p_short(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_sum_eval_p_ushort(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_sum_eval_p_int(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_sum_eval_p_uint(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_sum_eval_p_long(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_sum_eval_p_ulong(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_sum_eval_p_long_long(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_sum_eval_p_ulong_long(struct inter* inter, struct ast_node_with_type* root, void* value, int size);



error inter_func_sum_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);



error inter_func_sub_eval_char(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_sub_eval_uchar(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_sub_eval_short(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_sub_eval_ushort(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_sub_eval_int(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_sub_eval_uint(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_sub_eval_long(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_sub_eval_ulong(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_sub_eval_long_long(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_sub_eval_ulong_long(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_sub_eval_float(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_sub_eval_double(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_sub_eval_long_double(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_sub_eval_p_char(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_sub_eval_p_uchar(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_sub_eval_p_short(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_sub_eval_p_ushort(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_sub_eval_p_int(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_sub_eval_p_uint(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_sub_eval_p_long(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_sub_eval_p_ulong(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_sub_eval_p_long_long(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_sub_eval_p_ulong_long(struct inter* inter, struct ast_node_with_type* root, void* value, int size);

error inter_func_sub_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);


error inter_func_mul_eval_char(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_mul_eval_uchar(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_mul_eval_short(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_mul_eval_ushort(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_mul_eval_int(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_mul_eval_uint(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_mul_eval_long(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_mul_eval_ulong(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_mul_eval_long_long(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_mul_eval_ulong_long(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_mul_eval_float(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_mul_eval_double(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_mul_eval_long_double(struct inter* inter, struct ast_node_with_type* root, void* value, int size);

error inter_func_mul_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);


error inter_func_div_eval_char(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_div_eval_uchar(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_div_eval_short(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_div_eval_ushort(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_div_eval_int(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_div_eval_uint(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_div_eval_long(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_div_eval_ulong(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_div_eval_long_long(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_div_eval_ulong_long(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_div_eval_float(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_div_eval_double(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_div_eval_long_double(struct inter* inter, struct ast_node_with_type* root, void* value, int size);

error inter_func_div_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);


error inter_func_mod_eval_char(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_mod_eval_uchar(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_mod_eval_short(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_mod_eval_ushort(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_mod_eval_int(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_mod_eval_uint(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_mod_eval_long(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_mod_eval_ulong(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_mod_eval_long_long(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_mod_eval_ulong_long(struct inter* inter, struct ast_node_with_type* root, void* value, int size);

error inter_func_mod_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);

error inter_func_bit_and_eval_char(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_bit_and_eval_uchar(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_bit_and_eval_short(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_bit_and_eval_ushort(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_bit_and_eval_int(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_bit_and_eval_uint(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_bit_and_eval_long(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_bit_and_eval_ulong(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_bit_and_eval_long_long(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_bit_and_eval_ulong_long(struct inter* inter, struct ast_node_with_type* root, void* value, int size);

error inter_func_bit_and_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);


error inter_func_bit_or_eval_char(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_bit_or_eval_uchar(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_bit_or_eval_short(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_bit_or_eval_ushort(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_bit_or_eval_int(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_bit_or_eval_uint(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_bit_or_eval_long(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_bit_or_eval_ulong(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_bit_or_eval_long_long(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_bit_or_eval_ulong_long(struct inter* inter, struct ast_node_with_type* root, void* value, int size);

error inter_func_bit_or_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);


error inter_func_bit_not_eval_char(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_bit_not_eval_uchar(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_bit_not_eval_short(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_bit_not_eval_ushort(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_bit_not_eval_int(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_bit_not_eval_uint(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_bit_not_eval_long(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_bit_not_eval_ulong(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_bit_not_eval_long_long(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_bit_not_eval_ulong_long(struct inter* inter, struct ast_node_with_type* root, void* value, int size);

error inter_func_bit_not_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);

error inter_func_shift_left_eval_char(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_shift_left_eval_uchar(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_shift_left_eval_short(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_shift_left_eval_ushort(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_shift_left_eval_int(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_shift_left_eval_uint(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_shift_left_eval_long(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_shift_left_eval_ulong(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_shift_left_eval_long_long(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_shift_left_eval_ulong_long(struct inter* inter, struct ast_node_with_type* root, void* value, int size);

error inter_func_shift_left_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);


error inter_func_shift_right_eval_char(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_shift_right_eval_uchar(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_shift_right_eval_short(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_shift_right_eval_ushort(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_shift_right_eval_int(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_shift_right_eval_uint(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_shift_right_eval_long(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_shift_right_eval_ulong(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_shift_right_eval_long_long(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_shift_right_eval_ulong_long(struct inter* inter, struct ast_node_with_type* root, void* value, int size);

error inter_func_shift_right_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);








error inter_func_gt_eval_char(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_gt_eval_uchar(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_gt_eval_short(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_gt_eval_ushort(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_gt_eval_int(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_gt_eval_uint(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_gt_eval_long(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_gt_eval_ulong(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_gt_eval_long_long(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_gt_eval_ulong_long(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_gt_eval_float(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_gt_eval_double(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_gt_eval_long_double(struct inter* inter, struct ast_node_with_type* root, void* value, int size);

error inter_func_gt_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);

error inter_func_g_eval_char(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_g_eval_uchar(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_g_eval_short(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_g_eval_ushort(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_g_eval_int(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_g_eval_uint(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_g_eval_long(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_g_eval_ulong(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_g_eval_long_long(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_g_eval_ulong_long(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_g_eval_float(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_g_eval_double(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_g_eval_long_double(struct inter* inter, struct ast_node_with_type* root, void* value, int size);


error inter_func_g_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);

error inter_func_lt_eval_char(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_lt_eval_uchar(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_lt_eval_short(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_lt_eval_ushort(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_lt_eval_int(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_lt_eval_uint(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_lt_eval_long(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_lt_eval_ulong(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_lt_eval_long_long(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_lt_eval_ulong_long(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_lt_eval_float(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_lt_eval_double(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_lt_eval_long_double(struct inter* inter, struct ast_node_with_type* root, void* value, int size);


error inter_func_lt_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);

error inter_func_l_eval_char(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_l_eval_uchar(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_l_eval_short(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_l_eval_ushort(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_l_eval_int(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_l_eval_uint(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_l_eval_long(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_l_eval_ulong(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_l_eval_long_long(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_l_eval_ulong_long(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_l_eval_float(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_l_eval_double(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_l_eval_long_double(struct inter* inter, struct ast_node_with_type* root, void* value, int size);

error inter_func_l_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);




error inter_func_eq_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_func_eq_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_neq_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_func_neq_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_and_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_func_or_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_func_and_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_or_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_not_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_func_not_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_addr_of_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_func_addr_of_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_array_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_func_array_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_array_length_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_func_array_length_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_sizeof_type_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_func_sizeof_value_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_func_sizeof_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_type_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_func_typeof_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_func_type_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_type_size_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_func_type_size_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);


error inter_func_cast_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_func_cast_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_context_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_func_context_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_to_char_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_func_to_uchar_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_func_to_char_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_to_uchar_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_to_short_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_to_ushort_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_to_int_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_func_to_int_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_to_uint_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_to_long_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_to_ulong_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_to_long_long_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_to_ulong_long_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_to_float_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_func_to_float_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_to_double_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_to_long_double_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_to_pointer_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_func_to_pointer_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_code_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_func_code_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_code_unsafe_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_func_code_unsafe_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_quote_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_func_quote_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_free_quote_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_func_free_quote_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_begin_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_func_begin_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_begin__check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_func_begin__eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_do_file(struct inter* inter, struct ast_node_with_type* root, struct token_reader* reader, int interactive);
error inter_do_filename(struct inter* inter, struct ast_node* location, char* filename, int filename_len, int interactive);
error inter_func_load_file_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_func_load_file_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);

struct error_cannot_open_file {
   struct error_vtable* vtable;
   struct ast_node* location;
   char* file_name;
   struct inter* inter; // file_name is a symbol.
};

void error_cannot_open_file_print(FILE* output, struct error_cannot_open_file* err);
void error_cannot_open_file_free(struct error_cannot_open_file* err);
extern struct error_vtable error_cannot_open_file_vtable;
error error_cannot_open_file(struct ast_node* location, char* file_name, struct inter* inter);

error inter_func_load_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_func_load_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);

error inter_func_import_path_check(struct ast_node* root);
void inter_func_parse_import_path(struct inter* inter, struct ast_node* root, struct filepath* filepath);
error inter_func_import_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_func_import_abs_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_import_rel_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);


error inter_func_struct_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_func_struct_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);

error inter_func_set_slot_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_func_set_slot_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);

error inter_func_set_slot_eval_bit_char(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_set_slot_eval_bit_uchar(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_set_slot_eval_bit_short(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_set_slot_eval_bit_ushort(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_set_slot_eval_bit_int(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_set_slot_eval_bit_uint(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_set_slot_eval_bit_long(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_set_slot_eval_bit_ulong(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_set_slot_eval_bit_long_long(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_set_slot_eval_bit_ulong_long(struct inter* inter, struct ast_node_with_type* root, void* value, int size);

error inter_func_dot_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_func_dot_eval_lval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_dot_eval_rval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);

error inter_func_def_type_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_func_def_type_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_set_type_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_func_set_type_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_undef_type_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_func_undef_type_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_def_type_cons_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_func_def_type_cons_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_undef_type_cons_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_func_undef_type_cons_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_def_enum_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_func_def_enum_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_prefix_make_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_func_prefix_make_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_current_prefix_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_func_current_prefix_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_in_prefix_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_func_in_prefix_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_use_prefix_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_func_use_prefix_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_clear_prefixes_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_func_clear_prefixes_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_script_function_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_func_script_function_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_free_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_func_free_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);

void error_break_print(FILE* output, struct error_at_node* self);
extern struct error_vtable error_break_vtable;
error error_break(struct ast_node* location);
void error_continue_print(FILE* output, struct error_at_node* self);
extern struct error_vtable error_continue_vtable;
error error_continue(struct ast_node* location);

error inter_func_break_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_func_continue_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_func_break_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_continue_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);



error inter_func_forever_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_func_forever_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_while__check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_func_while_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_func_do_while_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_func_while_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_do_while_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_for_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_func_for_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_let_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_func_let_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_func_macro_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_func_macro_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);




void error_splice_outside_list_print(FILE* output, struct error_at_node* self);
extern struct error_vtable error_splice_outside_list_vtable;
error error_splice_outside_list(struct ast_node* location);

struct error_cannot_splice_atom {
   struct error_vtable* vtable;
   struct ast_node* location;
   struct ast_node_with_type* quote;
   struct inter* inter;
};

void error_cannot_splice_atom_print(FILE* output, struct error_cannot_splice_atom* self);
void error_cannot_splice_atom_free(struct error_cannot_splice_atom* self);
extern struct error_vtable error_cannot_splice_atom_vtable;
error error_cannot_splice_atom(struct ast_node* location, struct ast_node_with_type* quote, struct inter* inter);


struct quasiquote_syms {
   char* unquote;
   char* unquote_abrv;
   char* splice;
   char* splice_abrv;
};

void quasiquote_syms_init(struct quasiquote_syms* syms, struct inter* inter);
void quasiquote_syms_destruct(struct quasiquote_syms* syms, struct inter* inter);

error inter_func_quasiquote_check_rec(
      struct inter* inter,
      struct quasiquote_syms* syms,
      struct ast_node_with_type* root,
      int level);

error inter_func_quasiquote_check(struct inter* inter, type expected_type, struct ast_node_with_type* root);

error inter_func_quasiquote_eval_rec(
      struct inter* inter,
      struct quasiquote_syms* syms,
      struct ast_node_with_type* root,
      struct ast_node_with_type* adopter);

error inter_func_quasiquote_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);





void inter_def_std_funcs(struct inter* inter);
void inter_undef_std_funcs(struct inter* inter);






#endif
