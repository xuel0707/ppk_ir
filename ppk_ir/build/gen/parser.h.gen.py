
# SPDX-License-Identifier: Apache-2.0
# Copyright 2019 Eotvos Lorand University, Budapest, Hungary

# Autogenerated file (from compiler/backend/parser.h.py), do not modify directly.
# Generator: PPK

global file_indentation_level
file_indentation_level = 0

# The last element is the innermost (current) style.
file_sugar_style = ['line_comment']


def add_code(line, lineno = None, file = "compiler/backend/parser.h.py"):
    global file_indentation_level
    global file_sugar_style
    global generated_code

    line_ends = {
        "line_comment": "\n",
        "inline_comment": "",
    }

    stripped_line = line.strip()
    no_sugar_on_line = stripped_line.startswith('//') or stripped_line.startswith('# ') or stripped_line == ""

    indent = '    ' * file_indentation_level
    return indent + line + sugar(no_sugar_on_line, file, lineno) + line_ends[file_sugar_style[-1]]


class SugarStyle():
    def __init__(self, sugar):
        global file_sugar_style
        file_sugar_style.append(sugar)

    def __enter__(self):
        global file_sugar_style
        return file_sugar_style[-1]

    def __exit__(self, type, value, traceback):
        global file_sugar_style
        file_sugar_style.pop()


def sugar(no_sugar_on_line, file, lineno):
    if no_sugar_on_line or file is None or lineno is None:
        return ""

    import re
    global file_sugar_style

    if file_sugar_style[-1] == 'line_comment':
        if no_sugar_on_line:
            return ""
        return " // {}:{}".format(file, lineno)
    if file_sugar_style[-1] == 'inline_comment':
        if file == "../compiler/backend/parser.h.py":
            return " /* {} */ {}".format(lineno)
        return " /* {}:{} */".format(file, lineno)
    return line


generated_code += "// Autogenerated file (from compiler/backend/parser.h.py via ../compiler/backend/parser.h.py), do not modify directly.\n"
generated_code += "// Generator: PPK\n"
generated_code += "\n"

# Copyright 2016 Eotvos Lorand University, Budapest, Hungary  Licensed under ## compiler/backend/parser.h.py:2
# the Apache License, Version 2.0 (the "License"); you may not use this file ## compiler/backend/parser.h.py:3
# except in compliance with the License. You may obtain a copy of the License ## compiler/backend/parser.h.py:4
# at  http://www.apache.org/licenses/LICENSE-2.0  Unless required by ## compiler/backend/parser.h.py:5
# applicable law or agreed to in writing, software distributed under the ## compiler/backend/parser.h.py:6
# License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS ## compiler/backend/parser.h.py:7
# OF ANY KIND, either express or implied. See the License for the specific ## compiler/backend/parser.h.py:8
# language governing permissions and limitations under the License. ## compiler/backend/parser.h.py:9
from hlir16.utils_hlir16 import * ## compiler/backend/parser.h.py:10

generated_code += add_code("")
generated_code += add_code(" #ifndef __HEADER_INFO_H__", 12)
generated_code += add_code(" #define __HEADER_INFO_H__", 13)

generated_code += add_code("")
generated_code += add_code(" #include <byteswap.h>", 15)
generated_code += add_code(" #include <stdbool.h>", 16)

generated_code += add_code("")
generated_code += add_code(" // TODO add documentation", 18)
generated_code += add_code(" #define MODIFIED true", 19)

generated_code += add_code("")
# TODO put this in a proper header ## compiler/backend/parser.h.py:22
generated_code += add_code(" typedef struct {} InternetChecksum_t;", 22)

generated_code += add_code("")
generated_code += add_code(" typedef struct parsed_fields_s {", 25)

