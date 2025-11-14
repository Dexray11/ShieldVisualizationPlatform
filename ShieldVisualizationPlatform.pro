QT       += core gui widgets sql network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

SOURCES += \
    main.cpp \
    src/ui/loginwindow.cpp \
    src/ui/mainmenuwindow.cpp \
    src/ui/dashboardwindow.cpp \
    src/ui/projectwindow.cpp \
    src/ui/projectmanagementwindow.cpp \
    src/ui/geological2dwidget.cpp \
    src/utils/stylehelper.cpp \
    src/database/DatabaseManager.cpp \
    src/database/UserDAO.cpp \
    src/database/ProjectDAO.cpp \
    src/database/WarningDAO.cpp \
    src/database/NewsDAO.cpp \
    src/models/User.cpp \
    src/models/Project.cpp \
    src/models/Warning.cpp \
    src/models/News.cpp

HEADERS += \
    src/ui/loginwindow.h \
    src/ui/mainmenuwindow.h \
    src/ui/dashboardwindow.h \
    src/ui/projectwindow.h \
    src/ui/projectmanagementwindow.h \
    src/ui/geological2dwidget.h \
    src/utils/stylehelper.h \
    src/database/DatabaseManager.h \
    src/database/UserDAO.h \
    src/database/ProjectDAO.h \
    src/database/WarningDAO.h \
    src/database/NewsDAO.h \
    src/models/User.h \
    src/models/Project.h \
    src/models/Warning.h \
    src/models/News.h

RESOURCES += \
    resources/resources.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# Windows specific settings
win32 {
    RC_ICONS = resources/icons/app_icon.ico
    VERSION = 1.0.0.0
    QMAKE_TARGET_COMPANY = "Shandong University of Science and Technology"
    QMAKE_TARGET_PRODUCT = "Shield Visualization Platform"
    QMAKE_TARGET_DESCRIPTION = "Intelligent Shield Construction Geological Visualization Platform"
    QMAKE_TARGET_COPYRIGHT = "Copyright (C) 2024"
}

DISTFILES += \
    resources/images/welcome_bg.jpg
