# Stellar Conflicts - Game Architecture Document

## 1. Introduction

### 1.1. Project Overview
**Stellar Conflicts** is a real-time, multiplayer space action game. This document outlines its technical architecture, engineered for performance, security, and maintainability.

### 1.2. Architectural Philosophy
This architecture is built on a single, non-negotiable principle: **the server is the absolute authority.** The client is a dumb terminal.

*   **Authoritative C++ Server:** A from-scratch C++23 application that is the single source of truth. Its implementation is mandated by the *Oat Interactive C++ Engineer Onboarding Manual*.
*   **"Dumb" UE5 Rendering Client:** The Unreal Engine 5 client's sole function is to render the state dictated by the server and capture user input. It contains no game logic. All native UE5 networking is bypassed.

## 2. High-Level Architecture

The system consists of the C++ Server, the UE5 Client, and a local SQLite database for immutable game data. Communication occurs over a single, custom TCP stream per client.



### 2.1. Technology Stack

*   **Server:**
    *   **Language:** C++23 (Strictly following `CPP_MANUAL.md`)
    *   **Networking:** Standalone Asio
    *   **Scripting:** LuaJIT for zone-specific logic.
    *   **Database:** SQLite with `sqlite-orm` for immutable data access.
    *   **Build System:** CMake (following all `CM` rules)
*   **Client:**
    *   **Engine:** Unreal Engine 5
    *   **Custom C++:** All non-framework client code follows `CPP_MANUAL.md`.
    *   **Networking:** A custom TCP client system.
    *   **Database:** SQLite.
*   **Shared:**
    *   **Data Format:** A custom, bitpacked binary serialization protocol.

## 3. Server Architecture

The server is a standalone C++23 application. **It does not use an Entity-Component-System (ECS).** An ECS introduces unnecessary complexity and indirection that would be immediately negated by the scripting layer. We are using a direct, object-oriented model that is simpler to implement and debug.

### 3.1. Core Design Principles (`CPP_MANUAL.md` Adherence)
*   **Error Handling:** All fallible operations **shall** return `std::expected<T, Error>`.
*   **Object Creation:** Non-POD objects **shall** be created via static `Create(...)` factory methods returning `std::expected<std::unique_ptr<T>, Error>`.
*   **Interface-Based Design:** Polymorphism is achieved through abstract interfaces (`ILogger`, `INetworkServer`, `IGameZone`).
*   **Update Loop:** A deterministic, **30 Hz** main loop drives all systems via a `tick(uint64_t now_ns)` method.

### 3.2. Game Object Model
*   **`GameZone`:** A class representing a self-contained game world instance. It owns collections of all game objects.
*   **Game Objects (`Ship`, `Projectile`, `Mine`):** Game objects are uniquely owned (`std::unique_ptr`) by their `GameZone`, ensuring clean destruction and lifecycle management. They are C++ classes encapsulating their own state.
*   **Systems (`PhysicsSystem`, `CombatSystem`):** Handler classes that operate upon the collections of game objects owned by the `GameZone`.

### 3.3. Scripting Environment and Garbage Collection
*   **Sandboxing:** Scripts are sandboxed per `GameZone`. Each script environment is created when its zone is activated and completely destroyed when the zone is deactivated, preventing cross-zone state contamination.
*   **Garbage Collection:** To prevent memory leaks in long-running script environments, a full garbage collection cycle is forced explicitly every N ticks via `lua_gc(L, LUA_GCCOLLECT, 0)`.

### 3.4. Key Server Systems
*   **`PhysicsSystem`:** The sole authority for movement and collision detection.
*   **`CombatSystem`:** Manages weapon firing, cooldowns, and damage application.
*   **`InterestManagementSystem`:** Manages entity visibility using a spatial grid. This is not optional; it is critical for performance. It determines *which* clients should be notified about an entity.
*   **`StateBroadcastSystem`:** Works with the `InterestManagementSystem`. When an entity's state changes, it serializes the `ShipState` struct for clients in the interest list. **Crucially, each tick has a strict per-client byte budget (e.g., 512 bytes).** Entities are prioritized by distance and activity (moving, firing). Excess updates are deferred to the next tick to prevent network stalls.

## 4. Client Architecture (Unreal Engine 5)

The client is a hybrid system: our C++ code (following the manual) handles logic, while the UE framework handles rendering.

### 4.1. Client-Side Threading & State Synchronization
The potential for race conditions is handled explicitly:
1.  **Reader Thread:** A dedicated C++ thread reads from the TCP socket using a `FramedPacketReader`.
2.  **Thread-Safe Packet Queue:** Decoded packets are moved into a `std::queue<std::unique_ptr<Packet>>` protected by a `std::mutex`.
3.  **Game Thread:** The main UE game tick, via a `ClientGameManager` object, polls this queue, dequeues all available packets, and applies their state updates to the local `AStellarShip` actors.

### 4.2. Interpolation
The client uses linear interpolation for the MVP.
*   Each rendered actor (`AStellarShip`) stores its `last_server_state_` and `target_server_state_`.
*   The actor's `Tick` function smoothly interpolates its visual transform between these two states over the 33.3ms server tick interval.

## 5. Network Protocol

The protocol is minimal, explicit, and forward-compatible.

### 5.1. Packet Layout Example
All packets adhere to a standard header format. This structure is non-negotiable.

**Base Packet Structure:**
`[1B: Version][2B: Length][1B: PacketType][...Payload]`

**Example `ShipState` Payload (Conceptual):**
`[4B: EntityID][2B: PosX][2B: PosY][1B: Angle][1B: Shield][1B: Hull][...OtherFields]`

### 5.2. Versioning
Every packet sent **shall** be prefixed with an 8-bit `protocol_version`. The server will reject connections from clients with an incompatible version.

### 5.3. Packet Framing
All packets are framed to prevent stream corruption. A `FramedPacketReader` utility on both client and server handles buffering to ensure only complete packets are processed.

### 5.4. Quantization Policy
The precision of quantized values is explicitly defined to balance bandwidth with visual quality.
*   **Position (`X`, `Y`):** Quantized to 16-bit integers. With a world range of +/- 20,000 units, this provides a resolution of ~0.6 units, sufficient to avoid visual snapping.
*   **Rotation:** Quantized to a 9-bit integer (0-359 degrees).
*   **Velocity:** Quantized to an 8-bit integer (0-130 units/sec).

### 5.5. State Updates
The server communicates state changes through explicit messages, **not** a generic replication graph. For the MVP, the `StateBroadcastSystem` sends the full `ShipState` struct for any entity that has changed within a client's area of interest, respecting the per-tick send budget.

## 6. Future Considerations & Scalability Path

This architecture is the MVP foundation. The path forward includes:

*   **V2 - Snapshot Interpolation:** Upgrade the client to store the last N>2 state snapshots to allow for higher-order interpolation, more accurately smoothing the path of accelerating or turning objects.
*   **V2 - State Delta Compression:** Implement state diffing. The server will only serialize changed fields within a state struct, dramatically reducing packet size.
*   **V3 - Entity Streaming Zones:** For massive maps, evolve the `InterestManagementSystem` into a full streaming system. The server will only load and tick entities in zones immediately surrounding players, allowing for a virtually limitless game world without overwhelming server resources.