generated_code += add_code("")
for hdr in hlir16.header_instances_with_refs: ## compiler/backend/parser.h.py:28
    if hdr.type.type_ref.node_type == 'Type_HeaderUnion': ## compiler/backend/parser.h.py:29
        raise NotImplementedError("Header unions are not supported") ## compiler/backend/parser.h.py:30
    for fld in hdr.type.type_ref.fields: ## compiler/backend/parser.h.py:32
        fld = fld._expression ## compiler/backend/parser.h.py:33
        fldtype = fld.canonical_type() ## compiler/backend/parser.h.py:34
        if fldtype.size <= 32: ## compiler/backend/parser.h.py:36
            generated_code += add_code(' uint32_t field_instance_{}_{};'.format(hdr.name, fld.name), 36)
            generated_code += add_code(' uint8_t attr_field_instance_{}_{};'.format(hdr.name, fld.name), 37)
generated_code += add_code(" } parsed_fields_t;", 38)

generated_code += add_code("")
generated_code += add_code(" // Header stack infos", 41)
generated_code += add_code(" // ------------------", 42)

generated_code += add_code("")
# TODO make proper header stacks (types grouped by instances? order?) ## compiler/backend/parser.h.py:46

generated_code += add_code("")
generated_code += add_code(' #define HEADER_STACK_COUNT {}'.format(len(hlir16.header_instances_with_refs)), 47)

generated_code += add_code("")
generated_code += add_code(" enum header_stack_e {", 50)
for hi in hlir16.header_instances_with_refs: ## compiler/backend/parser.h.py:52
    generated_code += add_code(' header_stack_{},'.format(hi.name), 52)
generated_code += add_code(" header_stack_, // dummy to prevent warning for empty stack", 53)
generated_code += add_code(" };", 54)

generated_code += add_code("")
generated_code += add_code(" // Header instance infos", 58)
generated_code += add_code(" // ---------------------", 59)

generated_code += add_code("")
generated_code += add_code(' #define HEADER_INSTANCE_COUNT {}'.format(len(hlir16.header_instances)), 61)

generated_code += add_code("")
# TODO maybe some more needs to be added for varlen headers? ## compiler/backend/parser.h.py:64
nonmeta_hdrlens = "+".join([str(hi.type.type_ref.byte_width) for hi in hlir16.header_instances_with_refs if not hi.canonical_type().is_metadata]) ## compiler/backend/parser.h.py:65
generated_code += add_code(' #define HEADER_INSTANCE_TOTAL_LENGTH ({})'.format(nonmeta_hdrlens), 65)

generated_code += add_code("")
generated_code += add_code(" struct header_instance_info {", 68)
generated_code += add_code("     const char* name;", 69)
generated_code += add_code("     int         byte_width;", 70)
generated_code += add_code("     int         byte_offset;", 71)
generated_code += add_code("     bool        is_metadata;", 72)
generated_code += add_code(" };", 73)

generated_code += add_code("")
generated_code += add_code(" extern const char* header_instance_names[HEADER_INSTANCE_COUNT];", 75)

generated_code += add_code("")
generated_code += add_code(" typedef enum header_instance_e {", 77)
for hdr in hlir16.header_instances: ## compiler/backend/parser.h.py:79
    if hasattr(hdr.type._type_ref, 'is_metadata'): ## compiler/backend/parser.h.py:80
        if not hdr.type._type_ref.is_metadata: ## compiler/backend/parser.h.py:81
            generated_code += add_code(' header_instance_{},'.format(hdr.name), 81)
generated_code += add_code(" header_instance_all_metadatas,", 83)
generated_code += add_code(" header_instance_meta,", 84)
generated_code += add_code(" } header_instance_t;", 85)

