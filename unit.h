#ifndef UNIT_H
#define UNIT_H
#include "obstacle.h"
#include<QRect>
#include<QPainter>

using namespace std;

class Unit
{
public:

    Unit(int x = 1000, int y = 1000, double width = 39, double height = 24);

    void stop();
    bool selectUnit(const QPointF);
    void setPosition(const QPointF v);
    void setTarget(const QPointF point);
    void moveTo();
    void setSelection(bool exp);
    QPointF getPosition() const;
    QPainterPath getNextPath() const;
    QPainterPath getCurrentPath() const;
    virtual QPainterPath getAttackCollider() const;
    virtual int attack(Unit& enemy);
    void setColor(const QColor);
    virtual void draw(QPainter *);
    void manualMove(QPointF point,QRectF border,QVector<Obstacle*>,QVector<Unit*>);
    double getHealth() const;

protected:
    double angle;
    QColor color;
    QPointF target;
    QPointF center;
    QImage img;
    double attackPower;
    double health;
    double maxHealth;
    double speed;
    double defensePower;
    bool selected;
    bool movable;
    double width,height;

private:
    
};

#endif // UNIT_H
