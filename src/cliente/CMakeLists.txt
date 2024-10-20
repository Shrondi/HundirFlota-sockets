add_library(funcionesCliente funcionesCliente.c funcionesCliente.h)
target_link_libraries(funcionesCliente PUBLIC macros)

target_include_directories(funcionesCliente PUBLIC
                          ${CMAKE_CURRENT_LIST_DIR}
                          ${PROJECT_SOURCE_DIR}/src/cliente)
                          
# ${CMAKE_CURRENT_LIST_DIR} dir. de este CMakeLisis.txt

#add_executable(computer_main computer_main.cc)
#target_link_libraries(computer_main PUBLIC computer product)
