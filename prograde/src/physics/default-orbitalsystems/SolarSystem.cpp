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
#include "../../../include/physics/default-orbitalsystems/SolarSystem.hpp"

float sunMass   = 1.9891f * 1e30;
float sunRadius = 696342.f * 1000;

SolarSystem::SolarSystem()
    : OrbitalSystem(sunMass, sunRadius)
{
	createPlanets();
	createEarthSubSystem();
	createMarsSubSystem();
	createJupiterSubSystem();
	createSaturnSubSystem();
	createUranusSubSystem();
	createNeptuneSubSystem();
	createPlutoSubSystem();
}

void SolarSystem::createPlanets()
{
	Orbit::Parameters orbitalParams;
	CelestialBody::Parameters physicalParams;

	// mercury
	orbitalParams.inclination            = constant::pi * 7.005 / 180.f;
	orbitalParams.ascendingNodeLongitude = constant::pi * 48.331 / 180.f;
	orbitalParams.periapsisArgument      = constant::pi * 29.124 / 180.f;
	orbitalParams.eccentricity           = 0.205630;
	orbitalParams.semiMajorAxis          = 0.387098 * au;
	orbitalParams.meanAnomalyAtEpoch     = constant::pi * 174.796 / 180.f;
	physicalParams.radius                = 2469.7 * km;
	physicalParams.color                 = Color(100, 100, 100);
	createChild("Mercury", orbitalParams, physicalParams);

	// venus
	orbitalParams.inclination            = constant::pi * 3.39458 / 180.f;
	orbitalParams.ascendingNodeLongitude = constant::pi * 76.680 / 180.f;
	orbitalParams.periapsisArgument      = constant::pi * 54.884 / 180.f;
	orbitalParams.eccentricity           = 0.006772;
	orbitalParams.semiMajorAxis          = 0.723332 * au;
	orbitalParams.meanAnomalyAtEpoch     = constant::pi * 50.115 / 180.f;
	physicalParams.radius                = 6051.8 * km;
	physicalParams.color                 = Color(255, 255, 204);
	createChild("Venus", orbitalParams, physicalParams);

	// earth
	orbitalParams.inclination            = constant::pi * 0.00005f / 180.f;
	orbitalParams.ascendingNodeLongitude = constant::pi * -11.26064 / 180.f;
	orbitalParams.periapsisArgument      = constant::pi * 114.20783 / 180.f;
	orbitalParams.eccentricity           = 0.0167086;
	orbitalParams.semiMajorAxis          = 1.000001018 * au;
	orbitalParams.meanAnomalyAtEpoch     = constant::pi * 358.617 / 180.f;
	physicalParams.radius                = 6371.0 * km;
	physicalParams.color                 = Color(0, 204, 255);
	physicalParams.mass                  = 5.97237 * 1e24;
	physicalParams.atmosphere            = 0.3f;
	createChild("Earth", orbitalParams, physicalParams);

	// mars
	orbitalParams.inclination            = constant::pi * 1.850 / 180.f;
	orbitalParams.ascendingNodeLongitude = constant::pi * 49.558 / 180.f;
	orbitalParams.periapsisArgument      = constant::pi * 286.502 / 180.f;
	orbitalParams.eccentricity           = 0.0934;
	orbitalParams.semiMajorAxis          = 1.523679 * au;
	orbitalParams.meanAnomalyAtEpoch     = constant::pi * 19.373 / 180.f;
	physicalParams.radius                = 3389.5 * km;
	physicalParams.color                 = Color(204, 102, 51);
	physicalParams.mass                  = 6.4171 * 1e23;
	physicalParams.atmosphere            = 0.075f;
	createChild("Mars", orbitalParams, physicalParams);

	// jupiter
	orbitalParams.inclination            = constant::pi * 1.303 / 180.f;
	orbitalParams.ascendingNodeLongitude = constant::pi * 100.464 / 180.f;
	orbitalParams.periapsisArgument      = constant::pi * 273.867 / 180.f;
	orbitalParams.eccentricity           = 0.048498;
	orbitalParams.semiMajorAxis          = 5.20260 * au;
	orbitalParams.meanAnomalyAtEpoch     = constant::pi * 20.020 / 180.f;
	physicalParams.radius                = 69911 * km;
	physicalParams.color                 = Color(255, 204, 102);
	physicalParams.atmosphere            = 0.f;
	createChild("Jupiter", orbitalParams, physicalParams);

	// saturn
	orbitalParams.inclination            = constant::pi * 2.485240 / 180.f;
	orbitalParams.ascendingNodeLongitude = constant::pi * 113.665 / 180.f;
	orbitalParams.periapsisArgument      = constant::pi * 339.392 / 180.f;
	orbitalParams.eccentricity           = 0.05555;
	orbitalParams.semiMajorAxis          = 9.554909 * au;
	orbitalParams.meanAnomalyAtEpoch     = constant::pi * 317.020 / 180.f;
	physicalParams.radius                = 58232 * km;
	physicalParams.color                 = Color(255, 204, 80);
	physicalParams.innerRing             = 73788.72 * km;
	physicalParams.outerRing             = 139595.64 * km;
	createChild("Saturn", orbitalParams, physicalParams);

	// uranus
	orbitalParams.inclination            = constant::pi * 0.773 / 180.f;
	orbitalParams.ascendingNodeLongitude = constant::pi * 74.006 / 180.f;
	orbitalParams.periapsisArgument      = constant::pi * 96.998857 / 180.f;
	orbitalParams.eccentricity           = 0.046381;
	orbitalParams.semiMajorAxis          = 19.2184 * au;
	orbitalParams.meanAnomalyAtEpoch     = constant::pi * 142.238600 / 180.f;
	physicalParams.radius                = 25362 * km;
	physicalParams.color                 = Color(180, 180, 255);
	physicalParams.innerRing             = 73788.72 * km;
	physicalParams.outerRing             = 139595.64 * km;
	createChild("Uranus", orbitalParams, physicalParams);

	// neptune
	orbitalParams.inclination            = constant::pi * 1.767975 / 180.f;
	orbitalParams.ascendingNodeLongitude = constant::pi * 131.784 / 180.f;
	orbitalParams.periapsisArgument      = constant::pi * 276.336 / 180.f;
	orbitalParams.eccentricity           = 0.009456;
	orbitalParams.semiMajorAxis          = 30.110387 * au;
	orbitalParams.meanAnomalyAtEpoch     = constant::pi * 256.228 / 180.f;
	physicalParams.radius                = 24622 * km;
	physicalParams.color                 = Color(80, 51, 255);
	physicalParams.innerRing             = 0.0;
	physicalParams.outerRing             = 0.0;
	createChild("Neptune", orbitalParams, physicalParams);

	// pluto
	orbitalParams.inclination            = constant::pi * 17.1405 / 180.f;
	orbitalParams.ascendingNodeLongitude = constant::pi * 110.299 / 180.f;
	orbitalParams.periapsisArgument      = constant::pi * 113.834 / 180.f;
	orbitalParams.eccentricity           = 0.24905;
	orbitalParams.semiMajorAxis          = 39.54 * au;
	orbitalParams.meanAnomalyAtEpoch     = constant::pi * 14.53 / 180.f;
	physicalParams.radius                = 1187 * km;
	physicalParams.color                 = Color(178, 145, 67);
	createChild("Pluto", orbitalParams, physicalParams);
}

