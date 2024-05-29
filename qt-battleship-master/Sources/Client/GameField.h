#pragma once

#include <QGraphicsItem>
#include <functional>

/// Часть корабля (объявление)
struct ShipPart;

/// Корабль
struct Ship
{
    // Ориентация корабля
    enum Orientation {
        HORIZONTAL,
        VERTICAL
    } orientation = Orientation::HORIZONTAL;

    // Части корабля
    QVector<ShipPart*> parts;

    // Было нарушение правил размещения корабля
    bool placementRulesViolated = false;

    // Фантомный корабль (для обозначения)
    bool isPhantom = false;

    /**
     * Получить головную часть корабля
     * @return Указатель на головную часть (если есть) либо nullptr
     */
    ShipPart* getHead();

    /**
     * Уничтожен ли корабль
     */
    bool isDestroyed();
};

/// Часть корабля
struct ShipPart
{
    // Корабль
    Ship* ship = nullptr;

    // Часть уничтожена
    bool isDestroyed = false;

    // Начальная часть (относительно которой может осуществляться смена ориентации)
    bool isBeginning = false;

    // Положение на поле
    QPoint position = {};

    /**
     * Нарисовать часть
     * @param painter Объект QtPainter
     * @param existingParts Части кораблей
     * @param cellSize Размер ячейки
     */
    void draw(QPainter* painter, const QVector<ShipPart*>& existingParts, qreal cellSize);
};

/// Метка
struct CellMark
{
    // Тип отметки
    enum CheckType {
        MISS,
        CHECKED
    } type = CheckType::CHECKED;

    // Положение на поле
    QPoint position = {};
};

/// Предварительное объявление (для использования в аргументах функций)
class GameWindow;

/// Игровое поле
class GameField final : public QGraphicsItem {

public:
    /// Состояние поля
    enum FieldState {
        // Подготовка (есть корабли, можно перемещать их)
        PREPARING,
        // Готово (есть корабли, перемещать нельзя)
        READY,
        // Подготовка вражеского поля (поле пустое, блеклое, взаимодействовать нельзя)
        ENEMY_PREPARING,
        // Вражеское поле готово (поле пустое, можно стрелять по полю)
        ENEMY_READY
    };

    /// Тип выстрела по полю
    enum ShotType {
        MISS,
        HIT,
        DESTROYED
    };

public:
    /**
     * Конструктор
     * @param cellSize Размер ячейки
     * @param fieldSize Размер поля (ячеек по ширине и высоте)
     * @param fieldState Состояние поля
     */
    explicit GameField(qreal cellSize, const QPoint& fieldSize, FieldState fieldState, GameWindow* parentWindow);

    /**
     * Деструктор
     */
    ~GameField() override;

    /**
     * Описывающий прямоугольник
     * @return Прямоугольник
     */
    QRectF boundingRect() const override;

    /**
     * Отрисовка игрового поля со всеми кораблями и прочими объектами
     * @param painter Объект painter
     * @param option Параметры рисования
     * @param widget Указатель на виджет, на котором просиходит отрисовка
     */
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    /**
     * Добавление корабля
     * @param position Положение начальной ячейки (части) корабля
     * @param orientation Ориентация (относительно начальной ячейки)
     * @param shipLength Длина корабля
     * @param isPhantomShip Фантомный корабль (игнорирует правила размещения, но факт нарушения правил фиксируется)
     * @param justCreate Только создать корабль, но не добавлять его
     * @return Указатель на добавленный корабль
     */
    Ship* addShip(const QPoint& position, Ship::Orientation orientation, int shipLength = 1, bool isPhantomShip = false, bool justCreate = false);

    /**
     * Перемещение корабля
     * @param ship Указатель на корабль
     * @param newPosition Новое положение локального начала
     * @param origin Указатель на часть являющуюся локальным началом (если не указано будет взята "голова" коробля)
     * @param ignoreShip Игнорировать заданный корабль при валидации размещения
     */
    void moveShip(Ship* ship, const QPoint& newPosition, ShipPart* origin = nullptr, Ship* ignoreShip = nullptr);

    /**
     * Удаление корабля с поля
     * @param ship Указатель на указатель на корабль
     */
    void removeShip(Ship** ship);

    /**
     * Копирование корабля
     * @param sourceShip Исходный корабль
     * @param phantomCopy Фантомная копия
     * @param ignoreShip Игнорировать заданный корабль при валидации размещения
     * @return Указатель на копию
     */
    Ship* copyShip(Ship* sourceShip, bool phantomCopy = false, Ship* ignoreShip = nullptr);

    /**
     * Смена ориентации корабля
     * @param ship Указатель на корабль
     */
    void rotateShip(Ship* ship);

    /**
     * Добавить метку на клетке
     * @param position Положение
     * @param type Тип Тип метки
     */
    void addMark(const QPoint& position, CellMark::CheckType type);

