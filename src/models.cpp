#include "models.h"

#include <algorithm>
#include <cmath>

// -------------------- SpaceObject --------------------

SpaceObject::SpaceObject(const QString &name, double radius, double mass,
                         double orbit, double angle, double speed,
                         const QColor &color)
    : m_name(name),
      m_radius(radius),
      m_mass(mass),
      m_orbit(orbit),
      m_angle(angle),
      m_speed(speed),
      m_color(color)
{
}

QString SpaceObject::name() const
{
    return m_name;
}

void SpaceObject::setName(const QString &name)
{
    if (!name.trimmed().isEmpty())
        m_name = name;
}

double SpaceObject::radius() const
{
    return m_radius;
}

void SpaceObject::setRadius(double radius)
{
    if (radius > 0)
        m_radius = radius;
}

double SpaceObject::mass() const
{
    return m_mass;
}

void SpaceObject::setMass(double mass)
{
    if (mass > 0)
        m_mass = mass;
}

double SpaceObject::orbit() const
{
    return m_orbit;
}

void SpaceObject::setOrbit(double orbit)
{
    if (orbit >= 0)
        m_orbit = orbit;
}

double SpaceObject::angle() const
{
    return m_angle;
}

void SpaceObject::setAngle(double angle)
{
    m_angle = std::fmod(angle, 360.0);

    if (m_angle < 0)
        m_angle += 360.0;
}

double SpaceObject::speed() const
{
    return m_speed;
}

void SpaceObject::setSpeed(double speed)
{
    m_speed = speed;
}

QColor SpaceObject::color() const
{
    return m_color;
}

void SpaceObject::setColor(const QColor &color)
{
    if (color.isValid())
        m_color = color;
}

QString SpaceObject::description() const
{
    return QString("Тип: %1\n"
                   "Радиус: %2 км\n"
                   "Масса: %3 кг\n"
                   "Расстояние от Солнца: %4 а.е.")
        .arg(typeName())
        .arg(m_radius)
        .arg(m_mass, 0, 'e', 3)
        .arg(m_orbit);
}

void SpaceObject::move(double seconds)
{
    setAngle(m_angle + m_speed * seconds);
}

bool SpaceObject::operator<(const SpaceObject &other) const
{
    return m_mass < other.m_mass;
}

bool SpaceObject::operator==(const SpaceObject &other) const
{
    return m_name == other.m_name;
}

// -------------------- Star --------------------

Star::Star(const QString &name, double radius, double mass,double temperature, const QColor &color)
    : SpaceObject(name, radius, mass, 0, 0, 0, color),m_temperature(temperature){}

QString Star::typeName() const
{
    return "Звезда";
}

QString Star::description() const
{
    return SpaceObject::description()+ QString("\nТемпература: %1 К").arg(m_temperature);
}

double Star::temperature() const
{
    return m_temperature;
}

// -------------------- Planet --------------------

Planet::Planet(const QString &name, double radius, double mass,
               double orbit, double angle, double speed,
               const QColor &color, int moons)
    : SpaceObject(name, radius, mass, orbit, angle, speed, color),
      m_moons(moons)
{
}

QString Planet::typeName() const
{
    return "Планета";
}

QString Planet::description() const
{
    return SpaceObject::description()
           + QString("\nКоличество спутников: %1").arg(m_moons);
}

int Planet::moons() const
{
    return m_moons;
}

// -------------------- SolarSystem --------------------

SolarSystem::SolarSystem(){}

void SolarSystem::add(const std::shared_ptr<SpaceObject> &object)
{
    if (object)
        m_objects.push_back(object);
}

bool SolarSystem::remove(const QString &name)
{
    for (auto it = m_objects.begin(); it != m_objects.end(); ++it)
    {
        if ((*it)->name() == name)
        {
            m_objects.erase(it);
            return true;
        }
    }

    return false;
}

std::shared_ptr<SpaceObject> SolarSystem::find(const QString &name) const
{
    for (const auto &object : m_objects)
    {
        if (object->name() == name)
            return object;
    }

    return nullptr;
}

const std::vector<std::shared_ptr<SpaceObject>> &
SolarSystem::objects() const
{
    return m_objects;
}

int SolarSystem::count() const
{
    return static_cast<int>(m_objects.size());
}

// Создание начального набора небесных тел
SolarSystem SolarSystem::createDefault()
{
    SolarSystem system;

    system.add(std::make_shared<Star>(
        "Солнце", 696340, 1.989e30, 5778,
        QColor(255, 200, 60)));

    system.add(std::make_shared<Planet>(
        "Меркурий", 2439, 3.30e23, 0.39, 0, 50,
        QColor(170, 170, 170), 0));

    system.add(std::make_shared<Planet>(
        "Венера", 6051, 4.87e24, 0.72, 45, 38,
        QColor(230, 200, 120), 0));

    system.add(std::make_shared<Planet>(
        "Земля", 6371, 5.97e24, 1.00, 120, 30,
        QColor(70, 130, 220), 1));

    system.add(std::make_shared<Planet>(
        "Марс", 3389, 6.42e23, 1.52, 200, 24,
        QColor(200, 90, 60), 2));

    system.add(std::make_shared<Planet>(
        "Юпитер", 69911, 1.90e27, 5.20, 280, 15,
        QColor(210, 170, 130), 95));

    system.add(std::make_shared<Planet>(
        "Сатурн", 58232, 5.68e26, 9.58, 330, 11,
        QColor(230, 210, 150), 146));

    system.add(std::make_shared<Planet>(
        "Уран", 25362, 8.68e25, 19.20, 20, 8,
        QColor(150, 220, 230), 27));

    system.add(std::make_shared<Planet>(
        "Нептун", 24622, 1.02e26, 30.07, 80, 6,
        QColor(70, 100, 220), 14));

    return system;
}
