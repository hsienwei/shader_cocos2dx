#include "HelloWorldScene.h"

USING_NS_CC;

class EdgeMask : public CCSprite
{
public:
	static EdgeMask* create(const std::string& filename);
private:
	void initProgram();
};

EdgeMask* EdgeMask::create(const std::string& filename)
{
	EdgeMask *sprite = new EdgeMask();
	if (sprite && sprite->initWithFile(filename))
	{
		sprite->autorelease();
		sprite->initProgram();
		return sprite;
	}
	CC_SAFE_DELETE(sprite);
	return NULL;
}

void EdgeMask::initProgram()
{
	std::string sharderStr = CCFileUtils::getInstance()->fullPathForFilename("Shaders/shader_edge.fsh");
	GLchar * fragSource = (GLchar*)CCString::createWithContentsOfFile(sharderStr.c_str())->getCString();
	CCGLProgram* pProgram = new CCGLProgram();
	pProgram->initWithVertexShaderByteArray(ccPositionTextureColor_vert, fragSource);
	setShaderProgram(pProgram);
	pProgram->release();

	CHECK_GL_ERROR_DEBUG();

	getShaderProgram()->addAttribute(kCCAttributeNamePosition, kCCVertexAttrib_Position);
	getShaderProgram()->addAttribute(kCCAttributeNameColor, kCCVertexAttrib_Color);
	getShaderProgram()->addAttribute(kCCAttributeNameTexCoord, kCCVertexAttrib_TexCoords);

	CHECK_GL_ERROR_DEBUG();

	getShaderProgram()->link();

	CHECK_GL_ERROR_DEBUG();

	getShaderProgram()->updateUniforms();

	CHECK_GL_ERROR_DEBUG();
}

//=================================================================

class DynamicBackground : public CCLayerColor
{
public:
	static DynamicBackground* create();
	void setBackgroundPic();
private:

	GLint _bgLocation;
	GLint _bgSize;
	GLint _bgTextureCoordOffset;
	GLfloat offset[2];

	Texture2D *_bgTexture;

	

	void initProgram();
	void draw();
	void update(float dt);

};

DynamicBackground* DynamicBackground::create()
{
	DynamicBackground *layer = new DynamicBackground();
	if (layer && layer->initWithColor(ccc4(255, 255, 255, 255)))
	{
		layer->autorelease();
		layer->_bgTexture = NULL;
		layer->offset[0] = 0.0f;
		layer->offset[1] = 0.0f;
		return layer;
	}
	CC_SAFE_DELETE(layer);
	return NULL;
}

void DynamicBackground::setBackgroundPic()
{

	_bgTexture = CCTextureCache::sharedTextureCache()->addImage("Hive_background.PNG");
	//_maskTexture->setAliasTexParameters();
	_bgTexture->retain();

	this->initProgram();
	this->scheduleUpdate();
}

void DynamicBackground::initProgram()
{
	std::string sharderStr = CCFileUtils::getInstance()->fullPathForFilename("Shaders/shader_dynamic_bg.fsh");
	GLchar * fragSource = (GLchar*)CCString::createWithContentsOfFile(sharderStr.c_str())->getCString();

	sharderStr = CCFileUtils::getInstance()->fullPathForFilename("Shaders/shader_very_pos_color.vsh");
	GLchar * vertexSource = (GLchar*)CCString::createWithContentsOfFile(sharderStr.c_str())->getCString();

	CCGLProgram* pProgram = new CCGLProgram();
	pProgram->initWithVertexShaderByteArray(vertexSource, fragSource);
	setShaderProgram(pProgram);
	pProgram->release();

	CHECK_GL_ERROR_DEBUG();

	getShaderProgram()->addAttribute(kCCAttributeNamePosition, kCCVertexAttrib_Position);
	getShaderProgram()->addAttribute(kCCAttributeNameColor, kCCVertexAttrib_Color);
	getShaderProgram()->addAttribute(kCCAttributeNameTexCoord, kCCVertexAttrib_TexCoords);

	CHECK_GL_ERROR_DEBUG();

	getShaderProgram()->link();

	CHECK_GL_ERROR_DEBUG();

	getShaderProgram()->updateUniforms();

	CHECK_GL_ERROR_DEBUG();

	_bgLocation = glGetUniformLocation(getShaderProgram()->getProgram(), "u_bg_texture");
	_bgSize = glGetUniformLocation(getShaderProgram()->getProgram(), "u_bg_size");
	_bgTextureCoordOffset = glGetUniformLocation(getShaderProgram()->getProgram(), "u_bg_CoordOffset");

	CHECK_GL_ERROR_DEBUG();
}

