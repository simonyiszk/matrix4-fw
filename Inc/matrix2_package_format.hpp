/*
 * matrix2_package_format.hpp
 *
 *  Created on: Aug 12, 2018
 *      Author: kisada
 */

/*
 const auto& received_frame = net::matrix2_package::castFromByteArray(buff);

	if(!received_frame.isValid())
		return;

	if(!received_frame.isMyZone(szint, szoba))
		return;

	const auto& szoba_adatok = received_frame.getRoom(szint, szoba);

	windows::right_window.pixels[0].set(
			szoba_adatok.right.top.left_red << 5,
			szoba_adatok.right.top.left_green << 5,
			szoba_adatok.right.top.left_blue  << 5
			);

	windows::right_window.pixels[1].set(
			szoba_adatok.right.top.right_red  << 5,
			szoba_adatok.right.top.right_green  << 5,
			szoba_adatok.right.top.right_blue  << 5
			);

	windows::right_window.pixels[2].set(
			szoba_adatok.right.bottom.left_red  << 5,
			szoba_adatok.right.bottom.left_green  << 5,
			szoba_adatok.right.bottom.left_blue  << 5
			);

	windows::right_window.pixels[3].set(
			szoba_adatok.right.bottom.right_red  << 5,
			szoba_adatok.right.bottom.right_green << 5,
			szoba_adatok.right.bottom.right_blue << 5
			);
	//----------------------------------

	windows::left_window.pixels[0].set(
			szoba_adatok.left.top.left_red << 5,
			szoba_adatok.left.top.left_green << 5,
			szoba_adatok.left.top.left_blue << 5
			);

	windows::left_window.pixels[1].set(
			szoba_adatok.left.top.right_red << 5,
			szoba_adatok.left.top.right_green << 5,
			szoba_adatok.left.top.right_blue << 5
			);

	windows::left_window.pixels[2].set(
			szoba_adatok.left.bottom.left_red << 5,
			szoba_adatok.left.bottom.left_green << 5,
			szoba_adatok.left.bottom.left_blue << 5
			);

	windows::left_window.pixels[3].set(
			szoba_adatok.left.bottom.right_red << 5,
			szoba_adatok.left.bottom.right_green << 5,
			szoba_adatok.left.bottom.right_blue << 5
			);
 */
#pragma once

#include<stdint.h>

namespace net{

	#warning "Az atollic StruStudio9 beepitett forditoja rossz bytesorrendet hasznal. a gcc-t rossz kapcsolokkal forditottak, nem lehet atrini... :/"

	//C++11 §9.2 clause 15
	struct [[gnu::packed]] matrix2_package{
	private:
		uint8_t validity_byte; //must be 0x01

		uint8_t section; //Tells which forth of the Schonherz is described in the package

		struct [[gnu::packed]]{
			struct [[gnu::packed]]{
				struct [[gnu::packed]]{
					unsigned left_red    :4;
					unsigned left_green  :4;
					unsigned left_blue   :4;

					unsigned right_red   :4;
					unsigned right_green :4;
					unsigned right_blue  :4;
				} top;
				struct [[gnu::packed]]{
					unsigned right_red   :4;
					unsigned right_green :4;
					unsigned right_blue  :4;

					unsigned left_red    :4;
					unsigned left_green  :4;
					unsigned left_blue   :4;
				} bottom;
			} right, left;
		} rooms[26];

		matrix2_package()                        = delete;
		~matrix2_package()                       = delete;
		matrix2_package(const matrix2_package&)  = delete;

	public:
		inline static const size_t roomNumber2roomIndex(const uint8_t floor, const uint8_t roomnumber){
			return ((18-floor)*8 + (roomnumber-5))%26;
		}

		inline static const matrix2_package& castFromByteArray(const uint8_t* input){
			return *reinterpret_cast<const matrix2_package*>(input);
		}

		inline bool isValid() const {
			return validity_byte == 0x01;
		}

		inline const auto& getRoom(const size_t index) const {
			return rooms[index];
		}

		inline const auto& getRoom(const uint8_t floor, const uint8_t roomnumber) const {
			return rooms[roomNumber2roomIndex(floor, roomnumber)];
		}

		inline const bool isMyZone(const uint8_t floor, const uint8_t roomnumber) const {
			return ( ( (18-floor)*16 + (roomnumber-5)*2  )/52  ) == ( section -1 ) ;
		}
	};
};
