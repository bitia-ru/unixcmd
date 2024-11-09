Qt Example project
===================

To build the project run the following commands:

    mkdir build
    cd build
    conan install ..
    conan build ..

For Android an additional step is required:

    cmake --build .. --target aab
