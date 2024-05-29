#include "GameField.h"

#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QtMath>
#include <QtWidgets>

/**
 * Конструктор
 * @param cellSize Размер ячейки
 * @param fieldSize Размер поля (ячеек по ширине и высоте)
 * @param fieldState Состояние поля
 */
GameField::GameField(qreal cellSize, const QPoint &fieldSize, FieldState fieldState, GameWindow* parentWindow):
        parentWindow_(parentWindow),
        cellSize_(cellSize),
        fieldSize_(fieldSize),
        state_(fieldState),
        draggable_({})
{
    // Включить обработку событий кликов мыши
    this->setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton);
}

/**
 * Деструктор
 */
GameField::~GameField() {
    qDeleteAll(shipParts_);
    qDeleteAll(ships_);
}


/**
 * Описывающий прямоугольник
 * @return Прямоугольник
 */
QRectF GameField::boundingRect() const {
    return {QPointF(0.0f,0.0f),QPointF(cellSize_ * 11.0f, cellSize_ * 20.0f)};
}

/**
 * Отрисовка игрового поля со всеми кораблями и прочими объектами
 * @param painter Объект painter
 * @param option Параметры рисования
 * @param widget Указатель на виджет, на котором происходит отрисовка
 */
void GameField::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {

    // Заливка не нужна
    painter->setBrush(Qt::NoBrush);
    // Черные линии толщиной в 1 пиксель
    painter->setPen(QPen(QColor(0,0,0, this->state_ == ENEMY_PREPARING ? 100 : 255),1.0f,Qt::PenStyle::SolidLine,Qt::PenCapStyle::SquareCap,Qt::PenJoinStyle::MiterJoin));

    // Символы первого ряда
    //TODO: придумать что-то на случай если поле больше чем 10 клеток в ширину
    wchar_t symbols[10] = {u'Р',u'Е',u'С',u'П',u'У',u'Б',u'Л',u'И',u'К',u'А'};
//    wchar_t symbols[10] = {'Р','Е','С','П','У','Б','Л','И','К','А'};

    // Нарисовать поле
    for(int i = 0; i < fieldSize_.x()+1; i++)
    {
        for(int j = 0; j < fieldSize_.y()+1; j++)
        {
            // Номера
            if(j == 0 && i > 0){
                painter->drawText(QRectF(j * cellSize_, i * cellSize_, cellSize_, cellSize_), QString::number(i), QTextOption(Qt::AlignCenter));
            }

            // Буквы
            if(i == 0 && j > 0){
                painter->drawText(QRectF(j * cellSize_, i * cellSize_, cellSize_, cellSize_), QString(symbols[j - 1]), QTextOption(Qt::AlignCenter));
            }

            // Клетки
            if(j > 0 && i > 0){
                // Нарисовать клетку
                painter->drawRect(static_cast<int>(j * cellSize_), static_cast<int>(i * cellSize_), cellSize_, cellSize_);
            }
        }
    }

    // Рисование частей кораблей
    for(auto& part : shipParts_)
    {
        // Если часть не принадлежит кораблю, любо корабль есть и он НЕ фантомный
        if(part->ship == nullptr || (part->ship != nullptr && !(part->ship->isPhantom))){
            // Заливка не нужна
            painter->setBrush(Qt::NoBrush);
            // Перо (синие линии)
            painter->setPen(QPen(
                    QColor(0,0,255),
                    4.0f,
                    Qt::PenStyle::SolidLine,
                    Qt::PenCapStyle::SquareCap,
                    Qt::PenJoinStyle::MiterJoin));
            // Рисование части
            part->draw(painter,shipParts_,cellSize_);
        }
        // Если это фантомный корабль (для обозначения)
        else{
            // Отключить карандаш
            painter->setPen(Qt::NoPen);
            // Заливка (цвет меняется в зависимости от нарушения правил размещения)
            painter->setBrush(!part->ship->placementRulesViolated ? QBrush({0,255,0,100}) : QBrush({255,0,0,100}));
            // Рисование части
            painter->drawRect(
                    static_cast<int>((part->position.x() + 1) * cellSize_),
                    static_cast<int>((part->position.y() + 1) * cellSize_),
                    cellSize_,
                    cellSize_);
        }
    }

    // Рисование отметок на клетках
    for(auto& mark : cellMarks_)
    {
        // Черные линии толщиной в 1 пиксель
        painter->setPen(QPen(
                mark->type == CellMark::MISS ? QColor(0,0,0) :
                QColor(100,100,100),
                5.0f,
                Qt::PenStyle::SolidLine,
                Qt::PenCapStyle::RoundCap,
                Qt::PenJoinStyle::MiterJoin));

        // Рисование точки
        painter->drawPoint(
                static_cast<int>((mark->position.x() + 1.5) * cellSize_),
                static_cast<int>((mark->position.y() + 1.5) * cellSize_));
    }
}

