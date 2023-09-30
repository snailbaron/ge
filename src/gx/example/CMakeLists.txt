configure_file(bullet.png bullet.png COPYONLY)
configure_file(button.png button.png COPYONLY)
configure_file(cursor.png cursor.png COPYONLY)
configure_file(grass.png grass.png COPYONLY)
configure_file(hero.png hero.png COPYONLY)
configure_file(nasalization-rg.otf nasalization-rg.otf COPYONLY)
configure_file(press-animation.png press-animation.png COPYONLY)
configure_file(stone.png stone.png COPYONLY)
configure_file(tree.png tree.png COPYONLY)

add_executable(gx-example
    main.cpp
)
target_link_libraries(gx-example PRIVATE gx)