void DynamicBackground::draw()
{
	CC_NODE_DRAW_SETUP();

	GL::enableVertexAttribs(GL::VERTEX_ATTRIB_FLAG_POSITION | GL::VERTEX_ATTRIB_FLAG_COLOR);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, _bgTexture->getName());
	glUniform1i(_bgLocation, 1);
	glUniform2f(_bgSize, _bgTexture->getContentSize().width, _bgTexture->getContentSize().height);
	glUniform2f(_bgTextureCoordOffset, offset[0], offset[1]);
	//
	// Attributes
	//
#ifdef EMSCRIPTEN
	setGLBufferData(_squareVertices, 4 * sizeof(Vertex2F), 0);
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, 0, 0);

	setGLBufferData(_squareColors, 4 * sizeof(Color4F), 1);
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_COLOR, 4, GL_FLOAT, GL_FALSE, 0, 0);
#else
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, 0, _squareVertices);
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_COLOR, 4, GL_FLOAT, GL_FALSE, 0, _squareColors);
#endif // EMSCRIPTEN

	GL::blendFunc(_blendFunc.src, _blendFunc.dst);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	CC_INCREMENT_GL_DRAWS(1);

	glActiveTexture(GL_TEXTURE0);  //must set to texture0
}

void DynamicBackground::update(float dt)
{
	offset[0] += dt /3;
	offset[1] += dt /2;
}

//=================================================================

class LightLayer : public CCLayerColor
{
public:
	static LightLayer* create();
	void setSpotPosition(Point &point);
private:

	GLint _spotLocation;
	GLint _innerColor;
	GLint _spotInnerRadius;
	GLint _spotOuterRadius;

	Point _spotPoint;

	void initProgram();
	void draw();
};

LightLayer* LightLayer::create()
{
	LightLayer *layer = new LightLayer();
	if (layer && layer->initWithColor(ccc4(0, 0, 0, 255)))
	{
		layer->autorelease();
		layer->initProgram();
		return layer;
	}
	CC_SAFE_DELETE(layer);
	return NULL;
}

void LightLayer::initProgram()
{
	std::string sharderStr = CCFileUtils::getInstance()->fullPathForFilename("Shaders/shader_spot.fsh");
	GLchar * fragSource = (GLchar*)CCString::createWithContentsOfFile(sharderStr.c_str())->getCString();

	sharderStr = CCFileUtils::getInstance()->fullPathForFilename("Shaders/shader_very_pos_color.vsh");
	GLchar * vertexSource = (GLchar*)CCString::createWithContentsOfFile(sharderStr.c_str())->getCString();

	CCGLProgram* pProgram = new CCGLProgram();
	pProgram->initWithVertexShaderByteArray(vertexSource, fragSource);
	setShaderProgram(pProgram);
	pProgram->release();

	CHECK_GL_ERROR_DEBUG();

	getShaderProgram()->addAttribute(kCCAttributeNamePosition, kCCVertexAttrib_Position);
	getShaderProgram()->addAttribute(kCCAttributeNameColor, kCCVertexAttrib_Color);
	getShaderProgram()->addAttribute(kCCAttributeNameTexCoord, kCCVertexAttrib_TexCoords);

	CHECK_GL_ERROR_DEBUG();

	getShaderProgram()->link();

	CHECK_GL_ERROR_DEBUG();

	getShaderProgram()->updateUniforms();

	CHECK_GL_ERROR_DEBUG();

	_spotLocation = glGetUniformLocation(getShaderProgram()->getProgram(), "v_spot_pos");
	_innerColor = glGetUniformLocation(getShaderProgram()->getProgram(), "v_spot_innerColor");
	_spotInnerRadius = glGetUniformLocation(getShaderProgram()->getProgram(), "v_spot_innerRadius");
	_spotOuterRadius = glGetUniformLocation(getShaderProgram()->getProgram(), "v_spot_outerRadius");

	CHECK_GL_ERROR_DEBUG();

	_spotPoint = CCPoint(100, 100);
}

