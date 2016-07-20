//
//  BirdFlightPlayScene.cpp
//  gameTest
//
//  Created by Rudy Tan on 7/15/16.
//
//

#include "BirdFlightPlayScene.h"
#include "SimpleAudioEngine.h"
#include "ui/CocosGUI.h"
#include <iostream>

USING_NS_CC;

Scene* BirdFlightPlayScene::createScene()
{
    auto playScene = Scene::createWithPhysics();
    auto layer = BirdFlightPlayScene::create();
    playScene->addChild(layer);
    
    return playScene;
}

// just testing 2nd test 3rd test 4th test
bool BirdFlightPlayScene::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    audio = CocosDenshion::SimpleAudioEngine::getInstance();
    audio->preloadBackgroundMusic("BirdFlight/Assets/music_game.mp3");
    audio->preloadBackgroundMusic("BirdFlight/Assets/music_gameover.mp3");
    //audio->preloadBackgroundMusic("BirdFlight/Assets/music_home.mp3");
    audio->preloadEffect("BirdFlight/Assets/sfx_hero_die.mp3");
    audio->preloadEffect("BirdFlight/Assets/sfx_bullet_shot.wav");
    audio->preloadEffect("BirdFlight/Assets/sfx_button_touch.wav");
    audio->preloadEffect("BirdFlight/Assets/sfx_enemy_die.wav");
    audio->preloadEffect("BirdFlight/Assets/sfx_enemy_hit.wav");
    audio->preloadEffect("BirdFlight/Assets/sfx_get_coin.wav");
    audio->preloadEffect("BirdFlight/Assets/sfx_meteor_appear.wav");

    audio->playBackgroundMusic("BirdFlight/Assets/music_game.mp3", true);

    
    visibleSize = Director::getInstance()->getVisibleSize();
    
    cache = SpriteFrameCache::getInstance();
    cache->addSpriteFramesWithFile(PLIST_FILENAME);
    
    preloadCoins();
    
    int scoreX = 200;
    int distanceX = 400;
    int coinX = 575;
    int labelY = visibleSize.height-50;
    int displayY = visibleSize.height-100;
    const std::string fontType = "Arial";
    int fontSize = 40;
    cocos2d::Color3B fontColor = cocos2d::Color3B::WHITE;
    
    auto pauseButton = cocos2d::ui::Button::create("BirdFlight/Assets/button_pause.png", "BirdFlight/Assets/button_pause.png", "BirdFlight/Assets/button_pause.png");
    pauseButton->setPosition(Vec2(70, visibleSize.height-75));
    pauseButton->addTouchEventListener([&](Ref* sender, cocos2d::ui::Widget::TouchEventType type){
        switch (type)
        {
            case ui::Widget::TouchEventType::BEGAN:
                break;
            case ui::Widget::TouchEventType::ENDED:
                std::cout << "Pause 1 clicked" << std::endl;
                pauseGame();
                break;
            default:
                break;
        }
    });
    
    this->addChild(pauseButton, 10);


    
    auto scoreLabel = Label::createWithSystemFont("SCORE", fontType, fontSize);
    scoreLabel->setColor(fontColor);
    scoreLabel->setPosition(Vec2(scoreX, labelY));
    scoreLabel->enableOutline(Color4B::WHITE, 1);
    scoreDisplay = Label::createWithSystemFont("0", fontType, fontSize);
    scoreDisplay->setColor(fontColor);
    scoreDisplay->setPosition(Vec2(scoreX, displayY));
    scoreDisplay->enableOutline(Color4B::WHITE, 1);
    
    auto distanceLabel = Label::createWithSystemFont("DISTANCE", fontType, fontSize);
    distanceLabel->setColor(fontColor);
    distanceLabel->setPosition(Vec2(distanceX, labelY));
    distanceLabel->enableOutline(Color4B::WHITE, 1);
    distanceDisplay = Label::createWithSystemFont("0", fontType, fontSize);
    distanceDisplay->setColor(fontColor);
    distanceDisplay->setPosition(Vec2(distanceX, displayY));
    distanceDisplay->enableOutline(Color4B::WHITE, 1);
    
    auto coinLabel = Label::createWithSystemFont("COIN", fontType, fontSize);
    coinLabel->setColor(fontColor);
    coinLabel->setPosition(Vec2(coinX, labelY));
    coinLabel->enableOutline(Color4B::WHITE, 1);
    coinDisplay = Label::createWithSystemFont("0", fontType, fontSize);
    coinDisplay->setColor(fontColor);
    coinDisplay->setPosition(Vec2(coinX, displayY));
    coinDisplay->enableOutline(Color4B::WHITE, 1);
    
    this->addChild(scoreLabel, 10);
    this->addChild(scoreDisplay, 10);
    this->addChild(distanceLabel, 10);
    this->addChild(distanceDisplay, 10);
    this->addChild(coinLabel, 10);
    this->addChild(coinDisplay, 10);
    
    
    redBird = getAnimation(RED_BIRD_FILENAME_FORMAT.c_str(), true, 6, visibleSize.width/2, 0);
    redBird->setAnchorPoint(Vec2(0.5,0));
    redBird->setTag(RED_BIRD_SEED);
    redBird->getPhysicsBody()->setCollisionBitmask(HERO_CATEGORY);
    redBird->getPhysicsBody()->setCategoryBitmask(HERO_CATEGORY);
    redBird->getPhysicsBody()->setContactTestBitmask(true);
    redBird->getPhysicsBody()->setGravityEnable(false);
    
    runBackground();
    this->schedule(cocos2d::SEL_SCHEDULE(&BirdFlightPlayScene::bulletSmallMovement), 0.01f, kRepeatForever , 0);
    this->schedule(cocos2d::SEL_SCHEDULE(&BirdFlightPlayScene::createFormation), 3.0f, kRepeatForever , 0);
    this->schedule(cocos2d::SEL_SCHEDULE(&BirdFlightPlayScene::moveEnemies), 0.01f, kRepeatForever , 0);
    this->schedule(cocos2d::SEL_SCHEDULE(&BirdFlightPlayScene::coinMovement), 0.01f, kRepeatForever , 0);
    
    //add contact event listener
    auto contactListener = EventListenerPhysicsContact::create();
    contactListener->onContactBegin = CC_CALLBACK_1(BirdFlightPlayScene::onContactBegin, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);
    
    
    // touch event
    labelTouchInfo = Label::createWithSystemFont("Touch or clicksomewhere to begin", "Arial", 30);

    auto touchListener = EventListenerTouchOneByOne::create();
    
    touchListener->onTouchBegan = CC_CALLBACK_2(BirdFlightPlayScene::onTouchBegan, this);
    touchListener->onTouchEnded = CC_CALLBACK_2(BirdFlightPlayScene::onTouchEnded, this);
    touchListener->onTouchMoved = CC_CALLBACK_2(BirdFlightPlayScene::onTouchMoved, this);
    touchListener->onTouchCancelled = CC_CALLBACK_2(BirdFlightPlayScene::onTouchCancelled, this);
    
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
    
    return true;
}

