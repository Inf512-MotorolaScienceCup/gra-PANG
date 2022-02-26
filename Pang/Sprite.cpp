#include "Sprite.h"
#include "Game.h"
#include "raylib.h"

Texture2D Enemy::spriteSheet[NUM_KINDS];
Texture2D Enemy::spriteExplode[NUM_KINDS];

bool IsCollision(const Sprite *s1, const Sprite *s2) {
    bool collision = false;

    if (s1->state == Sprite::State::ACTIVE && s2->state == Sprite::State::ACTIVE) {
        if (s1->position.type == Position::Type::RECTANGLE) {
            if (s2->position.type == Position::Type::RECTANGLE) {
                collision = CheckCollisionRecs(s1->position.hitBox(), s2->position.hitBox());
            }
            else {
                collision = CheckCollisionCircleRec(s2->position.center, s2->position.radius, s1->position.hitBox());
            }
        }
        else {
            if (s2->position.type == Position::Type::RECTANGLE) {
                collision = CheckCollisionCircleRec(s1->position.center, s1->position.radius, s2->position.hitBox());
            }
            else {
                collision = CheckCollisionCircles(s1->position.center, s1->position.radius, s2->position.center, s2->position.radius);
            }
        }
  }
  return collision;
}

// Sprite
Sprite::Sprite(Game *game, const Position &position, Type type)
    : game(game), position(position), type(type), state(State::ACTIVE) {
}

// Player
Player::Player(Game *game, Position position)
    : Sprite(game, Position(position), Type::PLAYER), speed({ 0, 0 }) {
    spriteSheet = game->textures[PLAYER];
    moveRightRec = { 0.0f, 11 * 64.0f, 64.0f, 64.0f };
    moveLeftRec = { 0.0f, 9 * 64.0f, 64.0f, 64.0f };
    standRec = { 0.0f, 2 * 64.0f, 64.0f, 64.0f };
    moveUpRec = { 0.0f, 8 * 64.0f, 64.0f, 64.0f };
    hurtRec = { 0.0f, 20 * 64.0f, 64.0f, 64.0f };
}

Player::Player(Game *game, std::ifstream& s)
    : Sprite(game, Position(s), Type::PLAYER) {
    Read(s, &speed.x);
    Read(s, &speed.y);
    Read(s, &climbing);
    Read(s, &onIce);
    Read(s, &frameCounter);
    spriteSheet = game->textures[PLAYER];
    moveRightRec = { 0.0f, 11 * 64.0f, 64.0f, 64.0f };
    moveLeftRec = { 0.0f, 9 * 64.0f, 64.0f, 64.0f };
    standRec = { 0.0f, 2 * 64.0f, 64.0f, 64.0f };
    moveUpRec = { 0.0f, 8 * 64.0f, 64.0f, 64.0f };
    hurtRec = { 0.0f, 20 * 64.0f, 64.0f, 64.0f };
}

void Player::Draw() {
    //DrawRectangleRec(position.rectangle, GREEN);
    //DrawRectangleRec(position.hitBox(), RED);

    if (frameCounter) {
        if (timeLeft()) {
            color = WHITE;
            changeColor = 0;
        } else {
            if (changeColor <= 5)
                color = DARKGRAY;
            else
                color = WHITE;
            changeColor++;
            if (changeColor >= 10)
                changeColor = 0;
        }
    }

    if (state == State::FINISHING) {
        if (hurtRec.x < 64.0 * 5)
            DrawTextureRec(spriteSheet, hurtRec, {position.rectangle.x, position.rectangle.y}, color);
        if (hurtRec.x == 64.0 * 5 && cooldown % 5 == 0)
            DrawTextureRec(spriteSheet, hurtRec, {position.rectangle.x, position.rectangle.y}, color);

        if (cooldown % 5 == 0) {
            if (hurtRec.x < 64.0 * 5)
                hurtRec.x += 64;
        }
        cooldown++;
    }
    if (direction == Direction::RIGHT) {
        DrawTextureRec(spriteSheet, moveRightRec, { position.rectangle.x, position.rectangle.y }, color);
        if (cooldown % 5 == 0) {
            moveRightRec.x = (moveRightRec.x < 64.0 * 8) ? moveRightRec.x + 64 : 0;
        }
        cooldown++;
    } else if (direction == Direction::LEFT) {
        DrawTextureRec(spriteSheet, moveLeftRec, { position.rectangle.x, position.rectangle.y }, color);
        if (cooldown % 5 == 0) {
            moveLeftRec.x = (moveLeftRec.x < 64.0 * 8) ? moveLeftRec.x + 64 : 0;
        }
        cooldown++;
    } else if (direction == Direction::UP || direction == Direction::DOWN) {
        DrawTextureRec(spriteSheet, moveUpRec, {position.rectangle.x, position.rectangle.y}, color);
        if (cooldown % 5 == 0) {
            moveUpRec.x = (moveUpRec.x < 64.0 * 8) ? moveUpRec.x + 64 : 0;
        }
        cooldown++;
    } else if (climbing) {
        DrawTextureRec(spriteSheet, moveUpRec, { position.rectangle.x, position.rectangle.y }, color);
    } else {
        DrawTextureRec(spriteSheet, standRec, {position.rectangle.x, position.rectangle.y}, color);
        cooldown = 0;
    }
}

