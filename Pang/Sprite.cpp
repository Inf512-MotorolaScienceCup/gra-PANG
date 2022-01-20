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
Player::Player(Game *game, Position position, Color color)
    : Sprite(game, Position(position), Type::PLAYER),
    color(color), speed({ 0, 0 }) {
    spriteSheet = game->textures[PLAYER];
    moveRightRec = {0.0f, 11 * 64.0f, 64.0f, 64.0f};
    moveLeftRec = {0.0f, 9 * 64.0f, 64.0f, 64.0f};
    standRec = {0.0f, 2 * 64.0f, 64.0f, 64.0f};
    moveUpRec = {0.0f, 8 * 64.0f, 64.0f, 64.0f};
    hurtRec = {0.0f, 20 * 64.0f, 64.0f, 64.0f};
}

void Player::Draw() {
    //DrawRectangleRec(position.rectangle, GREEN);
    //DrawRectangleRec(position.hitBox(), RED);

    if (state == State::FINISHING) {
        if (hurtRec.x < 64.0 * 5)
            DrawTextureRec(spriteSheet, hurtRec, {position.rectangle.x, position.rectangle.y}, WHITE);
        if (hurtRec.x == 64.0 * 5 && cooldown % 5 == 0)
            DrawTextureRec(spriteSheet, hurtRec, {position.rectangle.x, position.rectangle.y}, WHITE);

        if (cooldown % 5 == 0) {
            if (hurtRec.x < 64.0 * 5)
                hurtRec.x += 64;
        }
        cooldown++;
    } else if (direction == Direction::RIGHT) {
        DrawTextureRec(spriteSheet, moveRightRec, { position.rectangle.x, position.rectangle.y }, WHITE);
        if (cooldown % 5 == 0) {
            moveRightRec.x = (moveRightRec.x < 64.0 * 8) ? moveRightRec.x + 64 : 0;
        }
        cooldown++;
    } else if (direction == Direction::LEFT) {
        DrawTextureRec(spriteSheet, moveLeftRec, { position.rectangle.x, position.rectangle.y }, WHITE);
        if (cooldown % 5 == 0) {
            moveLeftRec.x = (moveLeftRec.x < 64.0 * 8) ? moveLeftRec.x + 64 : 0;
        }
        cooldown++;
    } else if (direction == Direction::UP || direction == Direction::DOWN) {
        DrawTextureRec(spriteSheet, moveUpRec, {position.rectangle.x, position.rectangle.y}, WHITE);
        if (cooldown % 5 == 0) {
            moveUpRec.x = (moveUpRec.x < 64.0 * 8) ? moveUpRec.x + 64 : 0;
        }
        cooldown++;
    } else if (climbing) {
        DrawTextureRec(spriteSheet, moveUpRec, { position.rectangle.x, position.rectangle.y }, WHITE);
    } else {
        DrawTextureRec(spriteSheet, standRec, {position.rectangle.x, position.rectangle.y}, WHITE);
        cooldown = 0;
    }
}

