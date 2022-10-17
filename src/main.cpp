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
#include <gen/StaticBody2D.hpp>
#include <gen/Texture.hpp>
#include <gen/TextureRect.hpp>
#include <gen/ImageTexture.hpp>
#include <KinematicCollision2D.hpp>
#include <gen/Resource.hpp>
#include <gen/ResourceLoader.hpp>
#include <gen/Timer.hpp>
#include <gen/AnimatedSprite.hpp>
#include <gen/SceneTree.hpp>
#include <gen/PackedScene.hpp>
#include <gen/Viewport.hpp>
#include <gen/Button.hpp>
#include <gen/RandomNumberGenerator.hpp>

#include <string>
#include <vector>
#include <iostream>
#include <stdlib.h>
#include <Label.hpp>

using namespace godot;

class NewSweets : public Node2D
{
    GODOT_CLASS(NewSweets, Node2D)
private:
    float counter;
    float increaser;
    Label* txtNumSweets;
    Vector2 initialPos;

public:
    static void _register_methods()
    {
        register_method("_ready", &NewSweets::_ready);
        register_method("_physics_process", &NewSweets::_physics_process);
        register_method("start", &NewSweets::start);
    }

    void _physics_process(float delta)
    {
        if (counter > 0)
        {
            set_visible(true);
            Vector2 pos = get_position();
            counter -= increaser * delta;
            pos.y -= 15 * delta;
            set_position(pos);
            Color color = get_modulate();
            color.a -= increaser * delta;
            set_modulate(color);
        }
    }

    void _init()
    {
        counter = 0;
        increaser = 1.5;
    }

    void _ready()
    {
        initialPos = get_position();
        txtNumSweets = Object::cast_to<Label>(get_child(1));
    }

    void start(int numSweets)
    {
        counter = 10;
        String text;

        if (numSweets > 0)
        {
            text += "+ ";
        }
        else
        {
            text += "- ";
        }

        text += std::to_string(abs(numSweets)).c_str();

        txtNumSweets->set_text(text);
        Color color = get_modulate();
        color.a = 1;
        set_modulate(color);
        set_position(initialPos);
        set_visible(false);
    }
};



class ObstaclePNG : public Node2D
{
    GODOT_CLASS(ObstaclePNG, Node2D)

public:
    Node2D* obj;
    bool already;

    static void _register_methods()
    {
        register_method("unlock", &ObstaclePNG::unlock);
    }

    void _init() {
        already = false;
    }

    void unlock()
    {
        if (obj != nullptr)
        {
            obj->queue_free();
            already = true;
        }
    }
};

class Options : public Node2D
{
    GODOT_CLASS(Options, Node2D)

private:
    Array items;
    Color selectedColor = Color(0.55, 0.55, 0.55, 1);
    Color defColor = Color(0.937255, 0.937255, 0.937255, 1);
    int index;
    int max;
public:
    static void _register_methods()
    {
        register_method("_ready", &Options::_ready);
        register_method("setup", &Options::setup);
        register_method("set_selected", &Options::set_selected);
        register_method("get_index", &Options::get_index);
        register_method("get_max", &Options::get_max);
    }

    void _init()
    {
        index = 0;
        max = 0;
    }

    void _ready()
    {
        items = get_children();
    }

    void setup(Array texts)
    {
        const int size = texts.size();
        max = size;
        for (int i = 0; i < size; i++)
        {
            Node2D* item = Object::cast_to<Node2D>(items[i]);
            Sprite* sprite = Object::cast_to<Sprite>(item->get_child(0));
            Label* label = Object::cast_to<Label>(item->get_child(1));
            
            item->set_visible(true);
            label->set_text(texts[i]);
            sprite->set_modulate(defColor);
        }

        for (int i = size; i < items.size(); i++)
        {
            Node2D* item = Object::cast_to<Node2D>(items[i]);
            item->set_visible(false);
        }
    }

