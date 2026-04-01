// Ingredient.hpp
#pragma once
#include <Termina/World/Component.hpp>

namespace TerminaScript {
    class Ingredient : public Termina::Component {
    public:
        enum class Type { Mushroom, Flower };
        enum class State { Uncut, Cut };

        Type IngredientType = Type::Mushroom;
        State CurrentState = State::Uncut;
    };
}