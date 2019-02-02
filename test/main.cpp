#include "../test/math/EccentricAnomalySolverTest.hpp"
#include "../test/math/Vector3Test.hpp"
#include "../test/physics/OrbitTest.hpp"

int main(int argc, char** argv)
{
	int status       = 0;
	auto ASSERT_TEST = [&status, argc, argv](QObject* obj) {
		status |= QTest::qExec(obj, argc, argv);
		delete obj;
	};

	ASSERT_TEST(new OrbitTest());
	ASSERT_TEST(new Vector3Test());
	ASSERT_TEST(new EccentricAnomalySolverTest());

	return status;
}
