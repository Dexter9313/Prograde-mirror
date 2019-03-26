#ifndef MAINWIN_H
#define MAINWIN_H

#include "AbstractMainWin.hpp"

#include "Camera.hpp"
#include "graphics/renderers/StarryBackground.hpp"
#include "graphics/renderers/OrbitalSystemRenderer.hpp"
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

	// temp
	//OrbitalSystem* orbitalSystem = new KerbalSystem;
	OrbitalSystem* orbitalSystem = new SolarSystem;
	OrbitalSystemRenderer* systemRenderer;
	//SimulationTime clock = SimulationTime(0);
	SimulationTime clock = SimulationTime(1443394800 - 946728000);
};

#endif // MAINWIN_H