/// I N T E R A C T I O N

/**
 * Добавление корабля
 * @param position Положение начальной ячейки (части) корабля
 * @param orientation Ориентация (относительно начальной ячейки)
 * @param shipLength Длина корабля
 * @param isPhantomShip Фантомный корабль (игнорирует правила размещения, но факт нарушения правил фиксируется)
 * @param justCreate Только создать корабль, но не добавлять его
 * @return Указатель на добавленный корабль
 */
Ship* GameField::addShip(const QPoint &position, Ship::Orientation orientation, int shipLength, bool isPhantomShip, bool justCreate)
{
    // Создать корабль
    auto ship = new Ship;
    ship->orientation = orientation;
    ship->placementRulesViolated = false;
    ship->isPhantom = isPhantomShip;

    // Создать части корабля
    for(int i = 0; i < shipLength; i++)
    {
        // Положение новой части на поле
        QPoint partPosition = (ship->orientation == Ship::HORIZONTAL) ? position + QPoint(i,0) : position + QPoint(0,i);

        // Создать часть корабля
        auto part = new ShipPart;
        part->ship = ship;
        part->position = partPosition;
        part->isBeginning = i == 0;
        part->isDestroyed = false;

        // Добавить в корабль
        ship->parts.push_back(part);

        // Добавить на поле
        if(!justCreate){
            shipParts_.push_back(part);
        }
    }

    // Проверить валидность положения корабля
    validateShipPlacement(ship);

    // Добавить в список кораблей
    if(!justCreate){
        ships_.push_back(ship);
    }

    return ship;
}

/**
 * Перемещение корабля
 * @param ship Указатель на корабль
 * @param newPosition Новое положение локального начала
 * @param origin Указатель на часть являющуюся локальным началом (если не указано будет взята "голова" коробля)
 * @param ignoreShip Игнорировать заданный корабль при валидации размещения
 */
void GameField::moveShip(Ship *ship, const QPoint &newPosition, ShipPart *origin, Ship* ignoreShip)
{
    // Получить локальный центр корабля
    auto localOrigin = origin != nullptr ? origin : ship->getHead();
    localOrigin = (localOrigin == nullptr && !ship->parts.empty()) ? ship->parts[0] : localOrigin;

    // Если локальный центр найден
    if(localOrigin != nullptr){
        auto delta = newPosition - localOrigin->position;
        for (auto part : ship->parts) {
            part->position += delta;
        }
    }

    // Валидация положения
    this->validateShipPlacement(ship,ignoreShip);
}

/**
 * Удаление корабля с поля
 * @param ship Указатель на указатель на корабль
 */
void GameField::removeShip(Ship **ship) {

    if(*ship != nullptr && !(*ship)->parts.empty()){
        for(auto part : (*ship)->parts){
            shipParts_.removeOne(part);
            delete part;
        }
    }

    ships_.removeOne((*ship));
    delete (*ship);
    *ship = nullptr;
}

/**
 * Копирование корабля
 * @param sourceShip Исходный корабль
 * @param phantomCopy Фантомная копия
 * @param ignoreShip Игнорировать заданный корабль при валидации размещения
 * @return Указатель на копию
 */
