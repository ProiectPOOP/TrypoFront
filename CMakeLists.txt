cmake_minimum_required(VERSION 3.16)
project(Trypo_frontend VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Căutăm pachetele necesare (Widgets și Network)
find_package(Qt6 REQUIRED COMPONENTS Widgets Network)

# 2. Adăugăm fișierele sursă (Asigură-te că socketclient.cpp/h sunt aici dacă le-ai creat)
set(PROJECT_SOURCES





        socketclient.h
)

# 3. Definim executabilul
qt_add_executable(Trypo_frontend MANUAL_FINALIZATION ${PROJECT_SOURCES}
    AdminDashboardPage.cpp AdminDashboardPage.h AppData.h CMakeLists.txt DetailsPage.cpp DetailsPage.h json.hpp LoginPage.cpp LoginPage.h main.cpp Main.qml MainAppPage.cpp MainAppPage.h MainWindow.cpp MainWindow.h RegisterPage.cpp RegisterPage.h socketclient.cpp socketclient.h Styles.h UserProfilePage.cpp UserProfilePage.h






)

# 4. Legăm toate librăriile de proiectul nostru
# AICI era eroarea: "mytarget" trebuie să fie numele proiectului tău
target_link_libraries(Trypo_frontend PRIVATE
    Qt6::Widgets
    Qt6::Network
)

# 5. Finalizarea (specifică pentru Qt6)
qt_finalize_executable(Trypo_frontend)