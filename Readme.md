# Marshmallow
A clang based version of [exlaunch](https://github.com/shadowninja108/exlaunch/) with some useful built-in tools.
Supporting clang directly allows better integration with IDEs via clangd and various other clang tools.
This project does not depend on the devkitPro switch toolchain.

## Setup
The only supported host platforms are Windows and Linux.

Dependencies: [CMake](https://cmake.org/)(>=v3.21) and [Ninja](https://ninja-build.org/).  
Both should be available from your package manager, be it `winget`, `apt`, `pacman` or anything else.

To configure the CMake project, run this command. See [definitions](#CMake definition args) for flags to add on.
```shell
cmake -S . -B build -G Ninja --toolchain=cmake/toolchain.cmake
```
And to build using Ninja, simply run:
```shell
ninja -C build
```
Any other target generators are untested and unsupported.

## Logging
As you can see in user/src/main.cpp, there's a few logger destinations available.
The default file location in that file is `sd:/mallow.log`.
If you want network logging, you can make a json file at `sd:/mallow.json` with the contents:
```json
{
  "logger": {
    // The address (hostname or IPv4) where the log server is hosted.
    "ip": "XXX.XXX.XXX.XXX",
    // The default port of choice is 3080.
    "port": 3080
  }
}
```
Hosting a log server on Linux (not WSL!) is as easy as running `nc -lk 3080`.
On Windows, there is an included logserver.js which also opens a raw tcp server.
You will need Node.js if you want to use logserver.js, otherwise you should make your own log server.

### CMake definition arguments:
- `-DFTP_IP=XXX.XXX.XXX.XXX`
  - optional, will enable FTP deployment builds if specified 
  - your switch's IP for deployment with the `subsdk9_deploy_ftp` target
- `-DFTP_PORT=5000`, `-DFTP_USERNAME=anonymous`, `-DFTP_PASSWORD=anonymous`
  - optional, uses defaults above
  - your switch's FTP port, username, and password
- `-DFTP_TITLE_ID=0100000000010000`
  - optional, uses defaults above
  - what title id to use when placing deployments into `/atmosphere/contents/FTP_TITLE_ID/exefs/`

## Credits
- [shadowninja108](https://github.com/shadowninja108)
  - [exlaunch](https://github.com/shadowninja108/exlaunch)
  - Lots of love <3
- [EngineLessCC](https://github.com/EngineLessCC)
  - [exlaunch-cmake](https://github.com/EngineLessCC/exlaunch-cmake/)
- [CraftyBoss](https://github.com/CraftyBoss)
  - [SMO-Exlaunch-Base](https://github.com/CraftyBoss/SMO-Exlaunch-Base/)
- [Atmosphère contributors](https://github.com/Atmosphere-NX/Atmosphere/graphs/contributors)
  - [Atmosphère](https://github.com/Atmosphere-NX/Atmosphere/)
- [Thog](https://github.com/Thog)
  - [oss-rtld](https://github.com/Thog/oss-rtld)
