# Game Design Documentation

Welcome to the Stellar Conflicts game design documentation. This section contains all the information needed to understand the game's mechanics, requirements, and technical architecture.

## Overview

Stellar Conflicts is a real-time, multiplayer space action game built on the Oxide framework. The game features:

- **Authoritative server architecture** - The server is the absolute source of truth
- **Dumb client design** - The client only renders and captures input
- **Four ship classes** with unique abilities
- **Complex loadout system** with weapons, shields, and special equipment
- **Team and squad-based gameplay**
- **Trading and mission systems**
- **Territory control** via squad bases

## Documentation Structure

### 📋 [MVP Requirements](mvp-requirements.md)
The complete feature list and requirements for the Minimum Viable Product. This is your checklist for what needs to be implemented.

### 🏗️ [Network Architecture](network-architecture.md)
Technical details about the client-server architecture, networking protocol, and state synchronization.

### 🎮 [Game Mechanics](game-mechanics.md)
Detailed explanation of all game systems including combat, equipment, damage types, and player interactions.

### 🚀 [Ships and Equipment](ships-equipment.md)
Complete reference for all ship classes, weapons, special abilities, and equipment statistics.

## Quick Start for Developers

1. **Read the MVP Requirements** first to understand the full scope
2. **Study the Network Architecture** to understand how client and server communicate
3. **Review Game Mechanics** for implementation details of each system
4. **Reference Ships and Equipment** for specific stats and balancing

## Core Design Principles

1. **Server Authority**: The server has absolute authority over game state. Clients never make gameplay decisions.
2. **Deterministic Simulation**: All game logic runs at a fixed 30 Hz tick rate for consistency.
3. **Simple Client Prediction**: Clients perform basic prediction to reduce input latency while server maintains authority.
4. **Explicit State**: No hidden mechanics or implicit behaviors - everything is clearly defined.

## Technology Constraints

- **Server**: C++23 following Oxide/Oat Interactive standards
- **Client**: Custom C++23 renderer using bgfx and ImGui
- **Networking**: Custom TCP protocol with bitpacked serialization
- **Data**: TOML configuration files for game data

For technical implementation details, see the [Architecture Documentation](../architecture/).