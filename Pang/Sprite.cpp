#include "Sprite.h"
#include "Game.h"

Texture2D Enemy::spriteSheet;
Texture2D Enemy::spriteExplode;

bool IsCollision(const Sprite *s1, const Sprite *s2) {
  bool collision = false;
  if (s1->position.type == Position::Type::RECTANGLE) {
    if (s2->position.type == Position::Type::RECTANGLE) {
      collision =
          CheckCollisionRecs(s1->position.rectangle, s2->position.rectangle);
    } else {
      collision = CheckCollisionCircleRec(
          s2->position.center, s2->position.radius, s1->position.rectangle);
    }
  } else {
    if (s2->position.type == Position::Type::RECTANGLE) {
      collision = CheckCollisionCircleRec(
          s1->position.center, s1->position.radius, s2->position.rectangle);
    } else {
      collision =
          CheckCollisionCircles(s1->position.center, s1->position.radius,
                                s2->position.center, s2->position.radius);
    }
  }
  return collision;
}

// Player
void Player::Draw() {
  if (state == State::FINISHING) {
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
  } else if (direction == Direction::UP) {
    DrawTextureRec(spriteSheet, moveUpRec, {position.rectangle.x, position.rectangle.y}, WHITE);
    if (cooldown % 5 == 0) {
      moveUpRec.x = (moveUpRec.x < 64.0 * 8) ? moveUpRec.x + 64 : 0;
    }
    cooldown++;
  } else if (direction == Direction::DOWN) {
    DrawTextureRec(spriteSheet, moveDownRec, {position.rectangle.x, position.rectangle.y}, WHITE);
    if (cooldown % 5 == 0) {
      moveDownRec.x = (moveDownRec.x < 64.0 * 8) ? moveDownRec.x + 64 : 0;
    }
    cooldown++;
  } else {
    DrawTextureRec(spriteSheet, standRec, {position.rectangle.x, position.rectangle.y}, WHITE);
    cooldown = 0;
  }
}

void Player::Move() {
    speed = 0;

  if (state != State::ACTIVE) return;

  if (IsKeyDown(KEY_LEFT)) {
    direction = Direction::LEFT;
    speed = -7;
  } else if (IsKeyDown(KEY_RIGHT)) {
    direction = Direction::RIGHT;
    speed = 7;
  } else if (IsKeyDown(KEY_UP)) {
    direction = Direction::UP;
  } else if (IsKeyDown(KEY_DOWN)) {
    direction = Direction::DOWN;
  } else {
    direction = Direction::NONE;
  }

  position.rectangle.x += speed;
}

void Player::Collision(Sprite *sprite) {
  if (sprite->type == Sprite::Type::BLOCK) {
    if (speed > 0) {
      position.rectangle.x = sprite->position.rectangle.x - position.rectangle.width;
    } else {
      position.rectangle.x = sprite->position.rectangle.x + sprite->position.rectangle.width;
    }
    speed = 0;
  } else if (sprite->type == Sprite::Type::ENEMY) {
    state = Sprite::State::FINISHING;
  }
}

//Block
void Block::Draw() {
    DrawRectangleRec(position.rectangle, color);
}

// Ball
void Enemy::Draw() {
    if (state == State::ACTIVE) {
        switch (rad) {
        case 20:
            stand.x = 0;
            DrawTextureRec(spriteSheet, stand, { position.center.x - sizeX / 2, position.center.y - sizeY / 2 }, WHITE);
            break;
        case 15:
            stand.x = 40;
            DrawTextureRec(spriteSheet, stand, { position.center.x - sizeX / 2, position.center.y - sizeY / 2 }, WHITE);
            break;
        case 10:
            stand.x = 70;
            DrawTextureRec(spriteSheet, stand, { position.center.x - sizeX / 2, position.center.y - sizeY / 2 }, WHITE);
            break;
        case 5:
            stand.x = 90;
            DrawTextureRec(spriteSheet, stand, { position.center.x - sizeX / 2, position.center.y - sizeY / 2 }, WHITE);
            break;
        default:
            break;
        }
        DrawTextureRec(spriteSheet, stand, { position.center.x - sizeX / 2, position.center.y - sizeY / 2 }, WHITE);
        if (cooldown % 5 == 0) {
            if (stand.x < sizeX * frameNumber) stand.x += sizeX;
            else stand.x = 0.0;
        }
        cooldown++;
        if (cooldown == 5) cooldown = 0;
    }
    else if (state == State::FINISHING) {
        DrawFinish();
    }


}

