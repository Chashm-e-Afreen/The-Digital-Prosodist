SOURCES  += \
    $$PWD/MainWindow.cpp

HEADERS  += \
    $$PWD/MainWindow.h \
    $$PWD/edit_dist.h \
    $$PWD/meters-def.h

FORMS    += \
    CustomWindow.ui

RESOURCES += \
    $$PWD/fonts.qrc \
    CustomWindow.qrc

RC_ICONS = executable.ico

OTHER_FILES += \
    CustomWindow.css