Ship *GameField::copyShip(Ship *sourceShip, bool phantomCopy, Ship* ignoreShip) {

    if(sourceShip != nullptr)
    {
        // Копировать корабль
        auto copyShip = new Ship;
        copyShip->orientation = sourceShip->orientation;
        copyShip->placementRulesViolated = sourceShip->placementRulesViolated;
        copyShip->isPhantom = sourceShip->isPhantom || phantomCopy;

        // Копировать части корабля
        for(auto srcPart : sourceShip->parts)
        {
            auto copyPart = new ShipPart;
            copyPart->position = srcPart->position;
            copyPart->isBeginning = srcPart->isBeginning;
            copyPart->isDestroyed = srcPart->isDestroyed;
            copyPart->ship = copyShip;

            copyShip->parts.push_back(copyPart);
            shipParts_.push_back(copyPart);
        }

        // Валиация положения копии
        validateShipPlacement(copyShip,ignoreShip);

        // Добавить корабль в список
        ships_.push_back(copyShip);

        return copyShip;
    }

    return nullptr;
}

/**
 * Удаление корабля с поля
 * @param ship Указатель на указатель на корабль
 */
void GameField::rotateShip(Ship *ship)
{
    if(ship != nullptr)
    {
        // Найти головную часть корабля
        auto head = ship->getHead();

        // Если есть головная часть
        if(head != nullptr)
        {
            // Создать временный корабль-меркер, с отличающейся от исходного ориентацией, не добавляя его
            Ship* shipToTest = this->addShip(
                    head->position,
                    ship->orientation == Ship::VERTICAL ? Ship::HORIZONTAL : Ship::VERTICAL,
                    ship->parts.size(),
                    true,
                    true);

            // Проверить, может ли такой корабль быть размещен, если игнорировать исходный
            this->validateShipPlacement(shipToTest,ship);

            // Если корабль может быть размещен
            if(!shipToTest->placementRulesViolated)
            {
                // Удалить корабль для теста
                delete shipToTest;

                // Сменить ориентацию корабля
                ship->orientation = ship->orientation == Ship::VERTICAL ? Ship::HORIZONTAL : Ship::VERTICAL;

                // Переместить все части корабля согласно новой ориентации
                for(int i = 0; i < ship->parts.size(); i++)
                {
                    // Положение части
                    QPoint partPosition = (ship->orientation == Ship::HORIZONTAL) ? head->position + QPoint(i,0) : head->position + QPoint(0,i);
                    ship->parts[i]->position = partPosition;
                }
            }
        }
    }
}

/**
 * Добавить метку на клетке
 * @param position Положение
 * @param type Тип Тип метки
 */
void GameField::addMark(const QPoint &position, CellMark::CheckType type)
{
    // Создать отметку
    auto mark = new CellMark;
    mark->position = position;
    mark->type = type;

    // Добавить указатель в список
    this->cellMarks_.push_back(mark);
}

/**
 * Удаление метки
 * @param mark Указатель на указатель на метку
 */
void GameField::removeMark(CellMark **mark) {
    // Убрать из списка
    this->cellMarks_.removeOne(*mark);
    // Удалить из памяти
    delete (*mark);
    // Обнулить укаатель
    *mark = nullptr;
}

/**
 * Установить состояние игрового поля
 * @param state Состояние
 */
void GameField::setState(FieldState state) {
    this->state_ = state;
    this->update(this->boundingRect());
}

/**
 * Выстрел по полю (создание части, либо корабля)
 * @param position Положение на поле
 * @param shotType Тип выстрела
 */
