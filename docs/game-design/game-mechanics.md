# Game Mechanics

This document details the core gameplay mechanics and systems in Stellar Conflicts.

## Combat System

### Damage Types and Interactions

The game features four distinct damage types with specific shield/hull interactions:

1. **Shield Damage**
   - Only damages shields
   - No effect if shields are down
   - Used by certain turret weapons

2. **Structure Damage**
   - Only damages hull structure
   - No effect if shields are up
   - Bypasses shields entirely when they're down

3. **Pierce Damage**
   - Always damages hull structure
   - Completely ignores shield state
   - Premium damage type for anti-hull weapons

4. **Heat Damage**
   - Damages shields first
   - Overflows to hull when shields depleted
   - Used by all missiles, mines, and pulsars

### Weapon Systems

**Turrets**
- Can be set to manual (mouse aim) or auto (target lock)
- Manual mode fires at cursor on left click
- Auto mode fires at selected target
- Damage type varies by weapon level

**Hardpoints**
- Fixed-mount weapons
- Can be individually activated/deactivated
- Higher damage than turrets but less flexible

**Missile Banks**
- Can fire single or all missiles
- Player selects missile type per bank
- Fire without energy but won't reload without it
- Always deal Heat damage

### Energy Management

Energy is the core resource governing all ship systems:

- **Weapons**: Won't fire without sufficient energy
- **Specials**: Won't activate without energy; energy-drain specials deactivate
- **Shields**: Won't regenerate without energy
- **Missiles**: Exception - fire without energy but can't reload

Energy flow: Generator → Capacitor → Systems

### Death Mechanics

When a ship is destroyed:
1. All cargo items drop in space
2. All missiles drop as lootable items
3. Special equipment drops (mines, pulsars, flares)
4. Player respawns at last visited station
5. Ship loadout restored (minus dropped items)
6. Maximum 3 ships can be stored per player

## Movement and Physics

### Basic Movement
- **Acceleration/Deceleration**: Forward/backward thrust
- **Rotation**: Turn left/right
- **Afterburner**: Temporary max speed increase

### Physics Parameters
Each ship has:
- Maximum speed
- Maximum acceleration  
- Base rotation rate
- Rotation loss coefficient (turning slows with speed)

### Advanced Movement
**Jump Drive**
- Click destination on map
- Requirements:
  - Sufficient energy
  - Not near stations/planets
  - Not in combat
- Teleports to random location if no destination selected

### Mine Physics
- Inherit ship's velocity and heading when dropped
- Apply negative acceleration
- Velocity/acceleration set to zero when nearly stopped

## Team and Squad Systems

### Temporary Teams
- Join by entering secret team number
- Benefits:
  - Dedicated team chat
  - See all teammates on map
  - See cloaked/stealthed teammates
  - Complete friendly fire immunity

### Permanent Squads

**Hierarchy**:
```
Squad Master
    ├── Squad Leaders
    └── Squad Members
```

**Permissions**:
- **Members**: Leave squad
- **Leaders**: Invite/kick members, repair/upgrade bases
- **Master**: All leader powers plus promote/demote/kick leaders, change name, create base, disband

**Squad Bases**:
- 3 upgrade levels
- Attacked with missiles/turrets
- Repaired via member donations
- Sell exclusive Capital Ships

## Trading System

### Trade Goods
Seven commodity types:
1. Food
2. Wood
3. Iron
4. Machinery
5. Tools
6. Spare Parts
7. Liquor

### Station Trading
- Each station produces specific goods
- Each station demands specific goods
- Dynamic pricing based on supply/demand
- Trading provides income between combat

### Mission System
- Accept missions from stations
- Complete objectives (destroy pirates, deliver goods, etc.)
- Return to station for rewards
- Can abandon missions without penalty

## Visibility and Detection

### Cloaking vs Stealth

**Cloaking** (Feral ships only):
- Ship sprite completely invisible
- Untargetable by seekers and auto-turrets
- Still visible to teammates/squad
- Countered by Detector special

**Stealth** (All ships):
- Invisible to auto-turrets and seekers only
- Still visible to players
- Less powerful than cloaking
- Also countered by Detector

