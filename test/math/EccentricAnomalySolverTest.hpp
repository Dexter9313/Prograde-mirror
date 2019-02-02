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

#ifndef ECCENTRICANOMALYSOLVERTEST_H
#define ECCENTRICANOMALYSOLVERTEST_H

#include "math/EccentricAnomalySolver.hpp"
#include "math/constants.hpp"
#include <QtTest>

class EccentricAnomalySolverTest : public QObject
{
	Q_OBJECT
  private slots:

	void EllipticZeroEccentricityEqualsSameAngle()
	{
		// mA = meanAnomaly
		double mA1(0.0), mA2(constant::pi), mA3(2 * constant::pi),
		    mA4(1.234567);

		QVERIFY(qFuzzyCompare(
		    mA1, EccentricAnomalySolver::solveForEllipticOrbit(mA1, 0.f)));
		QVERIFY(qFuzzyCompare(
		    mA2, EccentricAnomalySolver::solveForEllipticOrbit(mA2, 0.f)));
		QVERIFY(qFuzzyCompare(
		    mA3, EccentricAnomalySolver::solveForEllipticOrbit(mA3, 0.f)));
		QVERIFY(qFuzzyCompare(
		    mA4, EccentricAnomalySolver::solveForEllipticOrbit(mA4, 0.f)));
	};
};

#endif // ECCENTRICANOMALYSOLVERTEST_H
