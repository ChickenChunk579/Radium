-- bird.lua

local velocity = 0
local gravity = 0.075
local topPipe
local bottomPipe

function AABB(r1, r2)
    return not (
        r1.x + r1.w < r2.x or
        r1.x > r2.x + r2.w or
        r1.y + r1.h < r2.y or
        r1.y > r2.y + r2.h
    )
end

function onLoad()
    topPipe = tree:GetNodeByPath("/PipeGroup/TopPipe");
    bottomPipe = tree:GetNodeByPath("/PipeGroup/BottomPipe");
end

function onTick()
    local me = me(script)
    velocity = velocity - gravity
    me.position.y = me.position.y + velocity

    if WasKeyPressedThisFrame(Key.SPACE) then
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

    local topPipePos = topPipe.globalPosition
    local topPipeSize = topPipe.size

    local topPipeRect = RectangleF(
        topPipePos.x - topPipeSize.x / 2,
        topPipePos.y - topPipeSize.y / 2,
        topPipeSize.x,
        topPipeSize.y
    )

    local bottomPipePos = bottomPipe.globalPosition
    local bottomPipeSize = bottomPipe.size

    local bottomPipeRect = RectangleF(
        bottomPipePos.x - bottomPipeSize.x / 2,
        bottomPipePos.y - bottomPipeSize.y / 2,
        bottomPipeSize.x,
        bottomPipeSize.y
    )

    local collisionTop = AABB(birdRect, topPipeRect)
    local collisionBottom = AABB(birdRect, bottomPipeRect)

    if collisionTop or collisionBottom then
        info("Collision detected")
        exit(0)
    end

end


function onRender()
    
end

function onImgui()
    
end