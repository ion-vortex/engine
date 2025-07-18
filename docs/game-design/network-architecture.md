# Network Architecture

## Architectural Philosophy

The network architecture is built on a single, non-negotiable principle: **the server is the absolute authority**. The client is a dumb terminal.

### Core Components

1. **Authoritative C++ Server**: A from-scratch C++23 application that is the single source of truth
2. **Custom C++ Client**: Uses bgfx for rendering, ImGui for UI, with simple prediction for responsiveness
3. **TOML Data Files**: Store immutable game data configuration
4. **Custom TCP Protocol**: Single TCP stream per client with bitpacked serialization

## Server Architecture

### Design Principles

Following the Ion Vortex C++ Manual:
- **Error Handling**: All fallible operations return `std::expected<T, Error>`
- **Object Creation**: Factory methods return `std::expected<std::unique_ptr<T>, Error>`
- **Interface-Based**: Polymorphism via abstract interfaces (`ILogger`, `INetworkServer`, `IGameZone`)
- **Update Loop**: Deterministic 30 Hz main loop via `tick(uint64_t now_ns)`

### Game Object Model

**No Entity-Component-System (ECS)**. Direct object-oriented model for simplicity and debuggability.

- **Game Objects**: `Ship`, `Projectile`, `Mine` - owned via `std::unique_ptr`
- **Systems**: `PhysicsSystem`, `CombatSystem` - operate on GameZone's object collections

### Scripting (LuaJIT)

- **Sandboxed per GameZone**: Each zone has isolated script environment
- **Garbage Collection**: Explicit GC every N ticks via `lua_gc(L, LUA_GCCOLLECT, 0)`
- **Zone-specific logic**: Custom behaviors without recompiling

### Key Server Systems

1. **PhysicsSystem**
   - Sole authority for movement
   - Collision detection
   - No client prediction

2. **CombatSystem**
   - Weapon firing logic
   - Cooldown management
   - Damage application

3. **InterestManagementSystem**
   - Spatial grid for visibility
   - Determines which clients see which entities
   - Critical for performance

4. **StateBroadcastSystem**
   - Works with InterestManagement
   - Serializes `ShipState` for relevant clients
   - **Per-client byte budget**: 512 bytes/tick
   - Prioritizes by distance and activity
   - Defers excess updates

## Client Architecture (Custom C++)

The client is built using the Ion Vortex engine:
- **bgfx**: Cross-platform rendering
- **ImGui**: Immediate mode UI
- **GLFW**: Window management and input
- **Custom networking**: TCP client implementation

### Architecture Overview

Following Ion Vortex principles:
- Factory-based object creation returning `std::expected<std::unique_ptr<T>, Error>`
- Interface-driven design (`IRenderer`, `INetworkClient`, `IInputHandler`)
- Tick-based update loop synchronized with server rate

### State Management

1. **Network Thread**
   - Dedicated thread for TCP communication
   - Uses `ion::protocol` for packet handling
   - Buffers incoming state updates

2. **State Buffer**
   - Ring buffer of recent server states
   - Thread-safe access via `std::mutex`
   - Maintains history for interpolation

3. **Game Thread**
   - Main render loop via bgfx
   - Polls state buffer each frame
   - Applies prediction and interpolation

### Client-Side Prediction

Simple prediction to reduce input latency:
- **Movement Prediction**: Apply local input immediately
- **State Reconciliation**: Correct when server state arrives
- **Smooth Corrections**: Blend predicted and authoritative states
- **Limited Scope**: Only position/rotation, no gameplay logic

### Rendering Pipeline

1. **State Interpolation**
   - Interpolate between two most recent server states
   - Account for network latency
   - Smooth visual representation

2. **Prediction Layer**
   - Apply local input on top of interpolated state
   - Visual-only, no gameplay impact
   - Reset on server update

3. **bgfx Rendering**
   - Submit draw calls for all visible entities
   - Instanced rendering for projectiles
   - Efficient sprite batching

### UI System (ImGui)

- **HUD Elements**: Energy, shields, hull status
- **Targeting**: Reticles and target information
- **Chat Windows**: Dockable chat panels
- **Minimap**: Custom ImGui rendering
- **Debug Overlays**: Network stats, prediction accuracy

## Network Protocol

### Packet Structure

Minimal, explicit, forward-compatible design.

**Base Header** (all packets):
```
[1B: Version][2B: Length][1B: PacketType][...Payload]
```

**Example ShipState Payload**:
```
[4B: EntityID][2B: PosX][2B: PosY][1B: Angle][1B: Shield][1B: Hull][...]
```

### Protocol Features

1. **Versioning**
   - 8-bit `protocol_version` prefix
   - Server rejects incompatible versions
   - Enables protocol evolution

2. **Packet Framing**
   - Prevents stream corruption
   - `FramedPacketReader` ensures complete packets
   - Handles TCP stream fragmentation

3. **Quantization**
   - **Position**: 16-bit integers (+/- 20,000 range = ~0.6 unit precision)
   - **Rotation**: 9-bit integer (0-359 degrees)
   - **Velocity**: 8-bit integer (0-130 units/sec)

4. **State Updates**
   - Explicit messages, not generic replication
   - Full `ShipState` for changed entities
   - Respects per-tick send budget

## Technology Stack

### Server
- **Language**: C++23 (Ion Vortex standards)
- **Networking**: libuv for async I/O
- **Serialization**: Custom bitpacked protocol
- **Data**: TOML configuration files
- **Build**: CMake with vcpkg

### Client
- **Language**: C++23 (Ion vortex standards)
- **Rendering**: bgfx (cross-platform)
- **UI**: Dear ImGui
- **Windowing**: GLFW
- **Networking**: Custom TCP client
- **Data**: Shared TOML format

### Shared
- **Protocol**: `ion::protocol` library
- **Serialization**: Bitpacked binary format
- **Math**: GLM for deterministic calculations
- **Compression**: zstd (future)

## Security Considerations

1. **No Client Authority**
   - Client prediction is visual only
   - Server validates all actions
   - Authoritative state always wins

2. **Input Validation**
   - Server validates all client messages
   - Rate limiting per action type
   - Sanity checks on all values
   - Prediction bounds checking

3. **State Protection**
   - No authoritative client state
   - Prediction limited to position/rotation
   - Server rollback on invalid predictions

## Performance Targets

- **Tick Rate**: 30 Hz (33.3ms)
- **Bandwidth**: 512 bytes/tick/client max
- **Latency Tolerance**: Up to 200ms
- **Player Count**: 100 per zone (MVP)

## Future Scalability

### V2 - Enhanced Prediction
- Client-side physics simulation
- Speculative execution of actions
- Advanced reconciliation
- Lag compensation

### V2 - Delta Compression
- Only send changed fields
- Dramatic bandwidth reduction
- More entities per packet

### V3 - Zone Streaming
- Load zones around players
- Virtually limitless world size
- Dynamic server resource allocation

## Implementation Notes

### Client Hit Reporting
Clients report hits for responsiveness:
- "I got hit by missile X"
- "I hit player Y with weapon Z"
- Server validates and applies if valid

### Clock Synchronization
- Initial handshake establishes offset
- Periodic sync messages
- All timestamps server-relative

### Connection Flow
1. TCP connection established
2. Version handshake
3. Clock synchronization
4. Authentication
5. Zone assignment
6. State streaming begins

### Disconnect Handling
- Graceful: Save state, notify zone
- Timeout: 10 second grace period
- Crash: Clean up after timeout