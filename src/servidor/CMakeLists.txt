add_library(funcionesServidor funcionesServidor.c funcionesServidor.h)
target_link_libraries(funcionesServidor PUBLIC macros usuario partida barco)

target_include_directories(funcionesServidor PUBLIC
                          ${CMAKE_CURRENT_LIST_DIR}
                          ${PROJECT_SOURCE_DIR}/src/servidor)

# ${CMAKE_CURRENT_LIST_DIR} dir. de este CMakeLisis.txt

#add_executable(computer_main computer_main.cc)
#target_link_libraries(computer_main PUBLIC computer product)
