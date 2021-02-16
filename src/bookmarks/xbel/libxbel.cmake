set(XBEL_SRCS
    src/bookmarks/xbel/xbelreader.cpp
    src/bookmarks/xbel/xbelwriter.cpp
    src/bookmarks/bookmarknode.cpp
)

add_library(xbel STATIC ${XBEL_SRCS})
target_link_libraries(xbel Qt5::Core)
