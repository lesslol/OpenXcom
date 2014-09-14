/*
 * Copyright 2010-2014 OpenXcom Developers.
 *
 * This file is part of OpenXcom.
 *
 * OpenXcom is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenXcom is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenXcom.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef OPENXCOM_RNG_H
#define OPENXCOM_RNG_H

#include <algorithm>
#define __STDC_LIMIT_MACROS
#include <stdint.h>

namespace OpenXcom
{

/**
 * Random Number Generator used throughout the game
 * for all your randomness needs. Uses a 64-bit xorshift
 * pseudorandom number generator.
 */
namespace RNG
{
	/// Gets the seed in use.
	uint64_t getSeed();
	/// Sets the seed in use.
	void setSeed(uint64_t n);
	/// Generates a random integer number, inclusive.
	int generate(int min, int max);
	/// Generates a random floating-point number.
	double generate(double min, double max);
	/// Get normally distributed value.
	double boxMuller(double m = 0, double s = 1);
	/// Generates a random integer number, exclusive.
	int generateEx(int max);
	/// Shuffles a list randomly.
	/**
	 * Randomly changes the orders of the elements in a list.
	 * @param list The container to randomize.
	 */
	template <typename T>
	void shuffle(T &list)
	{
		std::random_shuffle(list.begin(), list.end(), generateEx);
	}
	/// Generates a percentage chance.
	/**
	* Generates a random percent chance of an event occuring,
	* and returns the result
	* @param value Value percentage (0-100%)
	* @return True if the chance succeeded.
	*/
	template <typename T>
	inline bool percent(T value)
	{
		return (generate(0, 99) < static_cast<int>(value));
	}
	/// Generates a percentage chance. (float specialized)
	template <> inline bool percent<float>(float value)   { return (generate(0.0, 99.9) < value); }
	/// Generates a percentage chance. (double specialized)
	template <> inline bool percent<double>(double value) { return (generate(0.0, 99.9) < value); }
}

}

#endif
