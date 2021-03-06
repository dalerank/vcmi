
/*
 * CMapFormatTest.cpp, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */
#include "StdInc.h"

#include <boost/test/unit_test.hpp>

#include "../lib/filesystem/CMemoryBuffer.h"

#include "../lib/mapping/CMap.h"
#include "../lib/rmg/CMapGenOptions.h"
#include "../lib/rmg/CMapGenerator.h"
#include "../lib/mapping/MapFormatJson.h"

#include "../lib/VCMIDirs.h"

#include "MapComparer.h"


static const int TEST_RANDOM_SEED = 1337;

static std::unique_ptr<CMap> initialMap;

class CMapTestFixture
{
public:
	CMapTestFixture()
	{
		CMapGenOptions opt;

		opt.setHeight(CMapHeader::MAP_SIZE_MIDDLE);
		opt.setWidth(CMapHeader::MAP_SIZE_MIDDLE);
		opt.setHasTwoLevels(true);
		opt.setPlayerCount(4);

		opt.setPlayerTypeForStandardPlayer(PlayerColor(0), EPlayerType::HUMAN);
		opt.setPlayerTypeForStandardPlayer(PlayerColor(1), EPlayerType::AI);
		opt.setPlayerTypeForStandardPlayer(PlayerColor(2), EPlayerType::AI);
		opt.setPlayerTypeForStandardPlayer(PlayerColor(3), EPlayerType::AI);

		CMapGenerator gen;

		initialMap = gen.generate(&opt, TEST_RANDOM_SEED);
		initialMap->name = "Test";
	};
	~CMapTestFixture()
	{
		initialMap.reset();
	};
};

BOOST_GLOBAL_FIXTURE(CMapTestFixture);

BOOST_AUTO_TEST_CASE(CMapFormatVCMI_Simple)
{
	logGlobal->info("CMapFormatVCMI_Simple start");
	BOOST_TEST_CHECKPOINT("CMapFormatVCMI_Simple start");
	CMemoryBuffer serializeBuffer;
	{
		CMapSaverJson saver(&serializeBuffer);
		saver.saveMap(initialMap);
	}
	BOOST_TEST_CHECKPOINT("CMapFormatVCMI_Simple serialized");
	#if 1
	{
		auto path = VCMIDirs::get().userDataPath()/"test.vmap";
		boost::filesystem::remove(path);
		boost::filesystem::ofstream tmp(path, boost::filesystem::ofstream::binary);

		tmp.write((const char *)serializeBuffer.getBuffer().data(),serializeBuffer.getSize());
		tmp.flush();
		tmp.close();

		logGlobal->info("Test map has been saved to:");
		logGlobal->info(path.string());
	}
	BOOST_TEST_CHECKPOINT("CMapFormatVCMI_Simple saved");

	#endif // 1

	serializeBuffer.seek(0);
	{
		CMapLoaderJson loader(&serializeBuffer);
		std::unique_ptr<CMap> serialized = loader.loadMap();

		MapComparer c;
		c(serialized, initialMap);
	}

	logGlobal->info("CMapFormatVCMI_Simple finish");
}
