-- bird.lua

local active = false
local launch = false
local launchPower = 10000
local launchFinished = false

function lerp(a,b,t) -- a = current_value, b = target_value, t = percent max_value = 1
    return a * (1-t) + b * t
end

function onLoad()
    
end

function onTick()
    local me = me(script)
    local camera = Util.GetCamera()

    local mouse = Input.GetMousePosition()
    local scale = Util.GetPixelScale()
    local mouseWorld = Util.ScreenToWorld(mouse.x, mouse.y)

    local myPos = me.globalPosition

    local windowWidth = Util.GetWindowWidth()
    local windowHeight = Util.GetWindowHeight()

    local mouseWorldCentered = Util.WorldTopLeftToWorldCenter(mouseWorld.x, mouseWorld.y)

    if launchFinished then
        camera.offset.x = lerp(camera.offset.x, myPos.x, 0.05)
        camera.offset.y = lerp(camera.offset.y, myPos.y, 0.05)
    end


    if launch then
        launch = false

        local relativePos = Vector2f(
            mouseWorldCentered.x - myPos.x,
            mouseWorldCentered.y - myPos.y
        )

        local relativePosNormalized = relativePos:Normalize()


        info("position: " .. tostring(relativePosNormalized.x) .. " " .. tostring(relativePosNormalized.y))

        me:SetLinearVelocity(relativePosNormalized.x * launchPower, relativePosNormalized.y * launchPower)

        launchFinished = true
    end
    if not active then
        if Input.IsLeftMouseButtonDown() then
            launch = true
            active = true

            me.enabled = true
        end
    end
end


function onRender()
    
end

function onImgui()
    
end