#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPointF>
#include <vector>
#include <memory>
#include "models.h"

class QLabel;
class QListWidget;
class QPushButton;
class QTimer;

// Виджет для отображения Солнечной системы
class SceneWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SceneWidget(SolarSystem *system, QWidget *parent = nullptr);

    void advance(double seconds);
    void setSelected(const QString &name);

signals:
    void objectClicked(const QString &name);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    struct Body
    {
        std::shared_ptr<SpaceObject> object;
        QPointF position;
        double radius;
    };

    std::vector<Body> positions() const;

    SolarSystem *m_system;
    QString m_selected;
};

// Главное окно приложения
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void onTick();
    void onToggle();
    void onAddPlanet();
    void onRemove();
    void onObjectClicked(const QString &name);

private:
    void buildInterface();
    void refreshList();
    void showInfo(const QString &name);
    QString selectedName() const;

    SolarSystem m_system;

    SceneWidget *m_scene = nullptr;
    QListWidget *m_list = nullptr;
    QLabel *m_info = nullptr;
    QPushButton *m_startButton = nullptr;
    QTimer *m_timer = nullptr;

    bool m_running = true;
};

#endif // MAINWINDOW_H
