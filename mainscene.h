#ifndef MAINSCENE_H
#define MAINSCENE_H


#include <ctime>
#include <iostream>
#include <QIcon>
#include <QPainter>
#include <QSound>
#include <QCoreApplication>
#include <QGuiApplication>
#include <QWidget>
#include <QTimer>
#include <QMouseEvent>

#include "map.h"
#include "heroplane.h"
#include "enemyplane.h"
#include "bomb.h"
#include "Kinect.h"

class MainScene : public QWidget
{
    Q_OBJECT

public:
    MainScene(QWidget *parent = 0);
    ~MainScene();
    //初始化游戏场景
    void initScene();

    void initSensor();

    //启动游戏，用于启动定时器对象
    void playGame();
    //更新坐标
    void updatePosition();
    //绘图事件
    void paintEvent(QPaintEvent *event);
    //鼠标移动事件
    void mouseMoveEvent(QMouseEvent *event);
    //set cursor position by sensor
    void updateCursor();
    //敌机出场
    void enemyToScene();

    //子弹与飞机碰撞检测
    void collisonDetection();

public:
    QTimer m_Timer;
    Map m_map;

    //飞机对象
    HeroPlane m_hero;

    //敌机数组
    EnemyPlane m_enemys[ENEMY_NUM];
    //敌机出场间隔记录
    int m_recorder;

    //爆炸数组
    Bomb m_bombs[BOMB_NUM];

private:
    // Sensor
    IKinectSensor* pSensor;
    HRESULT hResult;
    bool kinect_available;
    //Source
    IBodyFrameSource* pBodySource;
    // Reader
    IBodyFrameReader* pBodyReader;
    // Coordinate Mapper
    ICoordinateMapper* pCoordinateMapper;
    //Frame
    IBodyFrame* pBodyFrame;

    float Skeletons[6][25][3];

    //screen
    QScreen *screen;
    //cursor
    QCursor *cursor;
    int cursor_x, cursor_y;
    //others
    BOOLEAN bTracked;
    ColorSpacePoint colorSpacePoint;


};

#endif // MAINSCENE_H
