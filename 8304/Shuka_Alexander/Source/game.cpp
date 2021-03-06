#include "game.h"


Game::Game()
{
    factory = std::make_shared<RandomObjectFactory>();
    playerUnits = std::make_shared<std::set<std::shared_ptr<unit::Unit>>>();
    enemyUnits = std::make_shared<std::set<std::shared_ptr<unit::Unit>>>();
}


void Game::run()
{
    init();

    isRun = true;
    while (isRun) {
        auto command = input();
        logic(command);
        enemyLogic();

        if (playerBase->getHealthPoints() <= 0) {
            std::cout << "ENEMY WON!\n";
            return;
        }
        else if (enemyBase->getHealthPoints() <= 0) {
            std::cout << "YOU WON!\n";
            return;
        }

        std::set<std::shared_ptr<unit::Unit>> tmp;
        for (auto i : *playerUnits) {
            if (i->getHealthPoints() <= 0) {
                tmp.insert(i);
            }
        }

        for (auto i : tmp) {
            playerUnits->erase(i);
            i->notify();
            field->deleteUnit(i);
        }

        tmp.clear();
        for (auto i : *enemyUnits) {
            if (i->getHealthPoints() <= 0) {
                tmp.insert(i);
            }
        }

        for (auto i : tmp) {
            enemyUnits->erase(i);
            i->notify();
            field->deleteUnit(i);
        }

        draw();
        Sleep(150);
    }
}


void Game::init()
{
    this->enemyGold = 0;
    this->playerGold = 0;
    isPlayerAttack = false;

    createField();
    mediator = std::make_shared<Mediator>(field);

    playerBase = std::make_shared<unit::Base>(Point2D(1, field->getHeight()/2),
                                              mediator, unit::PLAYER::ONE);

    enemyBase = std::make_shared<unit::Base>(Point2D(field->getWidth() - 2,
                                                     field->getHeight()/2),
                                             mediator, unit::PLAYER::TWO);

    field->addUnit(playerBase);
    field->addUnit(enemyBase);

    playerUnits->insert(playerBase);
    enemyUnits->insert(enemyBase);

    playerFacade = std::make_shared<Facade>(mediator, playerBase, playerUnits,
                                            enemyBase, field);
    enemyFacade = std::make_shared<Facade>(mediator, enemyBase, enemyUnits,
                                           playerBase, field);

    std::cout << "a - armor\n. - road\n+ - heal\n* - fog\nx - spikes\n"
                 "- - bomb\n^ - tower\nB- base\n";
    //Sleep(3000);
}


void Game::createField()
{
    field = std::make_shared<GameField>(19, 31);

    auto spikes = std::make_shared<SpikesLandscape>();
    auto fog = std::make_shared<FogLandscape>();
    auto proxy = std::make_shared<ProxyLandscape>(spikes);

    field->getCell(Point2D(0, 0))->setLandscape(spikes);

    for (size_t i = 0; i < field->getHeight(); ++i) {
        field->getCell(Point2D(0, i))->setLandscape(proxy);
        field->getCell(Point2D(field->getWidth() - 1, i))->setLandscape(proxy);
    }

    for (size_t i = 0; i < field->getWidth(); ++i) {
        field->getCell(Point2D(i, 0))->setLandscape(proxy);
        field->getCell(Point2D(i, field->getHeight() - 1))->setLandscape(proxy);
    }

    proxy = std::make_shared<ProxyLandscape>(fog);

    for (size_t i = 0; i < field->getHeight(); ++i) {
        if (abs(static_cast<long long>(i) -
                static_cast<long long>(field->getHeight() / 4)) >= 2 &&
                abs(static_cast<long long>(i) -
                    static_cast<long long>(3 * field->getHeight() / 4)) >= 2) {
            field->getCell(Point2D(field->getWidth()/2, i))->setLandscape(proxy);
        }
    }
}


void Game::draw() const
{
    std::string res;
    for (size_t i = 0; i < field->getHeight(); ++i) {
        for (size_t  j = 0; j < field->getWidth(); ++j) {
            auto cell = field->getCell(Point2D(j, i));

            if (cell->isEmpty()) {
                auto& object = *cell->getNeutralObject();
                auto landscape = cell->getLandscape();
                if (typeid (object) != typeid(NoObject)) {
                    res += object.draw();
                }
                else {
                    res += landscape->draw();
                }
            }
            else {
                res += cell->getUnit()->draw();
            }
            res += ' ';
        }
        res += '\n';
    }

    res += "\nGold: " + std::to_string(playerGold) + "\n";
    res += "Base HP: " + std::to_string(playerBase->getHealthPoints()) + "\n";

    res += "q - quit, 1 - standing, 2 - ground, 3 - flying\n"
           "a - attack, d - defense\n";
    system("cls");
    std::cout << res;
}


void Game::farmEnemy()
{
    for (auto i : *enemyUnits) {
        enemyGold += i->farm();
    }
}


void Game::farmPlayer()
{
    for (auto i : *playerUnits) {
        playerGold += i->farm();
    }
}


void Game::logic(COMMAND command)
{
    farmPlayer();

    switch (command) {
    case COMMAND::ATTACK:
        isPlayerAttack = true;
        break;
    case COMMAND::DEFFEND:
        isPlayerAttack = false;
        break;
    case COMMAND::CRT_FLYING:
        if (playerGold > 220) {
            auto isCreate = playerFacade->createFlyingUnit();
            if (isCreate) {
                playerGold -= 220;
            }
        }
        break;
    case COMMAND::CRT_GROUND:
        if (playerGold > 150) {
            auto isCreate = playerFacade->createGroundUnit();
            if (isCreate) {
                playerGold -= 150;
            }
        }
        break;
    case COMMAND::CRT_STANDING:
        if (playerGold > 200) {
            auto isCreate = playerFacade->createStandingUnit();
            if (isCreate) {
                playerGold -= 200;
            }
        }
        break;
    case COMMAND::EXIT:
        isRun = false;
        break;
    default:
        break;
    }

    if (isPlayerAttack) {
        playerFacade->charge();
    }
    else {
        playerFacade->deffend();
    }
}


void Game::enemyLogic()
{
    farmEnemy();
    enemyFacade->charge();

    srand(time(0));
    auto choose = rand() % 2;

    if (choose == 1) {
        choose = rand() % 3;
        switch (choose) {
        case 0:
            if (enemyGold > 200) {
                auto isCreate = enemyFacade->createStandingUnit();
                if (isCreate) {
                    enemyGold -= 200;
                }
            }
            break;
        case 1:
            if (enemyGold > 150) {
                auto isCreate = enemyFacade->createGroundUnit();
                if (isCreate) {
                    enemyGold -= 150;
                }
            }
            break;
        case 2:
            if (enemyGold > 220) {
                auto isCreate = enemyFacade->createFlyingUnit();
                if (isCreate) {
                    enemyGold -= 220;
                }
            }
            break;
        }
    }
}


COMMAND Game::input()
{
    if (kbhit()) {
        switch (getch()) {
        case 'a':
            return COMMAND::ATTACK;
        case 'd':
            return COMMAND::DEFFEND;
        case '1':
            return COMMAND::CRT_STANDING;
        case '2':
            return COMMAND::CRT_GROUND;
        case '3':
            return COMMAND::CRT_FLYING;
        case 'q':
            return COMMAND::EXIT;
        }
    }
    return COMMAND::NO_COMMAND;
}