void GameField::shotAt(const QPoint &position, GameField::ShotType shotType) {
    // Если это промашка
    if(shotType == ShotType::MISS){
        // Добавить отметку
        this->addMark(position,CellMark::MISS);
    }
    // Если это попадание (не уничтожение)
    else if(shotType == ShotType::HIT){
        // Добавить часть корабля (без корабля)
        auto part = new ShipPart;
        part->position = position;
        part->isDestroyed = true;
        part->isBeginning = false;
        part->ship = nullptr;
        this->shipParts_.push_back(part);
    }
    // Если это уничтожение корабля
    else if (shotType == ShotType::DESTROYED){

        // Добавить часть корабля (без корабля)
        auto part = new ShipPart;
        part->position = position;
        part->isDestroyed = true;
        part->isBeginning = false;
        part->ship = nullptr;
        this->shipParts_.push_back(part);

        // Добавить корабль
        auto ship = new Ship;
        ship->placementRulesViolated = false;
        ship->isPhantom = false;
        ship->parts.push_back(part);
        this->getAllNeighborsOf(part,&ship->parts);
        this->ships_.push_back(ship);
    }
    // Обновить поле
    this->update(this->boundingRect());
}

/**
 * Получить состояние игрового поля
 * @return Состояние поля
 */
GameField::FieldState GameField::getState() {
    return this->state_;
}

/**
 * Добавить стартовые корабли
 */
void GameField::addStartupShips()
{
    // 1 по 4
    this->addShip({0,11},Ship::HORIZONTAL,4);
    // 2 по 3
    this->addShip({0,13},Ship::HORIZONTAL,3);
    this->addShip({4,13},Ship::HORIZONTAL,3);
    // 3 по 2
    this->addShip({0,15},Ship::HORIZONTAL,2);
    this->addShip({3,15},Ship::HORIZONTAL,2);
    this->addShip({6,15},Ship::HORIZONTAL,2);
    // 4 по 1
    this->addShip({0,17},Ship::HORIZONTAL,1);
    this->addShip({2,17},Ship::HORIZONTAL,1);
    this->addShip({4,17},Ship::HORIZONTAL,1);
    this->addShip({6,17},Ship::HORIZONTAL,1);
}

/**
 * Все ли корабли размещены в пределах поля
 * @return Да или нет
 */
bool GameField::allShipsPlaced() {
    for(auto ship: ships_){
        if(ship->placementRulesViolated){
            return false;
        }
    }
    return true;
}

/**
 * Все ли корабли на поле уничтожены
 * @return Да или нет
 */
bool GameField::allShipsDestroyed()
{
    for(auto ship: ships_){
        if(!ship->isDestroyed()){
            return false;
        }
    }
    return true;
}


/**
 * Установить обработчик события выстрела по вражескому полю
 * @param callback Функция-обработчик
 */
void GameField::setShotAtEnemyCallback(const std::function<void(const QPoint &pos, GameField* gameField, GameWindow* gameWindow)>& callback)
{
    this->shotAtEnemyCallback_ = callback;
}

/**
 * Получить координаты последнего выстрела
 * @return Точка
 */
QPoint GameField::getLastShotCoordinates()
{
    return lastShot_;
}


/// H E L P E R S

/**
 * Проверить не нарушает ли правила размещения корабль
 * @param ship Указатель на корабль
 * @param ignoreShip Игнорировать заданный корабль
 */
void GameField::validateShipPlacement(Ship *ship, Ship* ignoreShip) {
    for(auto part : ship->parts){
        if(!this->validateShipPartPlacement(part,ignoreShip)){
            ship->placementRulesViolated = true;
            return;
        }
    }
    ship->placementRulesViolated = false;
}

/**
 * Проверить не нарушает ли правила размещения часть корабля
 * @param shipPart Указатель на часть корабля
 * @param ignoreShip Игнорировать заданный корабль
 * @return Нет ли нарушений
 */