void SolarSystem::createEarthSubSystem()
{
	Orbit::Parameters orbitalParams;
	CelestialBody::Parameters physicalParams;

	// moon
	orbitalParams.inclination            = constant::pi * 5.145 / 180.f;
	orbitalParams.ascendingNodeLongitude = constant::pi * -11.3 / 180.f;
	orbitalParams.periapsisArgument      = constant::pi * 114.2 / 180.f;
	orbitalParams.eccentricity           = 0.0549;
	orbitalParams.semiMajorAxis          = 0.00257 * au;
	orbitalParams.meanAnomalyAtEpoch     = constant::pi * 358.6 / 180.f;
	physicalParams.radius                = 1737.1 * km;
	physicalParams.color                 = Color(100, 100, 100);
	createChild("Moon", orbitalParams, physicalParams, "Earth");
}

void SolarSystem::createMarsSubSystem()
{
	Orbit::Parameters orbitalParams;
	CelestialBody::Parameters physicalParams;

	// phobos
	orbitalParams.inclination            = constant::pi * 26.04 / 180.f;
	orbitalParams.ascendingNodeLongitude = constant::pi * 0 / 180.f;
	orbitalParams.periapsisArgument      = constant::pi * 0 / 180.f;
	orbitalParams.eccentricity           = 0.0151;
	orbitalParams.semiMajorAxis          = 0.00057 * au;
	orbitalParams.meanAnomalyAtEpoch     = constant::pi * 358.6 / 180.f;
	physicalParams.radius                = 11.2667 * km;
	physicalParams.color                 = Color(100, 100, 100);
	createChild("Phobos", orbitalParams, physicalParams, "Mars");

	// deimos
	orbitalParams.inclination            = constant::pi * 27.58 / 180.f;
	orbitalParams.ascendingNodeLongitude = constant::pi * 0 / 180.f;
	orbitalParams.periapsisArgument      = constant::pi * 0 / 180.f;
	orbitalParams.eccentricity           = 0.00033;
	orbitalParams.semiMajorAxis          = 0.00207 * au;
	orbitalParams.meanAnomalyAtEpoch     = constant::pi * 358.6 / 180.f;
	physicalParams.radius                = 6.2 * km;
	physicalParams.color                 = Color(170, 150, 150);
	createChild("Deimos", orbitalParams, physicalParams, "Mars");
}

void SolarSystem::createJupiterSubSystem()
{
	/* content */
}

void SolarSystem::createSaturnSubSystem()
{
	/* content */
}

void SolarSystem::createUranusSubSystem()
{
	/* content */
}

void SolarSystem::createNeptuneSubSystem()
{
	/* content */
}

void SolarSystem::createPlutoSubSystem()
{
	/* content */
}
