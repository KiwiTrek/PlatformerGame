#include "Collisions.h"

#include "App.h"

Collisions::Collisions() {
    for (uint i = 0; i < MAX_COLLIDERS; ++i) { colliders[i] = nullptr; }
    matrix[Collider::Type::SOLID][Collider::Type::NONE] = false;
    matrix[Collider::Type::SOLID][Collider::Type::SOLID] = false;
    matrix[Collider::Type::SOLID][Collider::Type::PLAYER] = false;
    matrix[Collider::Type::SOLID][Collider::Type::SPIKE] = false;
    matrix[Collider::Type::SOLID][Collider::Type::FRUIT] = false;

    matrix[Collider::Type::PLAYER][Collider::Type::NONE] = false;
    matrix[Collider::Type::PLAYER][Collider::Type::SOLID] = false;
    matrix[Collider::Type::PLAYER][Collider::Type::PLAYER] = false;
    matrix[Collider::Type::PLAYER][Collider::Type::SPIKE] = false;
    matrix[Collider::Type::PLAYER][Collider::Type::FRUIT] = false;

    matrix[Collider::Type::SPIKE][Collider::Type::NONE] = false;
    matrix[Collider::Type::SPIKE][Collider::Type::SOLID] = false;
    matrix[Collider::Type::SPIKE][Collider::Type::PLAYER] = false;
    matrix[Collider::Type::SPIKE][Collider::Type::SPIKE] = false;
    matrix[Collider::Type::SPIKE][Collider::Type::FRUIT] = false;

    matrix[Collider::Type::FRUIT][Collider::Type::NONE] = false;
    matrix[Collider::Type::FRUIT][Collider::Type::SOLID] = false;
    matrix[Collider::Type::FRUIT][Collider::Type::PLAYER] = false;
    matrix[Collider::Type::FRUIT][Collider::Type::SPIKE] = false;
    matrix[Collider::Type::FRUIT][Collider::Type::FRUIT] = false;

}

Collisions::~Collisions() {}

void Collisions::Init() {

}

bool Collisions::Start() {
    return true;
}

bool Collisions::Awake(pugi::xml_node&) {
    return true;
}

bool Collisions::PreUpdate() {
    return true;
}

bool Collisions::Update(float dt) {
    return true;
}

bool Collisions::PostUpdate() {
    return true;
}

bool Collisions::CleanUp() {
    return true;
}

bool Collisions::Load(pugi::xml_node&) {
    return true;
}

bool Collisions::Save(pugi::xml_node&) {
    return true;
}

Collider* Collisions::AddCollider(SDL_Rect rect, Collider::Type type, Module* listener) {
    Collider* ret = nullptr;
    return ret;
}

void Collider::SetPos(int _x, int _y, int _w, int _h) {
    rect.x = _x;
    rect.y = _y;
    rect.w = _w;
    rect.h = _h;
}

bool Collider::Intersects(const SDL_Rect& r) const {
    return (rect.x < r.x + r.w &&
        rect.x + rect.w > r.x &&
        rect.y < r.y + r.h &&
        rect.h + rect.y > r.y);
}