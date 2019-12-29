#-------------------------------------------------
#
# Project created by QtCreator 2019-12-23T18:00:50
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = rc2ui
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
        pugixml/pugixml.cpp \
        rctoxml.cpp \
        tools.cpp \
        uchardet/CharDistribution.cpp \
        uchardet/JpCntx.cpp \
        uchardet/LangModels/LangArabicModel.cpp \
        uchardet/LangModels/LangBulgarianModel.cpp \
        uchardet/LangModels/LangCroatianModel.cpp \
        uchardet/LangModels/LangCzechModel.cpp \
        uchardet/LangModels/LangDanishModel.cpp \
        uchardet/LangModels/LangEsperantoModel.cpp \
        uchardet/LangModels/LangEstonianModel.cpp \
        uchardet/LangModels/LangFinnishModel.cpp \
        uchardet/LangModels/LangFrenchModel.cpp \
        uchardet/LangModels/LangGermanModel.cpp \
        uchardet/LangModels/LangGreekModel.cpp \
        uchardet/LangModels/LangHebrewModel.cpp \
        uchardet/LangModels/LangHungarianModel.cpp \
        uchardet/LangModels/LangIrishModel.cpp \
        uchardet/LangModels/LangItalianModel.cpp \
        uchardet/LangModels/LangLatvianModel.cpp \
        uchardet/LangModels/LangLithuanianModel.cpp \
        uchardet/LangModels/LangMalteseModel.cpp \
        uchardet/LangModels/LangPolishModel.cpp \
        uchardet/LangModels/LangPortugueseModel.cpp \
        uchardet/LangModels/LangRomanianModel.cpp \
        uchardet/LangModels/LangRussianModel.cpp \
        uchardet/LangModels/LangSlovakModel.cpp \
        uchardet/LangModels/LangSloveneModel.cpp \
        uchardet/LangModels/LangSpanishModel.cpp \
        uchardet/LangModels/LangSwedishModel.cpp \
        uchardet/LangModels/LangThaiModel.cpp \
        uchardet/LangModels/LangTurkishModel.cpp \
        uchardet/LangModels/LangVietnameseModel.cpp \
        uchardet/nsBig5Prober.cpp \
        uchardet/nsCharSetProber.cpp \
        uchardet/nsEUCJPProber.cpp \
        uchardet/nsEUCKRProber.cpp \
        uchardet/nsEUCTWProber.cpp \
        uchardet/nsEscCharsetProber.cpp \
        uchardet/nsEscSM.cpp \
        uchardet/nsGB2312Prober.cpp \
        uchardet/nsHebrewProber.cpp \
        uchardet/nsLatin1Prober.cpp \
        uchardet/nsMBCSGroupProber.cpp \
        uchardet/nsMBCSSM.cpp \
        uchardet/nsSBCSGroupProber.cpp \
        uchardet/nsSBCharSetProber.cpp \
        uchardet/nsSJISProber.cpp \
        uchardet/nsUTF8Prober.cpp \
        uchardet/nsUniversalDetector.cpp \
        uchardet/uchardet.cpp

HEADERS += \
        mainwindow.h \
        pugixml/pugiconfig.hpp \
        pugixml/pugixml.hpp \
        rctoxml.h \
        tools.h \
        uchardet/CharDistribution.h \
        uchardet/JpCntx.h \
        uchardet/nsBig5Prober.h \
        uchardet/nsCharSetProber.h \
        uchardet/nsCodingStateMachine.h \
        uchardet/nsEUCJPProber.h \
        uchardet/nsEUCKRProber.h \
        uchardet/nsEUCTWProber.h \
        uchardet/nsEscCharsetProber.h \
        uchardet/nsGB2312Prober.h \
        uchardet/nsHebrewProber.h \
        uchardet/nsLatin1Prober.h \
        uchardet/nsMBCSGroupProber.h \
        uchardet/nsPkgInt.h \
        uchardet/nsSBCSGroupProber.h \
        uchardet/nsSBCharSetProber.h \
        uchardet/nsSJISProber.h \
        uchardet/nsUTF8Prober.h \
        uchardet/nsUniversalDetector.h \
        uchardet/nscore.h \
        uchardet/prmem.h \
        uchardet/uchardet.h

FORMS += \
        mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=

RESOURCES += \
    resources.qrc
