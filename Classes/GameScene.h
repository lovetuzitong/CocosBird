#pragma once

#include "cocos2d.h"

USING_NS_CC;

enum GameStatus
{
	READY, STARTED, PAUSE, OVER
};

class GameScene
	: public Layer
{
public:
	GameScene();
	~GameScene();

	static Scene* createScene();

	virtual bool init();

	CREATE_FUNC(GameScene);

	void setupBackground();
	void setupBird();

	void pauseCallback(Object* sender);

	void addObstacle(float dt);

	void showScore();

	void update(float dt);

	void btnCallback(Object* sender);

	//virtual void onEnter();

private:
	Size winSize;
	Sprite* bird;
	Node* logoContainer;
	GameStatus mGameStatus;
	int mCurrentScore;

	Node* getNumberSprite(int type, int num);
};

