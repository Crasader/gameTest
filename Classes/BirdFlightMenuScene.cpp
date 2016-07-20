//
//  BirdFlightMenuScene.cpp
//  gameTest
//
//  Created by Rudy Tan on 7/15/16.
//
//

#include "BirdFlightMenuScene.h"
#include <iostream>

USING_NS_CC;

Scene* BirdFlightMenuScene::createScene()
{
    auto menuScene = Scene::create();
    auto layer = BirdFlightMenuScene::create();
    menuScene->addChild(layer);
    
    return menuScene;
}

bool BirdFlightMenuScene::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    
    SpriteFrameCache* cache = SpriteFrameCache::getInstance();
    cache->addSpriteFramesWithFile("BirdFlight/Assets/bird_flight.plist");
    
    // background
    Sprite *cover = Sprite::createWithSpriteFrameName("cover.png");
    cover->setAnchorPoint(Vec2(0.5,1));
    cover->setPosition(Vec2(visibleSize.width/2, visibleSize.height));
    this->addChild(cover,-1);
    
    
    Sprite *buttonPlay = Sprite::createWithSpriteFrameName("button_play.png");
    buttonPlay->setAnchorPoint(Vec2(0.5,0));
    // spritebatch->addChild(buttonPlay);
    
    auto scaleTo = ScaleTo::create(0,0.6);
    buttonPlay->runAction(scaleTo);
    //this->addChild(buttonPlay);
    
    // auto menuItem = MenuItemImage::create(buttonPlay);
    // auto menuItem = MenuItemImage::create("button_play.png", "button_play.png",
    //                                       CC_CALLBACK_1(BirdFlightMenuScene::menuPlay, this));
    
    auto *menuItem = MenuItemSprite::create(buttonPlay, buttonPlay ,CC_CALLBACK_1(BirdFlightMenuScene::menuPlay, this));
    menuItem->setAnchorPoint(Vec2(0.5,0));
    menuItem->setPosition(Vec2(visibleSize.width/2, 200));
    std::cout << visibleSize.width/2;
    
    auto menu = Menu::create(menuItem, NULL);
    menu->setPosition(Vec2(0,0));
    this->addChild(menu, 1);
    
    /*
    auto sprite = Sprite::create("decepticon.png");
    auto sprite2 = Sprite::create("autobot.png");
    sprite->setAnchorPoint(Vec2(0.0, 0.0));
    sprite2->setAnchorPoint(Vec2(0.0, 0.0));
    
    sprite->setPosition(100, 100);
    sprite2->setPosition(0, 0);
    sprite->addChild(sprite2);
    
    this->addChild(sprite, 0);
    */
    return true;
}

void BirdFlightMenuScene::menuPlay(Ref* pSender){
    
}
