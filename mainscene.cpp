#include "mainscene.h"
#include "config.h"

MainScene::MainScene(QWidget *parent)
    : QWidget(parent){
    //初始化场景
    initScene();
    //initialize kinect
    initSensor();
    //启动游戏
    playGame();

    setMouseTracking(true);
}

MainScene::~MainScene(){}

void MainScene::initScene(){
    //初始化窗口大小
    setFixedSize(GAME_WIDTH,GAME_HEIGHT);
    //设置窗口标题
    setWindowTitle(GAME_TITLE);
    //设置图标资源
    setWindowIcon(QIcon(GAME_ICON));
    //定时器设置
    m_Timer.setInterval(GAME_RATE);

    m_recorder = 0;
    //随机数种子
    srand((unsigned int)time(NULL));
}

void MainScene::initSensor(){
    //open sensor
    hResult = GetDefaultKinectSensor(&pSensor);
    if (FAILED(hResult)) {
        kinect_available = false;
        return ;
    }
    hResult = pSensor->Open();
    if (FAILED(hResult)) {
        std::cerr << "Error : IKinectSensor::Open()" << std::endl;
        kinect_available = false;
        return ;
    }
    //Source
    hResult = pSensor->get_BodyFrameSource(&pBodySource);
    if (FAILED(hResult)) {
        std::cerr << "Error : IKinectSensor::get_BodyFrameSource()" << std::endl;
        kinect_available = false;
        return ;
    }
    // Reader
    hResult = pBodySource->OpenReader(&pBodyReader);
    if (FAILED(hResult)) {
        std::cerr << "Error : IBodyFrameSource::OpenReader()" << std::endl;
        kinect_available = false;
        return ;
    }
    // Coordinate Mapper
    hResult = pSensor->get_CoordinateMapper(&pCoordinateMapper);
    if (FAILED(hResult)) {
        std::cerr << "Error : IKinectSensor::get_CoordinateMapper()" << std::endl;
        kinect_available = false;
        return ;
    }
    cursor = new QCursor();
    screen = QGuiApplication::primaryScreen();
    kinect_available = true;
}

void MainScene::playGame(){
    //启动背景音乐
    QSound::play(SOUND_BACKGROUND);
    //启动定时器
    m_Timer.start();
    //监听定时器
    connect(&m_Timer,&QTimer::timeout,[=](){
        //敌机出场
        enemyToScene();
        if(kinect_available == true){
            //set cursor position by sensor
            updateCursor();
        }
        //更新游戏中元素的坐标
        updatePosition();
        //重新绘制图片
        update();
        //碰撞检测
        collisonDetection();
    });
}

//释放接口需要自己定义
template<class Interface>
inline void SafeRelease(Interface *& pInterfaceToRelease){
    if (pInterfaceToRelease != NULL) {
        pInterfaceToRelease->Release();
        pInterfaceToRelease = NULL;
    }
}

void MainScene::updateCursor(){
    //更新骨骼帧
    pBodyFrame = nullptr;
    hResult = pBodyReader->AcquireLatestFrame(&pBodyFrame);
    if (SUCCEEDED(hResult)) {
        IBody* pBody[BODY_COUNT] = { 0 };
        //更新骨骼数据
        hResult = pBodyFrame->GetAndRefreshBodyData(BODY_COUNT, pBody);
        if (SUCCEEDED(hResult)) {
            for (int count = 0; count < BODY_COUNT; count++) {
                bTracked = false;
                hResult = pBody[count]->get_IsTracked(&bTracked);
                if (SUCCEEDED(hResult) && bTracked) {
                    Joint joint[JointType::JointType_Count];
                    hResult = pBody[count]->GetJoints(JointType::JointType_Count, joint);//joint
                    if (SUCCEEDED(hResult)) {
                        // use head position to control cursor
                        colorSpacePoint = { 0 };
                        pCoordinateMapper->MapCameraPointToColorSpace(joint[JointType::JointType_Head].Position, &colorSpacePoint);
                        cursor_x = static_cast<int>(colorSpacePoint.X);
                        cursor_y = static_cast<int>(colorSpacePoint.Y);
                    }
                }
            }
            for (int count = 0; count < BODY_COUNT; count++) {
                SafeRelease(pBody[count]);
            }
        }
        cursor->setPos(screen, cursor_x, cursor_y);
    }
    SafeRelease(pBodyFrame);
}

void MainScene::updatePosition(){
    //更新地图坐标
    m_map.mapPosition();

    //发射子弹
    m_hero.shoot();
    //计算子弹坐标
    for(int i = 0; i < BULLET_NUM; i++){
        //如果子弹状态为非空闲，计算发射位置
        if(!m_hero.m_bullets[i].m_Free)
        {
           m_hero.m_bullets[i].updatePosition();
        }
    }
    //敌机坐标计算
    for(int i = 0 ; i< ENEMY_NUM;i++){
        //非空闲敌机    更新坐标
        if(m_enemys[i].m_Free == false){
           m_enemys[i].updatePosition();
        }
    }
    //计算爆炸播放的图片
    for(int i = 0 ; i < BOMB_NUM;i++){
        if(m_bombs[i].m_Free == false){
            m_bombs[i].updateInfo();
        }
    }
}

