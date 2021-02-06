#ifndef HEROPLANE_H
#define HEROPLANE_H
#include <QPixmap>
#include <QRect>
#include "bullet.h"

class HeroPlane
{
public:
    HeroPlane();

    //发射子弹
    void shoot();
    //设置飞机位置
    void setPosition(int x, int y);

    //飞机资源对象
    QPixmap m_Plane;

    //飞机坐标
    int m_X;
    int m_Y;

    //飞机的矩形边框
    QRect m_Rect;
    
    //弹匣
    Bullet m_bullets[BULLET_NUM];
    
    //发射间隔记录
    int m_recorder;

    //subtract life value of hero
    void minusLifeValue(int value);

    //check if hero's life value is still greater than 0
    bool lifeValueGreaterThanZero();

private:
    int m_life;


};

#endif // HEROPLANE_H