    /**
     * Удаление метки
     * @param mark Указатель на указатель на метку
     */
    void removeMark(CellMark** mark);

    /**
     * Получить координаты последнего выстрела
     * @return Точка
     */
    QPoint getLastShotCoordinates();

    /**
     * Выстрел по полю (создание части, либо корабля)
     * @param position Положение на поле
     * @param shotType Тип выстрела
     */
    void shotAt(const QPoint& position, ShotType shotType);

    /**
     * Установить состояние игрового поля
     * @param state Состояние
     */
    void setState(FieldState state);

    /**
     * Получить состояние игрового поля
     * @return Состояние поля
     */
    FieldState getState();

    /**
     * Добавить стартовые корабли
     */
    void addStartupShips();

    /**
     * Все ли корабли размещены в пределах поля
     * @return Да или нет
     */
    bool allShipsPlaced();

    /**
     * Все ли корабли на поле уничтожены
     * @return Да или нет
     */
    bool allShipsDestroyed();

    /**
     * Установить обработчик события выстрела по вражескому полю
     * @param callback Функция-обработчик
     */
    void setShotAtEnemyCallback(const std::function<void(const QPoint &pos, GameField* gameField, GameWindow* gameWindow)>& callback);

    /**
     * Пуста ли ячейка по указанным координатам
     * @param position Координаты
     * @return Да или нет
     */
    bool isCellEmptyAt(const QPoint& position);

    /**
     * Получить часть корабля с заданным положением среди всех частей кораблей на поле
     * @param position Положение
     * @return Указатель на часть корабля
     */
    ShipPart* findAt(const QPoint& position);

    /**
     * Получить часть корабля с заданным положением среди частей
     * @param position Положение
     * @param parts Части среди которых искать
     * @return Указатель на часть корабля
     */
    static ShipPart* findAt(const QPoint& position, const std::vector<ShipPart*>& parts);

    /**
     * Получить все части удовлетворяющее определенному условию
     * @param parts Массив частей
     * @param condition Условие (функция обратного вызкова)
     * @return Массив удовлетворяющих условию частей
     */
    static std::vector<ShipPart*> findAllOf(const std::vector<ShipPart*>& parts, const std::function<bool(ShipPart*)>& condition);

protected:
    /**
     * Обработчик события нажатия кнопки мыши
     * @param event Событие
     */
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

    /**
     * Обработка события движения мыши при зажатой кнопке
     * @param event Событие
     */
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

    /**
     * Обработчик события отпускания кнопки мыши
     * @param event Событие
     */
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

private:
    /// Родительское окно
    GameWindow* parentWindow_;

    /// Координаты последнего выстрела по полю
    QPoint lastShot_;

    /// Размер клетки поля
    qreal cellSize_;

    /// Размер поля
    QPoint fieldSize_;

    /// Массив кораблей
    QVector<Ship*> ships_;

    /// Массив частей кораблей
    QVector<ShipPart*> shipParts_;

    /// Массив отметок клеток
    QVector<CellMark*> cellMarks_;

    /// Перетаскивание кораблей
    struct {
        // Целевой корабль (который нужно переместить)
        Ship* targetShip = nullptr;
        // Часть целевого корабля которая оказалось под курсором
        ShipPart* targetOriginPart = nullptr;
        // Корабль маркер (фантомный)
        Ship* marketShip = nullptr;
        // Часть фантомного корабля которая оказалось под курсором
        ShipPart* markerOriginPart = nullptr;
    } draggable_;

    /// Состояние поля
    FieldState state_;

    /// Функция обратного вызова для выстрелов по вражескому полю
    std::function<void(const QPoint &pos, GameField* gameField, GameWindow* gameWindow)> shotAtEnemyCallback_ = nullptr;

    /**
     * Проверить не нарушает ли правила размещения корабль
     * @param ship Указатель на корабль
     * @param ignoreShip Игнорировать заданный корабль
     */
    void validateShipPlacement(Ship* ship, Ship* ignoreShip = nullptr);

    /**
     * Проверить не нарушает ли правила размещения часть корабля
     * @param shipPart Указатель на часть корабля
     * @param ignoreShip Игнорировать заданный корабль
     * @return Нет ли нарушений
     */
    bool validateShipPartPlacement(ShipPart* shipPart, Ship* ignoreShip = nullptr);

    /**
     * Преобразовать координаты сцены в координаты игрового поля
     * @param sceneSpacePoint Точка в координатах сцены
     * @return Точка в координатах игрового поля
     */
    QPoint toGameFieldSpace(const QPointF& sceneSpacePoint);

    /**
     * Рекурсивно заполнить массив соседями части
     * @param part Исходная часть
     * @param neighbors Указатель на массив соседей
     */
    void getAllNeighborsOf(ShipPart* part, QVector<ShipPart*>* neighbors);
};