void Player::Move() {
    if (state == State::FINISHED) return;

    if (!climbing)
        speed.y++;
    else {
        speed.y = 0;
        speed.x = 0;
    }

    if (climbing) {
        if (IsKeyDown(KEY_UP)) {
            direction = Direction::UP;
            speed.y = -5;
        } else if (IsKeyDown(KEY_DOWN)) {
            direction = Direction::DOWN;
            speed.y = 5;
        } else {
            direction = Direction::NONE;
        }
    } else {
        if (!onIce) {
            if (IsKeyDown(KEY_LEFT)) {
                direction = Direction::LEFT;
                if (speed.x > -6.0f * game->speedBoost)
                    speed.x -= 1;
                else
                    speed.x = -6.0f * game->speedBoost;

                if (!IsSoundPlaying(game->audio[WALK]))
                    PlaySound(game->audio[WALK]);
            } else if (IsKeyDown(KEY_RIGHT)) {
                direction = Direction::RIGHT;
                if (speed.x < 6.0f * game->speedBoost)
                    speed.x += 1;
                else
                    speed.x = 6.0f * game->speedBoost;

                if (!IsSoundPlaying(game->audio[WALK]))
                    PlaySound(game->audio[WALK]);
            } else {
                direction = Direction::NONE;
                speed.x = 0;
                if (IsSoundPlaying(game->audio[WALK]))
                    StopSound(game->audio[WALK]);
            }
        } else {
            if (IsKeyDown(KEY_LEFT)) {
                direction = Direction::LEFT;
                speed.x -= 0.25f * game->speedBoost;
                if (!IsSoundPlaying(game->audio[WALK]))
                    PlaySound(game->audio[WALK]);
            } else if (IsKeyDown(KEY_RIGHT)) {
                direction = Direction::RIGHT;
                speed.x += 0.25f * game->speedBoost;
                if (!IsSoundPlaying(game->audio[WALK]))
                    PlaySound(game->audio[WALK]);
            } else {
                direction = Direction::NONE;
                if (speed.x > 0.2)
                    speed.x -= 0.2f;
                else if (speed.x < -0.2) 
                    speed.x += 0.2f;
                else {
                    speed.x = 0;
                    if (IsSoundPlaying(game->audio[WALK]))
                        StopSound(game->audio[WALK]);
                }
            }
        }

    }

    if (!climbing && IsKeyPressed(KEY_SPACE)) {
        Shooting();
    }

    position.rectangle.x += speed.x;
    position.rectangle.y += speed.y;
    onIce = false;
}

