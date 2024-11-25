# CMakeList.txt: проект CMake для CMakeProject1; включите исходный код и определения,
# укажите здесь логику для конкретного проекта.
#

# Добавьте источник в исполняемый файл этого проекта.
add_executable (spolks_mpi_4_group "spolks_mpi_4_group.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET spolks_mpi_4_group PROPERTY CXX_STANDARD 20)
endif()

	# Поиск MPI
find_package(MPI REQUIRED)

# Включаем директории заголовочных файлов MPI
target_include_directories(spolks_mpi_4_group PRIVATE ${MPI_INCLUDE_PATH})

# Линкуем MPI-библиотеки
target_link_libraries(spolks_mpi_4_group PRIVATE MPI::MPI_CXX)

# TODO: Добавьте тесты и целевые объекты, если это необходимо.
