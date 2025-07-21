Below is a “first-pass” shopping list of the pure-interface headers that belong in ion::core/….
They form the narrow waist between everything else (zone server, client, tooling) and the underlying OS / third-party libs.
Exact names are suggestions—feel free to bikeshed—but every interface here solves an unavoidable cross-cutting concern.

Area	Interface (namespace + file)	One-liner contract	Typical impl(s)
Diagnostics	ion::core::logger_base  (logging/logger.h)	Structured log sink with levels & source-location overload.	stdout_logger, file_logger, spdlog_logger
	ion::core::metrics_base  (metrics/metrics.h)	Increment/gauge/timing counters. Tags supported.	prom_metrics, noop_metrics
Time & Scheduling	ion::core::clock_base  (time/clock.h)	now_ns(); monotonic, mockable.	steady_clock, mock_clock
	ion::core::task_scheduler_base  (thread/task_scheduler.h)	Queue & execute jobs, optional affinity groups.	tbb_thread_pool, noop_scheduler
Configuration / Lifecycle	ion::core::service_base  (lifecycle/service.h)	init(), tick(), shutdown()—base for long-lived subsystems.	Every runtime subsystem
	ion::core::settings_provider_base  (config/settings.h)	Read-only key/value lookup (JSON/TOML/env/CLI).	json_settings, cli_settings
Filesystem & Assets	ion::core::vfs_base  (fs/vfs.h)	Abstract file reads, directory iterate, pack mounting.	std_vfs, pak_vfs, overlay_vfs
	ion::core::asset_cache_base  (asset/cache.h)	Async load → future, fingerprinting, eviction.	ram_asset_cache, null_cache
Networking Primitives	ion::core::socket_base  (net/socket.h)	Datagram/stream read-write, non-blocking.	uv_socket (libuv), web_socket
	ion::core::transport_base  (net/transport.h)	Packet/fragment framing, channelised send/recv.	udp_transport, tcp_transport, loop_transport

How they plug together

apps/
 └─ client/
      main.cpp
      ↳ GameApp : ion::core::service_base
         ↳ ctor receives logger_base*, metrics_base*, clock_base*, window_base*, ...
         ↳ tick() runs gameplay using data from subsystems
 └─ zoned/
      zone_server.cpp
      ↳ ZoneService : service_base
         ↳ owns transport + asset_cache + physics_world
         ...

	•	ion::core never knows about SDL3, or ImGui. Those libs live in modules that implement the interfaces.
	•	Unit tests and server builds link only to ion::core + mock implementations; the desktop client links to GL/Window implementations.
	•	If you swap libuv for another socket backend (say, asio), you touch only the adapter, not zone logic.

---

Me: "Shouldn't we put the other stuff like window, etc in their own respective internal libraries?"

---

Iterated result, takes precedence over previous post but refers to it:

That’s a perfectly reasonable split—​and, in fact, what many teams do once the “core” nucleus starts to feel bloated.

Think of ion::core as only the contracts that every executable (zone server, universe router, headless test-runner and desktop client) must see:

Keep in ion::core	Push into feature libs
Logging / metrics (logger_base, metrics_base)	Rendering (renderer_base, etc.)
Clock / basic scheduler	Physics & collision
Service lifecycle (service_base)	Platform window, input, audio
Settings / basic VFS	High-level asset cache, scene graph, animation
Low-level net socket / transport	Any client-only UI layer (ImGui)

Why split?
	•	Server binaries have no reason to pull in SDL.
If those headers sit in core, they leak into everything and complicate CI for minimal-footprint headless builds.
	•	Compile-time isolation – a change to window_base shouldn’t force every microservice to rebuild.
	•	Clear ownership – if physics is an optional plugin, house it under ion::physics so teams can swap it (e.g., mock collider for unit tests).

Suggested directory / namespace map

ion/
├─ core/              # ultra–thin contracts every target needs
│   ├─ logging/
│   ├─ metrics/
│   ├─ net/
│   ├─ fs/
│   └─ lifecycle/
├─ platform/          # window + input (SDL, etc.)
│   └─ include/ion/platform/...
├─ render/            # bgfx wrapper + material system
├─ physics/           # SAT collider factory
├─ audio/             # miniaudio backend
└─ game/              # gameplay logic, depends on render+physics when building the client

Each sub-module:
	•	owns its own include/ion/<module>/…
	•	links upward only to ion::core, never sideways to another peer unless truly required
	•	exposes a factory (create_<module>_impl(...)) returning the core interface so callers stay decoupled.

What changes from the list I gave earlier?
	•	Move window_base, input_base, collision_shape_factory (and any math helpers tied to physics/graphics) out of core into their respective modules.
	•	Keep glm types in public method signatures if you like, but consider wrapping them (or typedef) in ion::math to avoid hard dependency bleed-through.

TL;DR

Yes—​trim ion::core to only cross-cutting, platform-agnostic contracts.
Put graphics, physics, audio, and platform specifics in their own leaf libraries.
Your servers stay slim, your client still links everything, and your rebuild graph (and cognitive load) stay small.