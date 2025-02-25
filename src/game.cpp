#include "game.h"
#include "gamemenu.h"
#include "ui_game.h"
#define FPS 60

Game::Game(Scenario scenario, Settings settings, QWidget *parent)
    :QWidget(parent), scenario(scenario), user(scenario), ai(scenario),ui(new Ui::Game),isGameStarted(false),settings(settings), map(scenario,settings)
{
    ui->setupUi(this);
    setFocusPolicy(Qt::StrongFocus);
    setGeometry(0,0,settings.getScreenSize().width(),settings.getScreenSize().height());
    gameSetup();
}

Game::~Game(){
    delete timer;
    delete ui;
}

void Game::gameSetup(){
    ui->gameW->setGeometry(0,0,width(),height());
    ui->restrictedArea->resize(width(),height()/2);
    ui->restrictedArea->move(0,height()/2);
    ui->info->move(0,ui->info->pos().y()*settings.getScale());
    isPauseState = false;
    ai.deployUnits(scenario);
    user.deployUnits(scenario);
    ui->pauseIcon->hide();
    ui->gameW->setCurrentIndex(0);
    timer=new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Game::updateGame);
    connect(ui->exitToGMenu,&QPushButton::clicked, this,&Game::exitToMenu);
    connect(ui->playAgainButton,&QPushButton::clicked,this,&Game::playAgain);
    connect(ui->exitToMenuButton,&QPushButton::clicked,this,&Game::exitToMenu);
    connect(ui->pauseConButton, &QPushButton::clicked, this, &Game::pauseGame);
    connect(ui->startGameButton, &QPushButton::clicked, this, &Game::startGame);
    timer->start(1000/FPS);

}

void Game::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    QPainter painter(this);
    map.draw(&painter);


    for(Unit *unit:user.getUnits()){
        if(unit!=0){
            QPainter painter(this);
            unit->draw(&painter);
        }
    }

    for(Unit *unit:ai.getUnits()){
        if(unit!=0){
            QPainter painter(this);
            unit->draw(&painter);
        }
    }

}


void Game::showResult(){
    ui->gameW->setCurrentIndex(1);
    ui->resultMenu->move((ui->gameW->width()-ui->resultMenu->width())/2,0);

}


void Game::pauseGame(){
    isPauseState = !isPauseState;
    isPauseState ? timer->stop(): timer->start(1000 / FPS);
    isPauseState?ui->pauseIcon->show():ui->pauseIcon->hide();
}

void Game::mousePressEvent(QMouseEvent *event)
{

        if (!isPauseState&&event->button() == Qt::LeftButton) {
            for (Unit *unit : user.getUnits()) {
            unit->selectUnit(event->pos());
            }

        }

        else if (!isPauseState&&event->button() == Qt::RightButton
                 && map.contains(event->pos())) {
            for (Unit *unit : user.getUnits()) {
            isGameStarted?unit->setTarget(event->pos()):unit->manualMove(event->pos(),QRectF(0,0,ui->restrictedArea->width(),ui->restrictedArea->height()),map.getObstacles(),user.getUnits());
            }
        }



}


void Game::manageCollisions() {

    auto coll = [&](const auto& units1, const auto& units2) {

        for (Unit* unit : units1) {
            QPainterPath nextPath = unit->getNextPath();

            // For direct collisions.
            for (Unit* trUnit : units1) {
                if (trUnit != unit && nextPath.intersects(trUnit->getCurrentPath())) {
                    unit->stop();
                }
            }

            for (Obstacle* o : map.getObstacles()) {
                if (unit->getNextPath().intersects(o->getPath())) {
                    unit->stop();
                }
            }

            for (Unit* trUnit : units2) {
                if (nextPath.intersects(trUnit->getCurrentPath())) {
                    unit->stop();
                }

                // Receive attacks (may do opposite)
                if (nextPath.intersects(trUnit->getAttackCollider())) {
                    trUnit->attack(*unit);
                }
            }


        }


    };

    if (user.getUnits().empty() || ai.getUnits().empty()) {
        QPalette palette = ui->resText->palette();
        palette.setColor(QPalette::WindowText, Qt::white);
        ui->resText->setPalette(palette);
        ui->resText->setText(ai.getUnits().empty()?("YOU WON"):("YOU ARE DEFEATED"));
        timer->stop();
        emit showResult();
    } else {
        coll(user.getUnits(), ai.getUnits());
        coll(ai.getUnits(), user.getUnits());
    }
}

void Game::updateGame(){
    if(isGameStarted){
    ai.makeMove(user.getUnits(),map.getObstacles());
    manageCollisions();
    checkHealth();

    for (Unit *unit : user.getUnits()) {
        unit->moveTo();
    }

    for (Unit *unit : ai.getUnits()) {
        unit->moveTo();
    }
    }
    // Update info in the UI.
    QString info="";
    info.append(" Remaining user units: ").append(QString::number(user.getUnits().size())).append("\n");
    info.append(" Remaining enemy units: ").append(QString::number(ai.getUnits().size())).append("\n");
    ui->info->setText(info);
    update(); // calls paintEvent
}



void Game::checkHealth() {

    for (Unit* unit : user.getUnits()) {
        if (unit->getHealth() <= 0) {
            user.removeUnits(unit);
        }
    }

    for (Unit* unit : ai.getUnits()) {
        if (unit->getHealth() <= 0) {
            ai.removeUnits(unit);
        }
    }

}


void Game::startGame()
{
    ui->restrictedArea->setVisible(false);
    ui->startGameButton->setVisible(false);
    isGameStarted=true;
}

