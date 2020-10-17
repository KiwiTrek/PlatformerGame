#include "Player.h"

#include "App.h"

#include "Textures.h"
#include "Input.h"
#include "Audio.h"
#include "Render.h"
#include "Scene.h"


Player::Player() {

}

Player::~Player() {}

bool Player::Start() {

    return true;
}

bool Player::PreUpdate() {

    return true;
}

bool Player::Update(float dt) {

    return true;
}

bool Player::PostUpdate() {

    return true;
}

bool Player::CleanUp() {

    return true;
}

bool Player::Load(pugi::xml_node&) {

    return true;
}

bool Player::Save(pugi::xml_node&) {

    return true;
}