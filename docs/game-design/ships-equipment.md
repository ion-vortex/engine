# Ships and Equipment

This document provides a complete reference for all ships, weapons, and equipment in Stellar Conflicts.

## Ship Classes

### Overview

Each ship class has unique characteristics and one exclusive special ability:

| Class   | Exclusive Special | Playstyle                |
|---------|------------------|--------------------------|
| Human   | Wingman          | Balanced, team support   |
| Feral   | Cloaking         | Stealth, hit-and-run     |
| Alkari  | Rapid Missile    | Missile-focused combat   |
| Tuskan  | HardShields      | Tank, defensive play     |

### Ship Specifications

Each ship has maximum limits for:
- Shield generator class
- Energy generator class  
- Energy capacitor class
- Number of turrets
- Number of hardpoints
- Number of missile banks
- Number of specials
- Cargo capacity
- Missiles per bank
- Maximum weapon class

*Note: Specific values per ship model to be defined in game data.*

### Ship Physics

Every ship has these movement characteristics:
- **Maximum Speed**: Top velocity in units/second
- **Maximum Acceleration**: Thrust power
- **Rotation Rate**: Base turning speed
- **Rotation Loss Coefficient**: How much turning slows with speed
- **Afterburner Boost**: Speed multiplier when engaged

## Energy Systems

### Energy Generators (11 Levels)

Higher levels provide:
- Increased energy regeneration rate (energy/second)
- More total power for systems
- Support for higher-tier equipment

*Installation Rule*: Generator output must meet or exceed total system consumption.

### Energy Capacitors (10 Levels)

Higher levels provide:
- Larger energy storage capacity
- Burst power for weapons/specials
- Buffer for high-drain situations

### Shield Generators (9 Levels)

Each level defines:
- **Shield Capacity**: Maximum shield points
- **Regeneration Rate**: Shield points/second
- **Energy Consumption**: Drain on energy system

*Installation Rule*: Shield consumption cannot exceed ship's energy generation.

## Weapons

### Turret/Hardpoint Weapons (11 Levels)

1. **Laser**
   - Entry-level energy weapon
   - Low damage, low energy cost
   - Shield damage primary

2. **Double Laser**
   - Twin-linked laser system
   - Moderate damage increase
   - Higher energy consumption

3. **Mass Driver**
   - Kinetic projectile weapon
   - Balanced shield/structure damage
   - No energy requirement

4. **Neutron Blast**
   - High-energy particle weapon
   - Strong shield damage
   - Moderate pierce component

5. **Ion Blast**
   - Disruption-focused weapon
   - Shield specialist
   - Energy drain effect

6. **Plasma Cannon**
   - Superheated matter projectile
   - High heat damage
   - Slower projectile speed

7. **Phasor**
   - Continuous beam weapon
   - Sustained damage
   - High energy drain

8. **Neutron Pulse**
   - Burst-fire neutron weapon
   - Very high damage
   - Significant cooldown

9. **Ion Pulse**
   - Rapid-fire ion bursts
   - Shield shredder
   - Chain-disruption potential

10. **Plasma Pulse**
    - Plasma burst weapon
    - Extreme heat damage
    - Area effect potential

11. **Mauler**
    - Ultimate weapon system
    - Massive all-damage types
    - Extreme energy cost

### Missile Types

All missiles deal **Heat Damage** (shield first, then hull).

#### Disrupters (Tiers 1-3)
- **Effect**: Sets target velocity to zero
- **Duration**: Scales with tier
- **Use Case**: Stopping fleeing enemies

#### EMPs (Tiers 1-3)
- **Effect**: Disables shields and energy generation
- **Duration**: Scales with tier
- **Use Case**: Disabling defenses

#### Multi-Kill Vehicles (Tiers 1-3)
- **Type**: Dumbfire (no tracking)
- **Damage**: Extremely high
- **Cost**: Very expensive
- **Use Case**: Stationary or large targets

#### Seekers (Tiers 1-3)
- **Type**: Lock-on tracking
- **Damage**: Moderate
- **Tracking**: Follows target
- **Counter**: Flares, stealth

#### Torpedoes (Tiers 1-3)
- **Type**: Dumbfire
- **Damage**: Moderate
- **Cost**: Affordable
- **Use Case**: General combat

## Special Equipment

### Combat Specials

#### Pulsar (Tiers 1-3)
- **Type**: Area damage
- **Damage**: Heat type
- **Range**: Scales with tier
- **Cooldown**: Long