void BirdFlightPlayScene::restartGame(){
    Director::getInstance()->replaceScene(createScene());
}

void BirdFlightPlayScene::createFormation(){
    int random = 5; //cocos2d::random(1, 5);
    Sprite* enemy;
    Sprite* firstEnemy;
    //std::vector<Sprite*>* formation;
    bool reuse = false;
    /*
    for(std::vector<Sprite*>* currentFormation : birdFormations){
        if(currentFormation->size() == 0){
            formation = currentFormation;
            reuse = true;
        }
    }
    */
    
    for(int i = 0; i < random; i++){
        enemy = getEnemyBird(0);
        if(i == 0){
            enemy->setPosition(Vec2(i*120, visibleSize.height+enemy->getBoundingBox().size.height+10));
            firstEnemy = enemy;
        } else {
            enemy->setPosition(Vec2(i*120, firstEnemy->getPosition().y));
        }
        //formation->push_back(enemy);
        birdFormations.push_back(enemy);
    }
    /*
    if(!reuse){
        birdFormations.push_back(formation);
    }
     */
}

void BirdFlightPlayScene::moveEnemies(){
    //std::vector<Sprite*>* formation;
    Sprite* sprite;
    /*
    for(int i = 0; i < birdFormations.size(); i++){
        formation = birdFormations.at(i);
        for(int j = formation->size() - 1; j >= 0; j--){
            sprite = formation->at(j);
            sprite->runAction(MoveBy::create(0.1, Vec2(0,-10)));
            if(sprite->getPosition().y < 0){
                sprite->setPosition(Vec2(-1000,-1000));
                formation->erase(formation->begin()+j);
            }
        }
    }
    */
    for(int i = birdFormations.size()-1; i >= 0; i--){
        sprite = birdFormations.at(i);
        sprite->runAction(MoveBy::create(0.1, Vec2(0,-3)));
        if(sprite->getPosition().y < 0){
            gameOver();
            spriteConsumed(sprite);
            birdFormations.erase(birdFormations.begin()+i);
        }
    }
}

