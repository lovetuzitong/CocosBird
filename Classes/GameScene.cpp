#include "GameScene.h"
#include "SimpleAudioEngine.h"

#define GAP_SIZE 100;

enum{
	kTagActionUpDpwn
};

enum {
	kTagSpriteBird,
	kTagSpriteObtacle,
	kTagSpriteScore,
	kTagPassed
};

GameScene::GameScene() :
	logoContainer(NULL),
	mGameStatus(GameStatus::READY),
	mCurrentScore(0)
{
	
}


GameScene::~GameScene()
{
}

Scene* GameScene::createScene(){

	auto scene = Scene::createWithPhysics();
	// scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
	scene->getPhysicsWorld()->setGravity(Point(0, -400));

	auto layer = GameScene::create();

	scene->addChild(layer);

	return scene;
}

void GameScene::setupBackground(){
	srand((unsigned)time(0));

	int n = 1+rand()%2;
	char * birdChar = nullptr;
	switch (n)
	{
	case 1:
		birdChar = "background_day.png";
		break;
	case 2:
		birdChar = "background_night.png";
		break;
	default:
		break;
	}

	Sprite* bg = Sprite::createWithSpriteFrameName(birdChar);
	bg->setPosition(winSize.width/2, winSize.height/2);
	this->addChild(bg, 0);

	/*
	Sprite* floor = Sprite::createWithSpriteFrameName("floor_bg.png");
	floor->setPosition(winSize.width/2, 40);
	this->addChild(floor, 1);

	float height = floor->getContentSize().height/2+39;
	*/

	Sprite* floor1 = Sprite::createWithSpriteFrameName("floor_bg.png");
	floor1->setPosition(winSize.width/2, 40);
	this->addChild(floor1, 1);
	auto move1 = MoveTo::create(2.8f, Point(-winSize.width/2, 40));
	auto reset1 = Place::create(Point(winSize.width/2, 40));
	floor1->runAction(RepeatForever::create(Sequence::createWithTwoActions(move1, reset1)));

	Sprite* floor2 = Sprite::createWithSpriteFrameName("floor_bg.png");
	floor2->setPosition(winSize.width*3/2, 40);
	this->addChild(floor2, 1);
	auto move2 = MoveTo::create(2.8f, Point(winSize.width/2, 40));
	auto reset2 = Place::create(Point(winSize.width*3/2, 40));
	floor2->runAction(RepeatForever::create(Sequence::createWithTwoActions(move2, reset2)));

	float height = floor1->getContentSize().height/2+39;

	auto node = Node::create();
	PhysicsBody* body = PhysicsBody::createEdgeSegment(Point(0,height), Point(320,height));
	node->setPhysicsBody(body);
	node->setPosition(Point::ZERO);
	this->addChild(node);

	
	Sprite* scoreSprite = Sprite::createWithSpriteFrameName("num_big_0.png");
	//Sprite* scoreSprite = (Sprite*)getNumberSprite(1, 10);
	scoreSprite->setPosition(winSize.width/2, winSize.height-80);
	scoreSprite->setTag(kTagSpriteScore);
	this->addChild(scoreSprite, 3);
	
}

