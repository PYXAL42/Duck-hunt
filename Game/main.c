#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <math.h>
#include "SFML/Graphics.h"
#include "SFML/Audio.h"

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 960
#define BPP 32
#define M_PI 14
#define MAX_SPEED 300
#define MAX_ANGLE 20
#define MAX_AMMO 3

void Load(struct EssentialData*);
void PollEvent(struct EssentialData*);
void KeyPressed();
void MouseButtonPressed(struct EssentialData*);
void MouseMoved();
void Update(struct EssentialData*);
void Draw(struct EssentialData*);
void Cleanup(struct EssentialData*);

void RoundStart(struct EssentialData*);

sfVector2f GetVectorDirection(float _angleDeg);
float GetAngleRad(float);
float GetAngleDeg(float);
float GetDistance(sfVector2f _point1, sfVector2f _point2);

void CheckCollisionDuckScreen(struct EssentialData* _data);
int GetMouseColision(struct EssentialData* _data);
void MoveCrosshairWithMouse(sfSprite*, sfVector2i);
void MoveSprite(struct Sprite*, float _deltaTime);


typedef enum State
{
	FLYING,
	FLEEING,
	FALLING,
	DEAD
}State;

typedef struct Sprite
{
	sfSprite* sprite;
	sfVector2f direction;
	State state;

} Sprite;

struct EssentialData
{
	sfRenderWindow* renderWindow;
	sfClock* clock;
	sfTexture* spriteTexture;
	
	Sprite background;
	Sprite duck;
	Sprite crosshair;

	int round;
	float roundTime;
	int ammo;
	int score;
};

int main(void)
	{
	// Initialisation
	struct EssentialData data = { NULL, NULL };
	Load(&data);
	
	while (sfRenderWindow_isOpen(data.renderWindow) && data.round<11)
	{
		// Gestion des events
		PollEvent(&data);
		// Update
		Update(&data);
		// Draw
		Draw(&data);
	}
	// Cleanup
	Cleanup(&data);
	system("pause");
	return EXIT_SUCCESS;
}

void Load(struct EssentialData* _data)
{
	//Initiating the random
	srand(_getpid());
	//Preparing the render window and the n
	sfVideoMode videoMode = { SCREEN_WIDTH, SCREEN_HEIGHT, 32 };
	_data-> renderWindow = sfRenderWindow_create(videoMode, "CSFML", sfDefaultStyle, NULL);
	_data-> clock = sfClock_create();
	_data->roundTime = 0;
	_data->score = 0;
	_data->spriteTexture = sfTexture_createFromFile("Assets/Sprites/DuckHunt-SpriteSheet.png", NULL);

	_data->background.sprite = sfSprite_create();
	sfSprite_setTexture(_data->background.sprite, _data->spriteTexture, 0);
	sfSprite_setTextureRect(_data->background.sprite, (sfIntRect){ 1532, 20, SCREEN_WIDTH, SCREEN_HEIGHT});

	_data->crosshair.sprite = sfSprite_create();
	sfSprite_setTexture(_data->crosshair.sprite, _data->spriteTexture, 0);
	sfSprite_setTextureRect(_data->crosshair.sprite, (sfIntRect) {1876, 1036, 100, 92});
	sfSprite_setOrigin(_data->crosshair.sprite, (sfVector2f) { 50 , 46 });

	_data->duck.sprite = sfSprite_create();
	sfSprite_setTexture(_data->duck.sprite, _data->spriteTexture, 0);

	RoundStart(_data);
}

void PollEvent(struct EssentialData* _data)
{
	sfEvent event;
	while (sfRenderWindow_pollEvent(_data->renderWindow, &event))
	{
		switch (event.type)
		{
		case sfEvtClosed:
			sfRenderWindow_close(_data->renderWindow);
			break;
		case sfEvtKeyPressed:
			KeyPressed();
			break;
		case sfEvtMouseButtonPressed:
			MouseButtonPressed(_data);
			break;
		case sfEvtMouseMoved:
			MouseMoved();
			break;
		default:

			break;
		}
	}
}

void KeyPressed()
{
	
}

