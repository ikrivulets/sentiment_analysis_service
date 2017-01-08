file(REMOVE_RECURSE
  "libcassandra.pdb"
  "libcassandra.so"
  "libcassandra.so.2.5.0"
  "libcassandra.so.2"
)

# Per-language clean rules from dependency scanning.
foreach(lang CXX)
  include(CMakeFiles/cassandra.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
