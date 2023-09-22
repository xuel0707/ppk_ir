
# SPDX-License-Identifier: Apache-2.0
# Copyright 2019 Eotvos Lorand University, Budapest, Hungary

# Autogenerated file (from compiler/backend/stateful_memory.c.py), do not modify directly.
# Generator: PPK

global file_indentation_level
file_indentation_level = 0

# The last element is the innermost (current) style.
file_sugar_style = ['line_comment']


def add_code(line, lineno = None, file = "compiler/backend/stateful_memory.c.py"):
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
        if file == "../compiler/backend/stateful_memory.c.py":
            return " /* {} */ {}".format(lineno)
        return " /* {}:{} */".format(file, lineno)
    return line


generated_code += "// Autogenerated file (from compiler/backend/stateful_memory.c.py via ../compiler/backend/stateful_memory.c.py), do not modify directly.\n"
generated_code += "// Generator: PPK\n"
generated_code += "\n"

# Copyright 2018 Eotvos Lorand University, Budapest, Hungary ## compiler/backend/stateful_memory.c.py:2
#  ## compiler/backend/stateful_memory.c.py:3
# Licensed under the Apache License, Version 2.0 (the "License"); ## compiler/backend/stateful_memory.c.py:4
# you may not use this file except in compliance with the License. ## compiler/backend/stateful_memory.c.py:5
# You may obtain a copy of the License at ## compiler/backend/stateful_memory.c.py:6
#  ## compiler/backend/stateful_memory.c.py:7
#     http://www.apache.org/licenses/LICENSE-2.0 ## compiler/backend/stateful_memory.c.py:8
#  ## compiler/backend/stateful_memory.c.py:9
# Unless required by applicable law or agreed to in writing, software ## compiler/backend/stateful_memory.c.py:10
# distributed under the License is distributed on an "AS IS" BASIS, ## compiler/backend/stateful_memory.c.py:11
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. ## compiler/backend/stateful_memory.c.py:12
# See the License for the specific language governing permissions and ## compiler/backend/stateful_memory.c.py:13
# limitations under the License. ## compiler/backend/stateful_memory.c.py:14

generated_code += add_code("")
from utils.codegen import format_expr, format_statement, statement_buffer_value, format_declaration ## compiler/backend/stateful_memory.c.py:16

generated_code += add_code("")
generated_code += add_code(" #include \"stateful_memory.h\"", 17)

generated_code += add_code("")
generated_code += add_code(" global_state_t global_smem;", 19)

generated_code += add_code("")
generated_code += add_code(" void gen_init_smems() {", 21)
generated_code += add_code(" //printf(\"\\033[1;32m\"\"Stateful_memory.c: nothing has been done?\\n\"\"\\033[0m\");", 22)
generated_code += add_code(" #ifdef PPK_DEBUG", 23)
for table, smem in hlir16.all_meters + hlir16.all_counters: ## compiler/backend/stateful_memory.c.py:25
    for target in smem.smem_for: ## compiler/backend/stateful_memory.c.py:26
        if not smem.smem_for[target]: ## compiler/backend/stateful_memory.c.py:27
            continue ## compiler/backend/stateful_memory.c.py:28
        table_postfix = "_{}".format(table.name) if smem.smem_type in ["direct_counter", "direct_meter"] else "" ## compiler/backend/stateful_memory.c.py:29
        for c in smem.components: ## compiler/backend/stateful_memory.c.py:31
            cname = c['name'] ## compiler/backend/stateful_memory.c.py:32
            if smem.smem_type not in ["direct_counter", "direct_meter"]: ## compiler/backend/stateful_memory.c.py:33
                generated_code += add_code(' for (int idx = 0; idx < {}; ++idx) {{'.format(smem.amount), 33)
                generated_code += add_code('     strcpy(global_smem.{}[idx].name, "{}/{}");'.format(cname, smem.name, c['for']), 34)
                generated_code += add_code(" }", 35)
                generated_code += add_code(' global_smem.{}_amount = {};'.format(cname, smem.amount), 37)
            else: ## compiler/backend/stateful_memory.c.py:39
                generated_code += add_code(' strcpy(global_smem.{}_{}.name, "{}/{}");'.format(cname, table.name, smem.name, c['for']), 39)
    generated_code += add_code("", 41)
for smem in hlir16.registers: ## compiler/backend/stateful_memory.c.py:44
    for c in smem.components: ## compiler/backend/stateful_memory.c.py:45
        generated_code += add_code(' for (int idx = 0; idx < {}; ++idx) {{'.format(smem.amount), 45)
        generated_code += add_code('     strcpy(global_smem.{}[idx].name, "{}");'.format(smem.name, smem.name), 46)
        generated_code += add_code(" }", 47)
        generated_code += add_code(' global_smem.{}_amount = {};'.format(smem.name, smem.amount), 49)
    generated_code += add_code("", 51)
generated_code += add_code(" #endif", 53)
generated_code += add_code(" }", 54)