void Player::Move() {
    if (state != State::ACTIVE) return;

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
                if (speed.x > -7 * game->speedBoost)
                    speed.x -= 1;
                else
                    speed.x = -7 * game->speedBoost;
            } else if (IsKeyDown(KEY_RIGHT)) {
                direction = Direction::RIGHT;
                if (speed.x < 7 * game->speedBoost)
                    speed.x += 1;
                else
                    speed.x = 7 * game->speedBoost;
            } else {
                direction = Direction::NONE;
                speed.x = 0;
            }
        } else {
            if (IsKeyDown(KEY_LEFT)) {
                direction = Direction::LEFT;
                    speed.x -= 0.25 * game->speedBoost;
            } else if (IsKeyDown(KEY_RIGHT)) {
                direction = Direction::RIGHT;
                speed.x += 0.25 * game->speedBoost;
            } else {
                direction = Direction::NONE;
                if (speed.x > 0.2)
                    speed.x -= 0.2;
                else if (speed.x < -0.2)
                    speed.x += 0.2;
                else
                    speed.x = 0;
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
    } else if (sprite->type == Sprite::Type::ENEMY) {
        // FIXME: only sprite change, game state change on moving to FINISHED
        state = State::FINISHING;
        game->ChangeState(Game::State::PLAYER_DIED);
    } else if (sprite->type == Sprite::Type::LADDER) {
        if (!climbing) {
            bool canClimbUp = position.rectangle.y < sprite->position.rectangle.y + sprite->position.rectangle.height && position.rectangle.y + position.rectangle.height > sprite->position.rectangle.y + sprite->position.rectangle.height;
            bool canClimbDown = position.rectangle.y < sprite->position.rectangle.y && position.rectangle.y + position.rectangle.height > sprite->position.rectangle.y;

            if (IsKeyDown(KEY_UP) && canClimbUp)
                climbing = true;
            else if (IsKeyDown(KEY_DOWN) && canClimbDown)
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

void Player::Shooting() {
    if (game->shootingLeft > 0) {
        game->AddWeapon(position.rectangle.x, position.rectangle.y + position.rectangle.height, game->weaponType);
        game->shootingLeft--;
    }
    else if (game->shootingLeft == 0 - game->multiWeapon) {
        game->AddWeapon(position.rectangle.x, position.rectangle.y + position.rectangle.height, 1);
        game->shootingLeft--;
    }
}

//Block
Block::Block(Game *game, float x, float y, float width, float height, Kind kind)
    : Sprite(game, Position(x, y, width, height), Sprite::Type::BLOCK), kind(kind) {
}

void Block::Draw() {
    // DrawRectangleRec(position.rectangle, color);
    Texture2D* texture = nullptr;
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
    DrawTextureTiled(*texture, {0, 0, 20, 20}, position.rectangle, {0, 0}, 0, 1, WHITE);
}

void Block::Collision(Sprite* sprite) {
    if (kind == Kind::PLATFORM_2) {
        health--;
        if (health == 0)
            state = State::FINISHED;
    }
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
            return new Enemy(game, x, y, spriteSheet[BALL1].height / 2, BALL1, heading);
        case BALL2:
            return new Enemy(game, x, y, spriteSheet[BALL2].height / 2, BALL2, heading);
        case BALL3:
            return new Enemy(game, x, y, spriteSheet[BALL3].height / 2, BALL3, heading);
        case BALL4:
            return new Enemy(game, x, y, spriteSheet[BALL4].height / 2, BALL4, heading);
        default: return 0;
    }
}

Enemy::Enemy(Game* game, float x, float y, float radius, Kind kind, int heading)
        : game(game), Sprite(game, Position(x, y, radius), Type::ENEMY), color(WHITE), sizeX(radius * 2), sizeY(radius * 2), kind(kind) {
    stand = { 0.0f, 0.0f, sizeX, sizeY };
    standExplode = { 0.0f, 0.0f, spriteExplode[kind].width / 12.0f, sizeY };
    rad = radius;
    switch (kind) {
    case BALL1:
        speed.x = 2.5;
        maxSpeedY = 35 * gravity;
        break;
    case BALL2:
        speed.x = 2;
        maxSpeedY = 30 * gravity;
        break;
    case BALL3:
        speed.x = 2;
        maxSpeedY = 27 * gravity;
        break;
    case BALL4:
        speed.x = 3;
        maxSpeedY = 25 * gravity;
        break;
    }
    speed.x *= heading;
    speed.y = -5;
}

void Enemy::Draw() {
    if (state == State::ACTIVE) {
        DrawTextureRec(spriteSheet[kind], stand, { position.center.x - sizeX / 2, position.center.y - sizeY / 2 }, WHITE);
        //DrawCircleLines(position.center.x, position.center.y, position.radius, RED);
        //if (cooldown % 5 == 0) {
        //    if (stand.x < sizeX * frameNumber) stand.x += sizeX;
        //    else stand.x = 0.0;
        //}
        //cooldown++;
        //if (cooldown == 5) cooldown = 0;
    }
    else if (state == State::FINISHING) {
        DrawFinish();
    }

}

void Enemy::Move() {
    if (state != State::ACTIVE || game->stopTime) {
        return;
    }

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
                position.center.x = sprite->position.rectangle.x - position.radius;
            } else {
                speed.x *= -1;
                position.center.x = sprite->position.rectangle.x + sprite->position.rectangle.width + position.radius;
            }
        } else {
            if (speed.y > 0) {
                speed.y = -maxSpeedY;
                position.center.y = sprite->position.rectangle.y - position.radius;
            } else {
                speed.y = maxSpeedY / 2;
                position.center.y = sprite->position.rectangle.y + sprite->position.rectangle.height + position.radius;
            }
        }
    } else if (sprite->type == Sprite::Type::WEAPON) {
        duality(sprite);
    }
}

