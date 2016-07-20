//
//  BirdFlightPlay.h
//  gameTest
//
//  Created by Rudy Tan on 7/15/16.
//
//

#pragma once

#include "cocos2d.h"
#include "SimpleAudioEngine.h"

class BirdFlightPlayScene : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    CREATE_FUNC(BirdFlightPlayScene);
    //void menuPlay(Ref* pSender);
    //cocos2d::Vector<cocos2d::SpriteFrame> getAnimation(const char *format, int count);
    
    virtual bool onTouchBegan(cocos2d::Touch*, cocos2d::Event*);
    virtual void onTouchEnded(cocos2d::Touch*, cocos2d::Event*);
    virtual void onTouchMoved(cocos2d::Touch*, cocos2d::Event*);
    virtual void onTouchCancelled(cocos2d::Touch*, cocos2d::Event*);
    cocos2d::Sprite* getAnimation(const char* filenameFormat, bool isForever, int numberOfFrames, int x, int y);
    void runBackground();
    void scrollBackground();
    void bulletSmallMovement();
    void setBulletSmallStartingPosition(cocos2d::Sprite* bulletSmall);
    void spriteConsumed(cocos2d::Sprite* sprite);
    void nodeConsumed(Node* node);
    cocos2d::Sprite* getEnemyBird(int seed);
    cocos2d::Sprite* findAvailableSprite(std::vector<cocos2d::Sprite*> vectorSprites);
    void moveEnemies();
    void createFormation();
    bool onContactBegin(cocos2d::PhysicsContact& contact);
    void enemyDies(cocos2d::PhysicsBody* body);
    void bulletUsed(cocos2d::PhysicsBody* body);
    void pauseGame();
    void resumeGame();
    void gameOver();
    cocos2d::Sprite* createSpriteFromStr(std::string filename);
    void coinMovement(cocos2d::Sprite* deadBird);
    cocos2d::Sprite* createCoin(int x, int y);
    void coinUsed(cocos2d::PhysicsBody* body);
    void preloadCoins();
    void runGetCoinGfx(const cocos2d::Vec2* effectPoint);
    void createExplosion(cocos2d::PhysicsBody* body);
    void restartGame();

    
private:
    CocosDenshion::SimpleAudioEngine* audio = CocosDenshion::SimpleAudioEngine::getInstance();
    cocos2d::Label* labelTouchInfo;
    cocos2d::Sprite* redBird;
    std::vector<cocos2d::Sprite*> vectorBulletSmall;
    std::vector<cocos2d::Sprite*> vectorCoins;
    std::vector<cocos2d::Sprite*> vectorExplosions;
    std::vector<cocos2d::Sprite*> vectorBlackBirds;
    std::vector<cocos2d::Sprite*> vectorYellowBirds;
    std::vector<cocos2d::Sprite*> vectorPurpleBirds;
    std::vector<cocos2d::Sprite*> birdFormations;
    cocos2d::Sprite* lastBullet;
    cocos2d::Vec2 previousPosition;
    cocos2d::Size visibleSize;
    cocos2d::Sprite* background1;
    cocos2d::Sprite* background2;
    cocos2d::Sprite* background3;
    int score;
    int distance;
    int coin;
    cocos2d::ScaleTo* scaleTo;
    cocos2d::SpriteFrameCache* cache;
    cocos2d::Vector<Node*> pausedNodes;
    std::set<void*> pausedSchedulers;
    bool isPaused = false;
    cocos2d::Repeat* repeatOnce;
    cocos2d::Label* scoreDisplay;
    cocos2d::Label* distanceDisplay;
    cocos2d::Label* coinDisplay;

    const float SCALE_TO_SIZE = 0.5;
    const std::string PLIST_FILENAME = "BirdFlight/Assets/bird_flight.plist";
    const std::string RED_BIRD_FILENAME_FORMAT = "red_bird_fly_%02d.png";
    const std::string YELLOW_BIRD_FILENAME_FORMAT = "yellow_bird_fly_%02d.png";
    const std::string PURPLE_BIRD_FILENAME_FORMAT = "purple_bird_fly_%02d.png";
    const std::string BLACK_BIRD_FILENAME_FORMAT = "black_bird_fly_%02d.png";
    const std::string METEOR_FILENAME_FORMAT = "meteor_%02d.png";
    const std::string BULLET_SMALL_FILENAME = "bullet_small.png";
    const std::string BULLET_BIG_FILENAME = "bullet_big.png";
    const std::string DIE_FILENAME = "die_%02d.png";
    const std::string COIN_FILENAME = "coin.png";
    const int BULLET_BIG = -2;
    const int BULLET_SMALL = -1;
    const int RED_BIRD_SEED = 0;
    const int YELLOW_BIRD_SEED = 1;
    const int PURPLE_BIRD_SEED = 2;
    const int BLACK_BIRD_SEED = 3;
    const int COIN = 6;
    const int METEOR = 10;
    const int BULLET_CATEGORY = -1;
    const int HERO_CATEGORY = 1;
    const int ENEMY_CATEGORY = 2;
    const int COIN_CATEGORY = 3;
};