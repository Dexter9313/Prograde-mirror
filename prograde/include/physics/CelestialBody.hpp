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

#ifndef CELESTIALBODY_HPP
#define CELESTIALBODY_HPP

#include <cmath>
#include <vector>

#include "Color.hpp"
#include "Orbit.hpp"

class CelestialBody
{
  public:
	struct Parameters
	{
		double mass;
		double radius;
		Color color;
	};

	CelestialBody(double influentBodyMass, Orbit::Parameters orbitalParams,
	              Parameters physicalParams);
	CelestialBody(CelestialBody const& parent,
	              Orbit::Parameters const& orbitalParams,
	              Parameters physicalParams);
	CelestialBody(CelestialBody const& copiedBody) = default;
	CelestialBody const* getParent() const;
	CelestialBody* createChild(Orbit::Parameters const& orbitalParams,
	                           Parameters const& physicalParams);
	Orbit const* getOrbit() const;
	Parameters getParameters() const;
	Vector3 getAbsolutePositionAtUT(UniversalTime uT) const;
	Vector3 getAbsoluteVelocityAtUT(UniversalTime uT) const;
	CoordinateSystem getAttachedCoordinateSystemAtUT(UniversalTime uT) const;
	virtual ~CelestialBody();

  private:
	CelestialBody const* parent;
	std::vector<CelestialBody*> children;
	Orbit orbit;
	Parameters parameters;
};

#endif // CELESTIALBODY_HPP
