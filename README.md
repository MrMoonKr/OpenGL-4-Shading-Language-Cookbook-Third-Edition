# OpenGL 4 Shading Language Cookbook, 3rd Edition

WIP ( Work in Progress )  

학생들을 위한 OpenGL 직무 교육( OJT, On the job Training )을 위해서 생성 하였습니다.  


## 책 관련 링크  

<img src="https://content.packt.com/_/image/original/B11014/cover_image.jpg" alt="" height="256px" align="right">

- [OpenGL 4 Shading Language Cookbook, 3rd Edition [ 원서 ]](https://www.packtpub.com/en-tw/product/opengl-4-shading-language-cookbook-9781789340662)  

- [알라딘 서점](https://www.aladin.co.kr/shop/wproduct.aspx?ItemId=171393849) ( 출판일 2018-09-28 )  


## 개발 및 테스트 환경

- 시스템 ( Computer System )  

  - AMD Ryzen 9 5900X 12-Core Processor
  - NVIDIA Geforce RTX 3080 10GB
  - Windows 11 64bit Korean

- 컴파일러 및 링커 ( Compiler , Linker )  

  - [Visual Studio Community 2022](https://visualstudio.microsoft.com/ko/free-developer-offers/)  
    - C++를 사용한 데스크톱 개발  
    - C++를 사용한 게임 개발  

  - [~~MingGW-w64 via MSYS2~~](https://code.visualstudio.com/docs/cpp/config-mingw)
    - 당분간 테스트 없음  
    - [설치파일](https://github.com/msys2/msys2-installer/releases/download/2024-12-08/msys2-x86_64-20241208.exe)
      ```
      $ pacman -S --needed base-devel mingw-w64-ucrt-x86_64-toolchain
      ```
      ```
      // 시스템 환경 변수 'PATH' 에 추가 등록  
      C:\msys64\ucrt64\bin
      ```
      ```
      $ gcc --version
      $ g++ --version
      $ gdb --version
      ```

- 에디터 ( Editor, Intergrated Development Environment )  

  - [VS Code](https://visualstudio.microsoft.com/ko/free-developer-offers/)  
    - C/C++ Extension for VS Code
      - c_cpp_properties.json ( compiler path and IntelliSense settings )  
      - tasks.json ( build instructions )  
      - launch.json ( debugger settings )  
      - Chapter01 동작 테스트 후 CMake 도입함.  

- 빌드 매니저 ( Build Manager )
  - [CMake](https://cmake.org/)  
    - [설치](https://cmake.org/download/)  
    - ...
    ```
    $ cmake --version
    ```
  - CMakeLists.txt
  - CMakePresets.json
  - CMakeUserPresets.json

- 패키지 매니저 ( Package Manager )
  - [vcpkg](https://vcpkg.io/en/)  
    - [설치](https://github.com/microsoft/vcpkg)  
    - ...
    ```
    $ vcpkg --version
    $ vcpkg new --application
    ```
  - vcpkg.json
  - vcpkg-configuration.json


## 사용된 패키지 목록

- GLAD
  - [vcpkg](https://vcpkg.io/en/package/glad)
    ```
    $ vcpkg add port glad[gl-api-43]
    ```
  - [GLAD](https://glad.dav1d.de/)  
  - C/C++ OpenGL Core 4.3 API

- GLFW
  - [vcpkg](https://vcpkg.io/en/package/glfw3)
    ```
    $ vcpkg add port glfw3
    ```
  - [GLFW](https://www.glfw.org/download.html)
  - [Windows Pre-Compiled Binaries](https://github.com/glfw/glfw/releases/download/3.4/glfw-3.4.bin.WIN64.zip)
  - glfw 3.4 win64

- GLM
  - [vcpkg](https://vcpkg.io/en/package/glm)  
    ```
    $ vcpkg add port glm
    ```
  - [glm](https://github.com/g-truc/glm)
  - OpenGL Mathematics  

- STB
  - [vcpkg](https://vcpkg.io/en/package/stb)  
  ```
  $ vcpkg add port stb
  ```
  - [stb](https://github.com/nothings/stb)
  - Image Loader

- IMGUI
  - [vcpkg](https://vcpkg.io/en/package/imgui)  
  ```
  $ vcpkg add port imgui[glfw-binding,opengl3-binding,freetype]
  ```
  - [imgui](https://github.com/ocornut/imgui)
  - Immediate Mode Graphical User Interface

- ...
  - [vcpkg]()  
  ```
  $ vcpkg add port ...
  ```
  - [...]()
  - ...

## ...

---
---
---




Example code from the [OpenGL 4 Shading Language Cookbook, 3rd Edition][cookbook]
=========================================================



The example code from the [OpenGL 4 Shading Language Cookbook, 3rd Edition][cookbook],
by David Wolff and published by Packt Publishing.

Requirements
-------------
To compile these examples, you'll need the following:

* The [GLM Mathematics Library][GLM] version 0.9.6 or later.  Note that versions
  prior to 0.9.6 may not work properly because of a switch from degrees to
  radians.  GLM 0.9.5 will work, but you'll need to add `#define GLM_FORCE_RADIANS`
  prior to including the glm header files.
* [GLFW][] version 3.0 or later.

Compiling the examples
----------------------
The example code builds with [CMake][].  Note that the
examples for Chapter 10 will not function on MacOS due to lack of support for
compute shaders on that platform.

1.  Install [GLFW][] by following the instructions on their [web site][GLFW].
2.  Install the latest version of [GLM][].  Note that for [CMake][] to find GLM
    correctly, you need to run the install "build" (e.g. `make install`) or install GLM from your
    favorite package manager.  Otherwise, the CMake config files will not be created/available.
3.  Download this example code from [github][ghcookbook], or clone using git.
4.  Run cmake.  If cmake has difficulties finding the GLFW or GLM installations,
    set the variable [`CMAKE_PREFIX_PATH`][cmake_prefix] to help cmake find them.  
    It can be tricky to get CMake to find the GLM libraries, unfortunately.  See
    below for tips.
5.  Compile by running `make`.

Any problems, [create an issue](https://github.com/PacktPublishing/OpenGL-4-Shading-Language-Cookbook-Third-Edition/issues) on [github][ghcookbook].

Tips for getting CMake to find GLM
-----------------------------------------
When searching for GLM, CMake looks for the files `glmConfig.cmake` and `glmConfigVersion.cmake`.

If you install GLM using a package manager such as Homebrew on macOS, or a Linux package manager the
 cmake files should already be included.  

Otherwise, if you're using the GLM source distribution, you'll have to run GLM through CMake to get it to
generate the `glmConfig.cmake` and `glmConfigVersion.cmake` files.  It is frustrating, I wish that the GLM 
maintainers didn't require this.

1.  Download GLM and extract it to some location: `GLM_SRC`
1. `cd $GLM_SRC`
1. `mkdir build`
1. `cd build`
1. `cmake -D GLM_TEST_ENABLE=OFF -D CMAKE_INSTALL_PREFIX=MY_GLM_LOCATION ..`
1. `cmake --build . --target install`

Replace `GLM_SRC` above with the place where you extracted the GLM zip file, and replace `MY_GLM_LOCATION` 
with the location where you want to install GLM. This should generate the needed cmake files and install
all of GLM to `MY_GLM_LOCATION`.

Tips for compiling for Windows with Visual Studio
---------------------------------------------
* Use the Visual Studio target in [CMake][]:  `-G "Visual Studio..."`, open the
  Visual Studio solution.  You should see one project per chapter.
* Each chapter requires a command line argument to choose a recipe.  When
  running in VS, be sure to set the 'Command Argument' under 'Properties' for
  the appropriate recipe.

OpenGL Function Loading
-----------------------

An OpenGL header file and a function loader for a 4.3 core profile are
included with this project.  They were generated using
[GLAD][].  This loader should also work on MacOS under a 4.1 core profile, but of course not all functions will load.

The code has been tested with OpenGL 4.3 on Windows/Linux and OpenGL 4.1 on MacOS.

[GLM]: http://glm.g-truc.net
[GLFW]:  http://glfw.org
[ghcookbook]:  https://github.com/PacktPublishing/OpenGL-4-Shading-Language-Cookbook-Third-Edition
[cookbook]: https://www.packtpub.com/en-us/product/opengl-4-shading-language-cookbook-9781789340662
[GLLoadGen]:  https://bitbucket.org/alfonse/glloadgen/wiki/Home
[CMake]: http://www.cmake.org/
[GLAD]: https://github.com/Dav1dde/glad
[cmake_prefix]: https://cmake.org/cmake/help/latest/variable/CMAKE_PREFIX_PATH.html

# OpenGL 4 Shading Language Cookbook - Third Edition

<a href="https://www.packtpub.com/application-development/hands-aspnet-core-2-and-vuejs?utm_source=GitHub&utm_medium=repository&utm_campaign=9781788839464"><img src="https://d255esdrn735hr.cloudfront.net/sites/default/files/imagecache/ppv4_main_book_cover/9781789342253.png" height="256px" align="right"></a>

This is the code repository for [OpenGL 4 Shading Language Cookbook - Third Edition](https://www.packtpub.com/game-development/opengl-4-shading-language-cookbook-third-edition?utm_source=GitHub&utm_medium=repository&utm_campaign=9781789342253), published by Packt.

**Build high-quality, real-time 3D graphics with OpenGL 4.6, GLSL 4.6 and C++17**

## What is this book about?
OpenGL 4 Shading Language Cookbook, Third Edition provides easy-to-follow recipes that first walk you through the theory and background behind each technique, and then proceed to showcase and explain the GLSL and OpenGL code needed to implement them. 

This book covers the following exciting features: 
* Compile, debug, and communicate with shader programs
* Use compute shaders for physics, animation, and general computing
* Learn about features such as shader storage buffer objects and image load/store
* Utilize noise in shaders and learn how to use shaders in animations
* Use textures for various effects including cube maps for reflection or refraction

If you feel this book is for you, get your [copy](https://www.amazon.com/dp/1789342252) today!

<a href="https://www.packtpub.com/?utm_source=github&utm_medium=banner&utm_campaign=GitHubBanner"><img src="https://raw.githubusercontent.com/PacktPublishing/GitHub/master/GitHub.png" 
alt="https://www.packtpub.com/" border="5" /></a>


## Instructions and Navigations
All of the code is organized into folders. For example, Chapter02.

The code will look like the following:
```
glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
GLFWwindow *window = glfwCreateWindow(800, 600, "Title", nullptr, nullptr);
```

**Following is what you need for this book:**
If you are a graphics programmer looking to learn the GLSL shading language, this book is for you. A basic understanding of 3D graphics and programming experience with C++ are required.

With the following software and hardware list you can run all code files present in the book (Chapter 1-11).

### Software and Hardware List

| Chapter | Software required                   | OS required                        |
| --------| ------------------------------------| -----------------------------------|
|  All    |  GLM Mathematics Library            | Windows, Mac OS X, and Linux (Any) |
|         |  GLFW                               | Windows, Mac OS X, and Linux (Any) |
|         |  CMake                              | Windows, Mac OS X, and Linux (Any) |
|         |  Visual Studio 2017                 | Windows, Mac OS X, and Linux (Any) |


We also provide a PDF file that has color images of the screenshots/diagrams used in this book. [Click here to download it](https://www.packtpub.com/sites/default/files/downloads/9781789342253_ColorImages.pdf).

## Errata 
 * Page 238 (line 24):  **GLenumdrawBuffers[] = {GL_NONE, GL_COLOR_ATTACHMENT0,** _should be_ **GLenum drawBuffers[] = {GL_NONE, GL_COLOR_ATTACHMENT0.**
 * Page 375 (last line):  **This recipe was inspired by a blog post by Wojciech Toman (no longer publicly
available), has been removed please ignore the line** 
 * Page 371 (Code snippet 1 ,second last line):  **vec3 rustColor = mix( MaterialColor.rgb, vec3(0.01), noise.a );,** _should be_ **vec3 rustColor = mix( MaterialColor.rgb, vec3(0.01), noise );**
 * Page 328 (line 24):  **When GL_NEAREST is the interpolation mode (as it is in our case) the result will be 1.0, or 0.0. **_should be_**When GL_COMPARE_REF_TO_TEXTURE is the texture comparison mode (as it is in our case) the result will be either 1.0 or 0.0**
 * Page 327 (tip second line):  **because we need a 2D position and a depth ,** _should be_ **because we need a 2D texture coordinate and a reference depth value for the comparison**
 * Page 279 ( line 10):  **For example, to find ha, we use the interior angle at vertex C (β) ,** _should be_ **For example, to find ha, we use the interior angle at vertex B (β)**
 * Page 264 ( line 2):  **indicates the index within the GL_ATOMIC_COUTER_BUFFER binding point (refer,** _should be_ **indicates the index within the GL_ATOMIC_COUNTER_BUFFER binding point (refer**
 * Page 255 ( line 4):  **a variety of techniques for doing this; one of the most common technique is to keep a list of** _should be_ **a variety of techniques for doing this; one of the most common techniques is to keep a list of**
 * Page 320 ( information tip):  **range for the depth values (say between 0 and 100) via** _should be_ **range for the depth values (say between 0 and 1) via**
  
 
 
### Related products 
* Godot Engine Game Development Projects [[Packt]](https://www.packtpub.com/game-development/godot-game-engine-projects) [[Amazon]](https://www.amazon.com/Godot-Engine-Game-Development-Projects-ebook/dp/B079HZD1S7)

* Mastering C++ Game Development [[Packt]](https://www.packtpub.com/game-development/mastering-c-game-development) [[Amazon]](https://www.amazon.com/Mastering-Game-Development-professional-realistic/dp/1788629221)

## Get to Know the Author
**David Wolff**
is a professor in the computer science department at Pacific Lutheran University (PLU). He received a PhD in Physics and an MS in computer science from Oregon State University. He has been teaching computer graphics to undergraduates at PLU for over 17 years, using OpenGL.

## Other books by the author
* [OpenGL 4.0 Shading Language Cookbook](https://www.packtpub.com/game-development/opengl-40-shading-language-cookbook?utm_source=GitHub&utm_medium=repository&utm_campaign=9781849514767)
* [OpenGL 4.0 Shading Language Cookbook, Second Edition](https://www.packtpub.com/game-development/opengl-4-shading-language-cookbook-second-edition?utm_source=GitHub&utm_medium=repository&utm_campaign=9781782167020)


### Suggestions and Feedback
[Click here](https://docs.google.com/forms/d/e/1FAIpQLSdy7dATC6QmEL81FIUuymZ0Wy9vH1jHkvpY57OiMeKGqib_Ow/viewform) if you have any feedback or suggestions.

