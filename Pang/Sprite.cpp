#include "stdafx.h"
#include "Sprite.h"

Texture2D Ball::spriteSheet;
Texture2D Ball::spriteExplode;

// Player
void Player::draw() {
	if (IsKeyDown(KEY_RIGHT)) {
		DrawTextureRec(spriteSheet, moveRightRec, { position.rectangle.x, position.rectangle.y }, WHITE);
		if (cooldown % 5 == 0) {
			if (moveRightRec.x < 64.0 * 8) moveRightRec.x += 64.0;
			else moveRightRec.x = 0.0;
		}
		cooldown++;
	}
	else if (IsKeyDown(KEY_LEFT)) {
		DrawTextureRec(spriteSheet, moveLeftRec, { position.rectangle.x, position.rectangle.y }, WHITE);
		if (cooldown % 5 == 0) {
			if (moveLeftRec.x < 64.0 * 8) moveLeftRec.x += 64.0;
			else moveLeftRec.x = 0.0;
		}
		cooldown++;
	}
	else {
		DrawTextureRec(spriteSheet, standRec, { position.rectangle.x, position.rectangle.y }, WHITE);
		cooldown = 0;
	}
}

void Player::move() {
	speed = 0;
	if (IsKeyDown(KEY_LEFT))  speed = -7;
	if (IsKeyDown(KEY_RIGHT)) speed = 7;

	position.rectangle.x += speed;
	Sprite* sprite = checkCollision();
	if (sprite) {
		if (sprite->position.type == Position::Type::RECTANGLE) {
			if (speed > 0)
				position.rectangle.x = sprite->position.rectangle.x - position.rectangle.width;
			else
				position.rectangle.x = sprite->position.rectangle.x + sprite->position.rectangle.width;
		}
		speed = 0;
	}
}

Sprite* Player::checkCollision() {
	for (auto sprite : game->sprites) {
		if (sprite->type == Sprite::Type::BLOCK && Game::isCollision(this, sprite)) {
			std::cout << "PLAYER collided with BLOCK:" << std::endl;
			return sprite;
		}
	}
	return 0;
}

//Block
void Block::draw() {
	DrawRectangleRec(position.rectangle, color);
}

// Ball
void Ball::draw() {
	DrawTextureRec(spriteSheet, stand, { position.center.x - sizeX / 2, position.center.y - sizeY / 2 }, WHITE);
	if (cooldown % 5 == 0) {
		if (stand.x < 80.0 * 10) stand.x += 80.0;
		else stand.x = 0.0;
	}
	cooldown++;
	if (cooldown == 5) cooldown = 0;
}

void Ball::move() {
	if (state != State::ACTIVE) {
		drawFinish();
		return;
	}

	speedY++;

	position.center.x += speedX;
	auto spriteX = checkCollision();
	if (spriteX) {
		if (spriteX->type == Sprite::Type::BLOCK) {
			if (speedX > 0) position.center.x = spriteX->position.rectangle.x - position.radius - 1;
			else position.center.x = spriteX->position.rectangle.x + spriteX->position.rectangle.width + position.radius + 1;
			speedX *= -1;
		}
	}
	position.center.y += speedY;
	auto spriteY = checkCollision();
	if (spriteY && spriteY != spriteX) {
		if (spriteY->type == Sprite::Type::BLOCK) {
			speedY = -maxSpeedY;
			position.center.y = spriteY->position.rectangle.y - position.radius - 1;
		}
	}
	duality();
}

void Ball::drawFinish() {
	std::cout << "Explosion" << std::endl;

	for (cooldown = 0; standExplode.x < spriteExplode.width;) {
		if (cooldown % 20 == 0) {
			DrawTextureRec(spriteExplode, standExplode, { position.center.x - sizeX / 2, position.center.y - sizeY / 2 }, WHITE);
			standExplode.x += 100;
		}
		cooldown++;
	}

	state = State::FINISHED;
}

void Ball::duality() {
	auto sprite = checkCollision();
	if (sprite) {
		if (sprite->type == Sprite::Type::WEAPON) {
			Kind newKind;
			switch (kind) {
			case Kind::BALL1:
				newKind = Kind::BALL2;
				game->sprites.push_back(Ball::create(game, sprite->position.rectangle.x + sprite->position.rectangle.width + position.radius + 1, position.center.y, newKind, 1));
				game->sprites.push_back(Ball::create(game, sprite->position.rectangle.x - position.radius - 1, position.center.y, newKind, -1));
				game->AddScore(1);
				break;
			case Kind::BALL2:
				newKind = Kind::BALL3;
				game->sprites.push_back(Ball::create(game, sprite->position.rectangle.x + sprite->position.rectangle.width + position.radius + 1, position.center.y, newKind, 1));
				game->sprites.push_back(Ball::create(game, sprite->position.rectangle.x - position.radius - 1, position.center.y, newKind, -1));
				game->AddScore(2);
				break;
			case Kind::BALL3:
				newKind = Kind::BALL4;
				game->sprites.push_back(Ball::create(game, sprite->position.rectangle.x + sprite->position.rectangle.width + position.radius + 1, position.center.y, newKind, 1));
				game->sprites.push_back(Ball::create(game, sprite->position.rectangle.x - position.radius - 1, position.center.y, newKind, -1));
				game->AddScore(3);
				break;
			default:
				game->AddScore(4);
				break;
			}
			state = State::FINISHING;
		}
	}
}

Sprite* Ball::checkCollision() {
	for (auto sprite : game->sprites) {
		if (sprite->type == Sprite::Type::BLOCK && Game::isCollision(this, sprite)) {
			//std::cout << "PLAYER collided with BLOCK:" << std::endl;
			return sprite;
		}
		else if (sprite->type == Sprite::Type::WEAPON && Game::isCollision(this, sprite)) {
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

void Weapon::draw() {
	if (active) {
		DrawRectangleRec(position.rectangle, color);
	}
}

void Weapon::move() {
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
		}
		else {
			active = false;
			init();
		}
	}
}

Sprite* Weapon::checkCollision() {
	for (auto sprite : game->sprites) {
		if (sprite->type == Sprite::Type::BLOCK && Game::isCollision(this, sprite)) {
			std::cout << "PLAYER collided with BLOCK:" << std::endl;
			return sprite;
		}
	}
	return 0;
}