bool GameScene::init(){
	if( !Layer::init() ){
		return false;
	}

	CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("sfx_hit.wav");
	CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("sfx_point.wav");
	CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("sfx_wing.wav");

	logoContainer = Node::create();

	winSize = Director::getInstance()->getVisibleSize();

	setupBackground();

	Sprite* guide = Sprite::createWithSpriteFrameName("img_game_guide.png");
	guide->setPosition(winSize.width/2, winSize.height/2);
	//this->addChild(guide, 1);

	Sprite* readyLabel = Sprite::createWithSpriteFrameName("img_game_ready.png");
	readyLabel->setPosition(winSize.width/2, winSize.height/2+100);
	//this->addChild(readyLabel, 1);

	logoContainer->addChild(guide);
	logoContainer->addChild(readyLabel);

	this->addChild(logoContainer, 1);

	setupBird();

	auto touchListener = EventListenerTouchOneByOne::create();

	touchListener->onTouchBegan = [=](Touch* touch, Event* event){
		log("touch began");

		//auto fadeOut = FadeIn::create(1.5f);

		if(mGameStatus == GameStatus::STARTED){

			// bird fly
			bird->getPhysicsBody()->setVelocity(Point(0, 300));

			CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sfx_wing.wav");
			
		}

		if(mGameStatus == GameStatus::READY){
			logoContainer->removeFromParent();

			bird->getActionManager()->removeActionByTag(kTagActionUpDpwn, bird);

			bird->getPhysicsBody()->setDynamic(true);

			this->schedule(schedule_selector(GameScene::addObstacle), 1.4f);

			mGameStatus = GameStatus::STARTED;
		}

		return false;
	};

	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);

	MenuItemSprite* pauseItem = MenuItemSprite::create(
		Sprite::createWithSpriteFrameName("btn_game_pause.png"),
		NULL, CC_CALLBACK_1(GameScene::pauseCallback, this));
	pauseItem->setPosition(winSize.width - 20, winSize.height - 20);

	auto menu = Menu::create(pauseItem, NULL);
	menu->setPosition(Point::ZERO);
	this->addChild(menu, 1);
	/*
	if(mGameStatus == GameStatus::STARTED){
		this->schedule(schedule_selector(GameScene::addObstacle), 1.6f);
	}
	*/

	// Åö×²¼ì²â
	auto contactListener = EventListenerPhysicsContact::create();
	contactListener->onContactBegin = [this](EventCustom* event, const PhysicsContact& contact){

		auto spA = static_cast<Sprite*>(contact.getShapeA()->getBody()->getNode());
		auto spB = static_cast<Sprite*>(contact.getShapeB()->getBody()->getNode());
		if(spA->getTag() == kTagSpriteBird || spB->getTag() == kTagSpriteBird){
			if(mGameStatus != GameStatus::OVER){
				CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sfx_hit.wav");
				log("bird dead...");
				this->unschedule(schedule_selector(GameScene::addObstacle));
				this->unscheduleUpdate();
				spA->getActionManager()->removeAllActions();
				mGameStatus = GameStatus::OVER;
				showScore();
			}
			
		}

		return true;
	};

	_eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);

	this->scheduleUpdate();


	return true;
}

void GameScene::update(float dt){

	if(mGameStatus == OVER) return;

	auto world = this->getScene()->getPhysicsWorld();
	Vector<PhysicsBody*> allBodys = world->getAllBodies();
	for(auto &body : allBodys){
		Sprite* obs = static_cast<Sprite*>(body->getNode());
		if(obs->getTag() == kTagSpriteObtacle){
			float x = obs->getPositionX();
			if(x < bird->getPositionX()){

				Node* score = this->getChildByTag(kTagSpriteScore);

				if(score != nullptr){
					score->removeFromParent();
				}

				CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sfx_point.wav");

				mCurrentScore += 1;
				log("score is: %d", mCurrentScore);

				Sprite* scoreSprite = static_cast<Sprite*>(getNumberSprite(1, mCurrentScore));
				scoreSprite->setPosition(winSize.width/2, winSize.height-80);
				scoreSprite->setTag(kTagSpriteScore);
				this->addChild(scoreSprite);

				obs->setTag(kTagPassed);
			}
		}

		if(obs->getPositionX() <= -40){
			obs->removeFromParentAndCleanup(true);
			log("removed");
		}

	}
}

