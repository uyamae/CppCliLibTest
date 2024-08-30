# C++/CLI のライブラリをCMake で使用するサンプル

C++/CLI のライブラリをCMake で作成して利用する時にいくつか詰まったところを解決したサンプルプロ江ジェクトです。

## CMake でC++/CLI プロジェクトを作成する

CMake でC++/CLI のプロジェクトを作成する場合、ターゲットのプロパティにCOMMON_LANGUAGE_RUNTIME を指定します。

C++/CLI にはMixed, Pure, Safe などの種類がありますが、Visual Studio 2015 以降ではMixed 以外は非推奨となっています。
[混在 (ネイティブおよびマネージド) アセンブリ混在 (ネイティブおよびマネージド) アセンブリ](https://learn.microsoft.com/ja-jp/cpp/dotnet/mixed-native-and-managed-assemblies?view=msvc-170)

Mixed を指定する場合はプロパティの値に空文字列を指定します。

```cmake
set_target_properties(target PROPERTIES COMMON_LANGUAGE_RUNTIME "")
```

## CMake 3.28 以降でのC++ モジュールへの対応

CMake 3.27 で試した時にはSHARED 指定のライブラリをtarget_link_library に渡すだけでC++/CLI のアプリケーションからC++/CLI のライブラリを利用することができました。

しかし同じプロジェクトをCMake 3.29 でビルドした場合、cl.exe が/clr と/ifcOutput は同時に指定できない、というエラーを出力しました。

CMake 3.27 と3.29 の出力したvcxproj の差分を見るとScanSourceForModuleDependencies というプロパティの有無が違っていました。

CMake 3.28 でCXX_SCAN_FOR_MODULES というプロパティが追加されており、これにOFF を指定しておくとScanSourceForModuleDependencies がfalse となってコンパイラのエラーは回避できます。このサンプルプロジェクトではCMAKE_CXX_SCAN_FOR_MODULES を指定して、全体的に適用されるようにしています。

## using ディレクティブへの対応

C++/CLI では#using ディレクティブでdll ファイルを指定すると、そこに含まれる機能を利用できます。

#using ディレクティブで指定されたdll はルールに従って順に検索されます。
[#using ディレクティブ（C++/CLI）](https://learn.microsoft.com/ja-jp/cpp/preprocessor/hash-using-directive-cpp?view=msvc-170)

CMake ではcl.exe の/AI オプションに対応するAdditionalUsingDirectories プロパティをvcxproj に出力することができます。指定方法についての説明が見つけられず、CMake のソースコードを解析したところ次の条件に当てはまる場合に適用されることが分かりました。

1. ライブラリを利用するターゲットがネイティブではない(マネージドである)
2. ライブラリのターゲットがネイティブではない
3. ライブラリのターゲットがインポートされている
4. ライブラリがインターフェースではない

1, 2 は前述のCOMMON_LANGUAGE_RUNTIME がそれぞれのターゲットのプロパティで指定されていればOK です。3 は、add_library にIMPORTED が指定されているかどうかです。4 はadd_library でINTERFACE が指定されていなければOK ですが、INTERFACE はヘッダオンリーのライブラリなどで指定するタイプですので今回は問題ありません。

### Imported Library の対応

IMPORTED 指定はプロジェクト外で用意されたライブラリなどを利用するときに指定します。
[Imported Libraries](https://cmake.org/cmake/help/latest/command/add_library.html#imported-libraries)

Imported Library はビルド済みのライブラリを指定する前提で、add_library にソースファイルなどは指定できないようです。おなじCMake プロジェクト内でライブラリを作成している場合、通常のSHARED ライブラリとアプリケーションの間にImported Library をはさむようにします。

```cmake
add_library(lib SHARED lib.cpp)
set_target_property(lib PROPERTIES COMMON_LANGUAGE_RUNTIME "")

add_library(implib SHARED IMPORTED GLOBAL)
set_target_property(implib PROPERTIES
  IMPORTED_COMMON_LANGUAGE_RUNTIME "pure"
  IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/$(Configuration)/lib.dll
)

add_executable(exe main.cpp)
set_target_property(exe PROPERTIES COMMON_LANGUAGE_RUNTIME "")
target_link_libraries(exe implib)
```

この例ではまとめて書いていますが、実際にはadd_library とadd_executable は別のCMakeLists.txt に含まれています。

Imported Library では実際に利用するdll ファイルのパスをIMPORTED_LOCATION で指定します。また、このdll がマネージドであることを示すためにターゲットプロパティにIMPORTED_COMMON_LANGUAGE_RUNTIME を指定します。COMMON_LANGUAGE_RUNTIME ではないという点に注意です。

### IMPORTED_COMMON_LANGUAGE_RUNTIME にpure を指定する

Import Library のターゲットプロパティIMPORTED_COMMON_LANGUAGE_RUNTIME にpure を指定しているのは次のような理由です。

通常、C++ 用のdll をリンクする場合、インポートライブラリとしてlib ファイルを指定します。CMake のImported Library では、ターゲットプロパティIMPORTED_IMPLIB にdll に対応するlib ファイルのパスを指定します。このプロパティを指定しない場合、CMake は追加のライブラリとしてターゲット名-NOTFOUND をvcxproj に出力します。これは当然リンクエラーとなります。またCMake 自体の実行結果もエラー終了となります。

C++/CLI のdll でlib ファイルが出力されない場合、IMPORTED_COMMON_LANGUAGE_RUNTIME に"pure" を指定しておくとインポートライブラリは無視されるようです。Visual Studio 2015 以降ではpure は非推奨ですが、ここでのpure 指定はCMake の動作にのみ影響するものでビルドには影響ありません。

## dll を実行ファイルのパスにコピーする

C++/CLI でdll を利用する場合、dll は検索可能なパスに置いてある必要があります。CMake を利用する場合、ビルド後のカスタムコマンドで実行ファイルが依存するdll をコピーできるよう、Generator Expression が用意されています。
[$<TARGET_RUNTIME_DLLS:tgt>](https://cmake.org/cmake/help/latest/manual/cmake-generator-expressions.7.html#genex:TARGET_RUNTIME_DLLS)

```cmake
add_custom_command(TARGET exe POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_FILE_DIR:exe> $<TARGET_RUNTIME_DLLS:exe>
    COMMAND_EXPAND_LISTS
)
```

## このサンプルの実行方法

Windows, Visual Studio 2022 の場合

```
cmake -S . -B .\build -G "Visual Studio 17 2022"
cmake --build .\build
build\app\Debug\cliapp.exe
```

