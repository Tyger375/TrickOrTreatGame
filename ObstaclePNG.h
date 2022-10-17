#ifndef OBSTACLEPNG_H
#define OBSTACLEPNG_H
#include <core/Godot.hpp>
#include <gen/Reference.hpp>
#include <gen/Node2D.hpp>
#include <gen/Sprite.hpp>
#include <core/Vector2.hpp>
#include <gen/RigidBody2D.hpp>
#include <gen/KinematicBody2D.hpp>
#include <gen/InputEvent.hpp>
#include <gen/Input.hpp>
#include <gen/Area2D.hpp>
#include <KinematicCollision2D.hpp>

#include <string>
#include <iostream>
#include <stdlib.h>

using namespace godot;

class Obstacle;

class ObstaclePNG : public Node2D
{
    GODOT_CLASS(ObstaclePNG, Node2D)

public:
    Obstacle* obj;

    ObstaclePNG()
    {
        obj = nullptr;
    }
    ~ObstaclePNG() {}

    static void _register_methods()
    {
        register_method("unlock", &ObstaclePNG::unlock);
    }

    void _init() {}

    void unlock()
    {
        Godot::print("Oh, sorry, I'll remove the obstacle which unables you to pass");
        if (obj != nullptr)
            obj->queue_free();
    }
};
#endif // !OBSTACLEPNG_H