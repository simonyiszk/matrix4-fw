/*
 * firm_update.hpp
 *
 *  Created on: Dec 25, 2018
 *      Author: kisada
 */

#ifndef FIRM_UPDATE_HPP_
#define FIRM_UPDATE_HPP_

#include "stddef.h"

namespace firmware_update{
	enum flashPart{
		first,
		second,
		whole
	};

	uint64_t flash_checksum(flashPart);

	//TODO fix address in linker script
	void copyfirmware [[noreturn]] ();

	void copyToFlash2(const uint8_t* from);
}


#endif /* FIRM_UPDATE_HPP_ */
