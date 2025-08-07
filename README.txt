# Bomberman - Game Engine Project

## Overview
A recreation of the classic Bomberman game built on a custom C++ game engine, demonstrating clean architecture separation between engine and game logic with multiple game modes and robust input systems.

## Architecture

### Engine (Minigin)
A component-based 2D game engine featuring:

- **GameObject-Component System**: Entities are GameObjects with attached Component behaviors
- **Scene Management**: Scene graph with parent-child hierarchies and transition handling
- **Collision System**: AABB detection with pluggable CollisionResponder pattern
- **Observer Pattern**: Event-driven communication between game systems
- **Service Locator**: Global access to engine services (Audio, Input, Rendering)
- **Resource Management**: Centralized loading and caching of textures and fonts
- **Input Management**: Multi-device support (keyboard + gamepad) with player-specific bindings

## Game Implementation

### Game Modes
- **Story Mode**: Single-player progression through levels with name entry and high scores
- **Co-op Mode**: Two-player cooperative gameplay with shared lives system
- **Versus Mode**: Player vs Player competitive gameplay

### Level System
- Text file-based level loading (.txt format with character-based tile mapping)
- Dynamic level generation from grid data
- Scene transitions
- Multi-level progression with completion tracking

### Component Architecture Examples
- **BaseEnemyComponent**: Abstract base for all enemy types with shared AI behaviors
- **PlayerComponent**: Handles player-specific logic (movement, bomb placement, damage)
- **BombComponent**: State machine implementation for bomb lifecycle and chain reactions
- **PowerUpComponent**: Collectible system with type-based effects (Extra Bombs, Detonator, Flame Range)
- **GameModeController**: Mode-specific game state management (Story/Co-op/Versus)

### AI Implementation
- **Pathfinding**: Grid-based movement with obstacle avoidance
- **State-based AI**: Different enemy types with varying behaviors
- **Player Targeting**: Dynamic target acquisition and chase mechanics
- **Collision Avoidance**: Smart movement around obstacles and other entities

### Collision Handling Examples
- **CollisionResponder**: Base interface for collision reactions
- **StaticWallResponder**: Blocks movement for solid walls
- **DestructibleWallResponder**: Handles breakable wall destruction with animation
- **BlastResponder**: Manages explosion damage propagation and chain reactions
- **ExitResponder**: Level completion triggers with proper transitions
- **PowerUpCollisionResponder**: Item collection and effect application

## Manager Systems

### Core Managers
- **CollisionManager**: Centralized collision checking and component registration
- **EnemyManager**: Tracks enemy count, spawns exit portal, handles enemy registration
- **ScoreManager**: Persistent score tracking across levels with observer notifications
- **PlayerManager**: Multi-player support with closest player queries for AI
- **LevelManager**: Level progression, scene transitions, and cleanup coordination

### Specialized Managers
- **CoopLevelManager**: Co-op specific level progression without high score integration
- **GameOverManager**: End-game state handling with high score integration
- **HighScoreManager**: Persistent high score storage and retrieval
- **PlayerNameManager**: Name entry system for high score tracking

## Input System

### Multi-Device Support
- **Keyboard Input**: Traditional WASD/Arrow key controls with command binding
- **Gamepad Support**: XInput integration (not working for player)
- **Component-based Gamepad**: Custom GamepadMovementComponent for reliable input handling
- **Input Manager**: Command binding with player-specific contexts

### Input Handling Innovations
- **Edge Detection**: Proper button press/release handling for both keyboard and gamepad
- **Player-specific Bindings**: Multiple players can use different input devices simultaneously
- **Polling-based Gamepad**: Direct hardware polling to bypass complex edge detection issues

## Data Flow

### Event System
- Observer pattern for decoupled event notifications (enemy deaths, player damage, score changes)
- Event propagation with data payload support
- Multi-observer support for shared events (lives display, score tracking)

### Safe Object Management
- Deferred object deletion to prevent iterator invalidation
- Pending object queues for safe runtime object creation
- Cleanup sequences during level transitions to prevent memory leaks

## Technical Design Patterns

### Core Patterns
- **Component Pattern**: Composition over inheritance for game object behaviors
- **Observer Pattern**: Event system for loose coupling between systems  
- **Service Locator**: Access to engine services without tight coupling
- **State Pattern**: Bomb states, game states for managing transitions
- **Command Pattern**: Input handling with undoable actions
- **Singleton Pattern**: Manager classes with controlled global access

### Advanced Patterns
- **Strategy Pattern**: Different AI behaviors for enemy types
- **Factory Pattern**: Scene loading and object creation
- **Template Method**: Base enemy behavior with specialized overrides
- **Chain of Responsibility**: Collision response handling

## Memory Management

### Safety Measures
- **RAII**: Resource acquisition and cleanup in constructors/destructors
- **Smart Pointers**: Automatic memory management for GameObjects
- **Deferred Deletion**: Safe object removal during iteration
- **Reference Tracking**: Observer cleanup to prevent dangling pointers

### Performance Optimizations
- **Object Pooling**: Reusable blast objects during explosions
- **Dirty Flag**: Transform hierarchy optimization for position updates
- **Batched Operations**: Collision checks and scene updates in batches

## Key Implementation Details

### Technical Specifications
- **Grid-based World**: 16x16 pixel tiles with precise collision detection
- **Sprite Animation**: Frame-based animation system with configurable timing
- **Audio Integration**: SDL_Mixer for music and sound effects with volume control
- **Cross-platform**: Windows-compatible with XInput gamepad support

### Architecture Benefits
- **Separation of Concerns**: Clear boundaries between engine and game logic
- **Extensibility**: Easy addition of new enemy types, power-ups, and game modes
- **Maintainability**: Modular design with minimal coupling between systems
- **Testability**: Component-based architecture allows isolated unit testing

## Scene Management

### Transition Handling
- **Safe Cleanup**: Proper destruction order to prevent crashes during scene changes
- **State Preservation**: Score and progress maintained across level transitions  
- **Resource Management**: Automatic texture and audio cleanup between scenes
- **Input State Reset**: Clean input binding management during mode switches

### Multi-Scene Architecture
- **Scene Factory**: Dynamic scene creation based on game mode
- **Scene Graph**: Hierarchical object management with parent-child relationships
- **Delayed Loading**: Components for smooth transitions between game states