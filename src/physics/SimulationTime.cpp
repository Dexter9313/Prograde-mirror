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
#include "../../include/physics/SimulationTime.hpp"

SimulationTime::SimulationTime(UniversalTime startingUt)
    : lastTime(stdclock::now())
    , currentUt(std::move(startingUt))
    , timeCoeff(1.f)
    , lastFrameTime(stdclock::now())
    , currentFPS(0.f)
    , targetFPS(60.f)
    , drawFrame(false)
{
}

void SimulationTime::resetAtEpoch()
{
	lastTime   = stdclock::now();
	currentUt  = 0.0;
	currentFPS = 0.f;
}

void SimulationTime::update()
{
	stdclock::time_point now(stdclock::now());
	double simInterval
	    = std::chrono::duration_cast<std::chrono::duration<double>>(now
	                                                                - lastTime)
	          .count();
	currentUt += simInterval * timeCoeff;
	lastTime = now;

	double frameInterval
	    = std::chrono::duration_cast<std::chrono::duration<double>>(
	          now - lastFrameTime)
	          .count();
	if(targetFPS == 0 || frameInterval >= 1.0 / targetFPS)
	{
		currentFPS = 1.f / frameInterval;
		drawFrame  = true;
	}
}

bool SimulationTime::drawableFrame()
{
	if(drawFrame)
	{
		drawFrame     = false;
		lastFrameTime = stdclock::now();
		return true;
	}
	return false;
}
