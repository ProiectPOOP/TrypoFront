cmake_minimum_required(VERSION 3.16)
project(Trypo_frontend VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Căutăm pachetele necesare (Widgets și Network)
find_package(Qt6 REQUIRED COMPONENTS Widgets Network)

# 2. Definim absolut toate fișierele sursă și header-ele
set(PROJECT_SOURCES
    main.cpp
    MainWindow.cpp
    MainWindow.h
    AdminDashboardPage.cpp
    AdminDashboardPage.h
    DetailsPage.cpp
    DetailsPage.h
    LoginPage.cpp
    LoginPage.h
    MainAppPage.cpp
    MainAppPage.h
    RegisterPage.cpp
    RegisterPage.h
    UserProfilePage.cpp
    UserProfilePage.h
    socketclient.cpp
    socketclient.h
    AppData.h
    Styles.h

    # Fișierul care se ocupă de poze:
    resources.qrc
)

# 3. Definim executabilul
qt_add_executable(Trypo_frontend MANUAL_FINALIZATION ${PROJECT_SOURCES})

# 4. Legăm toate librăriile de proiectul nostru
target_link_libraries(Trypo_frontend PRIVATE
    Qt6::Widgets
    Qt6::Network
)

# 5. Finalizarea (specifică pentru Qt6)
qt_finalize_executable(Trypo_frontend)