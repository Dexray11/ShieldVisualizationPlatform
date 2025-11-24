QT       += core gui widgets sql network 3dcore 3drender 3dinput 3dextras

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
    src/ui/geological3dwidget.cpp \
    src/ui/mapwidget.cpp \
    src/ui/positioningdialog.cpp \
    src/utils/stylehelper.cpp \
    src/utils/CoordinateConverter.cpp \
    src/utils/DataImportTool.cpp \
    src/utils/GeoDataImporter.cpp \
    src/database/DatabaseManager.cpp \
    src/database/UserDAO.cpp \
    src/database/ProjectDAO.cpp \
    src/database/WarningDAO.cpp \
    src/database/NewsDAO.cpp \
    src/database/BoreholeDAO.cpp \
    src/database/TunnelProfileDAO.cpp \
    src/database/MileageDAO.cpp \
    src/database/ShieldPositionDAO.cpp \
    src/database/ExcavationParameterDAO.cpp \
    src/database/ProspectingDataDAO.cpp \
    src/models/User.cpp \
    src/models/Project.cpp \
    src/models/Warning.cpp \
    src/models/News.cpp \
    src/models/ExcavationParameter.cpp \
    src/models/ProspectingData.cpp \
    src/api/ApiServer.cpp \
    src/api/DataSimulator.cpp \
    src/api/ApiManager.cpp

HEADERS += \
    src/ui/loginwindow.h \
    src/ui/mainmenuwindow.h \
    src/ui/dashboardwindow.h \
    src/ui/projectwindow.h \
    src/ui/projectmanagementwindow.h \
    src/ui/geological2dwidget.h \
    src/ui/geological3dwidget.h \
    src/ui/mapwidget.h \
    src/ui/positioningdialog.h \
    src/utils/stylehelper.h \
    src/utils/CoordinateConverter.h \
    src/utils/DataImportTool.h \
    src/utils/GeoDataImporter.h \
    src/database/DatabaseManager.h \
    src/database/UserDAO.h \
    src/database/ProjectDAO.h \
    src/database/WarningDAO.h \
    src/database/NewsDAO.h \
    src/database/BoreholeDAO.h \
    src/database/TunnelProfileDAO.h \
    src/database/MileageDAO.h \
    src/database/ShieldPositionDAO.h \
    src/database/ExcavationParameterDAO.h \
    src/database/ProspectingDataDAO.h \
    src/models/User.h \
    src/models/Project.h \
    src/models/Warning.h \
    src/models/News.h \
    src/models/ExcavationParameter.h \
    src/models/ProspectingData.h \
    src/api/ApiServer.h \
    src/api/DataSimulator.h \
    src/api/ApiManager.h

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