bool GameField::validateShipPartPlacement(ShipPart *shipPart, Ship* ignoreShip) {

    // Если часть находится за пределами поля - размещение не валидно
    if(!QRect(0,0,fieldSize_.x(),fieldSize_.y()).contains(shipPart->position)){
        return false;
    }

    // Пройтись по всем добавленным на поле частям кораблей
    for(ShipPart* entryPart : shipParts_)
    {
        // Если указан корабль для игнорирования
        if(ignoreShip != nullptr && entryPart->ship == ignoreShip){
            continue;
        }

        // Расстояние между проверяемой и текущей частью
        QPoint delta = {
                qAbs(entryPart->position.x() - shipPart->position.x()),
                qAbs(entryPart->position.y() - shipPart->position.y())
        };

        // Если часть корабля слишком близко, и ее корабль это не корабль этой части
        if((delta.x() < 2 && delta.y() < 2) && entryPart->ship != shipPart->ship)
        {
            return false;
        }
    }

    return true;
}

/**
 * Нарисовать часть
 * @param painter Объект QtPainter
 * @param existingParts Части кораблей
 * @param cellSize Размер ячейки
 */
void ShipPart::draw(QPainter *painter, const QVector<ShipPart*> &existingParts, qreal cellSize)
{
    // Координаты вершин
    QPoint v0 = {static_cast<int>((this->position.x() + 1) * cellSize), static_cast<int>((this->position.y() + 1) * cellSize)};
    QPoint v1 = {static_cast<int>((this->position.x() + 2) * cellSize), static_cast<int>((this->position.y() + 1) * cellSize)};
    QPoint v2 = {static_cast<int>((this->position.x() + 2) * cellSize), static_cast<int>((this->position.y() + 2) * cellSize)};
    QPoint v3 = {static_cast<int>((this->position.x() + 1) * cellSize), static_cast<int>((this->position.y() + 2) * cellSize)};

    // Части среди которых ведестя поиск соседних
    // Если часть связана с кораблем, то достаточно перебрать его части, иначе нужны все части без корабля
    auto possibleNeighbors = this->ship != nullptr ? this->ship->parts.toStdVector() : GameField::findAllOf(existingParts.toStdVector(),[&](ShipPart* part){
        return part->ship == nullptr;
    });

    // Соседние части
    ShipPart* partUp = GameField::findAt(this->position - QPoint(0,1), possibleNeighbors);
    ShipPart* partRight = GameField::findAt(this->position + QPoint(1,0),possibleNeighbors);
    ShipPart* partBottom = GameField::findAt(this->position + QPoint(0,1),possibleNeighbors);
    ShipPart* partLeft = GameField::findAt(this->position - QPoint(1,0),possibleNeighbors);

    // Верхнее ребро
    if(partUp == nullptr){
        painter->drawLine(v0,v1);
    }

    // Правое ребро
    if(partRight == nullptr){
        painter->drawLine(v1,v2);
    }

    // Нижнее ребро
    if(partBottom == nullptr){
        painter->drawLine(v2,v3);
    }

    // Левое ребро
    if(partLeft == nullptr){
        painter->drawLine(v3,v0);
    }

    // Если часть корабля уничтожена - нарисовать крест
    if(this->isDestroyed){
        painter->drawLine(v0,v2);
        painter->drawLine(v1,v3);
    }
}

/**
 * Пуста ли ячейка по указанным координатам
 * @param position Координаты
 * @return Да или нет
 */
bool GameField::isCellEmptyAt(const QPoint &position)
{
    auto shipPartIt = std::find_if(shipParts_.begin(),shipParts_.end(),[&](ShipPart* entry){
        return entry->position == position;
    });

    auto cellMarkIt = std::find_if(cellMarks_.begin(),cellMarks_.end(),[&](CellMark* entry){
        return entry->position == position;
    });

    return shipPartIt == shipParts_.end() && cellMarkIt == cellMarks_.end();
}

/**
 * Получить часть корабля с заданным положением среди всех частей кораблей на поле
 * @param position Положение
 * @return Указатель на часть корабля
 */
ShipPart *GameField::findAt(const QPoint &position)
{
    return GameField::findAt(position,shipParts_.toStdVector());
}

/**
 * Получить часть корабля с заданным положением среди частей
 * @param position Положение
 * @param parts Части среди которых искать
 * @return Указатель на часть корабля
 */
