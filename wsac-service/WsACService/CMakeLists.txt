project(wsac-service)

configure_file(Config.cs.in "${CMAKE_CURRENT_SOURCE_DIR}/Config.cs" @ONLY)

add_custom_target(wsac-service ALL
        COMMAND dotnet publish -r win-x64 -o "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)
