Below is a “first-pass” shopping list of the pure-interface headers that belong in oxide::core/….
They form the narrow waist between everything else (zone server, client, tooling) and the underlying OS / third-party libs.
Exact names are suggestions—feel free to bikeshed—but every interface here solves an unavoidable cross-cutting concern.

Area	Interface (namespace + file)	One-liner contract	Typical impl(s)
Diagnostics	oxide::core::ILogger  (logging/logger.h)	Structured log sink with levels & source-location overload.	StdoutLogger, FileLogger, SpdlogLogger
	oxide::core::IMetrics  (metrics/metrics.h)	Increment/gauge/timing counters. Tags supported.	PromMetrics, NoopMetrics
Time & Scheduling	oxide::core::IClock  (time/clock.h)	now_ns(); monotonic, mockable.	SteadyClock, MockClock
	oxide::core::ITaskScheduler  (thread/task_scheduler.h)	Queue & execute jobs, optional affinity groups.	StdThreadPool, NoopScheduler
Configuration / Lifecycle	oxide::core::IService  (lifecycle/service.h)	init(), tick(), shutdown()—base for long-lived subsystems.	Every runtime subsystem
	oxide::core::ISettingsProvider  (config/settings.h)	Read-only key/value lookup (JSON/TOML/env/CLI).	JsonSettings, CliSettings
Filesystem & Assets	oxide::core::IVFS  (fs/vfs.h)	Abstract file reads, directory iterate, pack mounting.	StdVFS, PakVFS, OverlayVFS
	oxide::core::IAssetCache  (asset/cache.h)	Async load → future, fingerprinting, eviction.	RamAssetCache, NullCache
Networking Primitives	oxide::core::ISocket  (net/socket.h)	Datagram/stream read-write, non-blocking.	UvSocket (libuv), WebSocketAdaptor
	oxide::core::ITransport  (net/transport.h)	Packet/fragment framing, channelised send/recv.	UdpTransport, TcpTransport, InProcLoopback
Math & Physics Wrappers	oxide::core::ICollisionShapeFactory  (physics/collider.h)	Build convex hull / BVH from mesh for Bullet.	BtShapeFactory
Platform / Window (client-only)	oxide::core::IWindow  (platform/window.h)	Create GL/Vulkan surface, dispatch input events.	GlfwWindow, NullWindow (headless tests)
	oxide::core::IInput  (platform/input.h)	Query key/button state, text input callback.	GlfwInput

How they plug together

apps/
 └─ client/
      main.cpp
      ↳ GameApp : oxide::core::IService
         ↳ ctor receives ILogger*, IMetrics*, IClock*, IWindow*, ...
         ↳ tick() runs ECS / gameplay using data from subsystems
 └─ zoned/
      zone_server.cpp
      ↳ ZoneService : IService
         ↳ owns Transport + AssetCache + PhysicsWorld
         ...

	•	oxide::core never knows about bgfx, Bullet, or Ultralight. Those libs live in modules that implement the interfaces.
	•	Unit tests and server builds link only to oxide::core + mock implementations; the desktop client links to GL/Window/Bullet implementations.
	•	If you swap libuv for another socket backend (say, asio), you touch only the adapter, not zone logic.

---

Me: "Shouldn't we put the other stuff like window, etc in their own respective internal libraries?"

---

Iterated result, takes precedence over previous post but refers to it:

That’s a perfectly reasonable split—​and, in fact, what many teams do once the “core” nucleus starts to feel bloated.

Think of oxide::core as only the contracts that every executable (zone server, universe router, headless test-runner and desktop client) must see:

Keep in oxide::core	Push into feature libs
Logging / metrics (ILogger, IMetrics)	Rendering (IRenderBackend, etc.)
Clock / basic scheduler	Physics & collision (Bullet wrapper)
Service lifecycle (IService)	Platform window, input, audio
Settings / basic VFS	High-level asset cache, scene graph, animation
Low-level net socket / transport	Any client-only UI layer (ImGui)

Why split?
	•	Server binaries have no reason to pull in GLFW, bgfx, Bullet, or platform input.
If those headers sit in core, they leak into everything and complicate CI for minimal-footprint headless builds.
	•	Compile-time isolation – a change to IWindow shouldn’t force every microservice to rebuild.
	•	Clear ownership – if physics is an optional plugin, house it under oxide::physics (or oxide::bullet) so teams can swap it (e.g., mock collider for unit tests).

Suggested directory / namespace map

oxide/
├─ core/              # ultra–thin contracts every target needs
│   ├─ logging/
│   ├─ metrics/
│   ├─ net/
│   ├─ fs/
│   └─ lifecycle/
├─ platform/          # window + input (GLFW, SDL, etc.)
│   └─ include/oxide/platform/...
├─ render/            # bgfx wrapper + material system
├─ physics/           # Bullet adapters, collider factory
├─ audio/             # miniaudio backend
└─ game/              # ECS / gameplay logic, depends on render+physics when building the client

Each sub-module:
	•	owns its own include/oxide/<module>/…
	•	links upward only to oxide::core, never sideways to another peer unless truly required
	•	exposes a factory (create_<Module>_impl(...)) returning the core interface so callers stay decoupled.

What changes from the list I gave earlier?
	•	Move IWindow, IInput, ICollisionShapeFactory (and any math helpers tied to physics/graphics) out of core into their respective modules.
	•	Keep glm types in public method signatures if you like, but consider wrapping them (or typedef) in oxide::math to avoid hard dependency bleed-through.

TL;DR

Yes—​trim oxide::core to only cross-cutting, platform-agnostic contracts.
Put graphics, physics, audio, and platform specifics in their own leaf libraries.
Your servers stay slim, your client still links everything, and your rebuild graph (and cognitive load) stay small.