void Player::Collision(Sprite *sprite) {
    if (sprite->type == Sprite::Type::BLOCK && !climbing) {
        collision = CheckCollisionRecs({ position.rectangle.x + position.hbRectangle.x, position.rectangle.y + position.hbRectangle.y - speed.y, position.hbRectangle.width, position.hbRectangle.height }, sprite->position.rectangle);
        if (collision) {
            if (speed.x > 0) 
                position.rectangle.x = sprite->position.rectangle.x - position.hbRectangle.width - position.hbRectangle.x;
            else if (speed.x < 0) 
                position.rectangle.x = sprite->position.rectangle.x + sprite->position.rectangle.width - position.hbRectangle.x;

            speed.x = 0;
        } else {
            if (speed.y > 0)
                position.rectangle.y = sprite->position.rectangle.y - position.rectangle.height;
            else if (speed.y < 0)
                position.rectangle.y = sprite->position.rectangle.y + sprite->position.rectangle.height;

            speed.y = 0;
        }
    } else if (sprite->type == Sprite::Type::ENEMY || sprite->type == Sprite::Type::CRAB) {
        game->lives--;
        frameCounter = 1;
        speed.y -= 10;
        PlaySound(game->audio[HEALTH_LOSE]);
    } else if (sprite->type == Sprite::Type::LADDER) {
        if (!climbing) {
            bool canClimbUp = position.rectangle.y < sprite->position.rectangle.y + sprite->position.rectangle.height && position.rectangle.y + position.rectangle.height > sprite->position.rectangle.y + sprite->position.rectangle.height;
            bool canClimbDown = position.rectangle.y < sprite->position.rectangle.y && position.rectangle.y + position.rectangle.height > sprite->position.rectangle.y;

            if (IsKeyDown(KEY_UP) && canClimbUp && !IsKeyDown(KEY_DOWN))
                climbing = true;
            else if (IsKeyDown(KEY_DOWN) && canClimbDown && !IsKeyDown(KEY_UP))
                climbing = true;

        } else {
            Ladder* ladder = static_cast<Ladder*>(sprite);
            position.rectangle.x = sprite->position.rectangle.x + (sprite->position.rectangle.width / 2) - (position.rectangle.width / 2);
            bool canStopClimbingUp = position.rectangle.y + position.rectangle.height < sprite->position.rectangle.y + 10;
            bool canStopClimbingDown = position.rectangle.y > sprite->position.rectangle.y + sprite->position.rectangle.height - ladder->distanceToGround;
            if (canStopClimbingDown || canStopClimbingUp)
                climbing = false;
        }
    } else if (sprite->type == Sprite::Type::ICE) {
        onIce = true;
    }
}

void Player::Save(std::ofstream& s) {
    int t = static_cast<int>(type);
    Write(s, &t);
    position.Save(s);
    Write(s, &speed.x);
    Write(s, &speed.y);
    Write(s, &climbing);
    Write(s, &onIce);
    Write(s, &frameCounter);
}

void Player::Shooting() {
    switch (game->weaponType) {
    case 2:
        game->AddWeapon(position.rectangle.x + position.rectangle.width / 2, position.rectangle.y, game->weaponType);
        break;
    case 4:
        if (speed.y < 3) {
            game->AddWeapon(position.rectangle.x, position.rectangle.y + position.rectangle.height, game->weaponType);
            game->shootingLeft--;
            if (game->shootingLeft <= 0)
                game->weaponType = game->previousWeapon;
        }
        break;
    default:
        if (speed.y < 3 && (game->shootingLeft == 0 || game->shootingLeft == -game->multiWeapon)) {
            game->AddWeapon(position.rectangle.x + position.rectangle.width / 2, position.rectangle.y + position.rectangle.height, game->weaponType);
            game->shootingLeft--;
        }
        break;
    }
}

bool Player::timeLeft() {
    if (frameCounter > 60 * 4) {
        frameCounter = 0;
        return true;
    }
    frameCounter++;
    return false;
}

//Block
Block::Block(Game *game, float x, float y, float width, float height, Kind kind)
    : Sprite(game, Position(x, y, width, height), Sprite::Type::BLOCK), kind(kind) {
    switch (kind) {
    case Kind::PLATFORM_1:
        texture = &game->textures[PLATFORM_1];
        break;
    case Kind::PLATFORM_2:
        texture = &game->textures[PLATFORM_2];
        break;
    default:
        texture = &game->textures[WALL];
    }
}

Block::Block(Game* game, std::ifstream& s)
    : Sprite(game, Position(s), Sprite::Type::BLOCK) {
    int k;
    Read(s, &k);
    kind = static_cast<Block::Kind>(k);

    switch (kind) {
    case Kind::PLATFORM_1:
        texture = &game->textures[PLATFORM_1];
        break;
    case Kind::PLATFORM_2:
        texture = &game->textures[PLATFORM_2];
        break;
    default:
        texture = &game->textures[WALL];
    }
}

void Block::Draw() {
    // DrawRectangleRec(position.rectangle, color);    
    DrawTextureTiled(*texture, { 0, 0, 20, 20 }, position.rectangle, { 0, 0 }, 0, 1, WHITE);
}

void Block::Collision(Sprite* sprite) {
    if (kind == Kind::PLATFORM_2) {
        health--;
        if (health == 0) {
            state = State::FINISHED;
            PlaySound(game->audio[WALL_DESTRACTION]);
        }
    }
}

void Block::Save(std::ofstream& s) {
    int t = static_cast<int>(type);
    Write(s, &t);
    position.Save(s);
    int k = static_cast<int>(kind);
    Write(s, &k);
}