generated_code += add_code("")
generated_code += add_code(" static const struct header_instance_info header_instance_infos[HEADER_INSTANCE_COUNT+1] = {", 88)
byte_offsets = [] ## compiler/backend/parser.h.py:90
for hdr in hlir16.header_instances: ## compiler/backend/parser.h.py:91
    typ = hdr.type.type_ref if hasattr(hdr.type, 'type_ref') else hdr.type ## compiler/backend/parser.h.py:92
    typ_bit_width = typ.bit_width if hasattr(typ, 'bit_width') else 0 ## compiler/backend/parser.h.py:93
    typ_byte_width = typ.byte_width if hasattr(typ, 'byte_width') else 0 ## compiler/backend/parser.h.py:94
    generated_code += add_code(' {{ // header_instance_{}'.format(hdr.name), 95)
    generated_code += add_code('     "{}",'.format(hdr.name), 96)
    generated_code += add_code('     {}, // header_instance_{}, {} bits, {} bytes'.format(typ_byte_width, hdr.name, typ_bit_width, typ_bit_width/8.0), 97)
    generated_code += add_code('     {},'.format("+".join(byte_offsets) if byte_offsets != [] else "0"), 98)
    generated_code += add_code('     {},'.format('true' if hasattr(typ, 'is_metadata') and typ.is_metadata else 'false'), 99)
    generated_code += add_code(" },", 100)
    byte_offsets += [str(typ_byte_width)] ## compiler/backend/parser.h.py:103
generated_code += add_code(" { // dummy", 104)
generated_code += add_code(" },", 105)
generated_code += add_code(" };", 106)

generated_code += add_code("")
generated_code += add_code(" static const int header_instance_byte_width[HEADER_INSTANCE_COUNT] = {", 109)
for hdr in hlir16.header_instances_with_refs: ## compiler/backend/parser.h.py:111
    generated_code += add_code('   {}, // header_instance_{}, {} bits, {} bytes'.format(hdr.type.type_ref.byte_width, hdr.name, hdr.type.type_ref.bit_width, hdr.type.type_ref.bit_width/8.0), 111)
generated_code += add_code(" };", 112)

generated_code += add_code("")
generated_code += add_code(" static const int header_instance_byte_width_summed[HEADER_INSTANCE_COUNT+1] = {", 115)
generated_code += add_code("     0,", 116)
byte_widths = [] ## compiler/backend/parser.h.py:118
for hdr in hlir16.header_instances_with_refs: ## compiler/backend/parser.h.py:119
    byte_widths += [str(hdr.type.type_ref.byte_width)] ## compiler/backend/parser.h.py:120
    joined = "+".join(byte_widths) ## compiler/backend/parser.h.py:121
    generated_code += add_code('   {},'.format(joined), 121)
generated_code += add_code(" };", 122)

generated_code += add_code("")
generated_code += add_code(" static const int header_instance_is_metadata[HEADER_INSTANCE_COUNT] = {", 125)
for hdr in hlir16.header_instances_with_refs: ## compiler/backend/parser.h.py:127
    generated_code += add_code('   {}, // header_instance_{}'.format(1 if hdr.type.type_ref.is_metadata else 0, hdr.name), 127)
generated_code += add_code(" };", 128)

generated_code += add_code("")
# TODO move to hlir16.py/set_additional_attrs? ## compiler/backend/parser.h.py:132
def all_field_instances(): ## compiler/backend/parser.h.py:133
    return [fld for hdr in hlir16.header_instances_with_refs for fld in hdr.type.type_ref.fields] ## compiler/backend/parser.h.py:134
generated_code += add_code(" // Field instance infos", 136)
generated_code += add_code(" // --------------------", 137)

generated_code += add_code("")
generated_code += add_code(' #define FIELD_INSTANCE_COUNT {}'.format(len(all_field_instances())), 140)

generated_code += add_code("")
generated_code += add_code(" typedef enum field_instance_e {", 143)
for hdr in hlir16.header_instances_with_refs: ## compiler/backend/parser.h.py:145
    for fld in hdr.type.type_ref.fields: ## compiler/backend/parser.h.py:146
        generated_code += add_code('   field_instance_{}_{},'.format(hdr.name, fld.name), 146)
generated_code += add_code(" } field_instance_t;", 147)

