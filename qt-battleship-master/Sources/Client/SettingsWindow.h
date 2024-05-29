#pragma once

#include <QWidget>

// Класс для взаимодействия с UI интерфейсом
QT_BEGIN_NAMESPACE
namespace Ui { class SettingsWindow; }
QT_END_NAMESPACE

class SettingsWindow final : public QWidget
{
Q_OBJECT

public:
    /**
     * Конструктор окна
     * @param parent Родительский элемент
     */
    explicit SettingsWindow(QWidget *parent = nullptr);

    /**
     * Деструктор окна
     */
    ~SettingsWindow() override;

    /**
     * Обновить содержимое полей (взять из текущих настроек)
     */
    void refreshFields();

private slots:
    /**
     * Обработка события нажатия на кнопку подключения
     */
    void on_btnApply_clicked();

private:
    /// Указатель на UI объект
    Ui::SettingsWindow *ui_;

};