void LightLayer::setSpotPosition(Point &point)
{
	_spotPoint = Point(point);
}

void LightLayer::draw()
{
	CC_NODE_DRAW_SETUP();

	GL::enableVertexAttribs(GL::VERTEX_ATTRIB_FLAG_POSITION | GL::VERTEX_ATTRIB_FLAG_COLOR);
	glUniform3f(_spotLocation, _spotPoint.x, _spotPoint.y, 0);
	glUniform4f(_innerColor, 0, 0, 0, 0);
	glUniform1f(_spotInnerRadius, 50);
	glUniform1f(_spotOuterRadius, 220);

	//
	// Attributes
	//
#ifdef EMSCRIPTEN
	setGLBufferData(_squareVertices, 4 * sizeof(Vertex2F), 0);
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, 0, 0);

	setGLBufferData(_squareColors, 4 * sizeof(Color4F), 1);
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_COLOR, 4, GL_FLOAT, GL_FALSE, 0, 0);
#else
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, 0, _squareVertices);
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_COLOR, 4, GL_FLOAT, GL_FALSE, 0, _squareColors);
#endif // EMSCRIPTEN

	GL::blendFunc(_blendFunc.src, _blendFunc.dst);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	CC_INCREMENT_GL_DRAWS(1);
}

//=================================================================

PostProcessing* PostProcessing::create(Texture2D *texture)
{
	PostProcessing *layer = new PostProcessing();
	if (layer && layer->initWithTexture(texture))
	{
		layer->autorelease();
		layer->initProgram();
		return layer;
	}
	CC_SAFE_DELETE(layer);
	return NULL;
}

//bool PostProcessing::initWithTexture(CCTexture2D* texture, const CCRect& rect)
//{
//	if (CCSprite::initWithTexture(texture, rect))
//	{
//		CCSize s = getTexture()->getContentSizeInPixels();
//
//		return true;
//	}
//
//	return false;
//}

void PostProcessing::initProgram()
{
	std::string sharderStr = CCFileUtils::getInstance()->fullPathForFilename("Shaders/shader_postprocessing.fsh");
	GLchar * fragSource = (GLchar*)CCString::createWithContentsOfFile(sharderStr.c_str())->getCString();
	CCGLProgram* pProgram = new CCGLProgram();
	pProgram->initWithVertexShaderByteArray(ccPositionTextureColor_vert, fragSource);
	setShaderProgram(pProgram);
	pProgram->release();

	CHECK_GL_ERROR_DEBUG();

	getShaderProgram()->addAttribute(kCCAttributeNamePosition, kCCVertexAttrib_Position);
	getShaderProgram()->addAttribute(kCCAttributeNameColor, kCCVertexAttrib_Color);
	getShaderProgram()->addAttribute(kCCAttributeNameTexCoord, kCCVertexAttrib_TexCoords);

	CHECK_GL_ERROR_DEBUG();

	getShaderProgram()->link();

	CHECK_GL_ERROR_DEBUG();

	getShaderProgram()->updateUniforms();

	CHECK_GL_ERROR_DEBUG();

	_offsetLocation = glGetUniformLocation(getShaderProgram()->getProgram(), "u_offset");

	CHECK_GL_ERROR_DEBUG();

	this->scheduleUpdate();
}