Sprite* BirdFlightPlayScene::getEnemyBird(int seed){
    Sprite* enemy = nullptr;
    if(seed != YELLOW_BIRD_SEED && seed != PURPLE_BIRD_SEED && seed != BLACK_BIRD_SEED){
        seed = cocos2d::random(1, 3);
    }
    if(seed == YELLOW_BIRD_SEED ){
        enemy = findAvailableSprite(vectorYellowBirds);
        if(enemy == nullptr){
            enemy = getAnimation(YELLOW_BIRD_FILENAME_FORMAT.c_str(), true, 6, -1000, -1000);
            enemy->setAnchorPoint(Vec2(0,0));
            enemy->setTag(YELLOW_BIRD_SEED);
            vectorYellowBirds.push_back(enemy);
        }
    } else if (seed == PURPLE_BIRD_SEED){
        enemy = findAvailableSprite(vectorPurpleBirds);
        if(enemy == nullptr){
            enemy = getAnimation(PURPLE_BIRD_FILENAME_FORMAT.c_str(), true, 6, -1000, -1000);
            enemy->setAnchorPoint(Vec2(0,0));
            enemy->setTag(PURPLE_BIRD_SEED);
            vectorPurpleBirds.push_back(enemy);
        }
    } else if (seed == BLACK_BIRD_SEED) {
        enemy = findAvailableSprite(vectorBlackBirds);
        if(enemy == nullptr){
            enemy = getAnimation(BLACK_BIRD_FILENAME_FORMAT.c_str(), true, 6, -1000, -1000);
            enemy->setAnchorPoint(Vec2(0,0));
            enemy->setTag(BLACK_BIRD_SEED);
            vectorBlackBirds.push_back(enemy);
        }
    }
    enemy->getPhysicsBody()->setCollisionBitmask(12);
    enemy->getPhysicsBody()->setCategoryBitmask(11);
    enemy->getPhysicsBody()->setContactTestBitmask(true);
    enemy->getPhysicsBody()->setGravityEnable(false);
    return enemy;
}

Sprite* BirdFlightPlayScene::findAvailableSprite(std::vector<Sprite*> vectorSprites){
    if(vectorSprites.size() > 0){
        for (Sprite* sprite : vectorSprites){
            if(sprite->getPosition().x < -300 || sprite->getPosition().y < -300){
                return sprite;
            }
        }
    }
    return nullptr;
}

