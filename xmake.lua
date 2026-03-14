-- xmake.lua

add_rules("mode.debug", "mode.release", "mode.releasedbg")

-- Basics
set_languages("c++20", "c99")
set_rundir(".")
add_includedirs("Sources", "Sources/ThirdParty", { public = true })

-- Platform defines
if is_plat("windows") then
    add_defines("TRMN_WINDOWS", { public = true })
elseif is_plat("linux") then
    add_defines("TRMN_LINUX", { public = true })
    add_rpathdirs("Binaries/Linux", "$(builddir)/$(plat)/$(arch)/$(mode)/", { public = true })
    add_cflags("-fPIC", { public = true })
    add_cxxflags("-fPIC", { public = true })
elseif is_plat("macosx") then
    add_defines("TRMN_MACOS", { public = true })
    add_cflags("-x objective-c", "-fno-objc-arc", { public = true })
    add_cxxflags("-x objective-c++", "-fno-objc-arc", { public = true })
    add_mflags("-fno-objc-arc", { public = true })
    add_rpathdirs("Binaries/Mac", "$(builddir)/$(plat)/$(arch)/$(mode)/", { public = true })
    add_links("Binaries/Mac/libmetalirconverter.dylib", { public = true })
end

-- Other defines
add_defines("GLM_FORCE_DEPTH_ZERO_TO_ONE", "VK_NO_PROTOTYPES", "GLM_ENABLE_EXPERIMENTAL", "JPH_DEBUG_RENDERER", { public = true })


includes("Sources")