ShipPart *GameField::findAt(const QPoint &position, const std::vector<ShipPart*>& parts)
{
    auto elementIt = std::find_if(parts.begin(),parts.end(),[&](ShipPart* entry){
        return entry->position == position;
    });

    if(elementIt == parts.end()){
        return nullptr;
    }

    return *elementIt;
}

/**
 * Получить все части удовлетворяющее определенному условию
 * @param parts Массив частей
 * @param condition Условие (функция обратного вызкова)
 * @return Массив удовлетворяющих условию частей
 */
std::vector<ShipPart *> GameField::findAllOf(const std::vector<ShipPart *> &parts, const std::function<bool(ShipPart*)>& condition)
{
    std::vector<ShipPart*> results;
    std::copy_if(parts.begin(), parts.end(), std::back_inserter(results), condition);
    return results;
}

/**
 * Преобразовать координаты сцены в координаты игрового поля
 * @param sceneSpacePoint Точка в координатах сцены
 * @return Точка в координатах игрового поля
 */
QPoint GameField::toGameFieldSpace(const QPointF &sceneSpacePoint) {
    return {
        qFloor(sceneSpacePoint.x() / cellSize_) - 1,
        qFloor(sceneSpacePoint.y() / cellSize_) - 1
    };
}

/**
 * Получить головную часть корабля
 * @return Указатель на головную часть (если есть) либо nullptr
 */
ShipPart *Ship::getHead() {
    auto elementIt = std::find_if(parts.begin(),parts.end(),[&](ShipPart* entry){
        return entry->isBeginning;
    });

    if(elementIt == parts.end()){
        return nullptr;
    }

    return *elementIt;
}

/**
 * Уничтожен ли корабль
 */
bool Ship::isDestroyed()
{
    for(auto part : parts){
        if(!part->isDestroyed) return false;
    }

    return true;
}

/**
 * Рекурсивно заполнить массив соседями части
 * @param part Исходная часть
 * @param neighbors Указатель на массив соседей
 */
void GameField::getAllNeighborsOf(ShipPart *part, QVector<ShipPart*>* neighbors) {

    // Получить все части в виде обычного std массива
    auto parts = this->shipParts_.toStdVector();

    // Получить указатели на соседние части
    auto top = GameField::findAt(part->position - QPoint(0,1),parts);
    auto right = GameField::findAt(part->position + QPoint(1,0),parts);
    auto bottom = GameField::findAt(part->position + QPoint(0,1),parts);
    auto left = GameField::findAt(part->position - QPoint(1,0),parts);

    // Если были обнаружены части и если они еще не были добавлены с писок соседей
    if(top != nullptr && neighbors->indexOf(top) == -1){
        neighbors->push_back(top);
        this->getAllNeighborsOf(top, neighbors);
    }

    if(right != nullptr && neighbors->indexOf(right) == -1){
        neighbors->push_back(right);
        this->getAllNeighborsOf(right, neighbors);
    }

    if(bottom != nullptr && neighbors->indexOf(bottom) == -1){
        neighbors->push_back(bottom);
        this->getAllNeighborsOf(bottom, neighbors);
    }

    if(left != nullptr && neighbors->indexOf(left) == -1){
        neighbors->push_back(left);
        this->getAllNeighborsOf(left, neighbors);
    }
}

/// E V E N T S

/**
 * Обработчик события нажатия кнопки мыши
 * @param event Событие
 */
