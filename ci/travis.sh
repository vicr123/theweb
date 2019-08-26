function ensureDeps {
    # Ensures that dependencies of a binary are satisfied in the application bundle
    # Get a list of dependencies
    otool -L $1 | cut -f1 -d' ' - | awk '{$1=$1};1' | while read dep; do
        if [[ $dep == "/usr/local"* ]] && [[ $dep != *".framework"* ]]; then
            local depfn=$(basename $dep)
            
            # Run the install_name_tool on this file
            echo "Changing dep $dep for $1 to @executable_path/../Frameworks/$depfn"
            install_name_tool -change $dep @executable_path/../Frameworks/$depfn $1
            
            # Ensure this dependency doesn't exist in the resulting folder and isn't a framework
            if ! [ -f $2/$depfn ]; then
                # Copy the dependency into the required folder
                cp $dep $2/$depfn
                chmod +w $2/$depfn
                
                # Copy all the required dependencies for this file
                ensureDeps $2/$depfn $2
            fi
        fi
    done
}

export CI="false"
if [ $STAGE = "script" ]; then
  if [ $TRAVIS_OS_NAME = "linux" ]; then
    echo "[TRAVIS] Preparing build environment"
    source /opt/qt510/bin/qt510-env.sh

    echo "[TRAVIS] Building and installing the-libs"
    git clone https://github.com/vicr123/the-libs.git
    cd the-libs
    git checkout blueprint
    qmake
    make
    sudo make install INSTALL_ROOT=/
    cd ..
    
    
    echo "[TRAVIS] Building theWeb"
    node buildtool.js --debug
    cd build
    echo "[TRAVIS] Installing into appdir"
    make install INSTALL_ROOT=~/appdir
    echo "[TRAVIS] Getting linuxdeployqt"
    wget -c -nv "https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage"
    chmod a+x linuxdeployqt-continuous-x86_64.AppImage
    echo "[TRAVIS] Building AppImage"
    ./linuxdeployqt-continuous-x86_64.AppImage ~/appdir/usr/share/applications/*.desktop -appimage -extra-plugins=iconengines/libqsvgicon.so,imageformats/libqsvg.so
  else
    echo "[TRAVIS] Building for macOS"
    export PATH="/usr/local/opt/qt/bin:$PATH"
    export PKG_CONFIG_PATH="/usr/local/opt/libffi/lib/pkgconfig"
    export CURRENT_BREW_QT_VERSION="5.13.0"
    
    echo "[TRAVIS] Building and installing the-libs"
    git clone https://github.com/vicr123/the-libs.git
    cd the-libs
    git checkout blueprint
    qmake
    make
    sudo make install INSTALL_ROOT=/
    cd ..
    
    echo "[TRAVIS] Building theWeb"
    node buildtool.js --debug
    cd build/browser
    mkdir theWeb.app/Contents/Libraries
    cp /usr/local/lib/libthe-libs*.dylib theWeb.app/Contents/Libraries/
    install_name_tool -change libthe-libs.1.dylib @executable_path/../Libraries/libthe-libs.1.dylib theWeb.app/Contents/MacOS/theWeb
    install_name_tool -change @rpath/QtWidgets.framework/Versions/5/QtWidgets @executable_path/../Frameworks/QtWidgets.framework/Versions/5/QtWidgets theWeb.app/Contents/Libraries/libthe-libs.1.dylib
    install_name_tool -change @rpath/QtGui.framework/Versions/5/QtGui @executable_path/../Frameworks/QtGui.framework/Versions/5/QtGui theWeb.app/Contents/Libraries/libthe-libs.1.dylib
    install_name_tool -change @rpath/QtCore.framework/Versions/5/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/5/QtCore theWeb.app/Contents/Libraries/libthe-libs.1.dylib
    echo "[TRAVIS] Deploying Qt Libraries"
    macdeployqt theWeb.app
    ln -s ../../../../../../ theWeb.app/Contents/Frameworks/QtWebEngineCore.framework/Helpers/QtWebEngineProcess.app/Contents/Frameworks
    
    for lib in QtGui QtCore QtWebEngineCore QtQuick QtWebChannel QtQml QtNetwork QtPositioning; do
        install_name_tool -change /usr/local/Cellar/qt/5.13.0/lib/$lib.framework/Versions/5/$lib @executable_path/../Frameworks/$lib.framework/Versions/5/$lib theWeb.app/Contents/Frameworks/QtWebEngineCore.framework/Helpers/QtWebEngineProcess.app/Contents/MacOS/QtWebEngineProcess
    done

    echo "[TRAVIS] Preparing Disk Image creator"
    npm install appdmg
    echo "[TRAVIS] Building Disk Image"
    ./node_modules/appdmg/bin/appdmg.js ./node-appdmg-config.json ~/theWeb-macOS.dmg
  fi
elif [ $STAGE = "before_install" ]; then
  if [ $TRAVIS_OS_NAME = "linux" ]; then
    wget -O ~/vicr12345.gpg.key https://vicr123.com/repo/apt/vicr12345.gpg.key
    sudo apt-key add ~/vicr12345.gpg.key
    sudo add-apt-repository 'deb https://vicr123.com/repo/apt/ubuntu bionic main'
    sudo add-apt-repository -y ppa:beineri/opt-qt-5.10.1-trusty
    sudo apt-get update -qq
    sudo apt-get install qt510-meta-minimal qt510x11extras qt510tools qt510translations qt510svg qt510websockets qt510multimedia qt510webengine xorg-dev libxcb-util0-dev libgl1-mesa-dev cmake3 
  else
    echo "[TRAVIS] Preparing to build for macOS"
  fi
elif [ $STAGE = "after_success" ]; then
  if [ $TRAVIS_OS_NAME = "linux" ]; then
    echo "[TRAVIS] Publishing AppImage"
    wget -c https://github.com/probonopd/uploadtool/raw/master/upload.sh
    cp theWeb*.AppImage theWeb-linux.AppImage
    cp theWeb*.AppImage.zsync theWeb-linux.AppImage.zsync
    bash upload.sh theWeb-linux.AppImage*
  else
    echo "[TRAVIS] Publishing Disk Image"
    cd ~
    wget -c https://github.com/probonopd/uploadtool/raw/master/upload.sh
    bash upload.sh theWeb-macOS.dmg
  fi
fi