// Ball
Enemy* Enemy::create(Game* game, float x, float y, Kind kind, int heading) {
    if (!spriteSheet[BALL1].id) {
        spriteSheet[BALL1] = game->textures[BALL_1];
        spriteSheet[BALL2] = game->textures[BALL_2];
        spriteSheet[BALL3] = game->textures[BALL_3];
        spriteSheet[BALL4] = game->textures[BALL_4];
    }
    if (!spriteExplode[BALL1].id) {
        spriteExplode[BALL1] = game->textures[EXPLOSION];
        spriteExplode[BALL2] = game->textures[EXPLOSION];
        spriteExplode[BALL3] = game->textures[EXPLOSION];
        spriteExplode[BALL4] = game->textures[EXPLOSION];
    }


    switch (kind) {
        case BALL1:
            return new Enemy(game, x, y, spriteSheet[BALL1].height / 2.0f, BALL1, heading);
        case BALL2:
            return new Enemy(game, x, y, spriteSheet[BALL2].height / 2.0f, BALL2, heading);
        case BALL3:
            return new Enemy(game, x, y, spriteSheet[BALL3].height / 2.0f, BALL3, heading);
        case BALL4:
            return new Enemy(game, x, y, spriteSheet[BALL4].height / 2.0f, BALL4, heading);
        default: return 0;
    }
}

Enemy::Enemy(Game* game, float x, float y, float radius, Kind kind, int heading)
        : game(game), Sprite(game, Position(x, y, radius), Type::ENEMY), color(WHITE), sizeX(radius * 2), sizeY(radius * 2), kind(kind) {
    stand = { 0.0f, 0.0f, sizeX, sizeY };
    standExplode = { 0.0f, 0.0f, spriteExplode[kind].width / 12.0f, /*sizeY*/47 };
    //rad = radius;
    switch (kind) {
    case BALL1:
        speed.x = 2.2f;
        maxSpeedY = 43 * gravity;
        break;
    case BALL2:
        speed.x = 1.8f;
        maxSpeedY = 39 * gravity;
        break;
    case BALL3:
        speed.x = 1.6f;
        maxSpeedY = 35 * gravity;
        break;
    case BALL4:
        speed.x = 1.4f;
        maxSpeedY = 32 * gravity;
        break;
    }
    speed.x *= heading;
    speed.y = -5;
}

Enemy::Enemy(Game *game, std::ifstream& s) 
    : game(game), Sprite(game, s, Type::ENEMY) {
    Read(s, &speed);
    Read(s, &kind);
    Read(s, &maxSpeedY);
    Read(s, &collision);
    
    if (!spriteSheet[BALL1].id) {
        spriteSheet[BALL1] = game->textures[BALL_1];
        spriteSheet[BALL2] = game->textures[BALL_2];
        spriteSheet[BALL3] = game->textures[BALL_3];
        spriteSheet[BALL4] = game->textures[BALL_4];
    }
    if (!spriteExplode[BALL1].id) {
        spriteExplode[BALL1] = game->textures[EXPLOSION];
        spriteExplode[BALL2] = game->textures[EXPLOSION];
        spriteExplode[BALL3] = game->textures[EXPLOSION];
        spriteExplode[BALL4] = game->textures[EXPLOSION];
    }
    
    sizeX = spriteSheet[kind].height;
    sizeY = spriteSheet[kind].height;

    standExplode = { 0.0f, 0.0f, spriteExplode[kind].width / 12.0f, 47 };
    stand = { 0.0f, 0.0f, sizeX, sizeY };
}

void Enemy::Draw() {
    if (state == State::ACTIVE)
            DrawTextureRec(spriteSheet[kind], stand, { position.center.x - sizeX / 2, position.center.y - sizeY / 2 }, WHITE);
    else if (state == State::FINISHING)
        DrawFinish();
}

void Enemy::Move() {
    if (state != State::ACTIVE || game->stopTime)
        return;

    if (position.center.x < 10 || position.center.x > game->screenWidth - 10 || position.center.y < 5 || position.center.y > game->screenHeight - 5)
        state = State::FINISHING;

    speed.y += gravity;

    position.center.x += speed.x;
    position.center.y += speed.y;
}

