file(REMOVE_RECURSE
  "libcassandra_static.pdb"
  "libcassandra_static.a"
)

# Per-language clean rules from dependency scanning.
foreach(lang CXX)
  include(CMakeFiles/cassandra_static.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
