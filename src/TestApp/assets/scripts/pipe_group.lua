local classdb = require("radium::classdb")
local script = require("radium::script")
local log = require("radium::log")
local rand = require("radium::random")

local gap = 128 -- Vertical gap between pipes
local screenLeft = -420 -- When pipe goes off screen
local resetX = 420 -- Reposition x value

function OnLoad()
    -- Optional
end

function OnTick(dt)
    local me = script.me(this_script)
    local speed = 150

    me.position.x = me.position.x - speed * dt

    if me.position.x < screenLeft then
        me.position.y = rand.float_between(-125, 125)
        me.position.x = resetX
    end
end

function OnRender()
end

function OnImgui()
end