generated_code += add_code("")
generated_code += add_code(" #define FIXED_WIDTH_FIELD -1", 150)
generated_code += add_code(" static const int header_instance_var_width_field[HEADER_INSTANCE_COUNT] = {", 151)
for hdr in hlir16.header_instances_with_refs: ## compiler/backend/parser.h.py:153
    field_id_pattern = 'field_instance_{}_{}' ## compiler/backend/parser.h.py:154
    generated_code += add_code('   {}, // header_instance_{}'.format(reduce((lambda x, f: field_id_pattern.format(hdr.name, f.name) if hasattr(f, 'is_vw') and f.is_vw else x), hdr.type.type_ref.fields, 'FIXED_WIDTH_FIELD'), hdr.name), 154)
generated_code += add_code(" };", 155)

generated_code += add_code("")
generated_code += add_code(" static const int field_instance_bit_width[FIELD_INSTANCE_COUNT] = {", 158)
for hdr in hlir16.header_instances_with_refs: ## compiler/backend/parser.h.py:160
    for fld in hdr.type.type_ref.fields: ## compiler/backend/parser.h.py:161
        fldtype = fld.canonical_type() ## compiler/backend/parser.h.py:162
        generated_code += add_code('   {}, // field_instance_{}_{}'.format(fldtype.size, hdr.name, fld.name), 162)
generated_code += add_code(" };", 163)

generated_code += add_code("")
generated_code += add_code(" static const int field_instance_bit_offset[FIELD_INSTANCE_COUNT] = {", 166)
for hdr in hlir16.header_instances_with_refs: ## compiler/backend/parser.h.py:168
    for fld in hdr.type.type_ref.fields: ## compiler/backend/parser.h.py:169
        generated_code += add_code('   ({} % 8), // field_instance_{}_{}'.format(fld.offset, hdr.name, fld.name), 169)
generated_code += add_code(" };", 170)

generated_code += add_code("")
# TODO why does this name have "_hdr" at the end, but field_instance_bit_offset doesn't? ## compiler/backend/parser.h.py:174

generated_code += add_code("")
generated_code += add_code(" static const int field_instance_byte_offset_hdr[FIELD_INSTANCE_COUNT] = {", 175)
for hdr in hlir16.header_instances_with_refs: ## compiler/backend/parser.h.py:177
    for fld in hdr.type.type_ref.fields: ## compiler/backend/parser.h.py:178
        generated_code += add_code('   ({} / 8), // field_instance_{}_{}'.format(fld.offset, hdr.name, fld.name), 178)
generated_code += add_code(" };", 179)

generated_code += add_code("")
generated_code += add_code(" // TODO documentation", 183)
generated_code += add_code(" #define mod_top(n, bits) (((bits-(n%bits)) % bits))", 184)

generated_code += add_code("")
generated_code += add_code(" // Produces a 32 bit int that has n bits on at the top end.", 186)
generated_code += add_code(" #define uint32_top_bits(n) (0xffffffff << mod_top(n, 32))", 187)

generated_code += add_code("")
generated_code += add_code(" static const int field_instance_mask[FIELD_INSTANCE_COUNT] = {", 189)
for hdr in hlir16.header_instances_with_refs: ## compiler/backend/parser.h.py:191
    for fld in hdr.type.type_ref.fields: ## compiler/backend/parser.h.py:192
        fldtype = fld.canonical_type() ## compiler/backend/parser.h.py:193
        generated_code += add_code('  __bswap_constant_32(uint32_top_bits({}) >> ({}%8)), // field_instance_{}_{},'.format(fldtype.size, fld.offset, hdr.name, fld.name), 193)
generated_code += add_code(" };", 194)

generated_code += add_code("")
generated_code += add_code(" static const header_instance_t field_instance_header[FIELD_INSTANCE_COUNT] = {", 197)
for hdr in hlir16.header_instances_with_refs: ## compiler/backend/parser.h.py:199
    for fld in hdr.type.type_ref.fields: ## compiler/backend/parser.h.py:200
        if hdr.type.type_ref.is_metadata: ## compiler/backend/parser.h.py:201
            if hdr.name == "meta": ## compiler/backend/parser.h.py:202
                generated_code += add_code(" header_instance_meta,", 202)
            else: ## compiler/backend/parser.h.py:204
                generated_code += add_code(" header_instance_all_metadatas,", 204)
        else: ## compiler/backend/parser.h.py:206
            generated_code += add_code(' header_instance_{}, // field_instance_{}_{}'.format(hdr.name, hdr.name, fld.name), 206)
