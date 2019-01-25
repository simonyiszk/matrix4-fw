/*
 * firm_update.hpp
 *
 *  Created on: Dec 25, 2018
 *      Author: kisada
 */

#ifndef FIRM_UPDATE_HPP_
#define FIRM_UPDATE_HPP_

#include "stddef.h"
#include <stdint.h>

namespace firmware_update{
	uint64_t flash_checksum(const uint8_t* const from,  const uint8_t* const to);
}


#endif /* FIRM_UPDATE_HPP_ */