void MouseButtonPressed(struct EssentialData* _data)
{
	if (_data->ammo > 0)
	{
		_data->ammo -= 1;
		printf("BANG!!\n");
		printf("%d bullets still in the chamber\n", _data->ammo);
		if (GetMouseColision(_data) && _data->duck.state == FLYING)
		{
			_data->duck.direction.x = 0;
			_data->duck.direction.y = 1;
			_data->duck.state = FALLING;
			_data->score += 500;
			sfSprite_setTextureRect(_data->duck.sprite, (sfIntRect){ 1412, 536, 65, 132});
			sfSprite_setOrigin(_data->duck.sprite, (sfVector2f) { 32, 66});
			sfSprite_setScale(_data->duck.sprite, (sfVector2f) {1,1});

			printf("You hit! Your score is now %d\n", _data->score);
		}
	}
	else
	{
		printf("No ammo left\n");
	}
	
}

void MouseMoved()
{

}

void Update(struct EssentialData* _data)
{

	float deltaTime = sfTime_asSeconds(sfClock_restart(_data->clock));
	_data->roundTime += deltaTime;

	MoveCrosshairWithMouse(_data->crosshair.sprite, sfMouse_getPositionRenderWindow(_data->renderWindow));
	CheckCollisionDuckScreen(_data);
	if (_data->duck.state != DEAD)
	{
		MoveSprite(&_data->duck, deltaTime);
	}
	
	if ((_data->duck.state == DEAD && _data->roundTime > 2))
	{
		RoundStart(_data);
	}
	if ((_data->roundTime > 5 || _data->ammo == 0) && _data->duck.state == FLYING)
	{
		_data->duck.state = FLEEING;
		_data->duck.direction.x = 1;
		_data->duck.direction.y = -1;
		sfSprite_setTextureRect(_data->duck.sprite, (sfIntRect) { 548, 504, 116, 148 });
		sfSprite_setOrigin(_data->duck.sprite, (sfVector2f) { 32, 66 });
		sfSprite_setScale(_data->duck.sprite, (sfVector2f) { 1, 1 });
	}

	if (sfKeyboard_isKeyPressed(sfKeyZ))
	{

	}
	if (sfMouse_isButtonPressed(sfMouseLeft))
	{

	}
}

void Draw(struct EssentialData* _data)
{
	sfRenderWindow_clear(_data->renderWindow, sfBlack);
	sfRenderWindow_drawSprite(_data->renderWindow, _data->background.sprite, NULL);
	sfRenderWindow_drawSprite(_data->renderWindow, _data->duck.sprite, NULL);
	sfRenderWindow_drawSprite(_data->renderWindow, _data->crosshair.sprite, NULL);

	sfRenderWindow_display(_data->renderWindow);
}

void Cleanup(struct EssentialData* _data)
{
	sfRenderWindow_destroy(_data->renderWindow);
	sfClock_destroy(_data->clock);
	_data->renderWindow = NULL;	
	_data->clock = NULL;

	system("cls");
	printf("Your end score was %d\n", _data->score);

	sfSprite_destroy(_data->background.sprite);
	sfSprite_destroy(_data->crosshair.sprite);
	sfSprite_destroy(_data->duck.sprite);
}

void RoundStart(struct EssentialData* _data)
{

	_data->round += 1;
	printf("Starting round %d\n", _data->round);
	_data->ammo = MAX_AMMO;
	printf("Shotgun reloaded with %d bullets\n", _data->ammo);
	_data->roundTime = 0;
	_data->duck.state = FLYING;
	int tempRand = rand();
	sfSprite_setPosition(_data->duck.sprite, (sfVector2f) { ((SCREEN_WIDTH) * (tempRand % 2)), SCREEN_HEIGHT / 2 });
	_data->duck.direction = GetVectorDirection((tempRand % (MAX_ANGLE * 2) - 20) + (-180 * (tempRand % 2)));
	sfSprite_setTextureRect(_data->duck.sprite, (sfIntRect) { 32, 508, 140, 136 });
	sfSprite_setOrigin(_data->duck.sprite, (sfVector2f) { 70, 68 });
	int temporaire = pow(-1, tempRand % 2);
	sfSprite_setScale(_data->duck.sprite, (sfVector2f) {temporaire, 1 });
}

