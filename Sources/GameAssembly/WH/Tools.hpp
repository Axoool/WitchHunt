// Tool.hpp
#pragma once
#include <Termina/World/Component.hpp>

namespace TerminaScript {
    class Tool : public Termina::Component {
    public:
        enum class Type { Knife, Hammer, Spoon };

        Type ToolType = Type::Knife;
    };
}