void BirdFlightPlayScene::createExplosion(PhysicsBody* body){
    auto explosion = findAvailableSprite(vectorExplosions);
    int x = body->getOwner()->getPosition().x;
    int y = body->getOwner()->getPosition().y + body->getOwner()->getBoundingBox().size.height/2;
    if( body->getOwner()->getAnchorPoint().x == 0){
        x = x + body->getOwner()->getBoundingBox().size.width/2;
    }
    if(explosion == nullptr){
        explosion = getAnimation(DIE_FILENAME.c_str(), true, 5, x, y);
        explosion->setAnchorPoint(Vec2(0.5,0.5));
        vectorExplosions.push_back(explosion);
    }
    explosion->setPosition(Vec2(x, y));
    auto delay = DelayTime::create(0.2f);
    auto moveTo = MoveTo::create(0, Vec2(-1000,-1000));
    auto sequence = Sequence::create(delay, moveTo, NULL);
    explosion->runAction(sequence);
    //explosion->setPosition(Vec2(body->getOwner()->getPosition().x + body->getOwner()->getBoundingBox().size.width/2, body->getOwner()->getPosition().y + body->getOwner()->getBoundingBox().size.height/2));
    //explosion->setPosition(Vec2(body->getOwner()->getPosition().x, body->getOwner()->getPosition().y));
    //explosion->autorelease();
    //createCoin(nodeB->getOwner()->getPosition().x + nodeB->getOwner()->getBoundingBox().size.width/2, nodeB->getOwner()->getPosition().y + nodeB->getOwner()->getBoundingBox().size.height/2);
}

Sprite* BirdFlightPlayScene::getAnimation(const char* filenameFormat, bool isForever, int numberOfFrames, int x, int y){
    Vector<SpriteFrame*> animFrames(numberOfFrames);
    char str[100] = {0};
    for(int i = 1; i < numberOfFrames; i++)
    {
        sprintf(str, filenameFormat, i);
        SpriteFrame* frame = cache->getSpriteFrameByName( str );
        animFrames.pushBack(frame);
    }
    if(isForever){
        for(int i = numberOfFrames; i > 1 ; i--)
        {
            sprintf(str, filenameFormat, i);
            SpriteFrame* frame = cache->getSpriteFrameByName( str );
            animFrames.pushBack(frame);
        }
    }
    
    auto animation = Animation::createWithSpriteFrames(animFrames, 0.075f);
    auto animate = Animate::create(animation);
    
    sprintf(str, filenameFormat, 1);
    Sprite* sprite = Sprite::createWithSpriteFrameName(str);
    sprite->setPosition(Vec2(x, y));
    sprite->setScale(SCALE_TO_SIZE);
    this->addChild(sprite,4);

    if (isForever){
        sprite->runAction(RepeatForever::create(animate));
        auto physicsBody = PhysicsBody::createBox(Size(sprite->getBoundingBox().size.width , sprite->getBoundingBox().size.height ), PhysicsMaterial(0.1f, 1.0f, 0.0f));
        physicsBody->setDynamic(false);
        //set the body isn't affected by the physics world's gravitational force
        physicsBody->setGravityEnable(false);
        sprite->addComponent(physicsBody);
    } else {
        repeatOnce = Repeat::create(animate, 1);
    }
    
    return sprite;
}

