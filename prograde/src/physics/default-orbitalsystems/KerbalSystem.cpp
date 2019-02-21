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
#include "../../../include/physics/default-orbitalsystems/KerbalSystem.hpp"

double kerbolMass   = 1.7565670f * 1e28;
double kerbolRadius = 261600000.f;

KerbalSystem::KerbalSystem()
    : OrbitalSystem(kerbolMass, kerbolRadius)
{
	createPlanets();
	createEveSubSystem();
	createKerbinSubSystem();
	createDunaSubSystem();
	createJoolSubSystem();
}

void KerbalSystem::createPlanets()
{
	Orbit::Parameters orbitalParams;
	CelestialBody::Parameters physicalParams;

	// moho
	orbitalParams.inclination            = constant::pi * 7.0 / 180.f;
	orbitalParams.ascendingNodeLongitude = constant::pi * 70.f / 180.f;
	orbitalParams.periapsisArgument      = constant::pi * 15.f / 180.f;
	orbitalParams.eccentricity           = 0.2;
	orbitalParams.semiMajorAxis          = 5263138304 * distanceMultiplier;
	orbitalParams.meanAnomalyAtEpoch     = 3.14;
	physicalParams.radius                = 250.0 * radiusMultiplier;
	physicalParams.color                 = Color(121, 85, 72);
	createChild("Moho", orbitalParams, physicalParams);

	// eve
	orbitalParams.inclination            = constant::pi * 2.1 / 180.f;
	orbitalParams.ascendingNodeLongitude = constant::pi * 15.f / 180.f;
	orbitalParams.periapsisArgument      = constant::pi * 0.f / 180.f;
	orbitalParams.eccentricity           = 0.01;
	orbitalParams.semiMajorAxis          = 9832684544 * distanceMultiplier;
	orbitalParams.meanAnomalyAtEpoch     = 3.14;
	physicalParams.mass                  = 1.2244127 * 1e23 * massMultiplier;
	physicalParams.radius                = 700.0 * radiusMultiplier;
	physicalParams.color                 = Color(106, 27, 154);
	createChild("Eve", orbitalParams, physicalParams);

	// kerbin
	orbitalParams.inclination            = constant::pi * 0.f / 180.f;
	orbitalParams.ascendingNodeLongitude = constant::pi * 0.f / 180.f;
	orbitalParams.periapsisArgument      = constant::pi * 0.f / 180.f;
	orbitalParams.eccentricity           = 0.f;
	orbitalParams.semiMajorAxis          = 13599840256 * distanceMultiplier;
	orbitalParams.meanAnomalyAtEpoch     = 3.14;
	physicalParams.mass                  = 5.2915793 * 1e22 * massMultiplier;
	physicalParams.radius                = 600.0 * radiusMultiplier;
	physicalParams.color                 = Color(40, 53, 147);
	createChild("Kerbin", orbitalParams, physicalParams);

	// duna
	orbitalParams.inclination            = constant::pi * 0.06f / 180.f;
	orbitalParams.ascendingNodeLongitude = constant::pi * 135.5f / 180.f;
	orbitalParams.periapsisArgument      = constant::pi * 0.f / 180.f;
	orbitalParams.eccentricity           = 0.05f;
	orbitalParams.semiMajorAxis          = 20726155264 * distanceMultiplier;
	orbitalParams.meanAnomalyAtEpoch     = 3.14;
	physicalParams.mass                  = 4.5154812 * 1e21 * massMultiplier;
	physicalParams.radius                = 320.0 * radiusMultiplier;
	physicalParams.color                 = Color(244, 67, 54);
	createChild("Duna", orbitalParams, physicalParams);

	// dres
	orbitalParams.inclination            = constant::pi * 5.f / 180.f;
	orbitalParams.ascendingNodeLongitude = constant::pi * 280.f / 180.f;
	orbitalParams.periapsisArgument      = constant::pi * 90.f / 180.f;
	orbitalParams.eccentricity           = 0.14f;
	orbitalParams.semiMajorAxis          = 40839348203 * distanceMultiplier;
	orbitalParams.meanAnomalyAtEpoch     = 314;
	physicalParams.radius                = 138.0 * radiusMultiplier;
	physicalParams.color                 = Color(158, 158, 158);
	createChild("Dres", orbitalParams, physicalParams);

	// jool
	orbitalParams.inclination            = constant::pi * 1.304f / 180.f;
	orbitalParams.ascendingNodeLongitude = constant::pi * 52.f / 180.f;
	orbitalParams.periapsisArgument      = constant::pi * 0.f / 180.f;
	orbitalParams.eccentricity           = 0.05f;
	orbitalParams.semiMajorAxis          = 68773560320 * distanceMultiplier;
	orbitalParams.meanAnomalyAtEpoch     = 0.1;
	physicalParams.mass                  = 4.2332635 * 1e24 * massMultiplier;
	physicalParams.radius                = 6000.0 * radiusMultiplier;
	physicalParams.color                 = Color(104, 159, 56);
	createChild("Jool", orbitalParams, physicalParams);

	// eeloo
	orbitalParams.inclination            = constant::pi * 6.15f / 180.f;
	orbitalParams.ascendingNodeLongitude = constant::pi * 50.f / 180.f;
	orbitalParams.periapsisArgument      = constant::pi * 260.f / 180.f;
	orbitalParams.eccentricity           = 0.26f;
	orbitalParams.semiMajorAxis          = 90118820000 * distanceMultiplier;
	orbitalParams.meanAnomalyAtEpoch     = 3.14;
	physicalParams.radius                = 210.0 * radiusMultiplier;
	physicalParams.color                 = Color(240, 240, 240);
	createChild("Eeloo", orbitalParams, physicalParams);
}