    void set_selected(int newIndex)
    {
        index = newIndex + 1;
        for (int i = 0; i < items.size(); i++)
        {
            Node2D* item = Object::cast_to<Node2D>(items[i]);
            Sprite* sprite = Object::cast_to<Sprite>(item->get_child(0));
            if (i == newIndex)
                sprite->set_modulate(selectedColor);
            else
                sprite->set_modulate(defColor);
        }
    }

    int get_index() { return index; }
    int get_max() { return max; }
    Node2D* get_selected()
    {
        return Object::cast_to<Node2D>(items[index-1]);
    }
};

class GameObject : public Node2D
{
    GODOT_CLASS(GameObject, Node2D)
private:
    bool isDialog;
    int sweets;
    bool isTrick;
    bool isShop;
    bool isOptions;
    Area2D* door;
    AnimatedSprite* animSprite;
    Label* dialogLabel;
    Label* sweetsCounter;
    Label* txtTimer;
    Options* options;
    KinematicBody2D* player;
    Node2D* menu;
    Label* gameEnded;
    Label* stats;
    Timer* timer;
    NewSweets* newSweets;
    std::vector<String> items = {
        String("+ speed 7"),
        String("+ sweets 10"),
        String("+ time 5")
    };
    bool alreadyEnded;
    int minSweets = 5;
    int maxSweets = 10;

    template <typename T> bool find(std::vector<T> vector, T item)
    {
        bool found = false;
        for (T i : vector)
        {
            if (i == item)
            {
                found = true;
                break;
            }
        }
        return found;
    }

    int randint(int min, int max)
    {
        return min + (rand() % (max - min + 1));
    }

    void add_sweets()
    {
        RandomNumberGenerator* rng = RandomNumberGenerator::_new();
        rng->randomize();
        int rand = rng->randi_range(minSweets, maxSweets);
        newSweets->start(rand);
        sweets += rand;
    }
    void sub_sweets()
    {
        RandomNumberGenerator* rng = RandomNumberGenerator::_new();
        rng->randomize();
        int rand = rng->randi_range(5, 10);
        newSweets->start(-rand);
        if (rand > sweets)
            sweets = 0;
        else
            sweets -= rand;
    }

    void showDialog(std::string text)
    {
        isDialog = true;
        Node2D* parent = Object::cast_to<Node2D>(dialogLabel->get_parent());
        parent->set_visible(true);
        dialogLabel->set_text(String(text.c_str()));
    }

    void showOptions(std::string type)
    {
        isOptions = true;
        Node2D* parent = Object::cast_to<Node2D>(options->get_parent());
        parent->set_visible(true);
        if (type == "shop")
        {
            Array list{};
            list.push_back("+ speed 7");
            list.push_back("+ sweets 10");
            list.push_back("+ time 5");
            options->setup(list);
        }
        else if (type == "trick")
        {
            Array list{};
            list.push_back("trick");
            list.push_back("nothing");
            options->setup(list);
        }
    }

    void hideOptions()
    {
        Node2D* parent = Object::cast_to<Node2D>(options->get_parent());
        parent->set_visible(false);
        isOptions = false;
    }

    void hideDialog()
    {
        Node2D* parent = Object::cast_to<Node2D>(dialogLabel->get_parent());
        parent->set_visible(false);
        isDialog = false;
    }

    void buy(String txt)
    {
        int price;
        if (txt == "+ sweets 10")
        {
            price = 10;
            if (sweets < price)
            {
                showDialog("Too expensive");
                return;
            }

            maxSweets += 5;
            sweets -= price;
        }
        else if (txt == "+ time 5")
        {
            price = 5;
            if (sweets < price)
            {
                showDialog("Too expensive");
                return;
            }

            timer->set_wait_time(timer->get_time_left() + 30);
            timer->start();
            sweets -= price;
        }
        else if (txt == "+ speed 7")
        {
            price = 7;
            if (sweets < price)
            {
                showDialog("Too expensive");
                return;
            }

            int speed = player->call("getSpeed");
            player->call("setSpeed", speed + 25);
            sweets -= price;
        }
    }