void PostProcessing::update(float dt)
{
	static float elapsedTIme = 0.0f;
	elapsedTIme += dt;
	move = elapsedTIme * 2 * 3.14159 * .75;  // 3/4 of a wave cycle per second
}

void PostProcessing::draw()
{
	CC_PROFILER_START_CATEGORY(kProfilerCategorySprite, "CCSprite - draw");

	CCASSERT(!_batchNode, "If Sprite is being rendered by SpriteBatchNode, Sprite#draw SHOULD NOT be called");

	CC_NODE_DRAW_SETUP();

	glUniform1f(_offsetLocation, move);

	GL::blendFunc(_blendFunc.src, _blendFunc.dst);

	GL::bindTexture2D(_texture->getName());
	GL::enableVertexAttribs(GL::VERTEX_ATTRIB_FLAG_POS_COLOR_TEX);

#define kQuadSize sizeof(_quad.bl)
#ifdef EMSCRIPTEN
	long offset = 0;
	setGLBufferData(&_quad, 4 * kQuadSize, 0);
#else
	long offset = (long)&_quad;
#endif // EMSCRIPTEN

	// vertex
	int diff = offsetof(V3F_C4B_T2F, vertices);
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, kQuadSize, (void*)(offset + diff));

	// texCoods
	diff = offsetof(V3F_C4B_T2F, texCoords);
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_TEX_COORDS, 2, GL_FLOAT, GL_FALSE, kQuadSize, (void*)(offset + diff));

	// color
	diff = offsetof(V3F_C4B_T2F, colors);
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, kQuadSize, (void*)(offset + diff));


	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	CHECK_GL_ERROR_DEBUG();


#if CC_SPRITE_DEBUG_DRAW == 1
	// draw bounding box
	Point vertices[4] = {
		Point(_quad.tl.vertices.x, _quad.tl.vertices.y),
		Point(_quad.bl.vertices.x, _quad.bl.vertices.y),
		Point(_quad.br.vertices.x, _quad.br.vertices.y),
		Point(_quad.tr.vertices.x, _quad.tr.vertices.y),
	};
	ccDrawPoly(vertices, 4, true);
#elif CC_SPRITE_DEBUG_DRAW == 2
	// draw texture box
	Size s = this->getTextureRect().size;
	Point offsetPix = this->getOffsetPosition();
	Point vertices[4] = {
		Point(offsetPix.x, offsetPix.y), Point(offsetPix.x + s.width, offsetPix.y),
		Point(offsetPix.x + s.width, offsetPix.y + s.height), Point(offsetPix.x, offsetPix.y + s.height)
	};
	ccDrawPoly(vertices, 4, true);
#endif // CC_SPRITE_DEBUG_DRAW

	CC_INCREMENT_GL_DRAWS(1);

	CC_PROFILER_STOP_CATEGORY(kProfilerCategorySprite, "CCSprite - draw");
}

//=================================================================

class GrayMask : public CCSprite
{
public:
	static GrayMask* create(const std::string& filename);
private:
	void initProgram();
};

GrayMask* GrayMask::create(const std::string& filename)
{
	GrayMask *sprite = new GrayMask();
	if (sprite && sprite->initWithFile(filename))
	{
		sprite->autorelease();
		sprite->initProgram();
		return sprite;
	}
	CC_SAFE_DELETE(sprite);
	return NULL;
}