void Enemy::Collision(Sprite* sprite) {
    if (sprite->type == Sprite::Type::BLOCK) {
        collision = CheckCollisionCircleRec({position.center.x, position.center.y - speed.y}, position.radius, sprite->position.hitBox());
        if (collision) {
            if (speed.x > 0) {
                speed.x *= -1;
                position.center.x = sprite->position.rectangle.x - position.radius - 1;
            } else {
                speed.x *= -1;
                position.center.x = sprite->position.rectangle.x + sprite->position.rectangle.width + position.radius + 1;
            }
        } else {
            if (speed.y > 0) {
                speed.y = -maxSpeedY;
                position.center.y = sprite->position.rectangle.y - position.radius - 1;
            } else {
                speed.y = maxSpeedY / 3;
                position.center.y = sprite->position.rectangle.y + sprite->position.rectangle.height + position.radius + 1;
            }
        }
        if (!game->stopTime)
            PlaySoundMulti(game->audio[BALL_BOUNCE]);
    } else if (sprite->type == Sprite::Type::WEAPON) {
        duality(sprite);
    } else if (sprite->type == Sprite::Type::PLAYER) {
        DrawFinish();
    }
}

void Enemy::Save(std::ofstream& s) {
    Write(s, &type);
    position.Save(s);
    Write(s, &speed);
    Write(s, &kind);
    Write(s, &maxSpeedY);
    Write(s, &collision);
}

void Enemy::DrawFinish() {
    //std::cout << "Cooldown: " << cooldown << std::endl;

    DrawTextureRec(spriteExplode[kind], standExplode, { position.center.x - sizeX / 2, position.center.y - sizeY / 2 }, WHITE);
    switch (kind) {
    case Kind::BALL1:
        DrawText("100", position.center.x, position.center.y, 20, WHITE);
        break;
    case Kind::BALL2:
        DrawText("200", position.center.x, position.center.y, 20, WHITE);
        break;
    case Kind::BALL3:
        DrawText("300", position.center.x, position.center.y, 20, WHITE);
        break;
    case Kind::BALL4:
        DrawText("400", position.center.x, position.center.y, 20, WHITE);
        break;
    }

    if (cooldown % 5 == 0)
        standExplode.x += standExplode.width;

    if (standExplode.x >= spriteExplode[kind].width)
        state = State::FINISHED;

    cooldown++;
}

void Enemy::duality(Sprite* sprite) {
    Kind newKind;
    switch (kind) {
        case Kind::BALL1:
            newKind = Kind::BALL2;
            game->Spawn(Enemy::create(game, sprite->position.rectangle.x + sprite->position.rectangle.width + position.radius + 1, position.center.y, newKind, 1));
            game->Spawn(Enemy::create(game, sprite->position.rectangle.x - position.radius - 1, position.center.y, newKind, -1));
            game->AddScore(100);
            break;
        case Kind::BALL2:
            newKind = Kind::BALL3;
            game->Spawn(Enemy::create(game, sprite->position.rectangle.x + sprite->position.rectangle.width + position.radius + 1, position.center.y, newKind, 1));
            game->Spawn(Enemy::create(game, sprite->position.rectangle.x - position.radius - 1, position.center.y, newKind, -1));
            game->AddScore(200);
            break;
        case Kind::BALL3:
            newKind = Kind::BALL4;
            game->Spawn(Enemy::create(game, sprite->position.rectangle.x + sprite->position.rectangle.width + position.radius + 1, position.center.y, newKind, 1));
            game->Spawn(Enemy::create(game, sprite->position.rectangle.x - position.radius - 1, position.center.y, newKind, -1));
            game->AddScore(300);
            break;
        default:
            game->AddScore(400);
            break;
        }
    state = State::FINISHING;
    cooldown = 0;
    PlaySound(game->audio[BALL_BREAKING]);
    game->AddPowerup(position.center.x, position.center.y);
}

// Weapon
Weapon::Weapon(Game *game, float x, float y, Kind kind)
    : Sprite(game, Position(x, y, 0, 0), Type::WEAPON), kind(kind) {
        
    switch (kind) {
    case Kind::WEAPON2:
        texture[0] = &game->textures[WEAPON_SHOT];
        position.rectangle.width = texture[0]->width;
        position.rectangle.height = texture[0]->height;
        position.rectangle.x -= position.rectangle.width / 2;
        break;
    case Kind::WEAPON4:
        texture[0] = &game->textures[WEAPON_MINE];
        texture[1] = &game->textures[MINE_EXPLOSION];
        position.rectangle.width = texture[0]->width;
        position.rectangle.height = texture[0]->height;
        position.rectangle.y -= position.rectangle.height;
        moveTexture = { 0.0f, 0.0f, 40, 40 };
        break;
    default:
        texture[0] = &game->textures[WEAPON_ENDING];
        texture[1] = &game->textures[LINE_WIGGLED];
        position.rectangle.width = texture[1]->width;
        moveTexture = { 0.0f, 0.0f, position.rectangle.width, speedY };
        break;
    }
    position.hbRectangle.width = position.rectangle.width;
    position.hbRectangle.height = position.rectangle.height;
    frameCounter = 0;
}