    String formatTime(int time)
    {
        int minutes = (int)(time / 60);
        int seconds = (int)(time % 60);
        String formatted;
        formatted += std::to_string(minutes).c_str();
        formatted += ":";
        if (seconds < 10)
            formatted += "0";
        formatted += std::to_string(seconds).c_str();
        return formatted;
    }
public:
    static void _register_methods()
    {
        register_method("_input", &GameObject::_input);
        register_method("_ready", &GameObject::_ready);
        register_method("_process", &GameObject::_process);
        register_method("_timeout", &GameObject::_timeout);
        register_method("wait_end_animation", &GameObject::wait_end_animation);
        register_method("play_toggled", &GameObject::play_toggled);
    }

    void _process(real_t delta)
    {
        sweetsCounter->set_text(String(std::to_string(sweets).c_str()));
        if (timer->get_time_left() <= 0 && !alreadyEnded)
        {
            alreadyEnded = true;
            _timeout();
        }
        txtTimer->set_text(formatTime((int)timer->get_time_left()));
    }

    void play_toggled(bool pressed)
    {
        start_game();
    }

    void _timeout()
    {
        hideDialog();
        hideOptions();
        txtTimer->set_visible(false);
        sweetsCounter->set_visible(false);
        isDialog = true;
        menu->set_visible(true);

        gameEnded->set_visible(true);
        stats->set_visible(true);
        String msg = "You collected ";
        msg += (std::to_string(sweets).c_str());
        msg += " sweets!";
        stats->set_text(msg);
    }

    void start_game()
    {
        get_tree()->change_scene("res://Main.tscn");
    }

    void _ready()
    {
        dialogLabel = get_node<Label>(NodePath("/root/Main/CanvasLayer/Dialog/Label"));
        options = get_node<Options>(NodePath("/root/Main/CanvasLayer/Options/Items"));
        sweetsCounter = get_node<Label>(NodePath("/root/Main/CanvasLayer/SweetsCounter"));
        player = get_node<KinematicBody2D>(NodePath("/root/Main/PlayerNode/KinematicBody2D"));
        timer = get_node<Timer>(NodePath("/root/Main/CanvasLayer/Timer"));
        txtTimer = get_node<Label>(NodePath("/root/Main/CanvasLayer/TxtTimer"));
        newSweets = get_node<NewSweets>(NodePath("/root/Main/CanvasLayer/NewSweets"));

        menu = get_node<Node2D>(NodePath("/root/Main/CanvasLayer/Menu"));
        Button* btnPlay = get_node<Button>(NodePath("/root/Main/CanvasLayer/Menu/BtnPlay/Play"));
        gameEnded = Object::cast_to<Label>(menu->get_child(2));
        stats = Object::cast_to<Label>(menu->get_child(3));
        btnPlay->connect("toggled", this, "play_toggled");

        timer->set_wait_time(2*60);
        timer->start();

        Node2D* NodeObstacles = get_node<Node2D>(NodePath("/root/Main/Obstacles"));
        Array obstacles = NodeObstacles->get_children();

        Node2D* ObstaclePNGs = get_node<Node2D>(NodePath("/root/Main/PNGs/ObstaclePNGs"));
        Array children = ObstaclePNGs->get_children();

        RandomNumberGenerator* rng = RandomNumberGenerator::_new();
        rng->randomize();

        for (int i = 0; i < obstacles.size(); i++)
        {
            Node2D* obstacle = Object::cast_to<Node2D>(obstacles[i]);
            int random = rng->randi_range(0, children.size() - 1);
            int count = 0;
            do
            {
                ObstaclePNG* png = Object::cast_to<ObstaclePNG>(children[random]);
                if (png != nullptr)
                {
                    if (!png->is_visible())
                    {
                        png->set_visible(true);
                        png->obj = obstacle;
                        break;
                    }
                }
                count++;
            } while (count < 50);
        }

        for (int i = 0; i < children.size(); i++)
        {
            ObstaclePNG* png = Object::cast_to<ObstaclePNG>(children[i]);
            if (png->obj == nullptr)
                png->get_child(0)->queue_free();
        }
    }

