if "%APPVEYOR_REPO_TAG_NAME%"=="continuous" (

    exit 1

)

git submodule init
git submodule update

powershell Install-Product node 10 x64

set QTDIR=C:\Qt\5.12\msvc2017_64
set PATH=%PATH%;%QTDIR%\bin
call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat"

git clone https://github.com/vicr123/the-libs.git
cd the-libs
git checkout blueprint
qmake the-libs.pro "CONFIG+=release"
nmake release
nmake install
cd ..

node buildtool.js
mkdir deploy
mkdir deploy\translations
copy build\browser\release\theWeb.exe deploy
copy browser\translations\*.qm deploy\translations
copy "C:\Program Files\thelibs\lib\the-libs.dll" deploy
cd deploy
windeployqt theWeb.exe