Weapon::Weapon(Game *game, std::ifstream& s)
    : Sprite(game, s, Type::WEAPON) {
    Read(s, &kind);
    Read(s, &state);
    Read(s, &numElements);
    Read(s, &cooldown);
    Read(s, &stopMoving);
    Read(s, &frameCounter);
    Read(s, &block);

    switch (kind) {
    case Kind::WEAPON2:
        texture[0] = &game->textures[WEAPON_SHOT];
        position.rectangle.width = texture[0]->width;
        position.rectangle.height = texture[0]->height;
        position.rectangle.x -= position.rectangle.width / 2;
        break;
    case Kind::WEAPON4:
        texture[0] = &game->textures[WEAPON_MINE];
        texture[1] = &game->textures[MINE_EXPLOSION];
        position.rectangle.width = texture[0]->width;
        position.rectangle.height = texture[0]->height;
        position.rectangle.y -= position.rectangle.height;
        moveTexture = { 0.0f, 0.0f, 40, 40 };
        break;
    default:
        texture[0] = &game->textures[WEAPON_ENDING];
        texture[1] = &game->textures[LINE_WIGGLED];
        position.rectangle.width = texture[1]->width;
        moveTexture = { 0.0f, 0.0f, position.rectangle.width, speedY };
        break;
    }
    position.hbRectangle.width = position.rectangle.width;
    position.hbRectangle.height = position.rectangle.height;
}

void Weapon::Draw() {
    if (state == State::ACTIVE) {
        //DrawRectangleRec(position.rectangle, RED);
        
        DrawTexture(*texture[0], position.rectangle.x, position.rectangle.y, WHITE);
        if (texture[1]) {
            if (!stopMoving) {
                int resetFrame = 1;
                int offset = 0;
                for (int i = 1; i < numElements; i++) {
                    moveTexture.height = speedY * resetFrame;
                    DrawTextureRec(*texture[1], moveTexture, { position.rectangle.x, position.rectangle.y + offset + texture[0]->height}, WHITE);

                    if (resetFrame % 3 == 0) {
                        resetFrame = 1;
                        offset += texture[1]->height;
                    } else
                        resetFrame++;
                }
            } else {
                texture[1] = &game->textures[LINE_STRAIGHT];
                DrawTextureTiled(*texture[1], { 0, 0, 20, 20 }, { position.rectangle.x, position.rectangle.y, position.rectangle.width, position.rectangle.height - texture[0]->height }, { 0, texture[0]->height * -1.0f }, 0, 1, color);
            }
        }
    } else if (state == State::FINISHING) {
        if (kind == Kind::WEAPON4) {
            DrawTextureRec(*texture[1], moveTexture, { position.rectangle.x, position.rectangle.y + position.rectangle.height - texture[1]->height }, WHITE);
            if (cooldown++ % 5 == 0)
                moveTexture.x += moveTexture.width;

            if (moveTexture.x >= texture[1]->width)
                state = State::FINISHED;
        }
    }
}

void Weapon::Move() {
    if (state == State::ACTIVE && cooldown++ % 4 == 0) {
        if (!stopMoving) {
            switch (kind) {
            case Kind::WEAPON1:
                position.rectangle.height += speedY;
                position.hbRectangle.height += speedY;
                position.rectangle.y -= speedY;
                numElements++;
                break;
            case Kind::WEAPON2:
                if (position.rectangle.height < 30) {
                    position.rectangle.height += speedY + 20;
                    position.hbRectangle.height += speedY + 20;
                }
                position.rectangle.y -= speedY;
                break;
            case Kind::WEAPON3:
                position.rectangle.height += speedY;
                position.hbRectangle.height += speedY;
                position.rectangle.y -= speedY;
                numElements++;
                break;
            default:
                break;
            }
        } else {
            timeLeft();
            if (!block || block->state == Sprite::State::FINISHED) {
                state = State::FINISHED;
                game->shootingLeft++;
            }
        }
    }
}

