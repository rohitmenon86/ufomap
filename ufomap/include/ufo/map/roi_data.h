/**
 * UFOMap: An Efficient Probabilistic 3D Mapping Framework That Embraces the Unknown
 *
 * @author D. Duberg, KTH Royal Institute of Technology, Copyright (c) 2020.
 * @see https://github.com/UnknownFreeOccupied/ufomap
 * License: BSD 3
 *
 */

/*
 * BSD 3-Clause License
 *
 * Copyright (c) 2020, D. Duberg, KTH Royal Institute of Technology
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef UFO_MAP_ROI_DATA_H
#define UFO_MAP_ROI_DATA_H

// STD
#include <cstdint>

namespace ufo::map
{

/**
 * @brief Region of Interest Probability
 *
 */
struct RoiData {

	uint8_t class_id;
	uint8_t instance_id;
	float roi_value;

	RoiData() : class_id(0), instance_id(0), roi_value(0) {}

	RoiData(uint8_t _class_id, uint8_t _instance_id, float _roi_value) : class_id(_class_id), instance_id(_instance_id), roi_value(_roi_value) {}

	RoiData(RoiData const& other) : class_id(other.class_id), instance_id(other.instance_id), roi_value(other.roi_value) {}

	RoiData(float _roi_value) : class_id(0), instance_id(0), roi_value(_roi_value) {}

	RoiData(uint8_t _class_id, float _roi_value) : class_id(_class_id), instance_id(0), roi_value(_roi_value) {}

	RoiData& operator=(RoiData const& rhs)
	{
		class_id = rhs.class_id;
		instance_id = rhs.instance_id;
		roi_value = rhs.roi_value;
		return *this;
	}

	bool operator==(RoiData const& other) const
	{
		return other.class_id == class_id && other.instance_id == instance_id && other.roi_value == roi_value;
	}

	bool operator!=(RoiData const& other) const
	{
		return other.class_id != class_id || other.instance_id != instance_id || other.roi_value != roi_value;;
	}

	bool isSet() const { return true; }
};
}  // namespace ufo::map

#endif  // UFO_MAP_COLOR_H
