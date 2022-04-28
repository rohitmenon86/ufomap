/*
 * UFOMap: An Efficient Probabilistic 3D Mapping Framework That Embraces the Unknown
 *
 * @author D. Duberg, KTH Royal Institute of Technology, Copyright (c) 2020.
 * @see https://github.com/UnknownFreeOccupied/ufomap
 * License: BSD 3
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

#ifndef UFO_MAP_OCCUPANCY_NODE_H
#define UFO_MAP_OCCUPANCY_NODE_H

// UFO
#include <ufo/map/color/color_node.h>
#include <ufo/map/octree/octree_node.h>
#include <ufo/map/semantic/semantic_node.h>

// STL
#include <cstdint>
#include <iostream>
#include <type_traits>

namespace ufo::map
{
struct OccupancyIndicators {
	// Indicates whether this is a leaf node (has no children) or not. If true then the
	// children are not valid and should not be accessed
	uint8_t is_leaf : 1;
	// Indicates whether this node has to be updated (get information from children and/or
	// update indicators). Useful when propagating information up the tree
	uint8_t modified : 1;

	// Indicates whether this node or any of its children contains unknown space
	uint8_t contains_unknown : 1;
	// Indicates whether this node or any of its children contains free space
	uint8_t contains_free : 1;
	// Indicates whether this node or any of its children contains occupied space
	uint8_t contains_occupied : 1;
};

template <typename OccupancyType>
struct OccupancyNode {
	OccupancyType occupancy;

	bool operator==(OccupancyNode const& rhs) const noexcept
	{
		return rhs.occupancy == occupancy;
	}
	bool operator!=(OccupancyNode const& rhs) const noexcept
	{
		return rhs.occupancy != occupancy;
	}
};

struct OccupancyTimeNode {
	uint32_t occupancy : 8;
	uint32_t time_step : 24;

	bool operator==(OccupancyTimeNode const& rhs) const noexcept
	{
		return rhs.occupancy == occupancy && rhs.time_step == time_step;
	}
	bool operator!=(OccupancyTimeNode const& rhs) const noexcept
	{
		return rhs.occupancy != occupancy && rhs.time_step != time_step;
	}
};

template <typename OccupancyType>
struct OccupancyColorNode : OccupancyNode<OccupancyType>, ColorNode {
	bool operator==(OccupancyColorNode const& rhs) const noexcept
	{
		return OccupancyNode<OccupancyType>::operator==(rhs) && ColorNode::operator==(rhs);
	}

	bool operator!=(OccupancyColorNode const& rhs) const noexcept
	{
		return OccupancyNode<OccupancyType>::operator!=(rhs) || ColorNode::operator!=(rhs);
	}
};

template <typename OccupancyType, typename SemanticType, size_t SemanticValueWidth>
struct OccupancySemanticNode : SemanticNode<SemanticType, SemanticValueWidth>,
                               OccupancyNode<OccupancyType> {
	bool operator==(OccupancySemanticNode const& rhs) const noexcept
	{
		return OccupancyNode<OccupancyType>::operator==(rhs) &&
		       SemanticNode<SemanticType, SemanticValueWidth>::operator==(rhs);
	}

	bool operator!=(OccupancySemanticNode const& rhs) const noexcept
	{
		return OccupancyNode<OccupancyType>::operator!=(rhs) ||
		       SemanticNode<SemanticType, SemanticValueWidth>::operator!=(rhs);
	}
};

struct OccupancyTimeColorNode : OccupancyTimeNode, ColorNode {
	bool operator==(OccupancyTimeColorNode const& rhs) const noexcept
	{
		return OccupancyTimeNode::operator==(rhs) && ColorNode::operator==(rhs);
	}

	bool operator!=(OccupancyTimeColorNode const& rhs) const noexcept
	{
		return OccupancyTimeNode::operator!=(rhs) && ColorNode::operator!=(rhs);
	}
};

template <typename SemanticType, size_t SemanticValueWidth>
struct OccupancyTimeSemanticNode : SemanticNode<SemanticType, SemanticValueWidth>,
                                   OccupancyTimeNode {
	bool operator==(OccupancyTimeSemanticNode const& rhs) const noexcept
	{
		return OccupancyTimeNode::operator==(rhs) &&
		       SemanticNode<SemanticType, SemanticValueWidth>::operator==(rhs);
	}

	bool operator!=(OccupancyTimeSemanticNode const& rhs) const noexcept
	{
		return OccupancyTimeNode::operator!=(rhs) ||
		       SemanticNode<SemanticType, SemanticValueWidth>::operator!=(rhs);
	}
};

template <typename OccupancyType, typename SemanticType, size_t SemanticValueWidth>
struct OccupancyColorSemanticNode : SemanticNode<SemanticType, SemanticValueWidth>,
                                    OccupancyNode<OccupancyType>,
                                    ColorNode {
	bool operator==(OccupancyColorSemanticNode const& rhs) const noexcept
	{
		return OccupancyNode<OccupancyType>::operator==(rhs) && ColorNode::operator==(rhs) &&
		       SemanticNode<SemanticType, SemanticValueWidth>::operator==(rhs);
	}

	bool operator!=(OccupancyColorSemanticNode const& rhs) const noexcept
	{
		return OccupancyNode<OccupancyType>::operator!=(rhs) || ColorNode::operator!=(rhs) ||
		       SemanticNode<SemanticType, SemanticValueWidth>::operator!=(rhs);
	}
};

template <typename SemanticType, size_t SemanticValueWidth>
struct OccupancyTimeColorSemanticNode : SemanticNode<SemanticType, SemanticValueWidth>,
                                        OccupancyTimeNode,
                                        Color {
	bool operator==(OccupancyTimeColorSemanticNode const& rhs) const noexcept
	{
		return OccupancyTimeNode::operator==(rhs) && ColorNode::operator==(rhs) &&
		       SemanticNode<SemanticType, SemanticValueWidth>::operator==(rhs);
	}

	bool operator!=(OccupancyTimeColorSemanticNode const& rhs) const noexcept
	{
		return OccupancyTimeNode::operator!=(rhs) || ColorNode::operator!=(rhs) ||
		       SemanticNode<SemanticType, SemanticValueWidth>::operator!=(rhs);
	}
};

// TODO: Create tiny semantic node where occupancy, color and time is inside semantic
// container
}  // namespace ufo::map

#endif  // UFO_MAP_OCCUPANCY_NODE_H