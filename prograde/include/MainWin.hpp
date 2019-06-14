#ifndef MAINWIN_H
#define MAINWIN_H

#include <QDateTime>
#include <QMessageBox>

#include "AbstractMainWin.hpp"

#include "Camera.hpp"
#include "Text3D.hpp"
#include "graphics/renderers/OrbitalSystemRenderer.hpp"
#include "graphics/renderers/StarryBackground.hpp"
#include "physics/OrbitalSystem.hpp"
#include "physics/SimulationTime.hpp"

// TEMP
#include "physics/default-orbitalsystems/KerbalSystem.hpp"
#include "physics/default-orbitalsystems/SolarSystem.hpp"

class MainWin : public AbstractMainWin
{
	Q_OBJECT
  public:
	MainWin();
	~MainWin();

  protected:
	virtual void keyPressEvent(QKeyEvent* e) override;
	virtual void mousePressEvent(QMouseEvent* e) override;
	virtual void mouseReleaseEvent(QMouseEvent* e) override;
	virtual void mouseMoveEvent(QMouseEvent* e) override;
	virtual void wheelEvent(QWheelEvent* e) override;

	// declare drawn resources
	virtual void initScene() override;

	// update physics/controls/meshes, etc...
	// prepare for rendering
	virtual void updateScene(BasicCamera& camera) override;

	// render user scene on camera
	// (no controllers or hands)
	virtual void renderScene(BasicCamera const& camera) override;

  private:
	StarryBackground stars;

	QPoint lastCursorPos;
	bool trackballEnabled = false;

	unsigned int bodyTracked = 0;

	Text3D* debugText;
	float timeSinceTextUpdate = FLT_MAX;

	static UniversalTime loadStartUT();
	static std::string timeToStr(UniversalTime uT);

	OrbitalSystem* orbitalSystem;
	OrbitalSystemRenderer* systemRenderer;

	// SimulationTime clock = SimulationTime(0);

	// SimulationTime clock = SimulationTime(496666800);

	// SimulationTime clock = SimulationTime(610595100); // MOON + MARS
	// ALIGNMENT

	// SimulationTime clock = SimulationTime(1748424); // "PERFECT" FULL MOON

	// SimulationTime clock = SimulationTime(675343080); // LUNAR ECLIPSE

	// SimulationTime clock = SimulationTime(615401700); // SOLAR ECLIPSE IN
	// SOUTH AMERICA

	// SimulationTime clock = SimulationTime(6852600); // March equinox

	SimulationTime clock = SimulationTime(loadStartUT());

	// TEMP
	const int textWidth  = 225;
	const int textHeight = 105;
};

#endif // MAINWIN_H
