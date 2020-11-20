project(cpp_di)

#module definitions

add_library(cpp_di "")

target_sources(cpp_di
   PRIVATE
      Container.h
      Container.cxx
      Module.h
      Module.cxx
      Registration.h
   PUBLIC
      di.h
)


target_include_directories(cpp_di PRIVATE ${SRC_DIR})
