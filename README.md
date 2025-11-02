# R‑TYPE — Online Shoot’em Up + Game Engine (C++20)

_Multiplayer R‑Type‑like with an in‑house engine (ECS), an authoritative UDP server, and a graphical SFML client._

> **Binaries:** `r-type_server` & `r-type_client` • **Build:** CMake (+ Ninja) • **Client:** SFML 2.6.x • **Server:** Boost.Asio (via CPM.cmake)

---

## ✨ Overview

- **Client:** SFML rendering, screens **Launch → Menu → Lobby → Settings**, particles/starfield, control remapping, embedded sounds & fonts.
- **Server:** **UDP** networking I/O with **Boost.Asio**, client sessions, **binary handshake** (Hello/Welcome), **heartbeat/ping**.
- **Engine (libengine):** **ECS** core (Entity/Component/System + Mediator, Event bus), components `Transform`, `Sprite`, `RigidBody`, `Gravity`, systems `PlayerControl`, `Physics`, `Render`.
- **Build:** `./ci/build.sh` (CMake + Ninja). Binaries are generated **at the project root**.

Project layout (excerpt):
```
.
├─ CMakeLists.txt                 # Places binaries at the repo root
├─ ci/                            # Local CI scripts
│  ├─ build.sh                    # CMake + Ninja (Release)
│  ├─ test.sh                     # ctest (if present)
│  └─ check_clang_format.sh
├─ cmake/CPM.cmake                # Dependencies via CPM
├─ libengine/                     # Engine library (ECS)
├─ include_common/                # Shared ECS headers
├─ Server/                        # UDP server code (Boost.Asio)
├─ Client/                        # SFML client (UI + game)
├─ assets/                        # R-Type fonts & spritesheets
├─ sound/                         # Music & SFX
└─ sprite/                        # PNGs for menu/logo/error
```

---

## 🚀 Quick Start

### Prerequisites (Linux — e.g., Fedora 41)
```bash
sudo dnf install cmake ninja-build gcc-c++
# Boost is fetched automatically for the server via CPM.cmake
```

_Ubuntu/Debian:_
```bash
sudo apt-get install cmake ninja-build g++
```


### Prerequisites & Build (Windows — Chocolatey)
```powershell 
choco install cmake python ninja mingw
git clone https://github.com/Jeami-s-Orga-Tek/R-TYPE.git
cd R-TYPE
cmake -S . -B build -G "Ninja" -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel
cp build\_deps\sfml-src\extlibs\bin\x64\openal32.dll .
```


### Build
From the **repository root**:
```bash
./ci/build.sh
# ➜ produces ./r-type_server and ./r-type_client at the repo root
```

### Run the server

```bash
./r-type_server [--ip <bind_ip>] [--port <port>] [--max-players <n>]
```
- `--ip <bind_ip>`: IP address to bind the server (default: `127.0.0.1`)
- `--port <port>`: UDP port to listen on (default: `8080`)
- `--max-players <n>`: Maximum number of simultaneous players (default: `1`)

- The server is authoritative: all game logic runs server-side.
- Handles async I/O, session management, and heartbeat/ping.
- You can run the server on a remote machine and connect clients over the network.

### Run the client
```bash
./r-type_client [--ip <server_ip>] [--port <port>]
```
- `--ip <server_ip>`: IP address of the server to connect to (default: `127.0.0.1`)
- `--port <port>`: UDP port of the server (default: `8080`)

- The client connects to the server when you click **Play** in the UI.
- **Important:** Run the client from the project root so it can access `assets/`, `sound/`, and `sprite/` directories.

---

## 🎮 Controls & Audio

- **Movement:** **← → ↑ ↓ (configurable in Settings)**
- **Shoot:** **Space**
- **Remap:** **Settings → Controls** menu (class `ControlsConfig`).
- **Audio:** music & SFX in `sound/` (e.g., `Menu_sound.mp3`, `Start_sound.mp3`).
- **Fonts:** `assets/r-type.otf`, `assets/retro.ttf`.

---

## 🧱 Architecture

### ECS (Entity‑Component‑System)
- **Entities:** numeric identifiers.
- **Components:** pure data (e.g., `Transform`, `Sprite`, `RigidBody`, `Gravity`).
- **Systems:** independent logic (e.g., `PlayerControl`, `Physics`, `Render`).
- **Mediator/Event bus:** decoupled communication between systems.

### Networking (binary UDP)
- **Transport:** UDP (binary packets).
- **Handshake:** `Hello` → `Welcome {playerId, roomId, baselineTick}`.
- **Sessions & timeouts:** via heartbeat/ping.

### Build system
- **C++20**, **CMake ≥ 3.10**, **Ninja**.
- **Client:** **SFML 2.6.x** (`sfml-graphics`, `sfml-window`, `sfml-system`).
- **Server:** **Boost 1.86** (Asio) via **CPM.cmake** (no system-wide install required).
- **Output:** `CMAKE_RUNTIME_OUTPUT_DIRECTORY = <repo root>` → binaries at the top level.

---

## 🧪 Quality & CI

