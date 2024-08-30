# C++/CLI のライブラリプロジェクト
add_library(clilib SHARED clilib.cpp)
# /clr オプションを適用するためCOMMON_LANGUAGE_RUNTIME を指定する
# Visual Studio 2015 以降はpure, safe は非推奨となっている
set_target_properties(clilib PROPERTIES COMMON_LANGUAGE_RUNTIME "")

# C++/CLI のライブラリをアプリケーションから利用するためのインポートライブラリを定義
# 別ディレクトリのプロジェクトで利用するためGLOBAL オプションを指定する
add_library(clilibimp SHARED IMPORTED GLOBAL)
# C++/CLI の#using ディレクティブで指定されたdll を検索するパスの指定オプションは
# マネージドライブラリかつインポートライブラリの時に適用される
# インポートライブラリをマネージドライブラリとして扱うためには
# IMPORTED_COMMON_LANGUAGE_RUNTIME を指定する
# C++/CLI ライブラリがlib ファイルを出力しない場合、IMPORTED_COMMON_LANGUAGE_RUNTIME に
# pure を指定しておく。空文字列を指定するとMixed と判定され、IMPORTED_IMPLIB を指定しないと
# エラー扱いになる
set_target_properties(clilibimp PROPERTIES
    IMPORTED_COMMON_LANGUAGE_RUNTIME "pure"
    IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/$(Configuration)/clilib.dll
)
