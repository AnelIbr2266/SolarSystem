#include "mainwindow.h"
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QTimer>
#include <QPainter>
#include <QMouseEvent>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDialog>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QMessageBox>
#include <QSignalBlocker>
#include <QRandomGenerator>
#include <cmath>
#include <algorithm>

// -------------------- SceneWidget --------------------
SceneWidget::SceneWidget(SolarSystem *system, QWidget *parent)
    : QWidget(parent),
      m_system(system)
{
    setMinimumSize(600, 480);
}

void SceneWidget::setSelected(const QString &name)
{
    m_selected = name;
    update();
}

void SceneWidget::advance(double seconds)
{
    for (const auto &object : m_system->objects())
    {
        if (object->typeName() == "Планета")
            object->move(seconds);
    }
}

// Вычисление положения небесных тел
std::vector<SceneWidget::Body> SceneWidget::positions() const
{
    std::vector<Body> result;

    double maxOrbit = 1.0;

    for (const auto &object : m_system->objects())
    {
        if (object->orbit() > maxOrbit)
            maxOrbit = object->orbit();
    }

    QPointF center(width() / 2.0, height() / 2.0);
    double maxRadius = std::min(width(), height()) / 2.0 - 40.0;
    for (const auto &object : m_system->objects())
    {
        QPointF position = center;
        double radius = 5.0 + object->radius() / 69911.0 * 10.0;
        // Солнце находится в центре
        if (object->typeName() == "Звезда")
        {
            radius = 34.0;
        }
        else
        {
            // Вычисляем положение планеты на орбите
            double orbit = 80.0
                + object->orbit() / maxOrbit * (maxRadius - 80.0);

            double radians = qDegreesToRadians(object->angle());

            position += QPointF(
                std::cos(radians) * orbit,
                std::sin(radians) * orbit
            );
        }

        result.push_back({object, position, radius});
    }

    return result;
}

// Отрисовка Солнечной системы
void SceneWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), QColor(10, 12, 30));

    auto bodies = positions();

    QPointF center(width() / 2.0, height() / 2.0);

    // Рисуем орбиты планет
    painter.setPen(QPen(QColor(90, 100, 130), 1));
    painter.setBrush(Qt::NoBrush);

    for (const Body &body : bodies)
    {
        if (body.object->typeName() == "Планета")
        {
            double orbit = QLineF(center, body.position).length();

            painter.drawEllipse(center, orbit, orbit);
        }
    }

    // Рисуем Солнце и планеты
    for (const Body &body : bodies)
    {
        painter.setPen(Qt::NoPen);
        painter.setBrush(body.object->color());

        painter.drawEllipse(
            body.position,
            body.radius,
            body.radius
        );

        // Выделяем выбранный объект
        if (body.object->name() == m_selected)
        {
            painter.setBrush(Qt::NoBrush);
            painter.setPen(QPen(Qt::white, 1.5, Qt::DashLine));

            painter.drawEllipse(
                body.position,
                body.radius + 6,
                body.radius + 6
            );
        }
    }

    // Подписываем небесные тела
    painter.setPen(QColor(220, 225, 240));

    for (const Body &body : bodies)
    {
        painter.drawText(
            body.position + QPointF(9, -9),
            body.object->name()
        );
    }
}

// Обработка нажатия мыши на небесное тело
void SceneWidget::mousePressEvent(QMouseEvent *event)
{
    auto bodies = positions();

    for (const Body &body : bodies)
    {
        QPointF difference = body.position - event->position();

        double distance = std::sqrt(
            difference.x() * difference.x()
            + difference.y() * difference.y()
        );

        if (distance <= body.radius + 8)
        {
            setSelected(body.object->name());
            emit objectClicked(body.object->name());
            return;
        }
    }
}

// -------------------- MainWindow --------------------

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      m_system(SolarSystem::createDefault())
{
    setWindowTitle("Солнечная система");
    resize(1100, 700);

    buildInterface();

    refreshList();
    showInfo("Солнце");

    m_timer = new QTimer(this);

    connect(m_timer, &QTimer::timeout,
            this, &MainWindow::onTick);

    m_timer->start(30);
}

// Создание интерфейса
void MainWindow::buildInterface()
{
    QWidget *central = new QWidget(this);

    QHBoxLayout *mainLayout = new QHBoxLayout(central);

    // Область отображения Солнечной системы
    m_scene = new SceneWidget(&m_system, central);

    connect(m_scene, &SceneWidget::objectClicked,
            this, &MainWindow::onObjectClicked);

    mainLayout->addWidget(m_scene, 1);

    // Правая панель
    QWidget *panel = new QWidget(central);

    panel->setFixedWidth(300);

    QVBoxLayout *panelLayout = new QVBoxLayout(panel);

    QLabel *title = new QLabel("Солнечная система");

    QFont font = title->font();
    font.setBold(true);
    font.setPointSize(14);

    title->setFont(font);

    panelLayout->addWidget(title);

    // Кнопка запуска и остановки движения
    m_startButton = new QPushButton("Стоп");

    connect(m_startButton, &QPushButton::clicked,
            this, &MainWindow::onToggle);

    panelLayout->addWidget(m_startButton);
    // Список небесных тел
    panelLayout->addWidget(new QLabel("Небесные тела:"));

    m_list = new QListWidget();

    connect(m_list, &QListWidget::currentItemChanged,
            this,
            [this](QListWidgetItem *current, QListWidgetItem *)
            {
                if (current != nullptr)
                {
                    showInfo(
                        current->data(Qt::UserRole).toString()
                    );
                }
            });

    panelLayout->addWidget(m_list, 1);

    // Кнопки добавления и удаления
    QPushButton *addButton = new QPushButton("Добавить планету");
    QPushButton *removeButton = new QPushButton("Удалить планету");

    connect(addButton, &QPushButton::clicked,
            this, &MainWindow::onAddPlanet);

    connect(removeButton, &QPushButton::clicked,
            this, &MainWindow::onRemove);

    panelLayout->addWidget(addButton);
    panelLayout->addWidget(removeButton);

    // Информация о выбранном объекте
    panelLayout->addWidget(new QLabel("Информация:"));

    m_info = new QLabel();

    m_info->setWordWrap(true);
    m_info->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    m_info->setMinimumHeight(160);
    m_info->setFrameShape(QFrame::StyledPanel);
    m_info->setMargin(6);

    panelLayout->addWidget(m_info);

    mainLayout->addWidget(panel);

    setCentralWidget(central);
}

