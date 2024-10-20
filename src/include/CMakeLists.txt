add_library(macros INTERFACE macros.h)
add_library(usuario INTERFACE usuario.h)
add_library(partida INTERFACE partida.h)
add_library(barco INTERFACE barco.h)


target_include_directories(macros INTERFACE
                          ${CMAKE_CURRENT_LIST_DIR}
                          ${PROJECT_SOURCE_DIR}/src/include)
                          
target_include_directories(usuario INTERFACE
                          ${CMAKE_CURRENT_LIST_DIR}
                          ${PROJECT_SOURCE_DIR}/src/include)                        

target_include_directories(partida INTERFACE
                          ${CMAKE_CURRENT_LIST_DIR}
                          ${PROJECT_SOURCE_DIR}/src/include)
                          
target_include_directories(barco INTERFACE
                          ${CMAKE_CURRENT_LIST_DIR}
                          ${PROJECT_SOURCE_DIR}/src/include)


