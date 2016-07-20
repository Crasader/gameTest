//
//  BirdFlightMenuScene.h
//  gameTest
//
//  Created by Rudy Tan on 7/15/16.
//
//

#pragma once

#include "cocos2d.h"

class BirdFlightMenuScene : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    CREATE_FUNC(BirdFlightMenuScene);
    void menuPlay(Ref* pSender);
};