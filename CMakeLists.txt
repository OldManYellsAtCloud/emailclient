cmake_minimum_required(VERSION 3.16)

project(emailclient VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.2 COMPONENTS Quick Sql REQUIRED)

set(HEADERS include/mailsettings.h
            include/curlresponse.h
            include/dbengine.h
            include/mail.h
            include/utils.h
            include/MailStructure.h
            include/parsers/imap/examine.h
            include/parsers/imap/folderlist.h
            include/parsers/imap/capability.h
            include/parsers/imap/mailheader.h
            include/parsers/imap/uid.h
            include/parsers/imap/flags.h
            include/parsers/imap/bodystructure.h
            include/parsers/imap/bodystructures.h
            include/mailengine.h
            include/models/mailmodel.h
            include/models/modelfactory.h)

set(SOURCES mailsettings.cpp
            curlresponse.cpp
            dbengine.cpp
            utils.cpp
            MailStructure.cpp
            src/parsers/imap/examine.cpp
            mailengine.cpp
            src/parsers/imap/folderlist.cpp
            src/parsers/imap/capability.cpp
            src/parsers/imap/mailheader.cpp
            src/parsers/imap/uid.cpp
            src/parsers/imap/bodystructure.cpp
            src/parsers/imap/bodystructures.cpp
            src/parsers/imap/flags.cpp
            src/models/modelfactory.cpp
            src/models/mailmodel.cpp)

qt_add_executable(emailclient
    main.cpp
    ${HEADERS}
    ${SOURCES}
)

qt_add_qml_module(emailclient
    URI mailclient
    VERSION 1.0
    QML_FILES main.qml
    SOURCES include/models/foldermodel.h src/models/foldermodel.cpp
    SOURCES include/models/mailmodel.h src/models/mailmodel.cpp
    SOURCES include/models/modelfactory.h src/models/modelfactory.cpp
    QML_FILES MailHeader.qml
    QML_FILES FolderListDelegate.qml
)

target_link_directories(emailclient PRIVATE $ENV{CMAKE_SYSROOT}/usr/lib)

target_include_directories(emailclient PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include
                    ${CMAKE_CURRENT_SOURCE_DIR}/include/models
                    ${CMAKE_CURRENT_SOURCE_DIR}/include/parsers)

target_link_libraries(emailclient
    PRIVATE Qt6::Quick Qt6::Sql curl SettingsLib LogLibrary)


install(TARGETS emailclient
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

option(enable_test "Compile and run tests" OFF)

if (enable_test)
    find_package(GTest REQUIRED)
    enable_testing()

    set(TEST_SOURCES tests/mailstructure_test.cpp
                     tests/mailengine_test.cpp
                     tests/misc_test.cpp)

    add_executable(mailclient_test ${HEADERS} ${SOURCES} ${TEST_SOURCES}
    include/models/foldermodel.h src/models/foldermodel.cpp
    )


    target_link_directories(mailclient_test PRIVATE $ENV{CMAKE_SYSROOT}/usr/lib)
    target_link_libraries(mailclient_test GTest::gtest_main Qt6::Quick Qt6::Sql curl SettingsLib LogLibrary)

    target_include_directories(mailclient_test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

    include(GoogleTest)
    # gtest_discover_tests(mailclient_test)

    install(TARGETS mailclient_test
        BUNDLE DESTINATION .
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

endif()
