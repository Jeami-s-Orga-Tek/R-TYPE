CollisionSystem = {}
CollisionSystem.__index = CollisionSystem

function CollisionSystem:new()
    local self = setmetatable({}, CollisionSystem)
    self.active_collisions = {}
    self.collision_count = 0
    return self
end

function CollisionSystem:rectanglesIntersect(rectA, rectB)
    return not (rectA.x + rectA.width < rectB.x or rectB.x + rectB.width < rectA.x or rectA.y + rectA.height < rectB.y or rectB.y + rectB.height < rectA.y)
end

function CollisionSystem:checkCollision(entityA, entityB)
    local hitboxA = ECS.getHitbox(entityA)
    local hitboxB = ECS.getHitbox(entityB)
    local transformA = ECS.getTransform(entityA)
    local transformB = ECS.getTransform(entityB)

    if not hitboxA or not hitboxB or not transformA or not transformB then
        print("no hb or tr")
        return false
    end
    
    if hitboxA.layer == hitboxB.layer then
        return false
    end

    if not hitboxA.active or not hitboxB.active then
        -- print("active")
        return false
    end

    local rectA = Rect(transformA.pos.x, transformA.pos.y, hitboxA.bounds.width, hitboxA.bounds.height)
    local rectB = Rect(transformB.pos.x, transformB.pos.y, hitboxB.bounds.width, hitboxB.bounds.height)
    
    -- print("to the end")
    return self:rectanglesIntersect(rectA, rectB)
end

function CollisionSystem:getCollisionPairKey(entityA, entityB)
    local first = math.min(entityA, entityB)
    local second = math.max(entityA, entityB)
    return first .. "_" .. second
end

function CollisionSystem:sendCollisionEvent(entityA, entityB)
    local hitboxA = ECS.getHitbox(entityA)
    local hitboxB = ECS.getHitbox(entityB)
    
    if hitboxA and hitboxB then
        local layerNames = {
            [0] = "PLAYER",
            [1] = "PLAYER_PROJECTILE", 
            [2] = "ENEMY",
            [3] = "ENEMY_PROJECTILE"
        }
        
        local layerNameA = layerNames[hitboxA.layer] or ("UNKNOWN("..hitboxA.layer..")")
        local layerNameB = layerNames[hitboxB.layer] or ("UNKNOWN("..hitboxB.layer..")")
        
        print(string.format("COLLISION EVENT: Entity %d (%s=%d) collided with Entity %d (%s=%d)", 
              entityA, layerNameA, hitboxA.layer, entityB, layerNameB, hitboxB.layer))
        
        local collisionEvent = Event(EventsIds.COLLISION)
        collisionEvent:setParamEntityById(0, entityA)
        collisionEvent:setParamEntityById(1, entityB)
        collisionEvent:setParamHitboxLayerById(2, hitboxA.layer)
        collisionEvent:setParamHitboxLayerById(3, hitboxB.layer)
        
        print(string.format("Sending collision event: entities %d<->%d, layers %s<->%s (%d<->%d)", entityA, entityB, layerNameA, layerNameB, hitboxA.layer, hitboxB.layer))
        
        ECS.sendEvent(collisionEvent)
    end
end

function CollisionSystem:update()
    local current_collisions = {}

    local entities = ECS.getEntitiesWithComponents({"Transform", "Hitbox"})
    
    if not entities or #entities == 0 then
        return
    end
    
    -- print(string.format("Collision system checking %d entities", #entities))

    -- print("Entities in collision system update:")
    -- for _, entity in ipairs(entities) do
    --     local hitbox = ECS.getHitbox(entity)
    --     local transform = ECS.getTransform(entity)
    --     if hitbox and transform then
    --         print(string.format("Entity %d: Layer=%d, Active=%s, Pos=(%.1f,%.1f), Size=(%.1f,%.1f)",  entity, hitbox.layer, tostring(hitbox.active), transform.pos.x, transform.pos.y, hitbox.bounds.width, hitbox.bounds.height))
    --     else
    --         print(string.format("Entity %d: Missing components (hitbox=%s, transform=%s)", entity, tostring(hitbox ~= nil), tostring(transform ~= nil)))
    --     end
    -- end

    for i = 1, #entities do
        local entityA = entities[i]
        
        for j = i + 1, #entities do
            local entityB = entities[j]
            
            if self:checkCollision(entityA, entityB) then
                local pairKey = self:getCollisionPairKey(entityA, entityB)
                current_collisions[pairKey] = true

                if not self.active_collisions[pairKey] then
                    self.collision_count = self.collision_count + 1
                    print(string.format("NEW COLLISION #%d: Entity %d <-> Entity %d", 
                          self.collision_count, entityA, entityB))
                    self:sendCollisionEvent(entityA, entityB)
                    -- return
                end
            end
        end
    end
    
    self.active_collisions = current_collisions
end

if not GlobalCollisionSystem then
    GlobalCollisionSystem = CollisionSystem:new()
    print("Lua Collision System initialized successfully !!!!!")
else
    print("Lua Collision System already initialized")
end

function updateCollisionSystem()
    if GlobalCollisionSystem then
        GlobalCollisionSystem:update()
    else
        print("Error: GlobalCollisionSystem not initialized")
    end
end

-- function getCollisionSystemStats()
--     if GlobalCollisionSystem then
--         return {
--             collision_count = GlobalCollisionSystem.collision_count,
--             active_collisions = 0
--         }
--     end
--     return nil
-- end

print("Lua Collision System loaded and ready !!!!!!!!")