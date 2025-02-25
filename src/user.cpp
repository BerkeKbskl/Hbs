#include "user.h"


User::User(Scenario scenario) {
    this->scenario=scenario;
    color=QColor(155,185,255);
    createUnits(scenario.getUnitsType(999));
}

void User::deployUnits(Scenario scenario) {


    QVector<QPointF>unitPositions=scenario.getUnitPositions(999);

    for(int i=0;i<unitPositions.size();i++){
        units[i]->setPosition({unitPositions[i].x(),unitPositions[i].y()});
         units[i]->setColor(this->color);
    }

}