#### Mine Layer (Tiers 1-3)
- **Type**: Deployable trap
- **Damage**: Heat type
- **Physics**: Inherits ship velocity
- **Duration**: Until triggered or timeout

#### Wingman (Tiers 1-3) - Human Only
- **Type**: AI companion
- **Power**: Scales with tier
- **Duration**: Limited time
- **Behavior**: Follows and assists

### Defensive Specials

#### Cloaking (Tiers 1-3) - Feral Only
- **Effect**: Complete invisibility
- **Duration**: Energy-dependent
- **Counter**: Detector special
- **Benefits**: Untargetable by seekers/turrets

#### Stealth (Tiers 1-3) - All Ships
- **Effect**: Hidden from automated systems
- **Duration**: Energy-dependent
- **Counter**: Detector special
- **Benefits**: Avoid seekers/turrets only

#### HardShields (Tiers 1-3) - Tuskan Only
- **Effect**: Complete invulnerability
- **Duration**: Brief (scales with tier)
- **Cooldown**: Very long
- **Energy Cost**: Extreme

#### Flares (Tiers 1-3)
- **Type**: Missile countermeasure
- **Uses**: Limited ammunition
- **Effect**: Diverts seeking missiles
- **Deployment**: Manual activation

### Utility Specials

#### Detector (Tiers 1-3)
- **Effect**: Reveals cloaked/stealthed enemies
- **Proc Chance**: Scales with tier
- **Range**: Area around ship
- **Duration**: Continuous with energy

#### Jump Drive (Tiers 1-3)
- **Type**: Teleportation
- **Range**: Scales with tier
- **Restrictions**: No combat, away from objects
- **Destination**: Targeted or random

#### Rapid Missile (Tiers 1-3) - Alkari Only
- **Effect**: Faster missile reload
- **Multiplier**: Scales with tier
- **Duration**: While active
- **Stacking**: Multiplicative with ship bonuses

### Restoration Specials

#### Reshield (Tiers 1-3)
- **Effect**: Instant shield restoration
- **Amount**: 100% of maximum
- **Cooldown**: Moderate
- **Uses**: Unlimited with cooldown

#### Restructure (Tiers 1-3)
- **Effect**: Instant hull repair
- **Amount**: 100% of maximum
- **Cooldown**: Long
- **Uses**: Unlimited with cooldown

## Capital Ships

Special ships available only at squad bases:

### General Features
- Significantly more hardpoints and turrets
- Much larger hull capacity
- No respawn on destruction
- Cannot equip cloaking (except Bird of Prey)
- Extreme cost

### Special Capital Ships

#### Carrier
- **Unique Feature**: Mobile docking platform
- **Services**: Repair and rearm for allies
- **Docking**: Match speed and position
- **Auto-Undock**: At 20% hull

#### Bird of Prey
- **Unique Feature**: Only cloaking-capable capital ship
- **Role**: Stealth capital operations
- **Limitations**: Fewer weapons than other capitals

## Equipment Compatibility

### Energy Balance Requirements
- Total shield consumption ≤ Total energy generation
- Weapon energy requirements must be sustainable
- Special equipment drain calculated in total

### Class Restrictions
- Each ship has maximum weapon class allowed
- Turrets and hardpoints limited by hull design
- Special slots vary by ship model
- Missile bank capacity ship-dependent

### Loadout Considerations
1. **Energy Budget**: Balance generation with consumption
2. **Weapon Mix**: Combine damage types for versatility
3. **Special Synergy**: Choose complementary abilities
4. **Role Optimization**: Build for intended playstyle

## Trading and Equipment Economy

### Purchase Locations
- **Stations**: Standard equipment up to certain tiers
- **Squad Bases**: Advanced equipment and capital ships
- **Special Events**: Unique or limited items

### Sell-Back Value
- Equipment sells for 100% purchase price
- Ships sell for purchase + outfit cost
- No depreciation or wear
- Last ship cannot be sold (only exchanged)

### Storage Limits
- Maximum 3 ships owned per player
- Unlimited equipment in station storage
- Active ship loadout limits apply

## Equipment Statistics Display

When examining equipment, players see:
- Damage values and types
- Energy consumption rates
- Cooldown/reload times
- Special effect descriptions
- Tier comparisons
- Compatibility warnings

This comprehensive system allows for diverse builds while maintaining clear roles and counterplay options for each equipment type.