void GrayMask::initProgram()
{
	std::string sharderStr = CCFileUtils::getInstance()->fullPathForFilename("Shaders/shader_gray.fsh");
	GLchar * fragSource = (GLchar*)CCString::createWithContentsOfFile(sharderStr.c_str())->getCString();
	CCGLProgram* pProgram = new CCGLProgram();
	pProgram->initWithVertexShaderByteArray(ccPositionTextureColor_vert, fragSource);
	setShaderProgram(pProgram);
	pProgram->release();

	CHECK_GL_ERROR_DEBUG();

	getShaderProgram()->addAttribute(kCCAttributeNamePosition, kCCVertexAttrib_Position);
	getShaderProgram()->addAttribute(kCCAttributeNameColor, kCCVertexAttrib_Color);
	getShaderProgram()->addAttribute(kCCAttributeNameTexCoord, kCCVertexAttrib_TexCoords);

	CHECK_GL_ERROR_DEBUG();

	getShaderProgram()->link();

	CHECK_GL_ERROR_DEBUG();

	getShaderProgram()->updateUniforms();

	CHECK_GL_ERROR_DEBUG();
}

//=================================================================

class SpriteMask : public CCSprite
{
public:
	~SpriteMask();
	void setMask(const char *pMaskFile);
	static SpriteMask* create(const char *pszFileName);

private:

	bool initWithTexture(CCTexture2D* texture, const CCRect&  rect);
	void listenBackToForeground(CCObject *obj);
	void initProgram();
	void draw();

	CCTexture2D *_maskTexture;
	GLint _maskLocation;

	CCPoint offsetWithPosition;
	GLint _offsetLocation;

	bool _isSetMask;

};

SpriteMask::~SpriteMask()
{
	if (_maskTexture != NULL)
	{
		_maskTexture->release();
	}

	if (_isSetMask)
	{
		CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, EVNET_COME_TO_FOREGROUND);
	}
}

SpriteMask* SpriteMask::create(const char *pszFileName)
{
	SpriteMask* pRet = new SpriteMask();
	if (pRet && pRet->initWithFile(pszFileName))
	{
		pRet->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(pRet);
	}

	return pRet;
}

bool SpriteMask::initWithTexture(CCTexture2D* texture, const CCRect& rect)
{
	if (CCSprite::initWithTexture(texture, rect))
	{
		CCSize s = getTexture()->getContentSizeInPixels();
		_maskTexture = NULL;
		_isSetMask = false;
		return true;
	}

	return false;
}


void SpriteMask::setMask(const char *pMaskFile)
{
	if (_isSetMask)    return;

	_maskTexture = CCTextureCache::sharedTextureCache()->addImage(pMaskFile);
	//_maskTexture->setAliasTexParameters();
	_maskTexture->retain();

	CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(SpriteMask::listenBackToForeground), EVNET_COME_TO_FOREGROUND, NULL);

	initProgram();

	_isSetMask = true;
}

void SpriteMask::listenBackToForeground(CCObject *obj)
{
	setShaderProgram(NULL);
	initProgram();
}

void SpriteMask::initProgram()
{
	std::string sharderStr = CCFileUtils::getInstance()->fullPathForFilename("Shaders/shader_mask.fsh");
	GLchar * fragSource = (GLchar*)CCString::createWithContentsOfFile(sharderStr.c_str())->getCString();
	CCGLProgram* pProgram = new CCGLProgram();
	pProgram->initWithVertexShaderByteArray(ccPositionTextureColor_vert, fragSource);
	setShaderProgram(pProgram);
	pProgram->release();

	CHECK_GL_ERROR_DEBUG();

	getShaderProgram()->addAttribute(kCCAttributeNamePosition, kCCVertexAttrib_Position);
	getShaderProgram()->addAttribute(kCCAttributeNameColor, kCCVertexAttrib_Color);
	getShaderProgram()->addAttribute(kCCAttributeNameTexCoord, kCCVertexAttrib_TexCoords);

	CHECK_GL_ERROR_DEBUG();

	getShaderProgram()->link();

	CHECK_GL_ERROR_DEBUG();

	getShaderProgram()->updateUniforms();

	CHECK_GL_ERROR_DEBUG();

	_maskLocation = glGetUniformLocation(getShaderProgram()->getProgram(), "u_mask");
	_offsetLocation = glGetUniformLocation(getShaderProgram()->getProgram(), "v_offset");

	CHECK_GL_ERROR_DEBUG();
}

