add_library(cliutils INTERFACE IMPORTED)

find_path(cliutils_INCLUDE_DIR CLI/CLI.hpp)
mark_as_advanced(cliutils_INCLUDE_DIR)

find_package_handle_standard_args(cliutils DEFAULT_MSG
    cliutils_INCLUDE_DIR
    )

set(URL https://github.com/CLIUtils/CLI11.git)
set(VERSION v1.2.0)
set_target_description(cliutils "Flags parsing engine" ${URL} ${VERSION})


if (NOT cliutils_FOUND)
  externalproject_add(cliutils_cli11
      GIT_REPOSITORY    ${URL}
      GIT_TAG           ${VERSION}
      CONFIGURE_COMMAND "" # remove configure step
      BUILD_COMMAND     "" # remove build step
      INSTALL_COMMAND   "" # remove install step
      TEST_COMMAND      "" # remove test step
      UPDATE_COMMAND    "" # remove update step
      )
  externalproject_get_property(cliutils_cli11 source_dir)
  set(cliutils_INCLUDE_DIR ${source_dir}/include)
  file(MAKE_DIRECTORY ${cliutils_INCLUDE_DIR})

  add_dependencies(cliutils cliutils_cli11)
endif ()

set_target_properties(cliutils PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${cliutils_INCLUDE_DIR}
    )
