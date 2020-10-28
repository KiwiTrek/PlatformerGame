#include "Collisions.h"

#include "App.h"

Collisions::Collisions() {
    for (uint i = 0; i < MAX_COLLIDERS; ++i) { colliders[i] = nullptr; }
    matrix[Collider::TYPE::SOLID][Collider::TYPE::NONE] = false;
    matrix[Collider::TYPE::SOLID][Collider::TYPE::SOLID] = false;
    matrix[Collider::TYPE::SOLID][Collider::TYPE::PLAYER] = false;
    matrix[Collider::TYPE::SOLID][Collider::TYPE::SPIKE] = false;
    matrix[Collider::TYPE::SOLID][Collider::TYPE::FRUIT] = false;

    matrix[Collider::TYPE::PLAYER][Collider::TYPE::NONE] = false;
    matrix[Collider::TYPE::PLAYER][Collider::TYPE::SOLID] = false;
    matrix[Collider::TYPE::PLAYER][Collider::TYPE::PLAYER] = false;
    matrix[Collider::TYPE::PLAYER][Collider::TYPE::SPIKE] = false;
    matrix[Collider::TYPE::PLAYER][Collider::TYPE::FRUIT] = false;

    matrix[Collider::TYPE::SPIKE][Collider::TYPE::NONE] = false;
    matrix[Collider::TYPE::SPIKE][Collider::TYPE::SOLID] = false;
    matrix[Collider::TYPE::SPIKE][Collider::TYPE::PLAYER] = false;
    matrix[Collider::TYPE::SPIKE][Collider::TYPE::SPIKE] = false;
    matrix[Collider::TYPE::SPIKE][Collider::TYPE::FRUIT] = false;

    matrix[Collider::TYPE::FRUIT][Collider::TYPE::NONE] = false;
    matrix[Collider::TYPE::FRUIT][Collider::TYPE::SOLID] = false;
    matrix[Collider::TYPE::FRUIT][Collider::TYPE::PLAYER] = false;
    matrix[Collider::TYPE::FRUIT][Collider::TYPE::SPIKE] = false;
    matrix[Collider::TYPE::FRUIT][Collider::TYPE::FRUIT] = false;

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

Collider* Collisions::AddCollider(SDL_Rect rect, Collider::TYPE type, Module* listener) {
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