void Weapon::Collision(Sprite *sprite) {
    if (sprite->type == Sprite::Type::BLOCK) {
        if (kind == Kind::WEAPON3) {
            stopMoving = true;
            overlap = sprite->position.rectangle.y + sprite->position.rectangle.height - position.rectangle.y;
            position.rectangle.y += overlap;
            position.rectangle.height -= overlap;
            position.hbRectangle.height -= overlap;
            block = sprite;
        } else {
            state = State::FINISHED;
            if (kind == Kind::WEAPON1 && game->shootingLeft < 0)
                game->shootingLeft++;
        }
    } else if (sprite->type == Sprite::Type::ENEMY || sprite->type == Sprite::Type::CRAB) {
        state = State::FINISHING;
        if (kind == Kind::WEAPON4) {
            PlaySound(game->audio[MINE_EXP_SOUND]);
        } else if (kind == Kind::WEAPON1 && game->shootingLeft < 0) {
            game->shootingLeft++;
        } else if (kind == Kind::WEAPON3 && game->shootingLeft < 0) {
            game->shootingLeft++;
        }
    }
}

void Weapon::Save(std::ofstream& s) {
    Write(s, &type);
    position.Save(s);
    Write(s, &kind);
    Write(s, &state);
    Write(s, &numElements);
    Write(s, &cooldown);
    Write(s, &stopMoving);
    Write(s, &frameCounter);
    Write(s, &block);
}

void Weapon::timeLeft() {
    if (frameCounter > 12 * 4) {
        state = State::FINISHED;
        game->shootingLeft++;
    } else if (frameCounter > 12 * 3)
        color = RED;
    else if (frameCounter > 12 * 2)
        color = YELLOW;

    frameCounter++;
}

// Ladder
Ladder::Ladder(Game *game, float x, float y, int numElements, float distanceToGround)
    : Sprite(game, Position(x, y, game->textures[LADDER].width, numElements * game->textures[LADDER].height), Type::LADDER), numElements(numElements),
      texture(&game->textures[LADDER]), distanceToGround(distanceToGround) {
    //position.rectangle.width = texture->width;
    //position.rectangle.height = numElements * texture->height;
}

Ladder::Ladder(Game *game, std::ifstream& s)
    : Sprite(game, s, Type::LADDER), texture(&game->textures[LADDER]) {
    Read(s, &numElements);
    Read(s, &distanceToGround);
}

void Ladder::Draw() {
    for (int i = 0; i < numElements; i++) {
        DrawTexture(*texture, position.rectangle.x, position.rectangle.y + i * texture->height, WHITE);
    }
}

void Ladder::Save(std::ofstream& s) {
    Write(s, &type);
    position.Save(s);
    Write(s, &numElements);
    Write(s, &distanceToGround);
}

// Powerup
Powerup::Powerup(Game *game, float x, float y, Kind kind)
    : Sprite(game, Position(x, y, 0, 0), Type::POWERUP), kind(kind) {

    switch (kind) {
    case Kind::BOOST:
        texture = &game->textures[POWERUP_BOOST];
        break;
    case Kind::DOUBLE:
        texture = &game->textures[POWERUP_DOUBLE];
        break;
    case Kind::HEAL:
        texture = &game->textures[POWERUP_HEAL];
        break;
    case Kind::TIME:
        texture = &game->textures[POWERUP_TIME];
        break;
    case Kind::WEAPON:
        texture = &game->textures[POWERUP_WEAPON];
        break;
    case Kind::SCORE:
        texture = &game->textures[POWERUP_SCORE];
        break;
    }

    position.rectangle.width = texture->width;
    position.rectangle.height = texture->height;
    position.hbRectangle.width = texture->width;
    position.hbRectangle.height = texture->height;
    
    frameCounter = 0;
}

Powerup::Powerup(Game *game, std::ifstream& s)
    : Sprite(game, s, Type::POWERUP) {
    Read(s, &speedY);
    Read(s, &kind);
    Read(s, &frameCounter);

    switch (kind) {
    case Kind::BOOST:
        texture = &game->textures[POWERUP_BOOST];
        break;
    case Kind::DOUBLE:
        texture = &game->textures[POWERUP_DOUBLE];
        break;
    case Kind::HEAL:
        texture = &game->textures[POWERUP_HEAL];
        break;
    case Kind::TIME:
        texture = &game->textures[POWERUP_TIME];
        break;
    case Kind::WEAPON:
        texture = &game->textures[POWERUP_WEAPON];
        break;
    case Kind::SCORE:
        texture = &game->textures[POWERUP_SCORE];
        break;
    }

    position.rectangle.width = texture->width;
    position.rectangle.height = texture->height;
    position.hbRectangle.width = texture->width;
    position.hbRectangle.height = texture->height;
}

void Powerup::Draw() {
    if (state == State::ACTIVE) {
        timeLeft();
        DrawTexture(*texture, position.rectangle.x, position.rectangle.y, Fade(WHITE, color));
    }
}

void Powerup::Move() {
    if (state == State::ACTIVE) {
        speedY += 0.5f;
        position.rectangle.y += speedY;
    }
}

