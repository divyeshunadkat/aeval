add_executable (apara APara.cpp)
target_link_libraries (apara ${Z3_LIBRARY} ${Boost_LIBRARIES} ${GMPXX_LIB} ${GMP_LIB})
llvm_config (apara bitwriter)
install(TARGETS apara RUNTIME DESTINATION bin)
