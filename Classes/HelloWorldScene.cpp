#include "HelloWorldScene.h"

USING_NS_CC;

extern const char* PHYSICSCONTACT_EVENT_NAME;

Scene* HelloWorld::createScene()
{
     // 'scene' is an autorelease object
    //auto scene = Scene::create();
	auto scene = Scene::createWithPhysics();
	scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);

	auto layer = HelloWorld::create();
	layer->setPhysicalWorld(scene->getPhysicsWorld());

	scene->addChild(layer);

    return scene;

}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }

	Size visiableSize = Director::getInstance()->getVisibleSize();
	Point origin = Director::getInstance()->getVisibleOrigin();

	auto edge = Sprite::create();
	auto body = PhysicsBody::createEdgeBox(visiableSize, PHYSICSBODY_MATERIAL_DEFAULT, 2);
	edge->setPosition(visiableSize.width/2, visiableSize.height/2);
	edge->setPhysicsBody(body);
	this->addChild(edge);

	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("flappy_bird.plist");

	this->setTouchEnabled(true);

	auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->onTouchBegan = CC_CALLBACK_2(HelloWorld::onTouchBegan, this);
	touchListener->onTouchEnded = CC_CALLBACK_2(HelloWorld::onTouchEnded, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);


    return true;
}

void HelloWorld::onEnter(){
	

}

Sprite* HelloWorld::addGrossiniAtPosition(Point p){
	log("Add Sprite at %0.2f,%0.2f", p.x, p.y);
	Sprite* sp = Sprite::createWithSpriteFrameName("bird_blue_fly_1.png");
	sp->setPhysicsBody(PhysicsBody::createBox(sp->getContentSize()));
	this->addChild(sp);

	return sp;
}

void HelloWorld::onTouchEnded(Touch *touch, Event *unused_event){
	log("touch ends");
		auto location = touch->getLocation();
		addGrossiniAtPosition( location );

}

bool HelloWorld::onTouchBegan(Touch *touch, Event *unused_event){
	log("touch start");
	return true;
}


void HelloWorld::menuCloseCallback(Object* pSender)
{
	if(m_world->getDebugDrawMask() != PhysicsWorld::DEBUGDRAW_NONE){
		m_world->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_NONE);
	}else{
		m_world->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
	}
}
