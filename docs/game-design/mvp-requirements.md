# MVP Requirements

This document defines the complete feature set required for the Stellar Conflicts Minimum Viable Product (MVP).

## Core Architecture Requirements

- **Project Name**: Stellar Conflicts
- **Client**: Custom C++ renderer using bgfx and ImGui (dumb terminal with simple prediction)
- **Server**: C++23 authoritative server following Ion Vortex C++ Manual
- **Input**: Support for both gamepad and keyboard/mouse

## Ship Systems

### Core Components
Each ship has exactly ONE of each:
- Shield generator
- Energy capacitor  
- Energy generator

### Weapon Systems
Ships can have:
- **Turrets**: 0 or more (limited by ship class)
- **Hardpoints**: 0 or more (limited by ship class)
- **Missile Banks**: Can store any combination of missiles up to capacity
- **Specials**: 0 or more (limited by ship class)

### Ship Classes
Four distinct classes with unique special abilities:
1. **Human** - Can use Wingman special
2. **Feral** - Can use Cloaking special
3. **Alkari** - Can use Rapid Missile special
4. **Tuskan** - Can use HardShields special

## Equipment Tiers

### Energy Systems
- **Capacitors**: 10 levels
- **Energy Generators**: 11 levels
- **Shield Generators**: 9 levels

### Weapons
**Missiles** (5 types, tiers 1-3):
- **Disrupters**: Set target velocity to zero
- **EMPs**: Disable shield and energy generators temporarily
- **Multi-kill Vehicles**: Expensive dumbfire, high damage
- **Seekers**: Lock-on torpedoes that follow targets
- **Torpedoes**: Standard dumbfire, moderate damage

**Turret/Hardpoint Weapons** (11 levels):
1. Laser
2. Double Laser
3. Mass Driver
4. Neutron Blast
5. Ion Blast
6. Plasma Cannon
7. Phasor
8. Neutron Pulse
9. Ion Pulse
10. Plasma Pulse
11. Mauler

### Special Equipment
**12 types** (tiers 1-3):
- **Cloaking**: Invisible sprite, untargetable (Feral only)
- **Detector**: Counter cloak/stealth
- **Flares**: Missile countermeasure
- **HardShields**: Temporary invulnerability (Tuskan only)
- **Jump Drive**: Random map teleport
- **Mine Layer**: Drop mines
- **Pulsar**: Area damage
- **Rapid Missile**: Faster reload (Alkari only)
- **Reshield**: Full shield restore
- **Restructure**: Full hull repair
- **Stealth**: Invisible to auto-turrets and seekers
- **Wingman**: Summon AI ally (Human only)

## Damage System

### Damage Types
1. **Shield Damage**: Only affects shields (no effect if shields down)
2. **Structure Damage**: Only affects hull (no effect if shields up)
3. **Pierce Damage**: Always hits hull, ignores shields
4. **Heat Damage**: Hits shields first, then hull

### Damage Sources
- Missiles: Always Heat damage
- Mines: Always Heat damage
- Pulsars: Always Heat damage
- Turrets/Hardpoints: Vary by weapon type

## Death and Respawn

When destroyed, ships drop:
- All cargo
- All missiles
- Mine specials (if equipped)
- Pulsar specials (if equipped)
- Flares (if equipped)

Players respawn at their last station with restored loadout (minus dropped items).

## Team Systems

### Temporary Teams
- Join via secret team number
- Features:
  - Team chat channel
  - See teammates on map
  - See cloaked/stealthed teammates
  - No friendly fire

### Squads (Permanent Teams)
- Hierarchy: Members → Leaders → Master
- Squad Master can:
  - Change squad name
  - Promote/demote leaders
  - Kick anyone
  - Create squad base
  - Disband squad
- Squad Leaders can:
  - Invite players
  - Kick members (not leaders)
- All squad members:
  - See each other on map
  - See cloaked/stealthed members
  - No friendly fire
  - Squad chat channel

### Squad Bases
- 3 upgrade levels
- Can be attacked and destroyed
- Repaired/upgraded with donations
- Attack with missiles and turrets
- Sell Capital Ships

## Capital Ships
- Very large ships with many weapons
- No respawn when destroyed
- Cannot use cloaking (except Bird of Prey)
- **Carrier**: Can be docked with for repair/rearm

## Stations

### Docking
- Enter docking circle at slow speed
- Press dock key
- Provides repair and rearm

### Services
- Buy/sell trade goods (7 types: Food, Wood, Iron, Machinery, Tools, Spare Parts, Liquor)
- Buy/sell ships (max 3 stored)
- Outfit ships
- Accept/turn in missions
- Deals and subsidies

### Station Mechanics
- Run by NPCs
- Can be attacked and destroyed
- Slowly repair over time
- If destroyed, players respawn at nearest valid station

## Map Features

### Points of Interest
- Mission targets
- Visible on map

### Meteors
- Spawn and head toward stations/planets
- Can be destroyed for rewards
- Destroy stations/planets on impact (respawn after time)

### AI Ships
- Trade ships move goods between stations
- Pirates attack players and stations
- Killing pirates earns rewards

## Player Progression

### Bounty System
- Actions increase bounty
- Visible on leaderboard
- Killer receives bounty as reward
- Portion added to killer's score

### Statistics Tracked
- Funds
- Score
- Bounty
- Kills
- Deaths

## Events
- **King Flag**: Capture and hold flag for time limit

## Administration

### Staff Levels
1. **Moderator**: Mute, kick
2. **Super Moderator**: Temporary bans
3. **Admin**: Permanent bans (except Super Admins)
4. **Super Admin**: All powers

All staff can:
- Access staff chat
- Make global announcements

## Combat Controls

### Weapons
- Turrets: Manual (mouse aim) or Auto (target lock)
- Missiles: Fire one or all
- Select missile type per bank
- Activate/deactivate hardpoints
- Activate/deactivate specials

### Energy Management
- Weapons won't fire without energy
- Specials won't activate without energy
- Shields won't regenerate without energy
- Missiles fire but won't reload without energy

### Targeting
- Cycle targets with target key
- See enemy stats if in range and not cloaked/stealthed
- Detector procs reveal cloaked enemies

## Movement

### Ship Physics
- Accelerate/decelerate
- Turn (rotation rate decreases with speed)
- Afterburner for speed boost
- Each ship has:
  - Max speed
  - Max acceleration
  - Rotation rate
  - Rotation loss coefficient

### Jump Drive
- Click map to jump
- Cannot jump near stations/planets
- Requires energy
- Cannot jump in combat

## User Interface

### HUD Elements
- Energy/shield/structure levels
- Weapon reload timers
- Special cooldown timers
- Minimap (centered on player)
- Zone map (full map view)

### Communication
- Global chat
- Team chat
- Squad chat
- Whisper to players
- Galactic News Network ticker

### Account Management
- Register/login
- Password recovery
- Zone selection
- New player guide

## Technical Requirements

### Synchronization
- Client-server clock sync
- Client reports hits to server
- Server validates all actions

### Map System
- Divided into 1000x1000 unit sectors
- Locations given as sector coordinates

### Equipment Restrictions
- Shield generator consumption ≤ ship's energy generation
- Energy generator regeneration ≥ shield consumption
- Weapon class limits per ship

### Special Mechanics
- Most specials have cooldown periods
- Most specials have duration
- Some specials have proc chance
- Some specials can fail
- All weapons/specials have sound effects
- Dropped mines inherit ship velocity with negative acceleration
- All weapons have limited lifetime