generated_code += add_code(" };", 207)

generated_code += add_code("")
generated_code += add_code(" // TODO current stacks are exactly 1 element deep ", 210)
generated_code += add_code(" static const header_instance_t header_stack_elements[HEADER_STACK_COUNT][10] = {", 211)
for hi in hlir16.header_instances_with_refs: ## compiler/backend/parser.h.py:213
    generated_code += add_code(' // header_instance_{}'.format(hi.name), 213)
    generated_code += add_code(" {", 214)
    for stack_elem in [hi.name]: ## compiler/backend/parser.h.py:216
        if hdr.type.type_ref.is_metadata: ## compiler/backend/parser.h.py:217
            if hdr.name == "meta": ## compiler/backend/parser.h.py:218
                generated_code += add_code(" header_instance_meta,", 218)
            else: ## compiler/backend/parser.h.py:220
                generated_code += add_code(" header_instance_all_metadatas,", 220)
        else: ## compiler/backend/parser.h.py:222
            generated_code += add_code(' header_instance_{},'.format(stack_elem), 222)
    generated_code += add_code(" },", 223)
generated_code += add_code(" };", 224)

generated_code += add_code("")
generated_code += add_code(" static const unsigned header_stack_size[HEADER_STACK_COUNT] = {", 226)
for hi in hlir16.header_instances_with_refs: ## compiler/backend/parser.h.py:228
    generated_code += add_code(' 1, // {}'.format(hi.name), 228)
generated_code += add_code(" };", 229)

generated_code += add_code("")
generated_code += add_code(" typedef enum header_stack_e header_stack_t;", 231)

generated_code += add_code("")
generated_code += add_code(" /////////////////////////////////////////////////////////////////////////////", 233)
generated_code += add_code(" // HEADER TYPE AND FIELD TYPE INFORMATION", 234)
generated_code += add_code(" // TODO remove instance info when no code refers to it", 235)

generated_code += add_code("")
generated_code += add_code(' #define HEADER_COUNT {}'.format(len(hlir16.header_types)), 237)

generated_code += add_code("")
generated_code += add_code(" enum header_e {", 239)
for hdr in hlir16.header_types: ## compiler/backend/parser.h.py:241
    generated_code += add_code('   header_{},'.format(hdr.name), 241)
generated_code += add_code(" };", 242)

generated_code += add_code("")
generated_code += add_code(" static const int header_byte_width[HEADER_COUNT] = {", 244)
for hdr in hlir16.header_types: ## compiler/backend/parser.h.py:246
    generated_code += add_code('   {}, // {}'.format(hdr.byte_width, hdr.name), 246)
generated_code += add_code(" };", 247)

generated_code += add_code("")
generated_code += add_code(" static const int header_is_metadata[HEADER_COUNT] = {", 249)
for hdr in hlir16.header_types: ## compiler/backend/parser.h.py:251
    generated_code += add_code('   {}, // {}'.format(1 if hdr.is_metadata else 0, hdr.name), 251)
generated_code += add_code(" };", 252)

generated_code += add_code("")
def all_fields(): ## compiler/backend/parser.h.py:256
    return [fld for hdr in hlir16.header_types for fld in hdr.fields] ## compiler/backend/parser.h.py:257
generated_code += add_code(' #define FIELD_COUNT {}'.format(len(all_fields())), 258)

generated_code += add_code("")
generated_code += add_code(" extern const char* field_names[FIELD_COUNT];", 260)

generated_code += add_code("")
generated_code += add_code(" typedef enum header_e header_t;", 262)
generated_code += add_code(" typedef enum field_e field_t;", 263)

