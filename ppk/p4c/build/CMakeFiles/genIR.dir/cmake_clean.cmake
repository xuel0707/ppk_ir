file(REMOVE_RECURSE
  "CMakeFiles/genIR"
  "ir/ir-generated.h"
  "ir/ir-generated.cpp"
  "ir/gen-tree-macro.h"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/genIR.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
