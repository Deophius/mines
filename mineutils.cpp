#include "mineutils.h"

namespace Holy {
	void GameData::recount() noexcept {
		for (int ix = 1; ix <= col; ix++) {
			for (int iy = 1; iy <= row; iy++) {
				// only number blocks have second data
				auto& iblock = blocks[ix][iy];
				if (iblock.status != Block::number)
					continue;
				iblock.second_init = true;
				iblock.elabel = iblock.label;
				iblock.vacant_nei = 0;
				// FIXME: Not sure whether this will work
				Point{ ix, iy }.for_each_nei8([&, this](Point np) {
					auto& nblock = blocks[ix][iy];
					if (nblock.status == Block::mine)
						iblock.elabel--;
					if (nblock.status == Block::unknown)
						iblock.vacant_nei++;
				});
			}
		}
	}

	void GameData::mark_semiknown(Point p) {
		if (!p.valid())
			throw std::runtime_error("p is not valid!");
		if ((*this)[p].status != Block::unknown)
			throw std::runtime_error("p does not refer to an unprobed block!");
		// Mark point p
		(*this)[p].status = Block::semiknown;
		// mark neighbors, to keep invariant, only take action if second_init is true
		// if second_init is false, this will be taken care of in recount()
		p.for_each_nei8([this](Point np) {
			if ((*this)[np].second_init)
				(*this)[np].vacant_nei--;
		});
	}

	void GameData::mark_mine(Point p) {
		if (!p.valid())
			throw std::runtime_error("p is not valid!");
		if ((*this)[p].status != Block::unknown)
			throw std::runtime_error("p does not refer to an unprobed block!");
		// Mark point p
		(*this)[p].status = Block::mine;
		p.for_each_nei8([this](Point np) {
			if ((*this)[np].second_init) {
				(*this)[np].vacant_nei--;
				(*this)[np].elabel--;
			}
		});
	}
}