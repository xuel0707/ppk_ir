/^#$/ { printf "#line %d \"/home/zjlab/ppk_ir/p4c/build/ir/%s\"\n", NR+1, name; next; } 1