    inline int getSweets() { return this->sweets; }

    void _init()
    {
        isDialog = false;
        isOptions = false;
        isTrick = false;
        isShop = false;
        sweets = 0;
        alreadyEnded = false;
        srand(time(0));
    }

    void _input(InputEvent* inputEvent)
    {
        if (isDialog)
        {
            if (inputEvent->is_action_pressed("ui_accept"))
            {
                hideDialog();
            }
        }
        if (isOptions)
        {
            if (inputEvent->is_action_pressed("ui_cancel"))
            {
                hideOptions();
                hideDialog();
            }
            if (inputEvent->is_action_pressed("ui_accept"))
            {
                if (isTrick)
                {
                    Node2D* selected = options->get_selected();
                    Label* text = Object::cast_to<Label>(selected->get_child(1));

                    if (text->get_text() == "trick")
                        accept_trick();
                    else
                        decline_trick();
                    startTimerAnim();
                }
                else if (isShop)
                {
                    Node2D* selected = options->get_selected();
                    Label* text = Object::cast_to<Label>(selected->get_child(1));

                    const String txt = text->get_text();
                    if (find(items, String(txt)))
                    {
                        buy(txt);
                    }
                    isShop = false;
                }
                hideOptions();
            }
            if (inputEvent->is_action_pressed("ui_up"))
            {
                int index = options->get_index();
                index -= 1;
                if (index <= 0)
                {
                    index = options->get_max();
                }

                options->set_selected(index-1);
            }
            else if (inputEvent->is_action_pressed("ui_down"))
            {
                int index = options->get_index();
                index += 1;
                if (index > options->get_max())
                {
                    index = 1;
                }

                options->set_selected(index-1);
            }
        }
    }

    void wait_end_animation()
    {
        if (animSprite != nullptr)
        {
            animSprite->set_animation("default");
            get_child(0)->queue_free();
        }
    }

    void start_trick_or_treat(Area2D* obj)
    {
        door = obj;
        animSprite = Object::cast_to<AnimatedSprite>(door->get_parent()->get_child(3));
        animSprite->set_animation("opened");
        isDialog = true;
        RandomNumberGenerator* rng = RandomNumberGenerator::_new();
        rng->randomize();
        int trick_or_treat = rng->randi_range(0, 1);
        if (trick_or_treat == 0)
        {
            add_sweets();
            isDialog = false;
            if (door != nullptr)
            {
                door->queue_free();
                door = nullptr;
            }

            startTimerAnim();
        }
        else
        {
            isTrick = true;
            showDialog("The guy chose trick\nReady to frighten?");
            showOptions("trick");
        }
    }
    void openShop()
    {
        isDialog = true;
        isShop = true;
        showDialog("Shop\nBuy things for sweets");
        showOptions("shop");
    }

    void accept_trick()
    {
        int random = randint(0, 1);
        if (random == 0)
        {
            door->get_parent()->get_child(2)->call("start_anim");
            add_sweets();
        }
        else
        {
            door->get_parent()->get_child(2)->call("start_bored");
            sub_sweets();
        }
        isTrick = false;
        isDialog = false;
        if (door != nullptr)
        {
            door->queue_free();
            door = nullptr;
        }
    }

    void startTimerAnim()
    {
        Timer* timer = Timer::_new();
        if (timer != nullptr)
        {
            timer->set_name("TIMER_CREATED_ON_DYNAMICALLY_SCENE");
            timer->set_wait_time(1);
            timer->set_one_shot(true);
            add_child(timer);
            timer->connect("timeout", this, "wait_end_animation");
            timer->start();
        }
    }

    void decline_trick()
    {
        isDialog = false;
        isTrick = false;
        if (door != nullptr)
        {
            door->queue_free();
            door = nullptr;
        }
    }

    void unlocking()
    {
        showDialog("Oh, sorry, I'll remove the obstacle\n which unables you to pass");
    }

    inline bool is_dialog() { return this->isDialog; }
};