// Обновление списка объектов
void MainWindow::refreshList()
{
    QString current = selectedName();

    m_list->clear();

    for (const auto &object : m_system.objects())
    {
        QListWidgetItem *item = new QListWidgetItem(
            object->name() + " — " + object->typeName()
        );

        item->setData(Qt::UserRole, object->name());

        m_list->addItem(item);
    }

    // Восстанавливаем выделение
    for (int i = 0; i < m_list->count(); ++i)
    {
        if (m_list->item(i)->data(Qt::UserRole).toString() == current)
        {
            QSignalBlocker blocker(m_list);
            m_list->setCurrentRow(i);
            break;
        }
    }

    m_scene->update();
}

// Отображение информации об объекте
void MainWindow::showInfo(const QString &name)
{
    auto object = m_system.find(name);
    if (!object)
    {
        m_info->clear();
        return;
    }

    m_scene->setSelected(name);
    m_info->setText( object->name() + "\n" + object->description());
}

// Получение названия выбранного объекта
QString MainWindow::selectedName() const
{
    QListWidgetItem *item = m_list->currentItem();

    if (item)
        return item->data(Qt::UserRole).toString();

    return QString();
}

// Обновление анимации
void MainWindow::onTick()
{
    if (!m_running)
        return;

    m_scene->advance(0.03);
    m_scene->update();
}

// Запуск и остановка движения
void MainWindow::onToggle()
{
    m_running = !m_running;

    if (m_running)
        m_startButton->setText("Стоп");
    else
        m_startButton->setText("Старт");
}

// Добавление новой планеты
void MainWindow::onAddPlanet()
{
    QDialog dialog(this);
    dialog.setWindowTitle("Добавление планеты");
    QLineEdit *nameEdit = new QLineEdit();

    QDoubleSpinBox *radiusSpin = new QDoubleSpinBox();
    radiusSpin->setRange(100, 200000);
    radiusSpin->setValue(6371);
    QDoubleSpinBox *massSpin = new QDoubleSpinBox();
    massSpin->setRange(0.01, 1000);
    massSpin->setValue(1);

    QDoubleSpinBox *orbitSpin = new QDoubleSpinBox();
    orbitSpin->setRange(0.1, 50);
    orbitSpin->setValue(1.5);
    QFormLayout *form = new QFormLayout();
    form->addRow("Название:", nameEdit);
    form->addRow("Радиус (км):", radiusSpin);
    form->addRow("Масса (в массах Земли):", massSpin);
    form->addRow("Расстояние (а.е.):", orbitSpin);

    QDialogButtonBox *buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel
    );
    connect(buttons, &QDialogButtonBox::accepted,&dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    layout->addLayout(form);
    layout->addWidget(buttons);

    if (dialog.exec() != QDialog::Accepted)
        return;
    QString name = nameEdit->text().trimmed();

    if (name.isEmpty())
    {
        QMessageBox::warning(this, "Ошибка", "Введите название планеты.");
        return;
    }

    if (m_system.find(name))
    {
        QMessageBox::warning(this, "Ошибка", "Такое название уже существует.");
        return;
    }

    auto planet = std::make_shared<Planet>(
        name,
        radiusSpin->value(),
        massSpin->value() * 5.97e24,
        orbitSpin->value(),
        QRandomGenerator::global()->bounded(360),
        20,
        QColor(90, 160, 230),
        0
    );
    m_system.add(planet);
    refreshList();
    onObjectClicked(name);
}

// Удаление выбранной планеты
void MainWindow::onRemove()
{
    QString name = selectedName();
    auto object = m_system.find(name);
    if (!object)
        return;
    if (object->typeName() == "Звезда")
    {
        QMessageBox::warning(this, "Удаление", "Солнце удалить нельзя.");
        return;}
    
    m_system.remove(name);
    m_info->clear();
    m_scene->setSelected("");
    refreshList();
}
// Выбор объекта на сцене
void MainWindow::onObjectClicked(const QString &name)
{
    showInfo(name);
    for (int i = 0; i < m_list->count(); ++i)
    {
        if (m_list->item(i)->data(Qt::UserRole).toString() == name)
        {
            QSignalBlocker blocker(m_list);
            m_list->setCurrentRow(i);
            break;
        }
    }
}