Node* GameScene::getNumberSprite(int type, int num){
	Node* node = Node::create();

	int _hundred = num / 100;
	int _ten = (num - _hundred*100)/10;
	int _bits = num % 10;

	log("%d,%d,%d", _hundred,_ten, _bits);

	char* str;

	switch (type)
	{
	case 1:
		str = "num_big_%d.png";
		break;
	case 2:
		str = "num_medium_%d.png";
		break;
	case 3:
		str = "num_small_%d.png";
		break;
	default:
		break;
	}

	char bStr[50] = {0};
	sprintf(bStr, str, _bits);
	char tStr[50] = {0};
	sprintf(tStr, str, _ten);
	char hStr[50] = {0};
	sprintf(hStr, str, _hundred);

	if(num < 10){
		log("number create here: %s", bStr);
		
		Sprite* _bSprite = Sprite::createWithSpriteFrameName(bStr);
		_bSprite->setPosition(Point::ZERO);
		node->addChild(_bSprite);
	}else if(num >= 10 && num <100){

		log("number create here: %s, %s", bStr, tStr);

		Sprite* _bSprite = Sprite::createWithSpriteFrameName(bStr);
		_bSprite->setPosition(_bSprite->getContentSize().width, 0);
		Sprite* _tSprite = Sprite::createWithSpriteFrameName(tStr);
		_tSprite->setPosition(Point::ZERO);
		node->addChild(_bSprite);
		node->addChild(_tSprite);
	}else if(num >= 100){
		Sprite* _bSprite = Sprite::createWithSpriteFrameName(bStr);
		_bSprite->setPosition(_bSprite->getContentSize().width*2, 0);
		Sprite* _tSprite = Sprite::createWithSpriteFrameName(tStr);
		_tSprite->setPosition(_bSprite->getContentSize().width, 0);
		Sprite* _hSprite = Sprite::createWithSpriteFrameName(hStr);
		_hSprite->setPosition(Point::ZERO);
		node->addChild(_bSprite);
		node->addChild(_tSprite);
		node->addChild(_hSprite);
	}

	return node;
}

void GameScene::showScore(){

	// save user data

	bool drawNew = false;

	int oldScore = UserDefault::getInstance()->getIntegerForKey("highest_score");
	if(mCurrentScore > oldScore){
		UserDefault::getInstance()->setIntegerForKey("highest_score", mCurrentScore);
		UserDefault::getInstance()->flush();
		drawNew = true;
	}

	Node* menuContanier = Node::create();

	Sprite* overLogo = Sprite::createWithSpriteFrameName("img_game_over.png");
	overLogo->setPosition(winSize.width/2, winSize.height);
	menuContanier->addChild(overLogo);

	auto gameOverAnim = EaseIn::create(MoveTo::create(0.5f, Point(winSize.width/2, winSize.height/2+100)), 0.2f);

	overLogo->runAction(gameOverAnim);

	Sprite* menuBackground = Sprite::createWithSpriteFrameName("result_bord_bg.png");
	menuBackground->setPosition(winSize.width/2, winSize.height/2);
	menuContanier->addChild(menuBackground);

	Size menuBgSize = menuBackground->getContentSize();

	Sprite* prize = Sprite::createWithSpriteFrameName("prize_level_4.png");
	prize->setPosition(50, 50);
	menuBackground->addChild(prize);

	Sprite* currentScore = (Sprite*)getNumberSprite(2, mCurrentScore);
	currentScore->setPosition(menuBgSize.width-48, menuBgSize.height-45);
	menuBackground->addChild(currentScore);

	Sprite* bestScore = (Sprite*)getNumberSprite(2, UserDefault::getInstance()->getIntegerForKey("highest_score"));
	bestScore->setPosition(menuBgSize.width-50, 25);
	menuBackground->addChild(bestScore);
	
	// add new label
	if(drawNew){
		Sprite* newLogo = Sprite::createWithSpriteFrameName("icon_rank_new.png");
		newLogo->setPosition(menuBgSize.width-80, 25);
		menuBackground->addChild(newLogo);
	}

	// ADD MENU
	Sprite* btnStartGame = Sprite::createWithSpriteFrameName("btn_game_start.png");
	Sprite* btnGameRank = Sprite::createWithSpriteFrameName("btn_game_rank.png");

	auto item1 = MenuItemSprite::create(
		btnStartGame,
		NULL, CC_CALLBACK_1(GameScene::btnCallback, this));
	auto item2 = MenuItemSprite::create(
		btnGameRank,
		NULL, NULL);

	auto menu = Menu::create(item1, item2, NULL);
	menu->alignItemsHorizontallyWithPadding(10.0f);
	menu->setPosition(winSize.width/2, winSize.height/2-100);
	menuContanier->addChild(menu);

	auto fadeAction = FadeIn::create(2.5f);

	menu->runAction(fadeAction);


	this->addChild(menuContanier, 3);
	menuContanier->setPosition(Point::ZERO);

}

void GameScene::btnCallback(Object* sender){
	log("Click");
	Director::getInstance()->replaceScene(GameScene::createScene());
}

