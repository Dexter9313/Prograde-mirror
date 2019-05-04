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

#include "../../include/physics/CelestialBody.hpp"

CelestialBody::CelestialBody(double influentBodyMass,
                             Orbit::Parameters orbitalParams,
                             Parameters physicalParams, Type type)
    : type(type)
    , parent(nullptr)
    , orbit(Orbit(Orbit::MassiveBodyMass(influentBodyMass), orbitalParams))
    , parameters(std::move(physicalParams))
{
}

CelestialBody::CelestialBody(CelestialBody const& parent,
                             Orbit::Parameters const& orbitalParams,
                             Parameters physicalParams, Type type)
    : type(type)
    , parent(&parent)
    , orbit(Orbit(Orbit::MassiveBodyMass(parent.getParameters().mass),
                  orbitalParams))
    , parameters(std::move(physicalParams))
{
}

CelestialBody const* CelestialBody::getParent() const
{
	return parent;
}

CelestialBody*
    CelestialBody::createChild(Orbit::Parameters const& orbitalParams,
                               Parameters const& physicalParams)
{
	auto newChild = new CelestialBody(*this, orbitalParams, physicalParams);
	children.push_back(newChild);
	return newChild;
}

Orbit const* CelestialBody::getOrbit() const
{
	return &orbit;
}

CelestialBody::Parameters CelestialBody::getParameters() const
{
	return parameters;
}

Vector3 CelestialBody::getAbsolutePositionAtUT(UniversalTime uT) const
{
	Vector3 result(orbit.getPositionAtUT(uT));

	if(parent != nullptr)
	{
		result += getAbsolutePositionFromRelative(
		    parent->getAttachedCoordinateSystemAtUT(uT), result);
	}

	return result;
}

CoordinateSystem
    CelestialBody::getAttachedCoordinateSystemAtUT(UniversalTime uT) const
{
	return orbit.getRelativeCoordinateSystemAtUT(uT);
}

CelestialBody::~CelestialBody()
{
	for(CelestialBody* child : children)
	{
		delete child;
	}
}
