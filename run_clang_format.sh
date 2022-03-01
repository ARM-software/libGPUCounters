clang-format -i -style=file *.h *.cpp

DIR=vendor/arm/mali/
clang-format -i -style=file ${DIR}*.h ${DIR}*.hpp ${DIR}*.cpp

DIR=vendor/arm/pmu/
clang-format -i -style=file ${DIR}*.h ${DIR}*.cpp
