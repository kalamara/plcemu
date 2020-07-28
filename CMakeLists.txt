cmake_minimum_required(VERSION 2.8)

set(CMAKE_BUILD_TYPE Debug)
set(REMOTE-UI OFF)

project("plcemu")

find_library(CUNIT cunit)

find_library(YAML yaml)
if(NOT YAML)
    message(FATAL ERROR "YAML not found")
endif(NOT YAML)        

if(${REMOTE-UI})
    find_library(ZMQ zmq)
    if(NOT ZMQ)
        message(FATAL ERROR "ZMQ not found, please build command line variation")
    endif(NOT ZMQ)        
endif(${REMOTE-UI})    

find_library(COMEDI comedi)
if(COMEDI)
    add_compile_definitions(COMEDI)
else(COMEDI)
    message(WARNING "COMEDI drivers not found")    
endif(COMEDI)

find_library(PTHREAD pthread)
if(NOT PTHREAD)
    message(FATAL ERROR "PTHREAD not found")
endif(NOT PTHREAD)  
      
find_library(YACS yacs-yml)

include_directories(
	${PROJECT_SOURCE_DIR}/src
	${PROJECT_SOURCE_DIR}/src/cfg
	${PROJECT_SOURCE_DIR}/src/hw
	${PROJECT_SOURCE_DIR}/src/ui
	${PROJECT_SOURCE_DIR}/src/vm
)
	
link_directories(
    ${PROJECT_BINARY_DIR}
    ${PROJECT_BINARY_DIR}/ui
    ${PROJECT_SOURCE_DIR}/lib
)	

#TODO: make yacs as subrepo
#ADD_LIBRARY(${PROJECT_NAME}-yml
#    SHARED 
#    include/config.h
#    src/config.c
#    src/config-yml.c
#)

#target_link_libraries(
#    ${PROJECT_NAME}-yml PUBLIC ${YAML}    
#)	
#    

# test targets
if(CUNIT)
    add_executable(test_vm
        ${PROJECT_SOURCE_DIR}/tst/white/vm/ut-vm.c
        ${PROJECT_SOURCE_DIR}/tst/white/vm/vm-stubs.c
        ${PROJECT_SOURCE_DIR}/src/vm/data.c
        ${PROJECT_SOURCE_DIR}/src/vm/instruction.c
        ${PROJECT_SOURCE_DIR}/src/vm/rung.c
        ${PROJECT_SOURCE_DIR}/src/vm/plclib.c
        ${PROJECT_SOURCE_DIR}/src/vm/parser-il.c
        ${PROJECT_SOURCE_DIR}/src/vm/parser-ld.c
        ${PROJECT_SOURCE_DIR}/src/vm/parser-tree.c
        ${PROJECT_SOURCE_DIR}/src/vm/codegen.c
    )

    target_link_libraries(
    test_vm PUBLIC ${CUNIT} -lgcov    
    )	

    add_executable(test_app
        ${PROJECT_SOURCE_DIR}/tst/white/app/app-stubs.c 
        ${PROJECT_SOURCE_DIR}/tst/white/app/ut-app.c  
        ${PROJECT_SOURCE_DIR}/src/cfg/schema.c 
        ${PROJECT_SOURCE_DIR}/src/ui/cli.c
        ${PROJECT_SOURCE_DIR}/src/app.c 
    )

    target_link_libraries(
        test_app PUBLIC ${CUNIT} ${YAML} -lyacs-yml -lgcov 
       
    )
endif(CUNIT)    

add_subdirectory(${PROJECT_SOURCE_DIR}/src/ui ${PROJECT_BINARY_DIR}/ui)

#console version
add_executable(${PROJECT_NAME}
    ${PROJECT_SOURCE_DIR}/src/vm/data.c
    ${PROJECT_SOURCE_DIR}/src/vm/instruction.c
    ${PROJECT_SOURCE_DIR}/src/vm/rung.c
    ${PROJECT_SOURCE_DIR}/src/vm/plclib.c
    ${PROJECT_SOURCE_DIR}/src/vm/parser-il.c
    ${PROJECT_SOURCE_DIR}/src/vm/parser-ld.c
    ${PROJECT_SOURCE_DIR}/src/vm/parser-tree.c
    ${PROJECT_SOURCE_DIR}/src/vm/codegen.c
    ${PROJECT_SOURCE_DIR}/src/hw/hardware.c
    ${PROJECT_SOURCE_DIR}/src/hw/hardware-comedi.c
    ${PROJECT_SOURCE_DIR}/src/hw/hardware-sim.c
    ${PROJECT_SOURCE_DIR}/src/hw/hardware-uspace.c
    ${PROJECT_SOURCE_DIR}/src/hw/hardware-dry.c
    ${PROJECT_SOURCE_DIR}/src/cfg/schema.c 
    ${PROJECT_SOURCE_DIR}/src/ui/cli.c
    ${PROJECT_SOURCE_DIR}/src/app.c 
    ${PROJECT_SOURCE_DIR}/src/project.c 
    ${PROJECT_SOURCE_DIR}/src/plcemu.c 
    ${PROJECT_SOURCE_DIR}/src/util.c 
)

if(${REMOTE-UI})

    add_dependencies(${PROJECT_NAME} ${PROJECT_NAME}-ui-zmq)
    target_link_libraries(
        ${PROJECT_NAME} PUBLIC ${YAML} ${ZMQ} -lyacs-yml -l${PROJECT_NAME}-ui-zmq -lm -lpthread
    )
    add_executable(cli
        ${PROJECT_SOURCE_DIR}/src/ui/cli-zmq.c
        ${PROJECT_SOURCE_DIR}/src/ui/cli.c
        ${PROJECT_SOURCE_DIR}/src/ui/ui-zmq.c
        ${PROJECT_SOURCE_DIR}/src/cfg/schema.c 
        ${PROJECT_SOURCE_DIR}/src/util.c 
    )
    target_link_libraries(
        cli PUBLIC ${YAML} ${ZMQ} -lyacs-yml -lm -lpthread
    )
else(${REMOTE-UI})
    add_dependencies(${PROJECT_NAME} ${PROJECT_NAME}-ui-std)
    target_link_libraries(
        ${PROJECT_NAME} PUBLIC ${YAML} -lyacs-yml -l${PROJECT_NAME}-ui-std -lm -lpthread
    ) 
endif(${REMOTE-UI})    