void Enemy::Move() {
    if (state != State::ACTIVE) {
        return;
    }

    speedY++;

    position.center.x += speedX;
    auto spriteX = checkCollision();
    if (spriteX) {
        if (spriteX->type == Sprite::Type::BLOCK) {
            if (speedX > 0) {
        position.center.x = spriteX->position.rectangle.x - position.radius - 1;
        printf("spriteX 1\n");
      } else {
        position.center.x = spriteX->position.rectangle.x + spriteX->position.rectangle.width + position.radius + 1;
        printf("spriteX 2\n");
      }
            speedX *= -1;
      printf("center:%f speedX:%f\n", position.center.x, speedX);
        }
    }
    position.center.y += speedY;
    auto spriteY = checkCollision();
    if (spriteY && spriteY != spriteX) {
        if (spriteY->type == Sprite::Type::BLOCK) {
            if (speedY > 0) {
                speedY = -maxSpeedY;
                position.center.y = spriteY->position.rectangle.y - position.radius - 1;
        printf("spriteY 1\n");
      } else {
                speedY = maxSpeedY / 2;
                position.center.y = spriteY->position.rectangle.y + spriteY->position.rectangle.height + position.radius + 1;
        printf("spriteY 2\n");
      }
      printf("center:%f speedY:%f\n", position.center.y, speedY);
    }
    }
    duality();
}

void Enemy::Collision(Sprite* sprite) {
  // if (sprite->type == Sprite::Type::BLOCK) {
  //   if (speedX > 0) {
  //     if (position.center.x >= sprite->position.rectangle.x - position.radius) {
  //       speedX *= -1;
  //       position.center.x = sprite->position.rectangle.x - position.radius;
  //       printf("spriteX 1\n");
  //     }
  //   } else {
  //     if (position.center.x <= sprite->position.rectangle.x + sprite->position.rectangle.width + position.radius) {
  //       speedX *= -1;
  //       position.center.x = sprite->position.rectangle.x + sprite->position.rectangle.width + position.radius;
  //       printf("spriteX 2\n");
  //     }
  //   }
  //   printf("center:%f speedX:%f\n", position.center.x, speedX);

  //   if (speedY > 0) {
  //     if (position.center.y > sprite->position.rectangle.y - position.radius) {
  //       speedY = -maxSpeedY;
  //       position.center.y = sprite->position.rectangle.y - position.radius;
  //       printf("spriteY 1\n");
  //     }
  //   } else {
  //     if (position.center.y <= sprite->position.rectangle.y + sprite->position.rectangle.height + position.radius) {
  //       speedY = maxSpeedY / 2;
  //       position.center.y = sprite->position.rectangle.y + sprite->position.rectangle.height + position.radius;
  //       printf("spriteY 2\n");
  //     }
  //   }

  //   printf("center:%f speedY:%f\n", position.center.y, speedY);
  // }
  // duality();
}

void Enemy::DrawFinish() {
    //std::cout << "Cooldown: " << cooldown << std::endl;

    DrawTextureRec(spriteExplode, standExplode, { position.center.x - sizeX / 2, position.center.y - sizeY / 2 }, WHITE);
    if (cooldown % 5 == 0)
        standExplode.x += 100;

    if (cooldown == 45)
        state = State::FINISHED;

    cooldown++;
}

void Enemy::duality() {
    auto sprite = checkCollision();
    if (sprite) {
        if (sprite->type == Sprite::Type::WEAPON) {
            Kind newKind;
            switch (kind) {
            case Kind::BALL1:
                newKind = Kind::BALL2;
                game->sprites.push_back(Enemy::create(game, sprite->position.rectangle.x + sprite->position.rectangle.width + position.radius + 1, position.center.y, newKind, 1));
                game->sprites.push_back(Enemy::create(game, sprite->position.rectangle.x - position.radius - 1, position.center.y, newKind, -1));
                game->AddScore(1);
                break;
            case Kind::BALL2:
                newKind = Kind::BALL3;
                game->sprites.push_back(Enemy::create(game, sprite->position.rectangle.x + sprite->position.rectangle.width + position.radius + 1, position.center.y, newKind, 1));
                game->sprites.push_back(Enemy::create(game, sprite->position.rectangle.x - position.radius - 1, position.center.y, newKind, -1));
                game->AddScore(2);
                break;
            case Kind::BALL3:
                newKind = Kind::BALL4;
                game->sprites.push_back(Enemy::create(game, sprite->position.rectangle.x + sprite->position.rectangle.width + position.radius + 1, position.center.y, newKind, 1));
                game->sprites.push_back(Enemy::create(game, sprite->position.rectangle.x - position.radius - 1, position.center.y, newKind, -1));
                game->AddScore(3);
                break;
            default:
                game->AddScore(4);
                break;
            }
            state = State::FINISHING;
            cooldown = 0;
        }
    }
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
void Weapon::init() {
    position.rectangle.height = 0;
}

void Weapon::Draw() {
    if (active) {
        DrawRectangleRec(position.rectangle, color);
    }
}

void Weapon::Move() {
    if (IsKeyPressed(KEY_SPACE)) {
        if (!active) {
            Rectangle pos = game->getPlayerPosition();
            position.rectangle.x = pos.x;
            position.rectangle.y = pos.y + pos.height;
            active = true;
        }
    }

    if (active && cooldown++ % 5 == 0) {
        if (position.rectangle.height < 400) {
            position.rectangle.height += speedY;
            position.rectangle.y -= speedY;
        } else {
            active = false;
            init();
        }
    }
}