- **GitHub Actions CI:** Linux/Windows builds, artifacts, formatting checks (see `.github/workflows/ci.yml`, `CI.md`).
- Scripts: `./ci/build.sh`, `./ci/test.sh`, `./ci/check_clang_format.sh`.

---

## 📐 Gameplay (prototype)

- **Starfield** (scrolling space background, time-step).
- **Up to 4 players** (visual distinction).
- **“Bydos” enemies** and **missiles**.
- Potential extensions: patterns, boss, evolving weapons, “Force,” etc.

---

## ⚙️ Runtime & Networking Tips

- Run **from the repo root** so the client loads resources.
- Open the chosen **UDP port** on your firewall/router.
- Remote connection: change IP/port in the UI (or adjust in `GameTypes.cpp` if needed).

---

## 🧭 Project Architecture (Mermaid)

```mermaid
classDiagram
    direction LR
    class EntityManager {
        -available_entities : queue
        -signatures : array
        -entity_count : uint32_t
        +createEntity() Entity
        +destroyEntity(e: Entity) void
        +setSignature(e: Entity, s: Signature) void
        +getSignature(e: Entity) Signature
    }
    class ComponentManager {
        -component_types : unordered_map
        -component_arrays : unordered_map
        +registerComponent<T>() void
        +addComponent<T>(e: Entity, c: T) void
        +removeComponent<T>(e: Entity) void
        +getComponent<T>(e: Entity) T
        +getComponentType<T>() ComponentType
        +entityDestroyed(e: Entity) void
    }
    class SystemManager {
        +registerSystem<T>() void
        +setSignature<T>(s: Signature) void
        +entityDestroyed(e: Entity) void
        +entitySignatureChanged(e: Entity, s: Signature) void
    }
    class Mediator {
        -entityManager : EntityManager
        -componentManager : ComponentManager
        -systemManager : SystemManager
        -eventManager : EventManager
        +init() void
        +createEntity() Entity
        +destroyEntity(e: Entity) void
        +addComponent<T>(e: Entity, c: T) void
        +getComponent<T>(e: Entity) T
        +getComponentType<T>() ComponentType
        +setSignature(e: Entity, s: Signature) void
        +registerSystem<T>() void
        +setSystemSignature<T>(s: Signature) void
        +addEventListener(id: uint32_t, fn) void
        +sendEvent(e: Event) void
        +sendEventById(id: uint32_t) void
    }
    class NetworkManager {
        -role : string %% "CLIENT" or "SERVER"
        -ip : string
        -port : uint16_t
        -mediator : Mediator
        +sendEntity(e: Entity, sig: Signature) void
        +sendComponent<T>(e: Entity, c: T) void
        +receive() void
        +connect() void
        +disconnect() void
    }
    class Transform {
        +pos : Rect
        +rot : float
        +scale : float
    }
    class Sprite {
        +sprite_name : string
        +frame_nb : int
    }
    class RigidBody {
        +velocity : Vec2
        +acceleration : Vec2
    }
    class Gravity {
        +force : Vec2
    }
    class Hitbox {
        +bounds : Rect
        +active : bool
        +layer : int
        +damage : int
    }
    class PlayerInfo {
        +player_id : uint32_t
        +health : int
        +max_health : int
    }
    class EnemyInfo {
        +enemy_type : int
        +health : int
    }
    class Animation {
        +frames : int
        +speed : float
    }
    class Sound {
        +sound_name : string
    }
    class LevelInfo {
        +level : int
    }
    class GameState {
        +state : int
    }

    Mediator *-- EntityManager
    Mediator *-- ComponentManager
    Mediator *-- SystemManager
    Mediator *-- EventManager
    Mediator o-- NetworkManager : network
    ComponentManager o-- Transform
    ComponentManager o-- Sprite
    ComponentManager o-- RigidBody
    ComponentManager o-- Gravity
    ComponentManager o-- Hitbox
    ComponentManager o-- PlayerInfo
    ComponentManager o-- EnemyInfo
    ComponentManager o-- Animation
    ComponentManager o-- Sound
    ComponentManager o-- LevelInfo
    ComponentManager o-- GameState
    SystemManager o-- PlayerControl
    SystemManager o-- PhysicsNoEngineSystem
    SystemManager o-- RenderSystem
    SystemManager o-- EnemySystem
    SystemManager o-- Collision
    SystemManager o-- Animate
    SystemManager o-- SoundSystem
    SystemManager o-- DevConsole
```
---

## 🔒 Accessibility & Docs

- **Accessibility:** color options, full remapping, readability.
- **Documentation:** this **README**, plus a Mermaid architecture diagram.

---

## 🧩 Troubleshooting (FAQ)

- **SFML not found:** install `SFML-devel` (Fedora) / `libsfml-dev` (Debian/Ubuntu).
- **Fonts/assets not found:** run the client **from the repo root**.
- **Slow Boost download (CPM):** configure a `CPM_SOURCE_CACHE` for CI/local envs.
- **“Connection refused”:** check the server is listening (correct port, UDP allowed).

---

## 🤝 Credits & License

- **Authors:** Tiziano BLANC, Edouard DUPUCH, Olivier DUBO, and Valentin ROUSSEAU.
- **Sprites & inspirations:** R‑Type universe.
