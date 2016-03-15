INCLUDE(PythonEnvironment)
GET_PYTHON_ENV()

# working directory of tester should be binary dir so link data folders
IF (NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/reference_results)
	IF (NOT EXISTS ${CMAKE_SOURCE_DIR}/data/testsuite/meta)
		MESSAGE(FATAL_ERROR "Test suite for meta examples does not exists!")
	ENDIF()
	EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E create_symlink "${CMAKE_SOURCE_DIR}/data/testsuite/meta/" "${CMAKE_CURRENT_BINARY_DIR}/reference_results")
ENDIF()
IF (NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/generated_results)
	IF (NOT EXISTS ${CMAKE_BINARY_DIR}/examples/meta)
		#MESSAGE(FATAL_ERROR "Meta examples directory does not exists!")
	ENDIF()
	EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E create_symlink "${CMAKE_BINARY_DIR}/examples/meta" "${CMAKE_CURRENT_BINARY_DIR}/generated_results")
ENDIF()

# compile tester c++ binary
ADD_EXECUTABLE(meta_example_integration_tester ${CMAKE_CURRENT_SOURCE_DIR}/tester.cpp)
ADD_DEPENDENCIES(meta_example_integration_tester shogun)
target_link_libraries(meta_example_integration_tester shogun ${SANITIZER_LIBRARY})

FILE(GLOB META_INTEGRATION_REFERENCES ${CMAKE_CURRENT_BINARY_DIR}/reference_results/*/*.dat)
FOREACH(REFERENCE_FILE ${META_INTEGRATION_REFERENCES})
	# assume a structure <target_language>/<category>/result.dat
	STRING(REGEX REPLACE ".*/(.*).dat" "\\1" NAME ${REFERENCE_FILE})
	STRING(REGEX REPLACE ".*/(.*//.*).dat" "\\1" NAME_WITH_DIR ${REFERENCE_FILE})
	STRING(REGEX REPLACE "//" "_" NAME_WITH_DIR ${NAME_WITH_DIR})
	STRING(REGEX REPLACE ".*/(.*)//.*.dat" "\\1" REL_DIR ${REFERENCE_FILE})

	# cpp
	# TODO implement other languages tests
	IF(HAVE_CXX0X OR HAVE_CXX11)
	    add_test(NAME integration_meta_cpp-${NAME_WITH_DIR}
		    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		    COMMAND meta_example_integration_tester ${REL_DIR} ${NAME}.dat cpp generated_results reference_results)
	    set_tests_properties(
		    integration_meta_cpp-${NAME_WITH_DIR}
			    PROPERTIES
			    DEPENDS generated_cpp-${NAME}
	    )
	ELSE()
	    MESSAGE(WARNING "Skipping C++ meta integration tests; requires a c++11 compatible compiler.")
	ENDIF()
ENDFOREACH()