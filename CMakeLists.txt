CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_LIST_DIR}/cmake/Modules/")

if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE) # Default to a Release build
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build [Release Debug]" FORCE)
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
    target_link_libraries(${PROJECT_TEST_NAME} gcov)
endif()

find_path(GLIB_INCLUDE_DIR NAMES glib.h PATH_SUFFIXES glib-2.0)

PROJECT(latino)

ADD_SUBDIRECTORY(src)
IF(UNIX)
  execute_process(COMMAND bash ${CMAKE_CURRENT_LIST_DIR}/tools/install_deps.sh)
  execute_process(COMMAND bash ${CMAKE_CURRENT_LIST_DIR}/tools/key.sh)
ELSE(UNIX)
  IF(MSVC)
      ADD_SUBDIRECTORY(jansson)
      ADD_SUBDIRECTORY(curl)
      #ADD_SUBDIRECTORY(hiredis)
  ENDIF(MSVC)
ENDIF(UNIX)

enable_testing ()
#se comentan los test que requieren entrada de datos leer()
add_test (NAME 00-vacio COMMAND latino ejemplos/00-vacio.lat)
add_test (NAME 01-comentarios COMMAND latino ejemplos/01-comentarios.lat)
add_test (NAME 02-hola COMMAND latino ejemplos/02-hola.lat)
add_test (NAME 03-variables COMMAND latino ejemplos/03-variables.lat)
add_test (NAME 04-expresiones COMMAND latino ejemplos/04-expresiones.lat)
add_test (NAME 05-si COMMAND latino ejemplos/05-si.lat)
add_test (NAME 06-mientras COMMAND latino ejemplos/06-mientras.lat)
add_test (NAME 07-hacer COMMAND latino ejemplos/07-hacer.lat)
#add_test (NAME test8 COMMAND latino ejemplos/08-elegir_error.lat)
add_test (NAME 09-desde COMMAND latino ejemplos/09-desde.lat)
add_test (NAME 10-funciones COMMAND latino ejemplos/10-funciones.lat)
add_test (NAME 11-fibonacci COMMAND latino ejemplos/11-fibonacci.lat)
add_test (NAME 12-factorial COMMAND latino ejemplos/12-factorial.lat)
#add_test (NAME test13 COMMAND latino ejemplos/13-leer-teclado.lat)
#add_test (NAME test14 COMMAND latino ejemplos/14-ejecutar_archivo.lat)
#add_test (NAME test15 COMMAND latino ejemplos/15-funciones_mate.lat)
add_test (NAME 16-funciones COMMAND latino ejemplos/16-funciones_cadena.lat)
add_test (NAME 17-escribir_archivo COMMAND latino ejemplos/17-escribir_archivo.lat)
add_test (NAME 18-leer_archivo COMMAND latino ejemplos/18-leer_archivo.lat)
add_test (NAME 19-operadores_logicos COMMAND latino ejemplos/19-operadores_logicos.lat)
add_test (NAME 20-operadores_comparacion COMMAND latino ejemplos/20-operadores_comparacion.lat)
add_test (NAME 21-operador_ternario COMMAND latino ejemplos/21-operador_ternario.lat)
#add_test (NAME test22 COMMAND latino ejemplos/22-teorema_pitagoras.lat)
#add_test (NAME test23 COMMAND latino ejemplos/23-listas.lat)
#add_test (NAME test24 COMMAND latino ejemplos/24-diccionarios.lat)


#if (CMAKE_BUILD_TYPE STREQUAL "Coverage")
#    include(CodeCoverage)
#    setup_target_for_coverage(${PROJECT}_coverage ${PROJECT_TEST_NAME} coverage)
#
#    SET(CMAKE_CXX_FLAGS "-g -O0 -fprofile-arcs -ftest-coverage -coverage")
#    SET(CMAKE_C_FLAGS "-g -O0 -fprofile-arcs -ftest-coverage -coverage")
#endif() #CMAKE_BUILD_TYPE STREQUAL "Coverage"

set(GCOV "gcov")
if(DEFINED ENV{GCOV})
  set(GCOV $ENV{GCOV})
endif()
add_custom_target(coverage
  COMMAND lcov --gcov-tool ${GCOV} --capture --directory . --output-file coverage.info
  COMMAND lcov --gcov-tool ${GCOV} --remove coverage.info "/usr/*" "*/test/*" --output-file coverage.info
  COMMAND lcov --gcov-tool ${GCOV} --list coverage.info
  COMMAND genhtml coverage.info --output-directory ./coverage
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  VERBATIM)