bool BirdFlightPlayScene::onContactBegin(PhysicsContact& contact)
{
    auto nodeA = contact.getShapeA()->getBody();
    auto nodeB = contact.getShapeB()->getBody();
    
    int tagA = nodeA->getOwner()->getTag();
    int tagB = nodeB->getOwner()->getTag();
    
    const Vec2* contactPoint = contact.getContactData()->points;
    
    if (nodeA && nodeB)
    {
        // enemy or meteor contact with hero
        if ( (tagA == RED_BIRD_SEED && (tagB == YELLOW_BIRD_SEED || tagB == PURPLE_BIRD_SEED || tagB == BLACK_BIRD_SEED || tagB == METEOR)) || (tagB == RED_BIRD_SEED && (tagA == YELLOW_BIRD_SEED || tagA == PURPLE_BIRD_SEED || tagA == BLACK_BIRD_SEED || tagA == METEOR)) )
        {
            createExplosion(nodeA);
            createExplosion(nodeB);
            audio->playEffect("BirdFlight/Assets/sfx_hero_die.mp3");
            audio->playEffect("BirdFlight/Assets/sfx_enemy_die.wav");
            gameOver();
        }
        // bullet contact with enemy
        else if ((tagA == BULLET_SMALL || tagA == BULLET_BIG) && (tagB == YELLOW_BIRD_SEED || tagB == PURPLE_BIRD_SEED || tagB == BLACK_BIRD_SEED))
        {
            // reduce enemy HP or enemy die -> send nodeA
            createCoin(nodeB->getOwner()->getPosition().x + nodeB->getOwner()->getBoundingBox().size.width/2, nodeB->getOwner()->getPosition().y + nodeB->getOwner()->getBoundingBox().size.height/2);
            bulletUsed(nodeA);
            enemyDies(nodeB);
            audio->playEffect("BirdFlight/Assets/sfx_enemy_die.wav");
        }
        else if((tagB == BULLET_SMALL || tagB == BULLET_BIG) && (tagA == YELLOW_BIRD_SEED || tagA == PURPLE_BIRD_SEED || tagA == BLACK_BIRD_SEED))
        {
            // reduce enemy HP or enemy die -> send nodeB
            createCoin(nodeA->getOwner()->getPosition().x + nodeA->getOwner()->getBoundingBox().size.width/2, nodeA->getOwner()->getPosition().y + nodeA->getOwner()->getBoundingBox().size.height/2);
            bulletUsed(nodeB);
            enemyDies(nodeA);
            audio->playEffect("BirdFlight/Assets/sfx_enemy_die.wav");
        }
        else if (tagA == RED_BIRD_SEED && tagB == COIN)
        {
            runGetCoinGfx(contactPoint);
            audio->playEffect("BirdFlight/Assets/sfx_get_coin.wav");
            coinUsed(nodeB);
        }
        else if (tagB == RED_BIRD_SEED && tagA == COIN)
        {
            runGetCoinGfx(contactPoint);
            audio->playEffect("BirdFlight/Assets/sfx_get_coin.wav");
            coinUsed(nodeA);
        }
    }
    
    //bodies can collide
    return true;
}

void BirdFlightPlayScene::runGetCoinGfx(const Vec2* effectPoint){
    auto emitter = ParticleSun::create();
    emitter->setPosition(Vec2(effectPoint->x,effectPoint->y));
    // set the duration
    emitter->setDuration(0.1f);
    // radius mode
    //emitter->setEmitterMode(ParticleSystem::Mode::RADIUS);
    // radius mode: 100 pixels from center
    emitter->setStartSize(100);
    emitter->setStartSizeVar(0);
    emitter->setEndSize(ParticleSystem::START_RADIUS_EQUAL_TO_END_RADIUS);
    emitter->setEndSizeVar(0);    // not used when start == end
    emitter->setAutoRemoveOnFinish(true);
    addChild(emitter, 3);
}

void BirdFlightPlayScene::coinUsed(PhysicsBody* body){
    for(int i = 0; i < vectorCoins.size(); i++){
        if(body == vectorCoins.at(i)->getPhysicsBody()){
            spriteConsumed(vectorCoins.at(i));
            //vectorCoins.erase(vectorCoins.begin() + i);
            coin = coin + 1;
            char coinText[10] = {0};
            sprintf(coinText, "%d", coin);
            coinDisplay->setString(coinText);
        }
    }
}

void BirdFlightPlayScene::gameOver(){
    pauseGame();
}

void BirdFlightPlayScene::pauseGame(){
    if(!isPaused){
        pausedSchedulers = cocos2d::Director::getInstance()->getScheduler()->pauseAllTargets();
        pausedNodes = cocos2d::Director::getInstance()->getActionManager()->pauseAllRunningActions();
        isPaused = true;
    }
}

