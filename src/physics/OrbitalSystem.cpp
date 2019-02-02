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

#include "../../include/physics/OrbitalSystem.hpp"

OrbitalSystem::OrbitalSystem(double centralMass, double centralRadius)
    : centralMass(centralMass)
    , centralRadius(centralRadius)
{
}

void OrbitalSystem::createChild(
    std::string const& name, Orbit::Parameters const& orbitalParameters,
    CelestialBody::Parameters const& physicalParameters,
    std::string const& parent)
{
	if(!parent.empty())
	{
		bodies[name] = bodies[parent]->createChild(orbitalParameters,
		                                           physicalParameters);
	}
	else
	{
		bodies[name] = new CelestialBody(centralMass, orbitalParameters,
		                                 physicalParameters);
	}
}

CelestialBody* OrbitalSystem::operator[](std::string const& name)
{
	return bodies[name];
}

std::vector<std::string> OrbitalSystem::getAllCelestialBodiesNames() const
{
	std::vector<std::string> result;

	for(auto pair : bodies)
	{
		result.push_back(pair.first);
	}

	return result;
}

std::vector<CelestialBody*> OrbitalSystem::getAllCelestialBodiesPointers() const
{
	std::vector<CelestialBody*> result;

	for(auto pair : bodies)
	{
		result.push_back(pair.second);
	}

	return result;
}

std::vector<std::string> OrbitalSystem::getParentCelestialBodiesNames() const
{
	std::vector<std::string> result;

	for(auto pair : bodies)
	{
		if(pair.second->getParent() == nullptr)
		{
			result.push_back(pair.first);
		}
	}

	return result;
}

std::vector<CelestialBody*>
    OrbitalSystem::getParentCelestialBodiesPointers() const
{
	std::vector<CelestialBody*> result;

	for(auto pair : bodies)
	{
		if(pair.second->getParent() == nullptr)
		{
			result.push_back(pair.second);
		}
	}

	return result;
}

OrbitalSystem::~OrbitalSystem()
{
	for(CelestialBody* parentBody : getParentCelestialBodiesPointers())
	{
		delete parentBody;
	}
}
