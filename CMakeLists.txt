cmake_minimum_required(VERSION 3.22)

set(CMAKE_BUILD_TYPE Debug)
set(REMOTE-UI OFF)

project("plcemu")

find_library(CUNIT cunit)

find_library(YAML yaml)
if(NOT YAML)
    message(FATAL ERROR ": YAML not found")
endif(NOT YAML)        

if(${REMOTE-UI})
    find_library(ZMQ zmq)
    if(NOT ZMQ)
        message(FATAL ERROR ": ZMQ not found, please build command line variation")
    endif(NOT ZMQ)        
endif(${REMOTE-UI})    

find_library(PTHREAD pthread)
if(NOT PTHREAD)
    message(FATAL ERROR "PTHREAD not found")
endif(NOT PTHREAD)  
      
#find_library(NACS nacs-yml)

include_directories(
	${PROJECT_SOURCE_DIR}/src
	${PROJECT_SOURCE_DIR}/src/cfg
	${PROJECT_SOURCE_DIR}/src/ui
	${PROJECT_SOURCE_DIR}/nacs/include
    ${PROJECT_SOURCE_DIR}/librelogic/include
)
	
link_directories(
    ${PROJECT_BINARY_DIR}
    ${PROJECT_BINARY_DIR}/ui
    ${PROJECT_BINARY_DIR}/nacs
    ${PROJECT_BINARY_DIR}/librelogic
)	

add_subdirectory(${PROJECT_SOURCE_DIR}/src/ui ${PROJECT_BINARY_DIR}/ui)
add_subdirectory(${PROJECT_SOURCE_DIR}/nacs ${PROJECT_BINARY_DIR}/nacs)
add_subdirectory(${PROJECT_SOURCE_DIR}/librelogic/src ${PROJECT_BINARY_DIR}/librelogic)


# test targets
if(CUNIT)

    add_executable(test_app
        ${PROJECT_SOURCE_DIR}/tst/white/app/app-stubs.c 
        ${PROJECT_SOURCE_DIR}/tst/white/app/ut-app.c  
        ${PROJECT_SOURCE_DIR}/src/cfg/schema.c 
        ${PROJECT_SOURCE_DIR}/src/ui/cli.c
        ${PROJECT_SOURCE_DIR}/src/app.c 
    )

    target_link_libraries(
        test_app PUBLIC ${CUNIT} ${YAML} -llogic -lnacs-yml -lgcov 
       
    )
    add_dependencies(test_app  nacs-yml logic)
endif(CUNIT)    

#console version
add_executable(${PROJECT_NAME}

    ${PROJECT_SOURCE_DIR}/src/cfg/schema.c 
    ${PROJECT_SOURCE_DIR}/src/ui/cli.c
    ${PROJECT_SOURCE_DIR}/src/app.c 
    ${PROJECT_SOURCE_DIR}/src/project.c 
    ${PROJECT_SOURCE_DIR}/src/plcemu.c 
    ${PROJECT_SOURCE_DIR}/src/util.c 
)
add_dependencies(${PROJECT_NAME} nacs-yml logic)

if(${REMOTE-UI})
    message("Building remote UI client...")
    add_dependencies(${PROJECT_NAME} ${PROJECT_NAME}-ui-zmq)
    target_link_libraries(
        ${PROJECT_NAME} PUBLIC ${YAML} ${ZMQ} -lnacs-yml -llogic -l${PROJECT_NAME}-ui-zmq -lm -lpthread
    )
    add_executable(cli
        ${PROJECT_SOURCE_DIR}/src/ui/cli-zmq.c
        ${PROJECT_SOURCE_DIR}/src/ui/cli.c
        ${PROJECT_SOURCE_DIR}/src/ui/ui-zmq.c
        ${PROJECT_SOURCE_DIR}/src/cfg/schema.c 
        ${PROJECT_SOURCE_DIR}/src/util.c 
    )
    target_link_libraries(
        cli PUBLIC ${YAML} ${ZMQ} -lnacs-yml -lm -lpthread
    )
else(${REMOTE-UI})
    message("Building std client...")
    add_dependencies(${PROJECT_NAME} ${PROJECT_NAME}-ui-std)
    target_link_libraries(
        ${PROJECT_NAME} PUBLIC ${YAML} -llogic -lnacs-yml -l${PROJECT_NAME}-ui-std -lm -lpthread
    ) 
endif(${REMOTE-UI})    





