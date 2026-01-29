add_library(implot STATIC 
    implot_demo.cpp
    implot_items.cpp
    implot.cpp
)

target_include_directories(implot PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ../imgui
)