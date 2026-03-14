-- xmake.lua

target("Jolt")
    set_kind("static")
    add_files("**.cpp")
    add_headerfiles("**.hpp")
    set_group("Third Party")
    add_defines("JPH_DEBUG_RENDERER", {public = true})

    if is_mode("debug") then
        set_symbols("debug")
        set_optimize("none")
    elseif is_mode("releasedbg") then
        set_symbols("debug")
        set_optimize("fastest")
    else
        set_symbols("hidden")
        set_optimize("fastest")
        set_strip("all")
    end