generated_code += add_code("")
global b_isContain_bit_mcast ## compiler/backend/parser.h.py:266
b_isContain_bit_mcast=False ## compiler/backend/parser.h.py:267

generated_code += add_code("")
global b_isContain_recirculate_flag ## compiler/backend/parser.h.py:269
b_isContain_recirculate_flag=False ## compiler/backend/parser.h.py:270

generated_code += add_code("")
generated_code += add_code(" enum field_e {", 271)
for hdr in hlir16.header_types: ## compiler/backend/parser.h.py:273
    for fld in hdr.fields: ## compiler/backend/parser.h.py:274
        if hdr.name == 'standard_metadata_t': ## compiler/backend/parser.h.py:275
            if fld.name == 'bit_mcast': ## compiler/backend/parser.h.py:276
                b_isContain_bit_mcast=True ## compiler/backend/parser.h.py:277
            if fld.name == 'recirculate_flag': ## compiler/backend/parser.h.py:278
                b_isContain_recirculate_flag=True ## compiler/backend/parser.h.py:279
        generated_code += add_code('   field_{}_{},'.format(hdr.name, fld.name), 279)
if b_isContain_bit_mcast is False: ## compiler/backend/parser.h.py:281
        generated_code += add_code("   field_standard_metadata_t_bit_mcast,", 281)
if b_isContain_recirculate_flag is False: ## compiler/backend/parser.h.py:283
        generated_code += add_code("   field_standard_metadata_t_recirculate_flag,", 283)
generated_code += add_code(" };", 284)

generated_code += add_code("")
generated_code += add_code(" static const int field_bit_width[FIELD_COUNT] = {", 286)
for hdr in hlir16.header_types: ## compiler/backend/parser.h.py:288
    for fld in hdr.fields: ## compiler/backend/parser.h.py:289
        fldtype = fld.canonical_type() ## compiler/backend/parser.h.py:290
        generated_code += add_code(' {}, // field_{}_{}'.format(fldtype.size, hdr.name, fld.name), 290)
generated_code += add_code(" };", 291)

generated_code += add_code("")
generated_code += add_code(" static const int field_bit_offset[FIELD_COUNT] = {", 293)
for hdr in hlir16.header_types: ## compiler/backend/parser.h.py:295
    for fld in hdr.fields: ## compiler/backend/parser.h.py:296
        generated_code += add_code('   ({} % 8), // field_{}_{}'.format(fld.offset, hdr.name, fld.name), 296)
generated_code += add_code(" };", 297)

generated_code += add_code("")
generated_code += add_code(" static const int field_byte_offset_hdr[FIELD_COUNT] = {", 299)
for hdr in hlir16.header_types: ## compiler/backend/parser.h.py:301
    for fld in hdr.fields: ## compiler/backend/parser.h.py:302
        generated_code += add_code('   ({} / 8), // field_{}_{}'.format(fld.offset, hdr.name, fld.name), 302)
generated_code += add_code(" };", 303)

generated_code += add_code("")
generated_code += add_code(" #define mod_top(n, bits) (((bits-(n%bits)) % bits))", 305)
generated_code += add_code(" // Produces a 32 bit int that has n bits on at the top end.", 306)
generated_code += add_code(" #define uint32_top_bits(n) (0xffffffff << mod_top(n, 32))", 307)

generated_code += add_code("")
generated_code += add_code(" static const int field_mask[FIELD_COUNT] = {", 309)
for hdr in hlir16.header_types: ## compiler/backend/parser.h.py:311
    for fld in hdr.fields: ## compiler/backend/parser.h.py:312
        fldtype = fld.canonical_type() ## compiler/backend/parser.h.py:313
        generated_code += add_code('  __bswap_constant_32(uint32_top_bits({}) >> ({}%8)), // field_{}_{},'.format(fldtype.size, fld.offset, hdr.name, fld.name), 313)
generated_code += add_code(" };", 314)