void KerbalSystem::createEveSubSystem()
{
	Orbit::Parameters orbitalParams;
	CelestialBody::Parameters physicalParams;

	// gilly
	orbitalParams.inclination            = constant::pi * 12.f / 180.f;
	orbitalParams.ascendingNodeLongitude = constant::pi * 80.f / 180.f;
	orbitalParams.periapsisArgument      = constant::pi * 10.f / 180.f;
	orbitalParams.eccentricity           = 0.55f;
	orbitalParams.semiMajorAxis          = 31500000 * distanceMultiplier;
	orbitalParams.meanAnomalyAtEpoch     = 0.9;
	physicalParams.radius                = 13.0 * radiusMultiplier;
	physicalParams.color                 = Color(188, 170, 164);
	createChild("Gilly", orbitalParams, physicalParams, "Eve");
}

void KerbalSystem::createKerbinSubSystem()
{
	Orbit::Parameters orbitalParams;
	CelestialBody::Parameters physicalParams;

	// mun
	orbitalParams.inclination            = constant::pi * 0.f / 180.f;
	orbitalParams.ascendingNodeLongitude = constant::pi * 0.f / 180.f;
	orbitalParams.periapsisArgument      = constant::pi * 0.f / 180.f;
	orbitalParams.eccentricity           = 0.f;
	orbitalParams.semiMajorAxis          = 12000000 * distanceMultiplier;
	orbitalParams.meanAnomalyAtEpoch     = 1.7;
	physicalParams.radius                = 200.0 * radiusMultiplier;
	physicalParams.color                 = Color(158, 158, 158);
	createChild("Mun", orbitalParams, physicalParams, "Kerbin");

	// minmus
	orbitalParams.inclination            = constant::pi * 6.f / 180.f;
	orbitalParams.ascendingNodeLongitude = constant::pi * 78.f / 180.f;
	orbitalParams.periapsisArgument      = constant::pi * 38.f / 180.f;
	orbitalParams.eccentricity           = 0.f;
	orbitalParams.semiMajorAxis          = 47000000 * distanceMultiplier;
	orbitalParams.meanAnomalyAtEpoch     = 0.9;
	physicalParams.radius                = 60.0 * radiusMultiplier;
	physicalParams.color                 = Color(178, 223, 219);
	createChild("Minmus", orbitalParams, physicalParams, "Kerbin");
}

