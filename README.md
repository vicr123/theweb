<img src="readme/splash.svg" width="100%" />

---
<p align="center">
<a href="https://travis-ci.org/vicr123/theWeb"><img src="https://img.shields.io/travis/vicr123/theweb/master?label=Linux%2C%20macOS&style=for-the-badge" alt="Travis CI Build Status" /></a>
<a href="https://ci.appveyor.com/project/vicr123/theWeb"><img src="https://img.shields.io/appveyor/ci/vicr123/theweb/master?label=Windows&style=for-the-badge" alt="AppVeyor Build Status" /></a>
<img src="https://img.shields.io/github/license/vicr123/theweb?style=for-the-badge" />
</p>

<p align="center">
<b>theWeb is currently in development.</b>
</p>

theWeb is a web browser.


---

# Dependencies
- Qt 5
  - Qt Core
  - Qt GUI
  - Qt Widgets
  - Qt WebEngine
  - Qt SVG
- node.js
- [the-libs](https://github.com/vicr123/the-libs)

# Get
If you're using a supported operating system, we may have binaries available:

| System | Package |
|-------------------|---------------------------------------------------------------------------------------------------------|
| Windows Portable | [Zip Archive on GitHub Releases](https://github.com/vicr123/theWeb/releases) |
| macOS | [Application Bundle on GitHub Releases](https://github.com/vicr123/theWeb/releases) |
| Linux | [AppImage on GitHub Releases](https://github.com/vicr123/theWeb/releases) |

**As theWeb is currently in development, these binaries may be unstable. Be sure to check back every so often for updates!**

## Build
Run the following commands in your terminal. 
```
node buildtool.js
```

## Install
On Linux, run the following command in your terminal (with superuser permissions)
```
cd build
make install
```

On macOS, drag the resulting application bundle (`theWeb.app` or `theWeb Blueprint.app`, depending on which branch you're building) into your Applications folder

# Contributing
Thanks for your interest in theWeb! Check out the [CONTRIBUTING.md](CONTRIBUTING.md) file to get started and see how you can help!

---

> © Victor Tran, 2019. This project is licensed under the GNU General Public License, version 3, or at your option, any later version.
> 
> Check the [LICENSE](LICENSE) file for more information.
