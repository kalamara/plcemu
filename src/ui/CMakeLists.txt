cmake_minimum_required(VERSION 3.22)

set(CMAKE_BUILD_TYPE Debug)

project("plcemu-ui")

find_library(YAML yaml)
find_library(ZMQ zmq)

#find_library(YACS yacs-yml)

include_directories(
	${PROJECT_SOURCE_DIR}
)
	
link_directories(
    ${PROJECT_BINARY_DIR}
)	

# console ui
add_library(${PROJECT_NAME}-std
    SHARED
    ${PROJECT_SOURCE_DIR}/cli.c
    ${PROJECT_SOURCE_DIR}/ui-std.c
    )
if(${REMOTE-UI})    
message("Building remote UI client...")
# remote ui
add_library(${PROJECT_NAME}-zmq
    SHARED
    ${PROJECT_SOURCE_DIR}/cli.c 
    ${PROJECT_SOURCE_DIR}/ui-zmq.c  
)

target_link_libraries(${PROJECT_NAME}-zmq ${ZMQ})
endif(${REMOTE-UI})  