void KerbalSystem::createDunaSubSystem()
{
	Orbit::Parameters orbitalParams;
	CelestialBody::Parameters physicalParams;

	// ike
	orbitalParams.inclination            = constant::pi * 0.2f / 180.f;
	orbitalParams.ascendingNodeLongitude = constant::pi * 0.f / 180.f;
	orbitalParams.periapsisArgument      = constant::pi * 0.f / 180.f;
	orbitalParams.eccentricity           = 0.03f;
	orbitalParams.semiMajorAxis          = 3200000 * distanceMultiplier;
	orbitalParams.meanAnomalyAtEpoch     = 1.7;
	physicalParams.radius                = 130.0 * radiusMultiplier;
	physicalParams.color                 = Color(158, 158, 158);
	createChild("Ike", orbitalParams, physicalParams, "Duna");
}

void KerbalSystem::createJoolSubSystem()
{
	Orbit::Parameters orbitalParams;
	CelestialBody::Parameters physicalParams;

	// laythe
	orbitalParams.inclination            = constant::pi * 0.f / 180.f;
	orbitalParams.ascendingNodeLongitude = constant::pi * 0.f / 180.f;
	orbitalParams.periapsisArgument      = constant::pi * 0.f / 180.f;
	orbitalParams.eccentricity           = 0.f;
	orbitalParams.semiMajorAxis          = 27184000 * distanceMultiplier;
	orbitalParams.meanAnomalyAtEpoch     = 3.14;
	physicalParams.radius                = 500.0 * radiusMultiplier;
	physicalParams.color                 = Color(26, 35, 126);
	createChild("Laythe", orbitalParams, physicalParams, "Jool");

	// vall
	orbitalParams.inclination            = constant::pi * 0.f / 180.f;
	orbitalParams.ascendingNodeLongitude = constant::pi * 0.f / 180.f;
	orbitalParams.periapsisArgument      = constant::pi * 0.f / 180.f;
	orbitalParams.eccentricity           = 0.f;
	orbitalParams.semiMajorAxis          = 43152000 * distanceMultiplier;
	orbitalParams.meanAnomalyAtEpoch     = 0.9;
	physicalParams.radius                = 300.0 * radiusMultiplier;
	physicalParams.color                 = Color(187, 222, 251);
	createChild("Vall", orbitalParams, physicalParams, "Jool");

	// tylo
	orbitalParams.inclination            = constant::pi * 0.025f / 180.f;
	orbitalParams.ascendingNodeLongitude = constant::pi * 0.f / 180.f;
	orbitalParams.periapsisArgument      = constant::pi * 0.f / 180.f;
	orbitalParams.eccentricity           = 0.f;
	orbitalParams.semiMajorAxis          = 68500000 * distanceMultiplier;
	orbitalParams.meanAnomalyAtEpoch     = 3.14;
	physicalParams.radius                = 600.0 * radiusMultiplier;
	physicalParams.color                 = Color(215, 204, 233);
	createChild("Tylo", orbitalParams, physicalParams, "Jool");

	// bop
	orbitalParams.inclination            = constant::pi * 15.f / 180.f;
	orbitalParams.ascendingNodeLongitude = constant::pi * 10.f / 180.f;
	orbitalParams.periapsisArgument      = constant::pi * 25.f / 180.f;
	orbitalParams.eccentricity           = 0.24f;
	orbitalParams.semiMajorAxis          = 128500000 * distanceMultiplier;
	orbitalParams.meanAnomalyAtEpoch     = 0.9;
	physicalParams.radius                = 65.0 * radiusMultiplier;
	physicalParams.color                 = Color(121, 85, 72);
	createChild("Bop", orbitalParams, physicalParams, "Jool");

	// pol
	orbitalParams.inclination            = constant::pi * 4.25f / 180.f;
	orbitalParams.ascendingNodeLongitude = constant::pi * 2.f / 180.f;
	orbitalParams.periapsisArgument      = constant::pi * 15.f / 180.f;
	orbitalParams.eccentricity           = 0.17f;
	orbitalParams.semiMajorAxis          = 179890000 * distanceMultiplier;
	orbitalParams.meanAnomalyAtEpoch     = 0.9;
	physicalParams.radius                = 44.0 * radiusMultiplier;
	physicalParams.color                 = Color(215, 204, 233);
	createChild("Pol", orbitalParams, physicalParams, "Jool");
}