void GameField::mousePressEvent(QGraphicsSceneMouseEvent *event) {

    // Если состояние поля "подготовка"
    if(this->state_ == FieldState::PREPARING)
    {
        // Если зажали ЛКМ
        if(event->button() == Qt::LeftButton)
        {
            // Сменить курсор
            setCursor(Qt::SizeAllCursor);

            // Получить положение курсора в координатах игрового поля
            QPoint pos = this->toGameFieldSpace(event->pos());
            // Часть которая может находится в данной клетке
            ShipPart* part = GameField::findAt(pos,shipParts_.toStdVector());

            // Если это часть не фантомного корабля
            if(part != nullptr && part->ship != nullptr && !part->ship->isPhantom)
            {
                // Информация о целевом корабле
                draggable_.targetOriginPart = part;
                draggable_.targetShip = part->ship;

                // Сделать фантомную копию корабля
                draggable_.marketShip = this->copyShip(draggable_.targetShip, true,draggable_.targetShip);
                // Найти ту часть фантомного корабля на которой сейчас курсор
                draggable_.markerOriginPart = GameField::findAt(pos,draggable_.marketShip->parts.toStdVector());
                // Перерисовать поле
                this->update(this->boundingRect());
            }
        }
            // Если зажали ПКМ, и до этого не было активировано перетаскивание
        else if(event->button() == Qt::RightButton && draggable_.targetShip == nullptr)
        {
            // Получить положение курсора в координатах игрового поля
            QPoint pos = this->toGameFieldSpace(event->pos());
            // Часть которая может находится в данной клетке
            ShipPart* part = GameField::findAt(pos,shipParts_.toStdVector());

            // Если это часть не фантомного корабля
            if(part != nullptr && part->ship != nullptr && !part->ship->isPhantom)
            {
                // Повернуть, если это возможно
                this->rotateShip(part->ship);
                // Перерисовать поле
                this->update(this->boundingRect());
            }
        }
    }
    // Если состояние поля "подготовка"
    else if(this->state_ == FieldState::ENEMY_READY && this->shotAtEnemyCallback_ != nullptr)
    {
        // Получить положение курсора в координатах игрового поля
        QPoint pos = this->toGameFieldSpace(event->pos());
        // Сохранить данные о последнем "выстреле"
        lastShot_ = pos;
        // Вызвать метод обратного вызова, передав координаты
        this->shotAtEnemyCallback_(pos,this,parentWindow_);
    }
}

/**
 * Обработка события движения мыши при зажатой кнопке
 * @param event Событие
 */
void GameField::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {

    // Если состояние поле "подготовка"
    if(this->state_ == FieldState::PREPARING)
    {
        // Получить положение курсора в координатах игрового поля
        QPoint pos = this->toGameFieldSpace(event->pos());

        // Если есть маркер и часть, за которую "схватили"
        if(draggable_.marketShip != nullptr && draggable_.markerOriginPart != nullptr)
        {
            // Если у части, за которую "схватили" позиция отличается от текущй поизиции курсора
            if(draggable_.markerOriginPart->position != pos){
                // Сдвинуть корабль-маркер
                this->moveShip(draggable_.marketShip,pos,draggable_.markerOriginPart,draggable_.targetShip);
                // Перерисовать поле
                this->update(this->boundingRect());
            }
        }
    }
}

/**
 * Обработчик события отпускания кнопки мыши
 * @param event Событие
 */
void GameField::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {

    // Если состояние поле "подготовка"
    if(this->state_ == FieldState::PREPARING)
    {
        // Если отпустили ЛКМ
        if(event->button() == Qt::LeftButton)
        {
            // Сменить курсор
            setCursor(Qt::ArrowCursor);

            // Получить положение курсора в координатах игрового поля
            QPoint pos = this->toGameFieldSpace(event->pos());

            // Если есть данные о фантомном корабле
            if(draggable_.marketShip != nullptr)
            {
                // Если положение в которое был перемещен фантомный корабль - легально
                if(!draggable_.marketShip->placementRulesViolated){
                    // Переместить туда корабль целевой
                    if(draggable_.targetShip != nullptr && draggable_.targetOriginPart != nullptr){
                        this->moveShip(draggable_.targetShip,pos,draggable_.targetOriginPart);
                        draggable_.targetShip->placementRulesViolated = false;
                    }
                }

                // Удалить фантомный корабль
                draggable_.markerOriginPart = nullptr;
                this->removeShip(&draggable_.marketShip);

                // Перерисовать поле
                this->update(this->boundingRect());
            }

            // Очистить указатели
            draggable_.targetShip = nullptr;
            draggable_.targetOriginPart = nullptr;
        }
    }
}