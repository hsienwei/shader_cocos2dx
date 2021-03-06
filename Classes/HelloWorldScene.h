#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"


class PostProcessing : public cocos2d::Sprite
{

public:
	static PostProcessing* create(cocos2d::Texture2D *texture);
private:
	GLfloat move;
	GLint _offsetLocation;

	//bool initWithTexture(CCTexture2D* texture, const CCRect&  rect);
	void listenBackToForeground(cocos2d::CCObject *obj);
	void initProgram();
	void update(float dt);
	void draw();
	//void update(float dt);
};

class HelloWorld : public cocos2d::Layer
{
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();  
    
    // a selector callback
    void menuCloseCallback(Object* pSender);
	void switchCallback(Object* pSender);
    
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);

private:
	~HelloWorld();
	cocos2d::Layer *mainLayer, *mainLayerParent, *postProcessingLayer, *uiLayer;
	cocos2d::RenderTexture *renderTexture;
	bool isPostProcessing;

	void update(float deltaTime);

	bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *unused_event);
	void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *unused_event);
};

#endif // __HELLOWORLD_SCENE_H__
