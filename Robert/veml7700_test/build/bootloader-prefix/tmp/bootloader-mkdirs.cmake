# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "D:/espressif/esp/v5.0.2/esp-idf/components/bootloader/subproject"
  "D:/Jakov Begovic/Desk/WES kod/TolkonN/Tolkon/Robert/veml7700_test/build/bootloader"
  "D:/Jakov Begovic/Desk/WES kod/TolkonN/Tolkon/Robert/veml7700_test/build/bootloader-prefix"
  "D:/Jakov Begovic/Desk/WES kod/TolkonN/Tolkon/Robert/veml7700_test/build/bootloader-prefix/tmp"
  "D:/Jakov Begovic/Desk/WES kod/TolkonN/Tolkon/Robert/veml7700_test/build/bootloader-prefix/src/bootloader-stamp"
  "D:/Jakov Begovic/Desk/WES kod/TolkonN/Tolkon/Robert/veml7700_test/build/bootloader-prefix/src"
  "D:/Jakov Begovic/Desk/WES kod/TolkonN/Tolkon/Robert/veml7700_test/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "D:/Jakov Begovic/Desk/WES kod/TolkonN/Tolkon/Robert/veml7700_test/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "D:/Jakov Begovic/Desk/WES kod/TolkonN/Tolkon/Robert/veml7700_test/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
