#include "MainScene.h"
#include "GameScene.h"

enum 
{
	kTagGameStart,
	kTagGameRank
};

MainScene::MainScene(){

}

MainScene::~MainScene(){

}

Scene* MainScene::createScene(){
	//auto scene = Scene::create();
	auto scene = Scene::createWithPhysics();

	//scene->getPhysicsWorld()->setGravity(Point(0, -250));

	auto layer = MainScene::create();
	scene->addChild(layer);

	return scene;
}

bool MainScene::init(){
	if( !Layer::init() ){
		return false;

	}

	Size size = Director::getInstance()->getVisibleSize();

	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("flappy_bird.plist");

	Sprite* bg = Sprite::createWithSpriteFrameName("background_day.png");
	bg->setPosition(size.width/2, size.height/2);
	this->addChild(bg, 0);
	Sprite* floor = Sprite::createWithSpriteFrameName("floor_bg.png");
	floor->setPosition(size.width/2, 40);
	this->addChild(floor, 0);

	Sprite* gameName = Sprite::createWithSpriteFrameName("img_game_name.png");
	gameName->setPosition(size.width/2, size.height/2+80);
	this->addChild(gameName, 1);

	srand((unsigned)time(0));

	int n = 1+rand()%3;
	char * birdChar = nullptr;
	switch (n)
	{
	case 1:
		birdChar = "bird_blue_fly_%d.png";
		break;
	case 2:
		birdChar = "bird_red_fly_%d.png";
		break;
	case 3:
		birdChar = "bird_yellow_fly_%d.png";
		break;
	default:
		break;
	}

	Vector<SpriteFrame*> flyFrames(3);

	char str[50] = {0};
	for(int i = 0; i < 3; i++) 
	{
		sprintf(str, birdChar, i+1);
		//log(str);
		auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName( str );
		flyFrames.pushBack(frame);
	}

	auto animation = Animation::createWithSpriteFrames(flyFrames, 0.15f);
	bird = Sprite::createWithSpriteFrameName("bird_blue_fly_1.png");

	auto actionRepeat = RepeatForever::create( Animate::create(animation) );


	auto moveUp = MoveTo::create(0.4f, Point(size.width/2, size.height/2+5));
	auto moveDown = MoveTo::create(0.4f, Point(size.width/2, size.height/2-5));

	auto actionUpDown = Sequence::createWithTwoActions(moveUp, moveDown);

	//auto tmp = RepeatForever::create(Sequence::create(moveUp, moveDown));

	bird->runAction( actionRepeat );
	bird->runAction( RepeatForever::create( actionUpDown ) );
	bird->setPosition(size.width/2, size.height/2);
	this->addChild(bird, 1);


	// ADD MENU
	Sprite* btnStartGame = Sprite::createWithSpriteFrameName("btn_game_start.png");
	Sprite* btnGameRank = Sprite::createWithSpriteFrameName("btn_game_rank.png");

	auto item1 = MenuItemSprite::create(
		btnStartGame,
		NULL, CC_CALLBACK_1(MainScene::startGameCallback, this));
	item1->setTag(kTagGameStart);
	auto item2 = MenuItemSprite::create(
		btnGameRank,
		NULL, CC_CALLBACK_1(MainScene::startGameCallback, this));
	item2->setTag(kTagGameRank);

	auto menu = Menu::create(item1, item2, NULL);
	menu->alignItemsHorizontallyWithPadding(10.0f);
	menu->setPosition(size.width/2, size.height/2-120);
	this->addChild(menu, 1);

	// end of add menu
	
	/*
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch* touch, Event* event){
		CCLOG("on touch begin");
		addSpriteAtPosition(touch->getLocation());
		return true;
	};
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

	auto node = Node::create();
	node->setPhysicsBody(PhysicsBody::createEdgeBox(size));
	node->setPosition(size.width/2, size.height/2);
	this->addChild(node);
	*/
	return true;
}

void MainScene::startGameCallback(Object* sender){
	Sprite* sp = static_cast<Sprite*>(sender);
	auto scale = ScaleBy::create(0.05f, 1.1f);
	sp->runAction(Sequence::create(scale, scale->reverse(), CallFunc::create(CC_CALLBACK_0(MainScene::start, this)), NULL));
}

void MainScene::setupBackground(){

}

void MainScene::start(){
	log("start game here");
	Director::getInstance()->replaceScene(GameScene::createScene());
}

Sprite* MainScene::addSpriteAtPosition(Point p){
	Sprite* sprite = Sprite::createWithSpriteFrameName("bird_blue_fly_1.png");
	PhysicsBody* spriteBody = PhysicsBody::createBox(sprite->getContentSize());
	
	sprite->setPhysicsBody(spriteBody);
	this->addChild(sprite);
	sprite->setPosition(p);

	return sprite;
}