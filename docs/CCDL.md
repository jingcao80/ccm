
# BNF rules of CCDL(C++ Component Description Language) Syntax

### Descriptions
<*top*> ::= <*top_definition*>?

<*top_definition*> ::=  <*definition*> | <*import_statement*> | <*include_statement*> | <*using_statement*> | <*comment*>

### Definitions

<*definition*> ::= <*interface_definition*> | <*class_definition*> | <*namespace_definition*> | <*enum_definition*> | <*module_definition*>

<*interface_definition*> ::= <*interface_attributes*> <*interface_header*> <*interface_body*>

<*interface_attributes*> ::= \[ <*interface_attribute_list*>? \]

<*interface_attribute_list*> ::= <*interface_attribute*> | <*interface_attribute_list*> , <*interface_attribute*>

<*interface_attribute*> ::= <*uuid_attribute*> | <*version_attribute*> | <*description_attribute*>

<*interface_header*> ::= interface <*interface_name*> <*parent_interface*>?

<*interface_name*> ::= <*identifier*>

<*parent_interface*> ::= : <*interface_name*>

<*interface_body*> ::= { <*interface_member*>? }

<*interface_member*> ::= <*constant_data_member*> | <*method_member*>

<*constant_data_member*> ::= <*constant_boolean_data_member*> | <*constant_integer_data_member*> | <*constant_string_data_member*> | <*constant_enum_data_member*>

<*constant_boolean_data_member*> ::= const Boolean <*member_name*> = <*Boolean*> ;

<*constant_integer_data_member*> ::= <*constant_byte_data_member*> | <>

<*constant_byte_data_member*> ::= const Byte <*member_name*> = <*inclusive_or_expression*> ;

<*inclusive_or_expression*> ::= <*exclusive_or_expression*> | <*inclusive_or_expression*> **|** <*exclusive_or_expression*>

<*exclusive_or_expression*> ::= <*and_expression*> | <*exclusive_or_expression*> ^ <*and_expression*>

<*and_expression*> ::= <*shift_expression*> | <*and_expression*> & <*shift_expression*>

<*shift_expression*> ::= <*additive_expression*> | <*shift_expression*> << <*additive_expression*> | <*shift_expression*> >> <*additive_expression*>

<*additive_expression*> ::= <*multiplicative_expression*> | <*additive_expression*> + <*multiplicative_expression*> | <*additive_expression*> - <*multiplicative_expression*>

<*multiplicative_expression*> ::= <*unary_expression*> | <*multiplicative_expression*> * <*unary_expression*> | <*multiplicative_expression*> / <*unary_expression*> | <*multiplicative_expression*> % <*unary_expression*>

<*unary_expression*> ::= <*preincrement_expression*> | <*predecrement_expression*> | + <*unary_expression*> | - <*unary_expression*> | <*unary_expression_not_plus_minus*>

<*preincrement_expression*> ::= ++ <*unary_expression*>

<*predecrement_expression*> ::= -- <*unary_expression*>

<*unary_expression_not_plus_minus*> ::= <*postfix_expression*> | ~ <*unary_expression*> | ! <*unary_expression*> | <*cast_expression*>

<*postfix_expression*> ::= <*primary*> | <*identifier*> | <*postincrement_expression*> | <*postdecrement_expression*>

<*postdecrement_expression*> ::= <*postfix_expression*> --

<*postincrement_expression*> ::= <*postfix_expression*> ++

<*primary*> ::= <*integer*> | <*floating_point*> | ( <*inclusive_or_expression*> )

<*integer*> ::= <*decimal_integer*> | <*hex_integer*> | <*octal_integer*>

<*floating_point*> ::= <*decimal_digits*> . <*decimal_digits*>? <*exponent_part*>? <*float_type_suffix*>? | <*decimal_digits*> <*exponent_part*>? <*float_type_suffix*>?

<*constant_string_data_member*> ::= const String <*member_name*> = <*string*> ;

<*method_member*> ::= <*method_name*> ( <*parameter_list*>? ) ;

<*method_name*> ::= <*identifier*>

<*parameter_list*> ::= <*parameter*> | <*parameter_list*> , <*parameter*>

<*parameter*> ::= <*parameter_attribute*> <*type*> <*parameter_name*>

<*parameter_attribute*> ::= \[ <*in_attribute*> | <*out_attribute*> | <*in_out_attribute*> | <*out_callee_attribute*> \]

<*in_attribute*> ::= in

<*out_attribute*> ::= out

<*in_out_attribute*> ::= in, out

<*out_callee_attribute*> ::= out, callee

<*type*> ::= <*basic_type*> | <*string_type*> | <*array_type*> | <*interface_type*> | <*pointer_type*>

<*basic_type*> ::= Boolean | Byte | Short | Integer | Long | Float | Double | HANDLE

<*string_type*> ::= String

<*array_type*> ::= Array < <*type*> >

<*interface_type*> ::= <*identifier*>

