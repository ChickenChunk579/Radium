-- pipe.lua

function onLoad()
    info("Lua script loaded!")
end

function onTick()
    local me = me(script)

    me.position.x = me.position.x - 3

    if me.position.x < -500 then
        me.position.x = 500
        me.position.y = randf_between(-100, 100)
    end
end


function onRender()
    
end

function onImgui()
    
end