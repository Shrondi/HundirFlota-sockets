#add_library(cliente cliente.c)
#target_link_libraries(cliente PUBLIC funcionesCliente)

#target_include_directories(cliente PUBLIC
                          #${CMAKE_CURRENT_LIST_DIR}
                          #${PROJECT_SOURCE_DIR}/src/cliente)
# ${CMAKE_CURRENT_LIST_DIR} dir. de este CMakeLisis.txt

add_executable(cliente cliente.c)
target_link_libraries(cliente PUBLIC funcionesCliente)



#add_library(servidor servidor.c)
#target_link_libraries(servidor PUBLIC funcionesServidor usuario macros)

#target_include_directories(servidor PUBLIC
                          #${CMAKE_CURRENT_LIST_DIR}
                          #${PROJECT_SOURCE_DIR}/src/servidor /src/include)
# ${CMAKE_CURRENT_LIST_DIR} dir. de este CMakeLisis.txt

add_executable(servidor servidor.c)
target_link_libraries(servidor PUBLIC funcionesServidor usuario macros partida barco)
