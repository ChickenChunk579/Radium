-- bird.lua
local velocity = 0
local gravity = 0.075
local pipeGroups = {}
local score = 0

function AABB(r1, r2)
    return not (
        r1.x + r1.w < r2.x or
        r1.x > r2.x + r2.w or
        r1.y + r1.h < r2.y or
        r1.y > r2.y + r2.h
    )
end

function onLoad()
    -- Load all three pipe groups
    table.insert(pipeGroups, {
        top = tree:GetNodeByPath("/PipeGroup/TopPipe"),
        bottom = tree:GetNodeByPath("/PipeGroup/BottomPipe"),
        scored = false
    })
    table.insert(pipeGroups, {
        top = tree:GetNodeByPath("/PipeGroup2/TopPipe"),
        bottom = tree:GetNodeByPath("/PipeGroup2/BottomPipe"),
        scored = false
    })
    table.insert(pipeGroups, {
        top = tree:GetNodeByPath("/PipeGroup3/TopPipe"),
        bottom = tree:GetNodeByPath("/PipeGroup3/BottomPipe"),
        scored = false
    })
end

function onTick()
    local me = me(script)
    velocity = velocity - gravity
    me.position.y = me.position.y + velocity
    
    if Input.WasKeyPressedThisFrame(Key.SPACE) then
        velocity = 2.5
    end
    
    local birdPos = me.globalPosition
    local birdSize = me.size
    
    local birdRect = RectangleF(
        birdPos.x - birdSize.x / 2,
        birdPos.y - birdSize.y / 2,
        birdSize.x,
        birdSize.y
    )
    
    Debug.AddRect(birdRect.x, birdRect.y, birdRect.w, birdRect.h, 1, 1, 1, 5, 1)
    
    -- Check collision with all pipe groups
    for i, pipeGroup in ipairs(pipeGroups) do
        local topPipe = pipeGroup.top
        local bottomPipe = pipeGroup.bottom
        
        local topPipePos = topPipe.globalPosition
        local topPipeSize = topPipe.size
        local topPipeRect = RectangleF(
            topPipePos.x - topPipeSize.x / 2,
            topPipePos.y - topPipeSize.y / 2,
            topPipeSize.x,
            topPipeSize.y
        )
        
        Debug.AddRect(topPipeRect.x, topPipeRect.y, topPipeRect.w, topPipeRect.h, 1, 0, 0, 5, 1)
        
        local bottomPipePos = bottomPipe.globalPosition
        local bottomPipeSize = bottomPipe.size
        local bottomPipeRect = RectangleF(
            bottomPipePos.x - bottomPipeSize.x / 2,
            bottomPipePos.y - bottomPipeSize.y / 2,
            bottomPipeSize.x,
            bottomPipeSize.y
        )
        
        Debug.AddRect(bottomPipeRect.x, bottomPipeRect.y, bottomPipeRect.w, bottomPipeRect.h, 1, 0, 0, 5, 1)
        
        local collisionTop = AABB(birdRect, topPipeRect)
        local collisionBottom = AABB(birdRect, bottomPipeRect)
        
        if collisionTop or collisionBottom then
            info("Collision detected with PipeGroup" .. (i == 1 and "" or tostring(i)))
            exit(0)
        end

        if birdPos.x > bottomPipePos.x then
            if not pipeGroup.scored then
                score = score + 1
                
                pipeGroup.scored = true
            end
        else
            pipeGroup.scored = false
        end
    end

    Debug.AddString("Radium Engine - Flappy Bird", 0, -32, 1, 1, 1, 3)
    Debug.AddString("Score: " .. tostring(score), 0, -64, 1, 1, 1, 3)
end

function onRender()
end

function onImgui()
end