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
	const uint64_t checksum_of_new_fw();
    void           refurbish /*[[noreturn]]*/ ();
}


#endif /* FIRM_UPDATE_HPP_ */