void Powerup::Collision(Sprite *sprite) {
    if (sprite->type == Type::BLOCK) {
        position.rectangle.y = sprite->position.rectangle.y - position.rectangle.height;
        speedY = 0;
    }
    else if (sprite->type == Type::PLAYER) {
        game->PickAction(kind);
        state = State::FINISHED;
    }
}

void Powerup::Save(std::ofstream& s) {
    Write(s, &type);
    position.Save(s);
    Write(s, &speedY);
    Write(s, &kind);
    Write(s, &frameCounter);
}

void Powerup::timeLeft() {
    if (frameCounter > 60 * 8)
        state = State::FINISHED;
    else if (frameCounter > 60 * 7 + 30)
        color += 0.03f;
    else if (frameCounter > 60 * 7)
        color -= 0.03f;
    else if (frameCounter > 60 * 6 + 30)
        color += 0.03f;
    else if (frameCounter > 60 * 6)
        color -= 0.03f;
    else if (frameCounter > 60 * 5 + 30)
        color += 0.03f;
    else if (frameCounter > 60 * 5)
        color -= 0.03f;

    frameCounter++;
}

//Ice
Ice::Ice(Game *game, float x, float y, float width, float height)
    : Sprite(game, Position(x, y, width, height), Type::ICE) {
    texture = &game->textures[ICE];
}

Ice::Ice(Game *game, std::ifstream& s)
    : Sprite(game, s, Type::ICE) {
    texture = &game->textures[ICE];
}

void Ice::Draw() {
    //DrawRectangle(position.rectangle.x, position.rectangle.y, position.rectangle.width, position.rectangle.height, RED);
    DrawTextureTiled(*texture, { 0, 0, 15, 30 }, position.rectangle, { 0, -7 }, 0, 1, WHITE);
}

void Ice::Save(std::ofstream& s) {
    Write(s, &type);
    position.Save(s);
}

Crab::Crab(Game* game, float x, float y, int heading)
    : Sprite(game, Position(x, y, 0, 0), Type::CRAB) {
    texture = &game->textures[CRAB];
    position.rectangle.width = texture->width / 4;
    position.hbRectangle.width = position.rectangle.width;
    position.rectangle.height = texture->height;
    position.hbRectangle.height = position.rectangle.height;
    moveTexture = { 0, 0, position.rectangle.width, position.rectangle.height };
    frameCounter = 0;

    speed.x = 1.7f * heading;
    speed.y = 0;
}

Crab::Crab(Game* game, std::ifstream& s)
    : Sprite(game, s, Type::CRAB) {
    Read(s, &speed);
    Read(s, &frameCounter);
    texture = &game->textures[CRAB];
    moveTexture = { 0, 0, position.rectangle.width, position.rectangle.height };
}

void Crab::Draw() {
    if (state == State::ACTIVE) {
        DrawTextureRec(*texture, moveTexture, { position.rectangle.x, position.rectangle.y }, WHITE);
        if (frameCounter++ % 10 == 0) {
            moveTexture.x = (moveTexture.x + moveTexture.width >= texture->width) ? moveTexture.x = 0 : moveTexture.x + moveTexture.width;
            frameCounter = 1;
        }
    }
}

void Crab::Move() {
    if (state == State::ACTIVE) {
        speed.y += 0.5;

        position.rectangle.x += speed.x;
        position.rectangle.y += speed.y;
    }
}

void Crab::Collision(Sprite* sprite) {
    if (state == State::ACTIVE) {
        if (sprite->type == Sprite::Type::BLOCK) {
            bool collision = CheckCollisionRecs({ position.rectangle.x, position.rectangle.y - speed.y, position.rectangle.width, position.rectangle.height }, sprite->position.rectangle);
            if (collision) {
                if (speed.x > 0) {
                    position.rectangle.x = sprite->position.rectangle.x - position.rectangle.width - 1;
                    speed.x *= -1;
                } else {
                    position.rectangle.x = sprite->position.rectangle.x + sprite->position.rectangle.width + 1;
                    speed.x *= -1;
                }
            } else {
                position.rectangle.y = sprite->position.rectangle.y - position.rectangle.height - 1;
                speed.y = 0;
            }
        } else if (sprite->type == Sprite::Type::WEAPON) {
            game->AddPowerup(position.rectangle.x, position.rectangle.y - 10);
            state = State::FINISHED;
        }
    }
}

void Crab::Save(std::ofstream& s) {
    Write(s, &type);
    position.Save(s);
    Write(s, &speed);
    Write(s, &frameCounter);
}
