# Host build
set(CMAKE_SYSTEM_PROCESSOR x86_64)

set(MINGW64_PATH "C:/tools/mingw64/bin")
# Compiler and tools
set(CMAKE_C_COMPILER   "${MINGW64_PATH}/gcc.exe"     CACHE INTERNAL "C compiler"      )
set(CMAKE_CXX_COMPILER "${MINGW64_PATH}/g++.exe"     CACHE INTERNAL "CXX compiler"    )
set(CMAKE_ASM_COMPILER "${CMAKE_C_COMPILER}"         CACHE INTERNAL "ASM compiler"    )
set(CMAKE_AR           "${MINGW64_PATH}/ar.exe"      CACHE INTERNAL "Archiving tool"  )
set(CMAKE_LINKER       "${MINGW64_PATH}/ld.exe"      CACHE INTERNAL "Linker tool"     )
set(CMAKE_OBJCOPY      "${MINGW64_PATH}/objcopy.exe" CACHE INTERNAL "Copy tool"       )
set(CMAKE_OBJDUMP      "${MINGW64_PATH}/objdump.exe" CACHE INTERNAL "Dump tool"       )
set(CMAKE_RANLIB       "${MINGW64_PATH}/ranlib.exe"  CACHE INTERNAL "Randomizing tool")
set(CMAKE_SIZE         "${MINGW64_PATH}/size.exe"    CACHE INTERNAL "Size tool"       )
set(CMAKE_STRIP        "${MINGW64_PATH}/strip.exe"   CACHE INTERNAL "Strip utility"   )

# This is necessary to pass test compilation check
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
