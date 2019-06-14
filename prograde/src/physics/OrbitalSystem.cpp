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

OrbitalSystem::OrbitalSystem(QJsonObject const& json)
{
	if(json["stars"] == QJsonValue::Undefined
	   || json["stars"].toArray().size() > 1)
	{
		// NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
		qWarning() << "Only single-star systems are supported right now.";
		exit(EXIT_FAILURE);
	}

	QJsonObject jsonStar(json["stars"].toArray()[0].toObject());

	star            = new Star(jsonStar);
	declinationTilt = json["declinationTilt"].toDouble();

	QJsonArray planets(jsonStar["planets"].toArray());

	current  = 0;
	progress = new QProgressDialog(QObject::tr("Loading System..."), QString(),
	                               0, planets.size());
	for(auto val : planets)
	{
		createChild(val.toObject());

		++current;
		progress->setValue(current);
		QCoreApplication::processEvents();
	}
	delete progress;
}

OrbitalSystem::OrbitalSystem(std::string const& starName,
                             Star::Parameters const& starParams,
                             double declinationTilt)
    : star(new Star(starName, starParams))
    , declinationTilt(declinationTilt)
{
}

void OrbitalSystem::createChild(QJsonObject const& json)
{
	auto body = new CelestialBody(json, star->getParameters().mass);
	bodies[body->getName()] = body;
	for(auto child : body->getAllDescendants())
	{
		bodies[child->getName()] = child;
	}
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
		bodies[name] = new CelestialBody(star->getParameters().mass,
		                                 orbitalParameters, physicalParameters);
	}
}

void OrbitalSystem::createChild(
    std::string const& name,
    CelestialBody::Parameters const& physicalParameters,
    std::string const& parent)
{
	if(!parent.empty())
	{
		bodies[name] = bodies[parent]->createChild(name, physicalParameters);
	}
	else
	{
		bodies[name] = new CelestialBody(star->getParameters().mass, name,
		                                 physicalParameters);
	}
}

CelestialBody* OrbitalSystem::operator[](std::string const& name)
{
	return bodies[name];
}

CelestialBody const* OrbitalSystem::operator[](std::string const& name) const
{
	return bodies.at(name);
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

QJsonObject OrbitalSystem::getJSONRepresentation() const
{
	QJsonObject result;
	result["declinationTilt"] = declinationTilt;
	QJsonArray stars;
	QJsonObject starJSON(star->getJSONRepresentation());

	QJsonArray planets;
	for(auto body : getParentCelestialBodiesPointers())
	{
		QJsonObject bodyObj = body->getJSONRepresentation();
		planets.push_back(bodyObj);
	}
	starJSON["planets"] = planets;

	stars.push_back(starJSON);
	result["stars"] = stars;

	return result;
}

OrbitalSystem::~OrbitalSystem()
{
	for(CelestialBody* parentBody : getParentCelestialBodiesPointers())
	{
		delete parentBody;
	}
}