void BirdFlightPlayScene::resumeGame(){
    if(isPaused){
        if (pausedNodes.size()){
            cocos2d::Director::getInstance()->getActionManager()->resumeTargets(pausedNodes);
            pausedNodes.clear();
        }
        cocos2d::Director::getInstance()->getScheduler()->resumeTargets(pausedSchedulers);
        isPaused = false;
    }
}

void BirdFlightPlayScene::enemyDies(PhysicsBody* body){
    for(int i = 0; i < birdFormations.size(); i++){
        if(body == birdFormations.at(i)->getPhysicsBody()){
            createExplosion(body);
            spriteConsumed(birdFormations.at(i));
            birdFormations.erase(birdFormations.begin() + i);
            // score = score + body->getOwner()->getTag() * 10;
            score = score + 10;
            char scoreText[10] = {0};
            sprintf(scoreText, "%d", score);
            scoreDisplay->setString(scoreText);
            break;
        }
    }
}

void BirdFlightPlayScene::preloadCoins(){
    Sprite* coin;
    if(vectorCoins.size() == 0){
        //pre load coins
        for(int i = 1; i <= 20; i++){
            coin = createSpriteFromStr(COIN_FILENAME);
            coin->setPosition(Vec2(-1000, -1000));
            coin->setAnchorPoint(Vec2(0.5,0.5));
            coin->getPhysicsBody()->setCategoryBitmask(COIN_CATEGORY);
            coin->getPhysicsBody()->setCollisionBitmask(COIN_CATEGORY);
            coin->setTag(COIN);
            this->addChild(coin, 5);
            vectorCoins.push_back(coin);
        }
    }
}

Sprite* BirdFlightPlayScene::createCoin(int x, int y){
    Sprite* coin = findAvailableSprite(vectorCoins);;
    if(coin == nullptr){
        coin = createSpriteFromStr(COIN_FILENAME);
        coin->setPosition(Vec2(x, y));
        coin->setAnchorPoint(Vec2(0.5,0.5));
        coin->getPhysicsBody()->setCategoryBitmask(COIN_CATEGORY);
        coin->getPhysicsBody()->setCollisionBitmask(COIN_CATEGORY);
        coin->setTag(COIN);
        this->addChild(coin, 5);
        vectorCoins.push_back(coin);
    } else {
        coin->setPosition(Vec2(x, y));
    }
    //coin->runAction(EaseOut::create(MoveBy::create(0.1f, Vec2(200,-200)), 5.0f));
    auto move = MoveBy::create(2, Vec2(0, 100)); //visibleSize.height - coin->getContentSize().height));
    int random = cocos2d::random((-x+200)/2, (static_cast<int>(visibleSize.width)-x-200)/2);
    //std::cout << "x: " << x << " random: " << random << " result: " << x+random << "\n";
    auto move2 = MoveBy::create(2, Vec2(random, 0));
    // create a BounceIn Ease Action
    auto move_ease_in = (EaseBounceIn::create(move))->reverse();
    // create the sequence of actions, in the order we want to run them
    auto spawn = Spawn::create(move2, move_ease_in, NULL);
    coin->runAction(spawn);
    return coin;
}

void BirdFlightPlayScene::coinMovement(Sprite* deadBird)
{
    for( Sprite* coin : vectorCoins){
        int currentCoinY = coin->getPosition().y;
        // coin too far away from visibility -> set for reuse
        if(currentCoinY < -(coin->getBoundingBox().size.height)){
            spriteConsumed(coin);
        } else {
            // move coin down
            coin->runAction(MoveBy::create(0.1, Vec2(0,-1)));
        }
    }
}

void BirdFlightPlayScene::bulletUsed(PhysicsBody* body){
    for(int i = 0; i < vectorBulletSmall.size(); i++){
        if(body == vectorBulletSmall.at(i)->getPhysicsBody()){
            spriteConsumed(vectorBulletSmall.at(i));
            //vectorBulletSmall.erase(vectorBulletSmall.begin() + i);
        }
    }
}