<*pointer_type*> ::= <*type*> *

<*parameter_name*> ::= <*identifier*>

<*class_definition*> ::= <*class_attributes*> <*class_header*> <*class_body*>

<*class_attributes*> ::= \[ <*class_attribute_list*>? \]

<*class_attribute_list*> ::= <*class_attribute*> | <*class_attribute_list*> , <*class_attribute*>

<*class_attribute*> ::= <*uuid_attribute*> | <*version_attribute*> | <*description_attribute*>

<*class_header*> ::= class <*class_name*>

<*class_name*> ::= <*identifier*>

<*class_body*> ::= { <*class_body_statement*>? }

<*class_body_statement*> ::= <*constructor_declaration*> | <*interface_declaration*>

<*constructor_declaration*> ::= constructor ( <*formal_parameter_list*>? )

<*interface_declaration*> ::= interface <*interface_name*> ;

<*namespace_definition*> ::= namespace <*identifier*> { <*namespace_body*> }

<*namespace_body*> ::= <*definition*>?

<*module_definition*> ::= <*module_attributes*> <*module_header*> <*module_body*>

<*module_attributes*> ::= \[ <*module_attribute_list*>? \]

<*module_attribute_list*> ::= <*module_attribute*> | <*module_attribute_list*> , <*module_attribute*>

<*module_attribute*> ::= <*uuid_attribute*> | <*version_attribute*> | <*description_attribute*> | <*url_attribute*>

<*module_header*> ::= module <*module_name*>

<*module_name*> ::= <*identifier*>

<*module_body*> ::= { <*module_body_statement*>? }

<*module_body_statement*> ::= <*import_statement*>

<*import_statement*> ::= import ( " <*midl_file_path*> " ) ;

<*midl_file_path*> ::= <*midl_file_absolute_path*> | <*midl_file_relative_path*>

<*uuid_attribute*> ::= uuid ( <*uuid_number*> )

<*version_attribute*> ::= version ( <*version_number*> )

<*description_attribute*> ::= description ( <*string*> )

<*uuid_number*> ::= <*time_low*> - <*time_mid*> - <*time_high_and_version*> - <*clock_seq_and_reserved*> <*clock_seq_low*> - <*node*>

<*time_low*> ::= 4\*<*hex_octet*>

<*time_mid*> ::= 2\*<*hex_octet*>

<*time_high_and_version*> ::= 2\*<*hex_octet*>

<*clock_seq_and_reserved*> ::= 2\*<*hex_octet*>

<*clock_seq_low*> ::= <*hex_octet*>

<*node*> ::= 6\*<*hex_octet*>

<*hex_octet*> ::= <*hex_digit*> <*hex_digit*>

<*hex_digit*> ::= 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | a | b | c | d | e | f | A | B | C | D | E | F

<*version_number*> ::= <*major_number*> . <*minor_number*>

<*major_number*> ::= <*decimal_number*>

<*minor_number*> ::= <*decimal_number*>

<*Boolean*> ::= true | false

<*decimal_integer*> ::= <*decimal_number*> <*integer_type_suffix*>?

<*hex_integer*> ::= <*hex_number*> <*integer_type_suffix*>?

<*octal_integer*> ::= <*octal_number*> <*integer_type_suffix*>?

<*integer_type_suffix*> ::= l | L

<*float_type_suffix*> ::= f | F | d | D

<*exponent_part*> ::= <*exponent_indicator*> <*signed_integer*>

<*exponent_indicator*> ::= e | E

<*signed_integer*> ::= <*sign*>? <*decimal_digits*>

<*sign*> ::= + | -

<*decimal_number*> ::= 0 | <*non_zero_decimal_digit*> <*decimal_digits*>?

<*decimal_digits*> ::= <*decimal_digit*> | <*decimal_digits*> <*decimal_digit*>

<*decimal_digit*> ::= 0 | <*non_zero_decimal_digit*>

<*non_zero_decimal_digit*> ::= 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9

<*hex_number*> ::= 0 x <*hex_digit*> | 0 X <*hex_digit*> | <*hex_number*> <*hex_digit*>

<*octal_number*> ::= 0 <*octal_digit*> | <*octal_number*> <*octal_digit*>

<*octal_digit*> ::= 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7

<*string*> ::= " <*string_characters*>? "

<*string_characters*> ::= <*string_character*> | <*string_characters*> <*string_character*>

<*string_character*> ::= <*input_character*> except " and \ | <*escape_character*>

<*escape_character*> ::= \" | \\ | \n | \t

<*all_characters*> :== <*input_character*> | <*all_characters*> <*input_character*>

<*comment*> ::= <*block_comment*> | <*line_comment*>

<*block_comment*> ::= / * <*all_characters*> * /

<*line_comment*> ::= / / <*all_characters*>


### Expressions


The character set for MIDL is 7-bit ASCII character set. This is the set denoted by <*input_character*>.

<*midl_file_absolute_path*>

<*midl_file_relative_path*>