# AGENTS.md — CMake Dependency DevOps Rules

이 문서는 Codex / AI Agent 가 프로젝트를 수정할 때 **반드시 따라야 하는 DevOps 규칙**을 정의한다.  
이 규칙은 프로젝트 구조 안정성과 dependency 관리 일관성을 보장한다.

---

## 1. Project Layout Rule

```text
project/
 ├─ .vscode/            # debug with vs code
 │   ├─ c_cpp_properties.json
 │   ├─ launch.json
 │   ├─ settings.json
 │   └─ tasks.json
 │
 ├─ bin/               # executables
 │   ├─ Debug/
 │   └─ Release/
 │
 ├─ build/             # cmake build directory
 │   └─ <config>
 │      └─ _deps/
 │         └─ <name>/
 │
 ├─ cmake/
 │   ├─ Dependencies.cmake
 │   ├─ Dep_GLFW.cmake
 │   ├─ Dep_GLAD.cmake
 │   └─ Dep_IMGUI.cmake
 │
 ├─ deps/              # dependency source only
 │   └─ <name>-src/
 │
 ├─ libs/              # static libraries
 │   ├─ Debug/
 │   └─ Release/
 │
 ├─ src/
 │
 ├─ CMakeLists.txt
 ├─ CMakePresets.json
 │
 │
 ├─ AGENTS.md
 ├─ CODEX_TASK.md
 └─ README.md
```

---

# 2. Dependency Layout Rule

모든 dependency 는 반드시 다음 구조 사용

```text
deps/<name>-src        # optional cache
build/<preset-name>/_deps/<name>-build
```


예

```text
deps/assimp-src
deps/casc-src

build/<preset-name>/_deps/assimp-build
build/<preset-name>/_deps/casc-build
```

---

# 3. FetchContent Rule

모든 dependency 는 FetchContent 사용

금지:

* add_subdirectory(deps/xxx)
* ExternalProject
* git submodule
* prebuilt binary

허용:

* FetchContent_Declare
* FetchContent_MakeAvailable

---

# 4. Dependency Naming Rule

dependency 이름 규칙

```text
lowercase name
```

예

```text
casc
assimp
glm
imgui
bullet
zlib
```

디렉토리

```text
deps/<name>-src
```

---

# 5. Dependencies.cmake Rule

`cmake/Dependencies.cmake` 는 공통 helper 만 담당

추가 규칙

* 각 dependency 구현은 반드시 개별 파일로 분리
* 파일명 규칙은 반드시 `cmake/Dep_<NAME>.cmake`
* `<NAME>` 은 대문자 사용 예: `Dep_GLFW.cmake`, `Dep_GLAD.cmake`, `Dep_IMGUI.cmake`
* 각 파일은 자기 dependency 하나만 정의
* dependency 의 repository, tag, FetchContent 선언, target 생성은 해당 `Dep_<NAME>.cmake` 내부에서 결정
* Root `CMakeLists.txt` 에서 필요한 `Dep_*.cmake` 를 직접 `include()`
* Root `CMakeLists.txt` 에서는 dependency 이름, git url, tag 를 다시 쓰지 않음

예시 패턴

```cmake
include(cmake/Dep_GLFW.cmake)
include(cmake/Dep_GLAD.cmake)
include(cmake/Dep_IMGUI.cmake)
```

---

# 6. Build Directory Rule

dependency build directory

```text
build/<preset-name>/_deps/<name>
```

---

# 7. Library Output Rule

정적 라이브러리 출력

```text
libs/Debug
libs/Release
```

CMake 설정

```cmake
CMAKE_ARCHIVE_OUTPUT_DIRECTORY_*
```

---

# 8. Executable Output Rule

실행파일 출력

```text
bin/Debug
bin/Release
```

CMake 설정

```cmake
CMAKE_RUNTIME_OUTPUT_DIRECTORY_*
```

---

# 9. Static Library Only

shared library 금지

CMake

```cmake
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
```

