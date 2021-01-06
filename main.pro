
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
QT       += core gui
QT       += network
QT       += svg

CONFIG += c++11 \
    static
    static-runtime

INCLUDEPATH += "C:\Program Files (x86)\Windows Kits\10\Include\10.0.18362.0\cppwinrt"
# QMAKE_CXXFLAGS += -std=c++17

#INCLUDEPATH += D:\Qt\Tools\OpenSSL\Win_x64\include
#LIBS += -LD:\Qt\Tools\OpenSSL\Win_x64\lib -llibcrypto -llibssl

LIBS += -lRasapi32 \
    -lAdvapi32

#QMAKE_LFLAGS_WINDOWS += /MANIFEST \
#    /MANIFESTFILE:vpn_cpp.exe.manifest \
#    /ALLOWISOLATION \
#    /MANIFESTUAC \
#    /MANIFESTUAC:level=highestAvailable \
#    /MANIFESTUAC:uiAccess='false'



# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    changepassword.cpp \
    forgetpassword.cpp \
    lineedit.cpp \
    listitem.cpp \
    login.cpp \
    main.cpp \
    mainwin.cpp \
    model_view/listmodel.cpp \
    passwordlineedit.cpp \
    runguard.cpp \
    update.cpp \
    win10/CertUtils.cpp \
    win10/IpRouteUtils.cpp \
    win10/RasCreateVpn.cpp

HEADERS += \
    changepassword.h \
    forgetpassword.h \
    lineedit.h \
    listitem.h \
    login.h \
    mainwin.h \
    model_view/listmodel.h \
    passwordlineedit.h \
    runguard.h \
    update.h \
    win10/CertUtils.h \
    win10/IpRouteUtils.h \
    win10/RasCreateVpn.h

FORMS += \
    changepassword.ui \
    forgetpassword.ui \
    listitem.ui \
    login.ui \
    mainwin.ui \
    update.ui

TRANSLATIONS += \
    vpn_cpp_zh_CN.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    images/pic_top_bg.png \
    images/pic_top_left.svg \
    login_bg.png

RESOURCES += \
    resource.qrc



win32 {
CONFIG += embed_manifest_exe
QMAKE_LFLAGS_WINDOWS += /MANIFESTUAC:level=\'requireAdministrator\'
}

VERSION = 0.7.2
QMAKE_TARGET_COMPANY = iLinkAll
QMAKE_TARGET_DESCRIPTION = An ipsec client for windows
RC_CODEPAGE = www.ilinkall.cn
RC_ICONS = images/icon.ico

#LANGUAGES = zh_CN

## parameters: var, prepend, append
#defineReplace(prependAll) {
# for(a,$$1):result += $$2$${a}$$3
# return($$result)
#}

#TRANSLATIONS = $$prependAll(LANGUAGES, $$PWD/translations/vpn_cpp_, .ts)


#TRANSLATIONS_FILES =

#qtPrepareTool(LRELEASE, lrelease)
#for(tsfile, TRANSLATIONS) {
# qmfile = $$shadowed($$tsfile)
# qmfile ~= s,.ts$,.qm,
# qmdir = $$dirname(qmfile)
# !exists($$qmdir) {
# mkpath($$qmdir)|error("Aborting.")
# }
# command = $$LRELEASE -removeidentical $$tsfile -qm $$qmfile
# system($$command)|error("Failed to run: $$command")
# TRANSLATIONS_FILES += $$qmfile
#}
