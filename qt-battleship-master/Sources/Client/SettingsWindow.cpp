#include "SettingsWindow.h"
#include "./ui_SettingsWindow.h"

/// Настройки - IP сервера
extern QString _ip;
/// Настройки - Порт сервера
extern unsigned _port;

/**
 * Конструктор окна
 * @param parent Родительский элемент
 */
SettingsWindow::SettingsWindow(QWidget *parent):QWidget(parent),ui_(new Ui::SettingsWindow)
{
    // Инициализация UI
    ui_->setupUi(this);

    // Обновить поля ввода
    this->refreshFields();
}

/**
 * Деструктор окна
 */
SettingsWindow::~SettingsWindow()
{
    delete ui_;
}

/**
 * Обновить содержимое полей (взять из текущих настроек)
 */
void SettingsWindow::refreshFields()
{
    this->ui_->editIp->setText(_ip);
    this->ui_->editPort->setText(QString::number(_port));
}

/// S L O T S

/**
 * Обработка события нажатия на кнопку подключения
 */
void SettingsWindow::on_btnApply_clicked()
{
    // Сохранить введенные значения в глобальных переменных настроек
    _ip = this->ui_->editIp->text();
    _port = this->ui_->editPort->text().toUInt();

    // Закрыть текущее окно
    this->close();
}