void GameScene::addObstacle(float dt){

	

	Sprite* obstacleUp = Sprite::createWithSpriteFrameName("obstacle_green_1.png");
	Sprite* obstacleDown = Sprite::createWithSpriteFrameName("obstacle_green_2.png");

	Size obstacleSize = obstacleDown->getContentSize();

	float halfObstacleHeight = obstacleSize.height/2;
	float downY = CCRANDOM_0_1() * 10 - 6;

	float upY =(halfObstacleHeight - (winSize.height - halfObstacleHeight+downY+100))+winSize.height;

	obstacleUp->setPosition(winSize.width+40, winSize.height-45 - downY*10 );
	obstacleDown->setPosition(winSize.width+40, 45 - downY*10);

	float floorOffsetHeight = halfObstacleHeight + obstacleDown->getPositionY() - 100;
	float topOffsetHeight = (winSize.height - obstacleUp->getPositionY())+halfObstacleHeight;
	
	log("floorOffsetHeight = %f",halfObstacleHeight - floorOffsetHeight);

	auto obsUpBody = PhysicsBody::createBox(
		Size(obstacleSize.width, topOffsetHeight), 
		PHYSICSBODY_MATERIAL_DEFAULT, 
		Point(0, -(obstacleSize.height-topOffsetHeight)/2));
	obsUpBody->setDynamic(false);
	obstacleUp->setPhysicsBody(obsUpBody);
	auto obsDownBody = PhysicsBody::createBox(
		Size(obstacleSize.width, floorOffsetHeight), 
		PHYSICSBODY_MATERIAL_DEFAULT, 
		Point(0, (obstacleSize.height-floorOffsetHeight)/2));
	obsDownBody->setDynamic(false);
	obstacleDown->setPhysicsBody(obsDownBody);
	
	auto moveTo = MoveTo::create(3.0f, Point(-50, obstacleUp->getPositionY()));
	obstacleUp->runAction(moveTo);
	auto moveTo2 = MoveTo::create(3.0f, Point(-50, obstacleDown->getPositionY()));
	obstacleDown->runAction(moveTo2);

	obstacleDown->setTag(kTagSpriteObtacle);

	this->addChild(obstacleUp, 0);
	this->addChild(obstacleDown, 0);
}

void GameScene::pauseCallback(Object* sender){
	
	auto world = this->getScene()->getPhysicsWorld();
	if(world->getDebugDrawMask() == PhysicsWorld::DEBUGDRAW_NONE){
		world->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
	}else{
		world->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_NONE);
	}
	/*
	if(Director::getInstance()->isPaused()){
		Director::getInstance()->resume();
	}else{
		Director::getInstance()->pause();
	}
	*/
}

void GameScene::setupBird(){

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
		auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName( str );
		flyFrames.pushBack(frame);
	}

	auto animation = Animation::createWithSpriteFrames(flyFrames, 0.15f);
	bird = Sprite::createWithSpriteFrameName("bird_blue_fly_1.png");
	bird->setTag(kTagSpriteBird);

	auto actionRepeat = RepeatForever::create( Animate::create(animation) );


	auto moveUp = MoveTo::create(0.4f, Point(winSize.width/2 - 60, winSize.height/2+15+5));
	auto moveDown = MoveTo::create(0.4f, Point(winSize.width/2 - 60, winSize.height/2+15-5));

	auto actionUpDown = Sequence::createWithTwoActions(moveUp, moveDown);

	auto upDownRepeat = RepeatForever::create( actionUpDown );
	upDownRepeat->setTag(kTagActionUpDpwn);

	bird->runAction( actionRepeat );
	bird->runAction( upDownRepeat );
	bird->setPosition(winSize.width/2 - 60, winSize.height/2 + 15);

	//PhysicsBody* birdBody = PhysicsBody::createBox(bird->getContentSize());
	PhysicsBody* birdBody = PhysicsBody::createCircle(bird->getContentSize().height/2);
	birdBody->setDynamic(false);

	bird->setPhysicsBody(birdBody);
	this->addChild(bird, 2);
}

/*
void GameScene::onEnter(){
	
	
	
}
*/