---

# 10. Multi Config Support

반드시 지원

* Visual Studio

사용

```cmake
CMAKE_*_OUTPUT_DIRECTORY_DEBUG
CMAKE_*_OUTPUT_DIRECTORY_RELEASE
```

---

# 11. Include Rule

dependency include 는 target 기반 사용

허용

```cmake
target_link_libraries(app PRIVATE assimp)
```

금지

```cmake
include_directories()
link_directories()
```

---

# 12. Root CMake Rule

Root CMakeLists.txt 는 다음 순서

```text
1 project()
2 cmake variables
3 output dirs
4 include Dep_<NAME>.cmake
5 add_executable()
6 target_link_libraries()
```

---

# 13. Dependency Add Example

예: casc + assimp

```cmake
include(cmake/Dep_CASC.cmake)
include(cmake/Dep_ASSIMP.cmake)
```

---

# 14. CMake Presets Rule

Agent 는 반드시 `CMakePresets.json` 기반으로 configure / build 수행

configure preset 예

```text
vs2022-debug
vs2022-release
```

build preset 예

```text
build-debug
build-release
```

규칙

* preset 이름은 목적이 명확해야 함
* Visual Studio 2022 x64 기준 유지
* Debug / Release preset 모두 유지
* `binaryDir` 는 반드시 `build/<preset-name>` 패턴 사용

---

# 15. Build Command

Agent 는 항상 다음 사용

configure

```bash
cmake --preset vs2022-debug
```

build

```bash
cmake --build --preset build-debug
cmake --build --preset build-release
```

---

# 16. VSCode Debug Rule

Agent 는 `.vscode` 기반 디버깅 환경을 유지해야 함

필수 파일

```text
.vscode/launch.json
.vscode/tasks.json
.vscode/settings.json
```

권장 파일

```text
.vscode/c_cpp_properties.json
.vscode/extensions.json
```

`launch.json` 규칙

* debugger 는 Windows 에서 `cppvsdbg` 사용
* `program` 은 반드시 `${workspaceFolder}/bin/Debug/<target>.exe`
* `cwd` 는 반드시 `${workspaceFolder}`
* 각 실행 타겟은 개별 launch configuration 사용
* `preLaunchTask` 는 반드시 `.vscode/tasks.json` 의 빌드 task 와 연결

`tasks.json` 규칙

* configure task 는 반드시 `cmake --preset <configure-preset>`
* build task 는 반드시 `cmake --build --preset <build-preset> --target <target>`
* 각 debug target 은 대응되는 build task 를 가져야 함

`settings.json` 규칙

* `cmake.useCMakePresets` 사용
* 기본 configure preset / build preset 지정
* generator 는 Visual Studio 17 2022 우선 사용

금지

* 절대 경로 exe 지정
* `.sln` 직접 실행을 전제로 한 launch 구성
* `build/bin`, `build/lib` 를 기준으로 한 debug 경로

---

# 17. Do Not Generate

Agent 는 다음 생성 금지

```text
build/lib
build/bin
external/
third_party/
vendor/
```

---

# 18. Always Use

Agent 는 반드시 사용

```text
bin/
build/
cmake/
deps/
libs/
src/
.vscode/
```

---

# 19. Dependency Examples

지원 dependency

* casc (CASCLib)
* assimp
* glm
* imgui
* bullet
* zlib
* stb
* spdlog

모두 동일 규칙

---

# 20. Agent Behavior

Agent 는 다음 순서로 작업

1. AGENTS.md 읽기
2. 구조 규칙 확인
3. CODEX_TASK.md 읽기
4. `CMakePresets.json` 확인
5. `.vscode` 디버깅 구성 확인
6. dependency 추가
7. 구조 유지
8. 코드 생성

---

# 21. Goal

모든 dependency 는 다음 패턴 유지

```text
deps/<name>-src
build/_deps/<name>
libs/
bin/
```

이 규칙은 절대 변경하지 않는다.