generated_code += add_code("")
generated_code += add_code(" static const header_t field_header[FIELD_COUNT] = {", 316)
for hdr in hlir16.header_types: ## compiler/backend/parser.h.py:318
    for fld in hdr.fields: ## compiler/backend/parser.h.py:319
        generated_code += add_code('   header_{}, // field_{}_{}'.format(hdr.name, hdr.name, fld.name), 319)
generated_code += add_code(" };", 320)

generated_code += add_code("")
generated_code += add_code(" static const int header_var_width_field[HEADER_COUNT] = {", 322)
for hdr in hlir16.header_types: ## compiler/backend/parser.h.py:324
    generated_code += add_code('   {}, // {}'.format(reduce((lambda x, f: f.id if hasattr(f, 'is_vw') and f.is_vw else x), hdr.fields, 'FIXED_WIDTH_FIELD'), hdr.name), 324)
generated_code += add_code(" };", 325)

generated_code += add_code("")
has_CloneType = True ## compiler/backend/parser.h.py:328
for enum in hlir16.objects['Type_Enum']: ## compiler/backend/parser.h.py:329
    generated_code += add_code(' enum {} {{ {} }};'.format(enum.c_name, ', '.join([m.c_name for m in enum.members])), 329)
    if enum.c_name == 'enum_CloneType': ## compiler/backend/parser.h.py:331
        has_CloneType = False ## compiler/backend/parser.h.py:332
if has_CloneType: ## compiler/backend/parser.h.py:333
    generated_code += add_code(" enum enum_CloneType { enum_CloneType_I2E, enum_CloneType_E2E };", 333)
for error in hlir16.objects['Type_Error']: ## compiler/backend/parser.h.py:335
    generated_code += add_code(' enum {} {{ {} }};'.format(error.c_name, ', '.join([m.c_name for m in error.members])), 335)
generated_code += add_code("", 337)
generated_code += add_code(" // HW optimization related infos", 338)
generated_code += add_code(" // --------------------", 339)
generated_code += add_code(' #define OFFLOAD_CHECKSUM {}'.format('true' if []!=[x for x in hlir16.sc_annotations if x.name=='offload'] else 'false'), 340)

generated_code += add_code("")
generated_code += add_code(" // Parser state local vars", 343)
generated_code += add_code(" // -----------------------", 344)

generated_code += add_code("")
parser = hlir16.objects['P4Parser'][0] ## compiler/backend/parser.h.py:347

generated_code += add_code("")
generated_code += add_code(" typedef struct parser_state_s {", 348)
for loc in parser.parserLocals: ## compiler/backend/parser.h.py:350
    if hasattr(loc.type, 'type_ref'): ## compiler/backend/parser.h.py:351
        if loc.type.type_ref.node_type == 'Type_Extern': ## compiler/backend/parser.h.py:352
            generated_code += add_code(' {} {};'.format(loc.type.type_ref.name, loc.name), 352)
        elif loc.type.type_ref.node_type == 'Type_Header': ## compiler/backend/parser.h.py:354
            generated_code += add_code(' {} {};'.format(loc.type.type_ref.name, loc.name), 354)
        else: ## compiler/backend/parser.h.py:356
            generated_code += add_code(' uint8_t {}[{}]; // type: {}'.format(loc.name, loc.type.type_ref.byte_width, loc.type.type_ref.name), 356)
    else: ## compiler/backend/parser.h.py:358
        if loc.type.node_type == 'Type_Bits': ## compiler/backend/parser.h.py:359
            if loc.type.size<=8: ## compiler/backend/parser.h.py:360
                generated_code += add_code(' uint8_t {};'.format(loc.name), 360)
            else: ## compiler/backend/parser.h.py:362
                generated_code += add_code(' uint16_t {};'.format(loc.name), 362)
        else: ## compiler/backend/parser.h.py:364
            generated_code += add_code(' uint8_t {}_var; // Width of the variable width field // type: {}'.format(loc.name, loc.type.type_ref.name), 364)
generated_code += add_code(" } parser_state_t;", 366)

generated_code += add_code("")
generated_code += add_code(" #endif // __HEADER_INFO_H__", 369)
