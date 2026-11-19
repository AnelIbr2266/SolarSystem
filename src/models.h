#ifndef MODELS_H
#define MODELS_H

#include <QString>
#include <QColor>
#include <vector>
#include <memory>

// Абстрактный базовый класс небесного тела
class SpaceObject
{
public:
    SpaceObject(const QString &name, double radius, double mass,
                double orbit, double angle, double speed,
                const QColor &color);
    virtual ~SpaceObject() = default;

    // Получение и изменение характеристик
    QString name() const;
    void setName(const QString &name);
    double radius() const;
    void setRadius(double radius);
    double mass() const;
    void setMass(double mass);
    double orbit() const;
    void setOrbit(double orbit);
    double angle() const;
    void setAngle(double angle);

    double speed() const;
    void setSpeed(double speed);

    QColor color() const;
    void setColor(const QColor &color);

    // Виртуальные методы
    virtual QString typeName() const = 0;
    virtual QString description() const;
    // Изменение положения объекта
    void move(double seconds);

    // Перегрузка операторов сравнения
    bool operator<(const SpaceObject &other) const;
    bool operator==(const SpaceObject &other) const;

protected:
    QString m_name;
    double m_radius;
    double m_mass;
    double m_orbit;
    double m_angle;
    double m_speed;
    QColor m_color;
};

// Класс звезды
class Star : public SpaceObject
{
public:
    Star(const QString &name, double radius, double mass,
         double temperature, const QColor &color);

    QString typeName() const override;
    QString description() const override;

    double temperature() const;

private:
    double m_temperature;
};

// Класс планеты
class Planet : public SpaceObject
{
public:
    Planet(const QString &name, double radius, double mass,
           double orbit, double angle, double speed,
           const QColor &color, int moons);

    QString typeName() const override;
    QString description() const override;

    int moons() const;

private:
    int m_moons;
};

// Класс Солнечной системы
class SolarSystem
{
public:
    SolarSystem();

    void add(const std::shared_ptr<SpaceObject> &object);
    bool remove(const QString &name);

    std::shared_ptr<SpaceObject> find(const QString &name) const;

    const std::vector<std::shared_ptr<SpaceObject>> &objects() const;

    int count() const;

    static SolarSystem createDefault();

private:
    std::vector<std::shared_ptr<SpaceObject>> m_objects;
};

#endif // MODELS_H
