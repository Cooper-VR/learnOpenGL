file(REMOVE_RECURSE
  "libglad.a"
  "libglad.pdb"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/glad.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