void SpriteMask::draw()
{
	if (!_isSetMask)   //沒有設定就直接用預設方法
	{
		CCSprite::draw();
		return;
	}

	CC_PROFILER_START_CATEGORY(kProfilerCategorySprite, "CCSprite - draw");

	CCASSERT(!_batchNode, "If Sprite is being rendered by SpriteBatchNode, Sprite#draw SHOULD NOT be called");

	CC_NODE_DRAW_SETUP();

	GL::blendFunc(_blendFunc.src, _blendFunc.dst);

	GL::bindTexture2D(_texture->getName());
	GL::enableVertexAttribs(GL::VERTEX_ATTRIB_FLAG_POS_COLOR_TEX);

	if (_texture != NULL) {
		ccGLBindTexture2D(_texture->getName());
	} else {
		ccGLBindTexture2D(0);
	}
	
	//mask 設定 設定使用mask貼圖
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, _maskTexture->getName());
	glUniform1i(_maskLocation, 1);
	glUniform2f(_offsetLocation, offsetWithPosition.x, offsetWithPosition.y);

#define kQuadSize sizeof(_quad.bl)
#ifdef EMSCRIPTEN
	long offset = 0;
	setGLBufferData(&_quad, 4 * kQuadSize, 0);
#else
	long offset = (long)&_quad;
#endif // EMSCRIPTEN

	// vertex
	int diff = offsetof(V3F_C4B_T2F, vertices);
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, kQuadSize, (void*)(offset + diff));

	// texCoods
	diff = offsetof(V3F_C4B_T2F, texCoords);
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_TEX_COORDS, 2, GL_FLOAT, GL_FALSE, kQuadSize, (void*)(offset + diff));

	// color
	diff = offsetof(V3F_C4B_T2F, colors);
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, kQuadSize, (void*)(offset + diff));


	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	CHECK_GL_ERROR_DEBUG();


#if CC_SPRITE_DEBUG_DRAW == 1
	// draw bounding box
	Point vertices[4] = {
		Point(_quad.tl.vertices.x, _quad.tl.vertices.y),
		Point(_quad.bl.vertices.x, _quad.bl.vertices.y),
		Point(_quad.br.vertices.x, _quad.br.vertices.y),
		Point(_quad.tr.vertices.x, _quad.tr.vertices.y),
	};
	ccDrawPoly(vertices, 4, true);
#elif CC_SPRITE_DEBUG_DRAW == 2
	// draw texture box
	Size s = this->getTextureRect().size;
	Point offsetPix = this->getOffsetPosition();
	Point vertices[4] = {
		Point(offsetPix.x, offsetPix.y), Point(offsetPix.x + s.width, offsetPix.y),
		Point(offsetPix.x + s.width, offsetPix.y + s.height), Point(offsetPix.x, offsetPix.y + s.height)
	};
	ccDrawPoly(vertices, 4, true);
#endif // CC_SPRITE_DEBUG_DRAW

	CC_INCREMENT_GL_DRAWS(1);

	glActiveTexture(GL_TEXTURE0);  //must set to texture0

	CC_PROFILER_STOP_CATEGORY(kProfilerCategorySprite, "CCSprite - draw");
}

//===================================================
const int LAYER_SPOT = 1;