### Detection Mechanics
**Detector Special**:
- Periodic chance to reveal cloaked/stealthed ships
- Makes them targetable again
- Brief window of visibility

### Targeting System
- Cycle through nearby targets with target key
- See enemy stats when:
  - In range
  - Not cloaked/stealthed
  - OR detector proc active
- Auto-turrets need valid target lock

## Special Equipment Mechanics

### Offensive Specials

**Pulsar**
- Area-of-effect damage
- Damages all ships in radius
- Heat damage type
- No friendly fire for teams/squads

**Mine Layer**
- Drops mines behind ship
- Mines inherit ship physics
- Triggered by proximity
- Heat damage on detonation

**Wingman** (Human only)
- Summons AI ally ship
- Follows and assists player
- Limited duration or until destroyed

### Defensive Specials

**HardShields** (Tuskan only)
- Complete damage immunity
- Limited duration
- High energy cost

**Flares**
- Countermeasure for seeking missiles
- Diverts missiles to flare
- Limited ammunition

**Reshield/Restructure**
- Instant full restoration
- Reshield: Shields to 100%
- Restructure: Hull to 100%
- Single use with cooldown

### Utility Specials

**Rapid Missile** (Alkari only)
- Increased missile reload rate
- Duration-based buff
- Stacks with ship bonuses

**Jump Drive**
- Covered in Movement section

## AI Behavior

### AI Pirates
Disengagement triggers:
- Another player enters area (switches target)
- Target moves too far away
- Target cloaks
- Target stealths (if pirate has seekers)
- Target uses jump drive

### AI Traders
- Move goods between stations
- Follow predetermined routes
- Can be attacked for cargo
- Respawn after destruction

### Meteor Events
- Spawn at map edges
- Target stations/planets
- Destroyable for rewards
- Cause temporary destruction on impact

## Progression Systems

### Bounty System
- Actions increase bounty (kills, destructions)
- Displayed on public leaderboard
- Killer claims full bounty as reward
- Portion of bounty adds to killer's score

### Statistics
Tracked per player:
- Funds (currency)
- Score (permanent record)
- Bounty (current risk/reward)
- Kills/Deaths ratio

Admin commands can modify all stats.

## User Interface Systems

### HUD Elements
Real-time display of:
- Energy bar with regeneration rate
- Shield bar with current/max
- Hull structure with damage percentage
- Weapon reload timers
- Special ability cooldowns and durations
- Target information (when applicable)

### Map Systems

**Minimap**:
- Centered on player ship
- Shows nearby ships and stations
- Range-limited view
- Real-time updates

**Zone Map**:
- Full zone overview
- Shows all teammates/squad members
- Points of Interest marked
- Sector grid overlay (1000x1000 units)

### Communication

**Chat Channels**:
1. Global (all players)
2. Team (temporary team)
3. Squad (permanent squad)
4. Whisper (private message)
5. Staff (admin only)

**Galactic News Network**:
- Ticker-style announcements
- Major events (king flag, meteors)
- AI messages to players
- Admin announcements

## Administrative Tools

### Staff Hierarchy
1. **Moderator**: Mute/kick powers
2. **Super Moderator**: Add temporary bans
3. **Admin**: Permanent bans (except Super Admins)
4. **Super Admin**: Full control

### Admin Commands
- Modify player funds/score/bounty/kills/deaths
- Make global announcements
- Access staff chat channel
- Direct database modifications

## Event Systems

### King Flag Event
- Flag spawns at announced location
- Players must capture and hold
- Timer counts down while held
- Winner receives major rewards
- Creates natural PvP hotspot

### Special Mechanics

**Docking**:
- Stations: Enter docking ring slowly, press dock key
- Carriers: Match speed and position, press dock key
- Provides full repair and rearm
- Access to station services while docked

**Spawn Protection**:
- Brief invulnerability after undocking
- Prevents spawn camping
- Carriers place ships with matched velocity

**Equipment Restrictions**:
- Energy systems must balance (generation ≥ consumption)
- Weapon class limits per ship type
- Special equipment class restrictions
- Capital ships can't use cloaking (except Bird of Prey)