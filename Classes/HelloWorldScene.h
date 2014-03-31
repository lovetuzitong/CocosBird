#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
USING_NS_CC;

class HelloWorld : public cocos2d::Layer
{
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();  
    
    // a selector callback
    void menuCloseCallback(Object* pSender);
    
	virtual void onEnter() override;
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);

	void setPhysicalWorld(PhysicsWorld* world){m_world = world ;};

	Sprite* addGrossiniAtPosition(Point p);
	bool onTouchBegan(Touch *touch, Event *unused_event); 
	void onTouchEnded(Touch *touch, Event *unused_event);

private:
	PhysicsWorld* m_world;
};

#endif // __HELLOWORLD_SCENE_H__