HelloWorld::~HelloWorld()
{
	renderTexture->release();
}

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
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
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Point origin = Director::getInstance()->getVisibleOrigin();


	mainLayer = Layer::create();
	mainLayerParent = Layer::create();
	postProcessingLayer = Layer::create();
	uiLayer = Layer::create();

	this->addChild(mainLayerParent);
	mainLayerParent->addChild(mainLayer);
	this->addChild(postProcessingLayer);
	this->addChild(uiLayer);

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));
    
	closeItem->setPosition(Point(origin.x + visibleSize.width - closeItem->getContentSize().width/2 ,
                                origin.y + closeItem->getContentSize().height/2));

	auto switchItem = MenuItemImage::create(
											"switchuser.png",
											"switchuser.png",
											CC_CALLBACK_1(HelloWorld::switchCallback, this));

	switchItem->setPosition(Point(origin.x + visibleSize.width - closeItem->getContentSize().width - switchItem->getContentSize().width/2,
		origin.y + closeItem->getContentSize().height / 2));

    // create menu, it's an autorelease object
	auto menu = Menu::create(closeItem, switchItem, NULL);
    menu->setPosition(Point::ZERO);
	uiLayer->addChild(menu, 1);

    
	// shader 顯示

	SpriteMask* pSprite2 = SpriteMask::create("wall_77.png");
	pSprite2->setPosition(ccp(visibleSize.width / 2 - 90, visibleSize.height / 2));
	mainLayer->addChild(pSprite2, 0);

	SpriteMask* pSprite3 = SpriteMask::create("wall_77.png");
	pSprite3->setMask("wall0.png");
	pSprite3->setPosition(ccp(visibleSize.width / 2 -30, visibleSize.height / 2));
	mainLayer->addChild(pSprite3, 0);

	GrayMask* pSprite4 = GrayMask::create("wall_77.png");
	pSprite4->setPosition(ccp(visibleSize.width / 2 + 30, visibleSize.height / 2));
	mainLayer->addChild(pSprite4, 0);

	EdgeMask* pSprite5 = EdgeMask::create("wall_77.png");
	pSprite5->setPosition(ccp(visibleSize.width / 2 + 90, visibleSize.height / 2));
	mainLayer->addChild(pSprite5, 0);

	LightLayer *layer2 = LightLayer::create();
	layer2->setTag(LAYER_SPOT);
	mainLayer->addChild(layer2, 0);

	DynamicBackground *layer3 = DynamicBackground::create();
	layer3->setBackgroundPic();
	mainLayer->addChild(layer3, -1);

	renderTexture = RenderTexture::create(visibleSize.width, visibleSize.height);
	renderTexture->retain();

	mainLayerParent->setVisible(false);
	isPostProcessing = true;
	
	PostProcessing *test = PostProcessing::create(renderTexture->getSprite()->getTexture());
	test->setFlippedY(true);
	//test->setScale(0.4f);
	postProcessingLayer->addChild(test);
	test->setPosition(Point(test->boundingBox().size.width/2, 
							visibleSize.height - test->boundingBox().size.height / 2));
	
	// Register Touch Event
	auto listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);

	listener->onTouchBegan = CC_CALLBACK_2(HelloWorld::onTouchBegan, this);
	listener->onTouchMoved = CC_CALLBACK_2(HelloWorld::onTouchEnded, this);
	listener->onTouchEnded = CC_CALLBACK_2(HelloWorld::onTouchEnded, this);

	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

	this->scheduleUpdate();
	return true;
}

void HelloWorld::update(float deltaTime)
{
	renderTexture->begin();
	mainLayer->visit();
	renderTexture->end();
}

void HelloWorld::menuCloseCallback(Object* pSender)
{
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

void HelloWorld::switchCallback(Object* pSender)
{
	if (isPostProcessing)
	{
		mainLayerParent->setVisible(true);
		postProcessingLayer->setVisible(false);
	}
	else
	{
		mainLayerParent->setVisible(false);
		postProcessingLayer->setVisible(true);
	}
	isPostProcessing = !isPostProcessing;
}

bool HelloWorld::onTouchBegan(Touch *touch, Event *unused_event)
{
	onTouchEnded(touch, unused_event);
	return true;
}

void HelloWorld::onTouchEnded(Touch *touch, Event *unused_event)
{
	CCLog("touch");
	Point touchPos = touch->getLocation();
	LightLayer *layer = (LightLayer*)mainLayer->getChildByTag(LAYER_SPOT);
	layer->setSpotPosition(touchPos);
}
