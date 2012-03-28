include_directories (${CMAKE_CURRENT_BINARY_DIR}
                     ${CMAKE_CURRENT_SOURCE_DIR}
)

set (ktp_widgets_private_SRCS
    contact-grid-widget.cpp
    add-contact-dialog.cpp
)

set (ktp_widgets_private_HDRS
     contact-grid-widget.h
     add-contact-dialog.h
)

kde4_add_ui_files (ktp_widgets_private_SRCS 
                   add-contact-dialog.ui
)

kde4_add_library (ktpwidgetsprivate SHARED
                 ${ktp_widgets_private_SRCS}
)

#Raise SOVERSION for every 0.x cycle
set_target_properties(ktpwidgetsprivate PROPERTIES
                                                VERSION ${KTP_VERSION}
                                                SOVERSION ${KTP_SONUMBER}
                                                DEFINE_SYMBOL MAKE_KTP_LIB)

target_link_libraries (ktpwidgetsprivate
                       ${KDE4_KDECORE_LIBS}
                       ${TELEPATHY_QT4_LIBRARIES}
                       ${KDE4_KDEUI_LIBS}
                       ktpcommoninternalsprivate
                       ktpmodelsprivate
)

install (TARGETS ktpwidgetsprivate
         DESTINATION ${LIB_INSTALL_DIR}
)

install (FILES ${ktp_widgets_private_HDRS}
         DESTINATION ${INCLUDE_INSTALL_DIR}/KTp/Widgets
)