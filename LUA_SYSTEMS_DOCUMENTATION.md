# Lua System Bindings Documentation

## Overview

The LuaLoader now provides comprehensive bindings for the core engine systems:
- **Physics**: PhysicsEngine integration for rigid body simulation
- **Renderer**: Graphics rendering capabilities for sprites, textures, fonts, and shapes
- **Audio**: AudioPlayer integration for sound effects and music

## Setup

To use these systems in Lua, you must first set them up in your C++ code:

```cpp
// Create your system instances
auto physicsEngine = std::make_shared<YourPhysicsEngine>();
auto renderer = std::make_shared<YourRenderer>(); 
auto audioPlayer = std::make_shared<YourAudioPlayer>();

// Set them in the LuaLoader
luaLoader.setPhysicsEngine(physicsEngine);
luaLoader.setRenderer(renderer);
luaLoader.setAudioPlayer(audioPlayer);
```

## Physics System (`Physics` table)

### Initialization
```lua
Physics.init(gravityX, gravityY)  -- Initialize physics with gravity vector
```

### Simulation
```lua
Physics.step(deltaTime)  -- Step the physics simulation forward
```

### Rigid Bodies
```lua
-- Add a rigid body to an entity
Physics.addRigidBody(entity, x, y, width, height, angle, hasGravity, density, friction)

-- Get rigid body position (returns table with x, y)
local pos = Physics.getRigidBodyPos(entity)
print(pos.x, pos.y)

-- Get rigid body rotation angle
local angle = Physics.getRigidBodyAngle(entity)
```

### Example
```lua
-- Initialize physics
Physics.init(0, 9.8)

-- Create entity with physics
local entity = ECS.createEntity()
Physics.addRigidBody(entity, 100, 100, 32, 32, 0, true, 1.0, 0.5)

-- Update simulation
Physics.step(0.016)  -- ~60 FPS

-- Get updated position
local newPos = Physics.getRigidBodyPos(entity)
```

## Renderer System (`Renderer` table)

### Window Management
```lua
Renderer.createWindow(width, height, title)  -- Create game window
Renderer.getWindowWidth()                    -- Get window width
Renderer.getWindowHeight()                   -- Get window height
Renderer.clearWindow()                       -- Clear screen
Renderer.displayWindow()                     -- Present frame
Renderer.closeWindow()                       -- Close window
Renderer.isWindowOpen()                      -- Check if window is open
```

### Texture Management
```lua
Renderer.loadTexture(id, filepath)     -- Load texture from file
Renderer.unloadTexture(id)             -- Unload texture
```

### Sprite Management
```lua
Renderer.createSprite(id, textureId)                    -- Create sprite
Renderer.setSpritePosition(id, x, y)                    -- Set position
Renderer.setSpriteTexture(id, textureId)                -- Change texture
Renderer.setSpriteTextureRect(id, left, top, w, h)      -- Set texture region
Renderer.setSpriteRotation(id, angle)                   -- Set rotation
Renderer.setSpriteScale(id, scale)                      -- Set scale
Renderer.setSpriteOrigin(id, x, y)                      -- Set origin point
Renderer.drawSprite(id)                                 -- Draw sprite
Renderer.removeSprite(id)                               -- Remove sprite
Renderer.scrollSprite(id)                               -- Scroll sprite
```

### Font and Text
```lua
Renderer.loadFont(id, filepath)                         -- Load font
Renderer.unloadFont(id)                                 -- Unload font
Renderer.drawText(fontId, text, x, y, size, color)      -- Draw text
```

### Shape Drawing
```lua
Renderer.drawRectangle(x, y, width, height, color)      -- Draw rectangle
```

### Example
```lua
-- Load assets
Renderer.loadTexture("player", "sprites/player.png")
Renderer.loadFont("ui", "fonts/arial.ttf")

-- Create and setup sprite
Renderer.createSprite("player_sprite", "player")
Renderer.setSpritePosition("player_sprite", 100, 200)
Renderer.setSpriteScale("player_sprite", 2.0)

-- Game loop
while Renderer.isWindowOpen() do
    Renderer.clearWindow()
    
    -- Draw game objects
    Renderer.drawSprite("player_sprite")
    Renderer.drawText("ui", "Score: 1000", 10, 10, 24, 0xFFFFFFFF)
    Renderer.drawRectangle(50, 50, 100, 20, 0xFF0000FF)  -- Red rectangle
    
    Renderer.displayWindow()
end
```

## Audio System (`Audio` table)

### Audio Management
```lua
Audio.loadAudio(id, filepath)      -- Load audio file
Audio.playAudio(id, loop)          -- Play audio (loop = true/false)
Audio.stopAudio(id)               -- Stop playing audio
Audio.unloadAudio(id)             -- Unload audio from memory
```

### Example
```lua
-- Load sounds
Audio.loadAudio("shoot", "sounds/laser.wav")
Audio.loadAudio("bgm", "music/theme.mp3")

-- Play sound effects
Audio.playAudio("shoot", false)        -- Play once
Audio.playAudio("bgm", true)           -- Loop background music

-- Stop and cleanup
Audio.stopAudio("bgm")
Audio.unloadAudio("shoot")
Audio.unloadAudio("bgm")
```

## Integration with ECS

These systems work seamlessly with the existing ECS bindings:

```lua
-- Create game entity
local player = ECS.createEntity()

-- Add ECS components
ECS.addTransform(player, {pos = {x = 100, y = 200}, rot = 0, scale = {x = 1, y = 1}})
ECS.addSprite(player, {sprite_name = "player_sprite", frame_nb = 0, scrolling = false, is_background = false})

-- Setup rendering
Renderer.loadTexture("player_tex", "player.png")
Renderer.createSprite("player_sprite", "player_tex")

-- Setup physics
Physics.addRigidBody(player, 100, 200, 32, 32, 0, true, 1.0, 0.0)

-- Game loop
function updateGame(dt)
    -- Update physics
    Physics.step(dt)
    
    -- Sync ECS components with physics
    local physicsPos = Physics.getRigidBodyPos(player)
    local transform = ECS.getTransform(player)
    transform.pos.x = physicsPos.x
    transform.pos.y = physicsPos.y
    
    -- Update sprite position
    Renderer.setSpritePosition("player_sprite", physicsPos.x, physicsPos.y)
end

function renderGame()
    Renderer.clearWindow()
    Renderer.drawSprite("player_sprite")
    Renderer.displayWindow()
end
```

## Error Handling

All binding functions include error checking:
- Functions will log errors if the underlying system is not available
- Boolean return values indicate success/failure for operations like loading
- Missing systems are handled gracefully with warning messages
