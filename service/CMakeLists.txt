add_executable(CacheCleanerService
    service_main.cpp
    memory_cleaner.cpp
)

set_target_properties(CacheCleanerService PROPERTIES
    LINK_FLAGS "-Wl,--subsystem,windows"
)

# 💣 Vollständig statisch linken:
# - kein DLL-Bedarf (libgcc, libstdc++, pthread)
# - kein msvcrt oder console-Fenster-Popup
target_compile_options(CacheCleanerService PRIVATE
    -static-libgcc
    -static-libstdc++
)


target_link_options(CacheCleanerService PRIVATE
    -static
)

target_link_libraries(CacheCleanerService PRIVATE
    advapi32
)