bool BirdFlightPlayScene::onTouchBegan(Touch* touch, Event* event)
{
    if(isPaused){
        resumeGame();
    }
    previousPosition = touch->getLocation();
    return true;
}

void BirdFlightPlayScene::onTouchEnded(Touch* touch, Event* event)
{
    //cocos2d::log("touch ended");
}

void BirdFlightPlayScene::onTouchMoved(Touch* touch, Event* event)
{
    if(!isPaused) {
        float x1 = previousPosition.x;
        float x2 = touch->getLocation().x;
        float currentX = redBird->getPosition().x;
        float movedToPosition = currentX + ( x2 - x1 );
        float sizeOfRedBird = redBird->getBoundingBox().size.width;
        if (movedToPosition <= sizeOfRedBird/2){
            movedToPosition = sizeOfRedBird/2;
        }
        if(movedToPosition >= visibleSize.width - sizeOfRedBird/2){
            movedToPosition = visibleSize.width - sizeOfRedBird/2;
        }
    
        redBird->setPosition(Vec2( movedToPosition, redBird->getPosition().y));
        previousPosition = touch->getLocation();
    
        //cocos2d::log("touch moved");
    }
}

void BirdFlightPlayScene::onTouchCancelled(Touch* touch, Event* event)
{
    cocos2d::log("touch cancelled");
}

void BirdFlightPlayScene::runBackground(){
    // background
    background1 = Sprite::createWithSpriteFrameName("background.png");
    background1->setAnchorPoint(Vec2(0,0));
    background1->setPosition(Vec2(0,0));
    this->addChild(background1,-1);
    
    background2 = Sprite::createWithSpriteFrameName("background.png");
    background2->setAnchorPoint(Vec2(0,0));
    background2->setPosition(Vec2(0, background1->getBoundingBox().size.height -1));
    this->addChild(background2,-1);
    
    
    background3 = Sprite::createWithSpriteFrameName("background.png");
    background3->setAnchorPoint(Vec2(0,0));
    background3->setPosition(Vec2(0, background2->getPosition().y + background2->getBoundingBox().size.height -1));
    this->addChild(background3,-1);
    
    this->schedule(cocos2d::SEL_SCHEDULE(&BirdFlightPlayScene::scrollBackground), 0.01f, kRepeatForever , 0);
}

void BirdFlightPlayScene::scrollBackground()
{
    background1->setPosition( Vec2(background1->getPosition().x, background1->getPosition().y-1) );
    background2->setPosition( Vec2(background2->getPosition().x, background2->getPosition().y-1) );
    background3->setPosition( Vec2(background3->getPosition().x, background3->getPosition().y-1) );
    
    if( background1->getPosition().y < -background1->getBoundingBox().size.height){
        //background1->runAction(MoveBy::create(0.1, Vec2( background1->getPosition().x, background3->getPosition().y+background3->getBoundingBox().size.height - 1)));
        background1->setPosition(Vec2( background1->getPosition().x, background3->getPosition().y+background3->getBoundingBox().size.height - 1) );
    }
    if( background2->getPosition().y < -background2->getBoundingBox().size.height){
        //background2->runAction(MoveBy::create(0.1, Vec2( background2->getPosition().x, background1->getPosition().y + background1->getBoundingBox().size.height - 1)));
        background2->setPosition( Vec2( background2->getPosition().x, background1->getPosition().y + background1->getBoundingBox().size.height - 1));
    }
    if( background3->getPosition().y < -background3->getBoundingBox().size.height){
        //background3->runAction(MoveBy::create(0.1, Vec2( background3->getPosition().x, background2->getPosition().y + background2->getBoundingBox().size.height - 1)));
        background3->setPosition( Vec2( background3->getPosition().x, background2->getPosition().y + background2->getBoundingBox().size.height - 1));
    }
}