sfVector2f GetVectorDirection(float _angleDeg)
{
	float angleRad = GetAngleRad(_angleDeg);

	return (sfVector2f) { cosf(angleRad), sinf(angleRad) };
}

float GetAngleRad(float _angleDeg)
{
	return _angleDeg * M_PI / 180;
}

float GetAngleDeg(float _angleRad)
{
	return _angleRad * 180 / M_PI;
}

float GetDistance(sfVector2f _point1, sfVector2f _point2)
{
	float distX = _point2.x - _point1.x;
	float distY = _point2.y - _point1.y;

	return sqrtf(distX * distX + distY * distY);
}

void CheckCollisionDuckScreen(struct EssentialData* _data)
{
	sfFloatRect hitbox = sfSprite_getGlobalBounds(_data->duck.sprite);

	// Collision à gauche de l'écran
	if (hitbox.left < 0 && _data->duck.state == FLYING && _data->duck.direction.x < 0)
	{
		sfVector2f position = sfSprite_getPosition(_data->duck.sprite);
		position.x = hitbox.width / 2;
		sfSprite_setPosition(_data->duck.sprite, position);
		_data->duck.direction.x *= -1;
		sfSprite_setScale(_data->duck.sprite, (sfVector2f){1,1});
	}
	// Collision à droite de l'écran
	else if (hitbox.left + hitbox.width > SCREEN_WIDTH && _data->duck.state == FLYING && _data->duck.direction.x > 0)
	{
		sfVector2f position = sfSprite_getPosition(_data->duck.sprite);
		position.x = SCREEN_WIDTH - hitbox.width / 2;
		sfSprite_setPosition(_data->duck.sprite, position);
		_data->duck.direction.x *= -1;
		sfSprite_setScale(_data->duck.sprite, (sfVector2f) { -1, 1 });
	}

	// Collision en haut de l'écran
	if (hitbox.top < 0 && _data->duck.state == FLYING)
	{
		sfVector2f position = sfSprite_getPosition(_data->duck.sprite);
		position.y = hitbox.height / 2;
		sfSprite_setPosition(_data->duck.sprite, position);
		_data->duck.direction.y *= -1;
	}
	// Collision en bas de l'écran
	else if (hitbox.top + hitbox.height > SCREEN_HEIGHT-200)
	{
		switch (_data->duck.state)
		{
		case FLYING:
		{
			sfVector2f position = sfSprite_getPosition(_data->duck.sprite);
			position.y = SCREEN_HEIGHT - 200 - hitbox.height / 2;
			sfSprite_setPosition(_data->duck.sprite, position);
			_data->duck.direction.y *= -1;
			break;
		}
		case FALLING:
			_data->duck.direction.y = 0;
			_data->duck.direction.x = 0;
			_data->duck.state = DEAD;
			break;
		}
	}
	if (_data->duck.state == FLEEING && (hitbox.top < 0 || hitbox.left < -hitbox.width || hitbox.left > SCREEN_WIDTH))
	{
		RoundStart(_data);
	}
}

int GetMouseColision(struct EssentialData* _data)
{
	sfFloatRect hitbox = sfSprite_getGlobalBounds(_data->duck.sprite);
	sfVector2i mousePosition = sfMouse_getPositionRenderWindow(_data->renderWindow);
	if (sfFloatRect_contains(&hitbox, (float)mousePosition.x, (float)mousePosition.y))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void MoveCrosshairWithMouse(sfSprite* _sprite, sfVector2i _mousePosition)
{
	sfSprite_setPosition(_sprite, (sfVector2f) { (float)_mousePosition.x, (float)_mousePosition.y });
}

void MoveSprite(struct Sprite* _sprite, float _deltaTime)
{
	float angleRad = atan2f(_sprite->direction.y, _sprite->direction.x);
	sfVector2f move = { MAX_SPEED * cosf(angleRad) * _deltaTime, MAX_SPEED * sinf(angleRad) * _deltaTime };
	sfSprite_move(_sprite->sprite, move);
}