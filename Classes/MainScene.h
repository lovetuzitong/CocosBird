#pragma once

#include "cocos2d.h"
USING_NS_CC;

class MainScene : 
	public Layer
{
public:

	MainScene();
	~MainScene();

	static Scene* createScene();

	virtual bool init();

	CREATE_FUNC(MainScene);

	Sprite* addSpriteAtPosition(Point p);

	PhysicsWorld* mWorld;

	void setupBackground();

	void startGameCallback(Object* sender);

	void start();

	Sprite* bird;

};