Sprite* BirdFlightPlayScene::createSpriteFromStr(std::string filename)
{
    Sprite* sprite = Sprite::createWithSpriteFrameName(filename);
    sprite->setScale(SCALE_TO_SIZE);
    sprite->setAnchorPoint(Vec2(0.5,0));

    auto physicsBody = PhysicsBody::createBox(Size(sprite->getBoundingBox().size.width , sprite->getBoundingBox().size.height ), PhysicsMaterial(0.1f, 1.0f, 0.0f));
    physicsBody->setDynamic(false);
    //set the body isn't affected by the physics world's gravitational force
    physicsBody->setGravityEnable(false);
    physicsBody->setContactTestBitmask(true);
    physicsBody->setGravityEnable(false);
    sprite->addComponent(physicsBody);
    

    return sprite;
}

void BirdFlightPlayScene::bulletSmallMovement()
{
    if(vectorBulletSmall.size() == 0){
        lastBullet = createSpriteFromStr(BULLET_SMALL_FILENAME);
        lastBullet->getPhysicsBody()->setCategoryBitmask(BULLET_CATEGORY);
        lastBullet->getPhysicsBody()->setCollisionBitmask(BULLET_CATEGORY);
        setBulletSmallStartingPosition(lastBullet);
        lastBullet->setTag(BULLET_SMALL);
        this->addChild(lastBullet, 3);
        vectorBulletSmall.push_back(lastBullet);
    } else {
        int highestBulletPointY = visibleSize.height + lastBullet->getBoundingBox().size.height + 5;
        for( Sprite* bulletSmall : vectorBulletSmall){
            int currentBulletSmallY = bulletSmall->getPosition().y;
            // bullet too far away from visibility -> set for reuse
            if(currentBulletSmallY > highestBulletPointY){
                spriteConsumed(bulletSmall);
            }
            // move bullet up
            if(currentBulletSmallY >= 0){
                bulletSmall->runAction(MoveBy::create(0, Vec2(0,10)));
                //bulletSmall->setPosition(Vec2(bulletSmall->getPosition().x,bulletSmall->getPosition().y+1));
            }
        }
    }
    
    // shoot another bullet
    if(lastBullet->getPosition().y >= redBird->getPosition().y + redBird->getBoundingBox().size.height || lastBullet->getPosition().y < 0){
        // check for available old bullet
        lastBullet = findAvailableSprite(vectorBulletSmall);
        // cant find old bullet to be reused
        if(lastBullet == nullptr){
            lastBullet = createSpriteFromStr(BULLET_SMALL_FILENAME);
            lastBullet->getPhysicsBody()->setCategoryBitmask(BULLET_CATEGORY);
            lastBullet->getPhysicsBody()->setCollisionBitmask(BULLET_CATEGORY);
            setBulletSmallStartingPosition(lastBullet);
            lastBullet->setTag(BULLET_SMALL);
            this->addChild(lastBullet, 3);
            vectorBulletSmall.push_back(lastBullet);
        }
        // set position for the next bullet
        if(lastBullet != nullptr){
            setBulletSmallStartingPosition(lastBullet);
        }
    }
    distance = distance + 1;
    char distanceText[10] = {0};
    sprintf(distanceText, "%d", distance);
    distanceDisplay->setString(distanceText);
}

void BirdFlightPlayScene::setBulletSmallStartingPosition(Sprite* bulletSmall){
    bulletSmall->setPosition(Vec2(redBird->getPosition().x, redBird->getPosition().y + redBird->getBoundingBox().size.height - bulletSmall->getBoundingBox().size.height));
    audio->playEffect("BirdFlight/Assets/sfx_bullet_shot.wav");
}

void BirdFlightPlayScene::spriteConsumed(Sprite* sprite){
    sprite->setPosition(Vec2(-1000,-1000)); // hide it somewhere
}

void BirdFlightPlayScene::nodeConsumed(Node* node){
    node->setPosition(Vec2(-1000,-1000)); // hide it somewhere
}