void Enemy::DrawFinish() {
    //std::cout << "Cooldown: " << cooldown << std::endl;

    DrawTextureRec(spriteExplode[kind], standExplode, { position.center.x - sizeX / 2, position.center.y - sizeY / 2 }, WHITE);
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
                game->AddScore(1);
                break;
            case Kind::BALL2:
                newKind = Kind::BALL3;
                game->Spawn(Enemy::create(game, sprite->position.rectangle.x + sprite->position.rectangle.width + position.radius + 1, position.center.y, newKind, 1));
                game->Spawn(Enemy::create(game, sprite->position.rectangle.x - position.radius - 1, position.center.y, newKind, -1));
                game->AddScore(2);
                break;
            case Kind::BALL3:
                newKind = Kind::BALL4;
                game->Spawn(Enemy::create(game, sprite->position.rectangle.x + sprite->position.rectangle.width + position.radius + 1, position.center.y, newKind, 1));
                game->Spawn(Enemy::create(game, sprite->position.rectangle.x - position.radius - 1, position.center.y, newKind, -1));
                game->AddScore(3);
                break;
            default:
                game->AddScore(4);
                break;
            }
            state = State::FINISHING;
            cooldown = 0;
            game->AddPowerup(position.center.x, position.center.y);
}

Sprite* Enemy::checkCollision() {
    for (auto sprite : game->sprites) {
        if (sprite->type == Sprite::Type::BLOCK && IsCollision(this, sprite)) {
            //std::cout << "PLAYER collided with BLOCK:" << std::endl;
            return sprite;
        }
        else if (sprite->type == Sprite::Type::WEAPON && IsCollision(this, sprite)) {
            //std::cout << "PLAYER collided with WEAPON:" << std::endl;
            return sprite;
        }
    }
    return 0;
}

// Weapon
Weapon::Weapon(Game *game, float x, float y, Kind kind)
    : Sprite(game, Position(x, y, 0, 0), Type::WEAPON), kind(kind) {
        
    switch (kind) {
    case Kind::WEAPON2:
        texture[0] = &game->textures[WEAPON_SHOT];
        position.rectangle.width = texture[0]->width;
        position.rectangle.height = texture[0]->height;
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
                moveTexture.height = position.rectangle.height - texture[0]->height;
                DrawTextureRec(*texture[1], moveTexture, { position.rectangle.x, position.rectangle.y + position.rectangle.height - texture[0]->height }, WHITE);
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
    if (state == State::ACTIVE && cooldown++ % 5 == 0) {
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
                    position.rectangle.height += speedY;
                    position.hbRectangle.height += speedY;
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
        }
        else
            checkTime();
    }
}

void Weapon::Collision(Sprite *sprite) {
    if (sprite->type == Sprite::Type::BLOCK) {
        if (kind == Kind::WEAPON3) {
            stopMoving = true;
            lifeTime = std::time(nullptr);
            overlap = sprite->position.rectangle.y + sprite->position.rectangle.height - position.rectangle.y;
            position.rectangle.y += overlap;
            position.rectangle.height -= overlap;
            position.hbRectangle.height -= overlap;
        } else {
            state = State::FINISHED;
            if (kind == Kind::WEAPON1 && game->shootingLeft < 0)
                game->shootingLeft++;
        }
    } else if (sprite->type == Sprite::Type::ENEMY) {
        state = State::FINISHING;
        if (kind == Kind::WEAPON1 && game->shootingLeft < 0) {
            game->shootingLeft++;
        } else if (kind == Kind::WEAPON3 && game->shootingLeft < 0) {
            game->shootingLeft++;
        }
    }
}

void Weapon::checkTime() {
    std::time_t now = std::time(nullptr);
    if (lifeTime + 5 <= now) {
        state = State::FINISHED;
        game->shootingLeft++;
    }
}

// Ladder
Ladder::Ladder(Game *game, float x, float y, int numElements, float distanceToGround)
    : Sprite(game, Position(x, y, game->textures[LADDER].width, numElements * game->textures[LADDER].height), Type::LADDER), numElements(numElements),
      texture(&game->textures[LADDER]), distanceToGround(distanceToGround) {
    //position.rectangle.width = texture->width;
    //position.rectangle.height = numElements * texture->height;
}

void Ladder::Draw() {
    for (int i = 0; i < numElements; i++) {
        DrawTexture(*texture, position.rectangle.x, position.rectangle.y + i * texture->height, WHITE);
    }
}

//Powerup
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
    }

    position.rectangle.width = texture->width;
    position.rectangle.height = texture->height;
    position.hbRectangle.width = texture->width;
    position.hbRectangle.height = texture->height;
}

void Powerup::Draw() {
    if (state == State::ACTIVE) {
        DrawTexture(*texture, position.rectangle.x, position.rectangle.y, WHITE);
    }
}

void Powerup::Move() {
    if (state == State::ACTIVE) {
        speedY++;
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

//Ice
Ice::Ice(Game *game, float x, float y, float width, float height)
    : Sprite(game, Position(x, y, width, height), Type::ICE) {
}

void Ice::Draw() {
    DrawRectangle(position.rectangle.x, position.rectangle.y, position.rectangle.width, position.rectangle.height, RED);
}