class Player : public KinematicBody2D {
    GODOT_CLASS(Player, KinematicBody2D)

private:
    float speed;
    Input* _keyboardInput;
    GameObject* gameObject;
    AnimatedSprite* sprite;
    Vector2 vector;
    bool already;
public:
    static void _register_methods()
    {
        register_method("_physics_process", &Player::_physics_process);
        register_method("_ready", &Player::_ready);
        //register_method("_input", &Player::_input);
        register_method("area_entered", &Player::_area_entered);
        register_method("body_entered", &Player::_body_entered);
        register_method("setSpeed", &Player::setSpeed);
        register_method("getSpeed", &Player::getSpeed);
        register_property<Player, float>("speed", &Player::setSpeed, &Player::getSpeed, 1);
    }

    inline void setSpeed(float newS)
    {
        speed = newS;
    }

    inline float getSpeed() { return speed; }

    void _body_entered(Variant body)
    {
        StaticBody2D* obj = Object::cast_to<StaticBody2D>(body);
        if (obj != nullptr)
        {
            //obj->unlock();
            ObstaclePNG* png = Object::cast_to<ObstaclePNG>(obj->get_parent());
            if (png != nullptr && !png->already)
            {
                png->unlock();
                gameObject->unlocking();
            }
        }
    }

    void _area_entered(Variant body)
    {
        Area2D* obj = Object::cast_to<Area2D>(body);
        if (obj->get_name() == "DoorArea")
        {
            gameObject->start_trick_or_treat(obj);
        }
        else if (obj->get_name() == "MarketArea")
        {
            gameObject->openShop();
        }
    }

    void _init()
    {
        speed = 1.f;
        already = false;
    }

    void _ready()
    {
        _keyboardInput = godot::Input::get_singleton();
        Node* child = get_child(0);
        child->connect("area_entered", this, "area_entered");
        child->connect("body_entered", this, "body_entered");
        sprite = Object::cast_to<AnimatedSprite>(get_child(2));

        gameObject = (get_node<GameObject>(NodePath("/root/Main/GameObject")));
    }

    void _physics_process(float delta)
    {
        if (!gameObject->is_dialog())
        {
            if (_keyboardInput->is_action_pressed("ui_right"))
            {
                sprite->set_animation("MovementRight");
                vector = Vector2(speed * delta, 0);
                move_and_collide(vector);
            }
            else if (_keyboardInput->is_action_pressed("ui_left"))
            {
                sprite->set_animation("MovementLeft");
                vector = Vector2(-(speed * delta), 0);
                move_and_collide(vector);
            }
            else if (_keyboardInput->is_action_pressed("ui_up"))
            {
                sprite->set_animation("MovementUp");
                vector = Vector2(0, -(speed * delta));
                move_and_collide(vector);
            }
            else if (_keyboardInput->is_action_pressed("ui_down"))
            {
                sprite->set_animation("MovementDown");
                vector = Vector2(0, (speed * delta));
                move_and_collide(vector);
            }
            else
                idle();
        }
        else
            idle();
    }

    void idle()
    {
        if (vector.x > 0)
        {
            sprite->set_animation("defaultright");
        }
        else if (vector.x < 0)
        {
            sprite->set_animation("defaultleft");
        }
        else if (vector.y > 0)
        {
            sprite->set_animation("defaultdown");
        }
        else if (vector.y < 0)
        {
            sprite->set_animation("defaultup");
        }
    }
};

/** GDNative Initialize **/
extern "C" void GDN_EXPORT godot_gdnative_init(godot_gdnative_init_options * o)
{
    Godot::gdnative_init(o);
}

/** GDNative Terminate **/
extern "C" void GDN_EXPORT godot_gdnative_terminate(godot_gdnative_terminate_options * o)
{
    Godot::gdnative_terminate(o);
}

/** NativeScript Initialize **/
extern "C" void GDN_EXPORT godot_nativescript_init(void* handle)
{
    Godot::nativescript_init(handle);

    register_class<Player>();
    register_class<GameObject>();
    register_class<ObstaclePNG>();
    register_class<Options>();
    register_class<NewSweets>();
}