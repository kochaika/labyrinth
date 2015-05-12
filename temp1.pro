#-------------------------------------------------
#
# Project created by QtCreator 2015-03-01T18:12:11
#
#-------------------------------------------------

QT       += core gui
QT += sensors

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = temp1
TEMPLATE = app

RC_FILE = icon.rc

SOURCES += main.cpp\
        mainwindow.cpp \
    square.cpp \
    result.cpp

HEADERS  += mainwindow.h \
    square.h \
    result.h

FORMS    += mainwindow.ui

DISTFILES += \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/AndroidManifest.xml \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew \
    android/gradlew.bat

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