void MainScene::paintEvent(QPaintEvent *event){
    QPainter painter(this);
    //绘制地图
    painter.drawPixmap(0,m_map.m_map1_posY,m_map.m_map1);
    painter.drawPixmap(0,m_map.m_map2_posY,m_map.m_map2);
    //绘制英雄
    painter.drawPixmap(m_hero.m_X,m_hero.m_Y,m_hero.m_Plane);
    //绘制子弹
    for(int i = 0; i < BULLET_NUM; i++){
        //如果子弹状态为非空闲，计算发射位置
        if(!m_hero.m_bullets[i].m_Free){
            painter.drawPixmap(m_hero.m_bullets[i].m_X, m_hero.m_bullets[i].m_Y,m_hero.m_bullets[i].m_Bullet);
        }
    }
    //绘制敌机
    for(int i = 0 ; i< ENEMY_NUM;i++){
        if(m_enemys[i].m_Free == false){
            painter.drawPixmap(m_enemys[i].m_X,m_enemys[i].m_Y,m_enemys[i].m_enemy);
        }
    }
    //绘制爆炸图片
    for(int i = 0 ; i < BOMB_NUM;i++){
        if(m_bombs[i].m_Free == false){
            painter.drawPixmap(m_bombs[i].m_X,m_bombs[i].m_Y,m_bombs[i].m_pixArr[m_bombs[i].m_index]);
        }
    }
}

void MainScene::mouseMoveEvent(QMouseEvent *event){
    int x = event->x() - m_hero.m_Rect.width()*0.5; //鼠标位置    - 飞机矩形的一半
    int y = event->y() - m_hero.m_Rect.height()*0.5;
    //边界检测
    if(x <= 0 ){
        x = 0;
    }
    if(x >= GAME_WIDTH - m_hero.m_Rect.width()){
        x = GAME_WIDTH - m_hero.m_Rect.width();
    }
    if(y <= 0){
        y = 0;
    }
    if(y >= GAME_HEIGHT - m_hero.m_Rect.height()){
        y = GAME_HEIGHT - m_hero.m_Rect.height();
    }
    m_hero.setPosition(x, y);
}

void MainScene::enemyToScene(){
    m_recorder++;
    if(m_recorder < ENEMY_INTERVAL){
        return;
    }
    m_recorder = 0;
    for(int i = 0 ; i< ENEMY_NUM;i++){
        if(m_enemys[i].m_Free){
            //敌机空闲状态改为false
            m_enemys[i].m_Free = false;
            //设置坐标
            m_enemys[i].m_X = rand() % (GAME_WIDTH - m_enemys[i].m_Rect.width());
            m_enemys[i].m_Y = -m_enemys[i].m_Rect.height();
            break;
        }
    }
}

void MainScene::collisonDetection(){
    //遍历所有非空闲的敌机
    for(int i = 0; i < ENEMY_NUM; i++){
        if(m_enemys[i].m_Free){
            //空闲飞机 跳转下一次循环
            continue;
        }

        if(m_enemys[i].m_Rect.intersects(m_hero.m_Rect)){
            //播放音效
            QSound::play(SOUND_BOMB);

            m_enemys[i].m_Free = true;
            m_hero.minusLifeValue(ENEMY_LIFE);

            //播放爆炸效果
            for(int k = 0 ; k < BOMB_NUM;k++){
                if(m_bombs[k].m_Free){
                    //爆炸状态设置为非空闲
                    m_bombs[k].m_Free = false;
                    //更新坐标
                    m_bombs[k].m_X = m_enemys[i].m_X;
                    m_bombs[k].m_Y = m_enemys[i].m_Y;
                    break;
                }
            }

            if(m_hero.lifeValueGreaterThanZero() == false){
                this->close();
                QCoreApplication::quit();
            }
        }


        //遍历所有 非空闲的子弹
        for(int j = 0; j < BULLET_NUM; j++){
            if(m_hero.m_bullets[j].m_Free){
                //空闲子弹 跳转下一次循环
                continue;
            }
            //如果子弹矩形框和敌机矩形框相交，发生碰撞，同时变为空闲状态即可
            if(m_enemys[i].m_Rect.intersects(m_hero.m_bullets[j].m_Rect)){
                //播放音效
                QSound::play(SOUND_BOMB);

                m_enemys[i].m_Free = true;
                m_hero.m_bullets[j].m_Free = true;

                //播放爆炸效果
                for(int k = 0 ; k < BOMB_NUM;k++){
                    if(m_bombs[k].m_Free){
                        //爆炸状态设置为非空闲
                        m_bombs[k].m_Free = false;
                        //更新坐标
                        m_bombs[k].m_X = m_enemys[i].m_X;
                        m_bombs[k].m_Y = m_enemys[i].m_Y;
                        break;
                    }
                }
            }
        }
    }
}
