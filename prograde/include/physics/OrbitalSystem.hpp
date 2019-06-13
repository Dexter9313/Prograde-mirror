/*
    Copyright (C) 2018 Florian Cabot <florian.cabot@hotmail.fr>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#ifndef ORBITALSYSTEM_HPP
#define ORBITALSYSTEM_HPP

#include "CelestialBody.hpp"
#include <QCoreApplication>
#include <QJsonArray>
#include <QProgressDialog>
#include <QtDebug>
#include <map>
#include <vector>

class OrbitalSystem
{
  public:
	OrbitalSystem(QJsonObject const& json);
	OrbitalSystem(double centralMass, double centralRadius,
	              double declinationTilt);
	OrbitalSystem(OrbitalSystem const&) = delete;
	OrbitalSystem& operator=(OrbitalSystem const&) = delete;
	double getCentralMass() const { return centralMass; };
	double getCentralRadius() const { return centralRadius; };
	double getDeclinationTilt() const { return declinationTilt; };
	void createChild(QJsonObject const& json);
	void createChild(std::string const& name,
	                 Orbit::Parameters const& orbitalParameters,
	                 CelestialBody::Parameters const& physicalParameters,
	                 std::string const& parent = std::string(""));
	void createChild(std::string const& name,
	                 CelestialBody::Parameters const& physicalParameters,
	                 std::string const& parent = std::string(""));
	CelestialBody* operator[](std::string const& name);
	CelestialBody const* operator[](std::string const& name) const;
	std::vector<std::string> getAllCelestialBodiesNames() const;
	std::vector<CelestialBody*> getAllCelestialBodiesPointers() const;
	std::vector<std::string> getParentCelestialBodiesNames() const;
	std::vector<CelestialBody*> getParentCelestialBodiesPointers() const;
	QJsonObject getJSONRepresentation() const;
	virtual ~OrbitalSystem();

  private:
	double centralMass;
	double centralRadius;
	double declinationTilt;

	std::map<std::string, CelestialBody*> bodies;

	// JSON loading
	unsigned int current;
	QProgressDialog* progress;
};

#endif // ORBITALSYSTEM_HPP
