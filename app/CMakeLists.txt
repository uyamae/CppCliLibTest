# C++/CLI アプリケーション
add_executable(cliapp main.cpp)
# /clr オプションを付加するためCOMMON_LANGUAGE_RUNTIME を指定する
set_target_properties(cliapp PROPERTIES COMMON_LANGUAGE_RUNTIME "")
# インポートライブラリをリンクする
target_link_libraries(cliapp clilibimp)
# C++/CLI ライブラリのdll を実行ファイルと同じパスにコピーする
add_custom_command(TARGET cliapp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_FILE_DIR:cliapp> $<TARGET_RUNTIME_DLLS:cliapp>
    COMMAND_EXPAND_LISTS
)