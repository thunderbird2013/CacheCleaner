add_executable(CacheCleanerGUI
    main.cpp
    tray_icon.cpp
    ipc_client.cpp
    about_dialog.cpp
    resources.rc
)

# GUI → echtes Windows-Tray ohne Konsole
set_target_properties(CacheCleanerGUI PROPERTIES
    LINK_FLAGS "-Wl,--subsystem,windows"
)

target_link_libraries(CacheCleanerGUI PRIVATE -static shell32 comctl32)

