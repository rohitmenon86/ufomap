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

// UFO
#include <ufo/geometry/helper.h>
#include <ufo/geometry/intersects.h>

namespace ufo::geometry
{
//
// AABB
//

bool intersects(AABB const& aabb_1, AABB const& aabb_2)
{
	Point min_1 = aabb_1.getMin();
	Point max_1 = aabb_1.getMax();
	Point min_2 = aabb_2.getMin();
	Point max_2 = aabb_2.getMax();
	return min_1.x() <= max_2.x() && min_1.y() <= max_2.y() && min_1.z() <= max_2.z() &&
	       min_2.x() <= max_1.x() && min_2.y() <= max_1.y() && min_2.z() <= max_1.z();
}

// bool intersects(AABB const& aabb, AAEBB const& aaebb)
// {
// 	Point min_1 = aabb.getMin();
// 	Point max_1 = aabb.getMax();
// 	Point min_2 = aaebb.getMin();
// 	Point max_2 = aaebb.getMax();
// 	return min_1.x() <= max_2.x() && min_1.y() <= max_2.y() && min_1.z() <= max_2.z() &&
// 	       min_2.x() <= max_1.x() && min_2.y() <= max_1.y() && min_2.z() <= max_1.z();
// }

bool intersects(AABB const& aabb, Frustum const& frustum)
{
	// FIXME:
	for (int i = 0; i < 6; ++i) {
		float side = classify(aabb, frustum.planes[i]);
		if (side < 0) {
			return false;
		}
	}
	return true;
}

bool intersects(AABB const& aabb, LineSegment const& line_segment)
{
	Ray ray;
	ray.origin = line_segment.start;
	ray.direction = (line_segment.end - line_segment.start);
	float length = ray.direction.norm();
	ray.direction /= length;
	return intersectsLine(aabb, ray, 0.0, length);
}

bool intersects(AABB const& aabb, OBB const& obb)
{
	// ufo::geometry::OBB obb_2(aabb.center, aabb.half_size, ufo::math::Point(0, 0,
	// 0)); return intersects(obb, obb_2);

	std::array<float, 9> obb_rot_matrix;
	obb.rotation.toRotMatrix(obb_rot_matrix);

	Point test[15] = {Point(1, 0, 0),  // AABB axis 1
	                  Point(0, 1, 0),  // AABB axis 2
	                  Point(0, 0, 1),  // AABB axis 3
	                  Point(obb_rot_matrix[0], obb_rot_matrix[1], obb_rot_matrix[2]),
	                  Point(obb_rot_matrix[3], obb_rot_matrix[4], obb_rot_matrix[5]),
	                  Point(obb_rot_matrix[6], obb_rot_matrix[7], obb_rot_matrix[8])};

	for (int i = 0; i < 3; ++i) {  // Fill out rest of axis
		test[6 + i * 3 + 0] = Point::cross(test[i], test[3]);
		test[6 + i * 3 + 1] = Point::cross(test[i], test[4]);
		test[6 + i * 3 + 2] = Point::cross(test[i], test[5]);
	}

	for (int i = 0; i < 15; ++i) {
		if (!overlapOnAxis(aabb, obb, test[i])) {
			return false;  // Seperating axis found
		}
	}

	return true;  // Seperating axis not found
}

bool intersects(AABB const& aabb, Plane const& plane)
{
	float p_len = aabb.half_size.x() * std::abs(plane.normal.x()) +
	              aabb.half_size.y() * std::abs(plane.normal.y()) +
	              aabb.half_size.z() * std::abs(plane.normal.z());
	float distance = Point::dot(plane.normal, aabb.center) - plane.distance;
	return std::abs(distance) <= p_len;
}

bool intersects(AABB const& aabb, Point const& point)
{
	Point min = aabb.getMin();
	Point max = aabb.getMax();
	if (point.x() < min.x() || point.y() < min.y() || point.z() < min.z() ||
	    point.x() > max.x() || point.y() > max.y() || point.z() > max.z()) {
		return false;
	}
	return true;
}

bool intersects(AABB const& aabb, Ray const& ray)
{
	// TODO: infinity or max?
	return intersectsLine(aabb, ray, 0.0, std::numeric_limits<float>::infinity());
}

bool intersects(AABB const& aabb, Sphere const& sphere)
{
	Point closest_point = closestPoint(aabb, sphere.center);
	float distance_squared = (sphere.center - closest_point).squaredNorm();
	float radius_squared = sphere.radius * sphere.radius;
	return distance_squared < radius_squared;
}

//
// AABB
//

// bool intersects(AAEBB const& aaebb, AABB const& aabb) { return intersects(aabb, aaebb);
// }

// bool intersects(AAEBB const& aaebb_1, AAEBB const& aaebb_2)
// {
// 	Point min_1 = aaebb_1.getMin();
// 	Point max_1 = aaebb_1.getMax();
// 	Point min_2 = aaebb_2.getMin();
// 	Point max_2 = aaebb_2.getMax();
// 	return min_1.x() <= max_2.x() && min_1.y() <= max_2.y() && min_1.z() <= max_2.z() &&
// 	       min_2.x() <= max_1.x() && min_2.y() <= max_1.y() && min_2.z() <= max_1.z();
// }

// bool intersects(AAEBB const& aaebb, Frustum const& frustum)
// {
// 	// FIXME:
// 	for (int i = 0; i < 6; ++i) {
// 		float side = classify(aaebb, frustum.planes[i]);
// 		if (side < 0) {
// 			return false;
// 		}
// 	}
// 	return true;
// }

// bool intersects(AAEBB const& aaebb, LineSegment const& line_segment)
// {
// 	Ray ray;
// 	ray.origin = line_segment.start;
// 	ray.direction = (line_segment.end - line_segment.start);
// 	float length = ray.direction.norm();
// 	ray.direction /= length;
// 	return intersectsLine(aaebb, ray, 0.0, length);
// }

// bool intersects(AAEBB const& aaebb, OBB const& obb)
// {
// 	// ufo::geometry::OBB obb_2(aabb.center, aabb.half_size, ufo::math::Point(0, 0,
// 	// 0)); return intersects(obb, obb_2);

// 	std::vector<float> obb_rot_matrix;
// 	obb.rotation.toRotMatrix(obb_rot_matrix);

// 	Point test[15] = {Point(1, 0, 0),  // AABB axis 1
// 	                  Point(0, 1, 0),  // AABB axis 2
// 	                  Point(0, 0, 1),  // AABB axis 3
// 	                  Point(obb_rot_matrix[0], obb_rot_matrix[1], obb_rot_matrix[2]),
// 	                  Point(obb_rot_matrix[3], obb_rot_matrix[4], obb_rot_matrix[5]),
// 	                  Point(obb_rot_matrix[6], obb_rot_matrix[7], obb_rot_matrix[8])};

// 	for (int i = 0; i < 3; ++i) {  // Fill out rest of axis
// 		test[6 + i * 3 + 0] = Point::cross(test[i], test[3]);
// 		test[6 + i * 3 + 1] = Point::cross(test[i], test[4]);
// 		test[6 + i * 3 + 2] = Point::cross(test[i], test[5]);
// 	}

// 	for (int i = 0; i < 15; ++i) {
// 		if (!overlapOnAxis(aaebb, obb, test[i])) {
// 			return false;  // Seperating axis found
// 		}
// 	}

// 	return true;  // Seperating axis not found
// }

// bool intersects(AAEBB const& aaebb, Plane const& plane)
// {
// 	float p_len = aaebb.half_size * std::abs(plane.normal.x()) +
// 	              aaebb.half_size * std::abs(plane.normal.y()) +
// 	              aaebb.half_size * std::abs(plane.normal.z());
// 	float distance = Point::dot(plane.normal, aaebb.center) - plane.distance;
// 	return std::abs(distance) <= p_len;
// }

// bool intersects(AAEBB const& aaebb, Point const& point)
// {
// 	Point min = aaebb.getMin();
// 	Point max = aaebb.getMax();
// 	if (point.x() < min.x() || point.y() < min.y() || point.z() < min.z() ||
// 	    point.x() > max.x() || point.y() > max.y() || point.z() > max.z()) {
// 		return false;
// 	}
// 	return true;
// }

// bool intersects(AAEBB const& aaebb, Ray const& ray)
// {
// 	// TODO: infinity or max?
// 	return intersectsLine(aaebb, ray, 0.0, std::numeric_limits<float>::infinity());
// }

// bool intersects(AAEBB const& aaebb, Sphere const& sphere)
// {
// 	Point closest_point = closestPoint(aaebb, sphere.center);
// 	float distance_squared = (sphere.center - closest_point).squaredNorm();
// 	float radius_squared = sphere.radius * sphere.radius;
// 	return distance_squared < radius_squared;
// }

//
// Frustum
//

bool intersects(Frustum const& frustum, AABB const& aabb)
{
	return intersects(aabb, frustum);
}

bool intersects(Frustum const& frustum, AAEBB const& aaebb)
{
	return intersects(aaebb, frustum);
}

bool intersects(Frustum const& frustum_1, Frustum const& frustum_2)
{
	throw std::logic_error("Function not yet implemented.");
	// TODO: Implement
}

bool intersects(Frustum const& frustum, LineSegment const& line_segment)
{
	throw std::logic_error("Function not yet implemented.");
	// TODO: Implement
}

bool intersects(Frustum const& frustum, OBB const& obb)
{
	for (int i = 0; i < 6; ++i) {
		float side = classify(obb, frustum.planes[i]);
		if (side < 0) {
			return false;
		}
	}
	return true;
}

bool intersects(Frustum const& frustum, Plane const& plane)
{
	throw std::logic_error("Function not yet implemented.");
	// TODO: Implement
}

bool intersects(Frustum const& frustum, Point const& point)
{
	// Do similar as for sphere
	return intersects(frustum, Sphere(point, 0.0));
}

bool intersects(Frustum const& frustum, Ray const& ray)
{
	throw std::logic_error("Function not yet implemented.");
	// TODO: Implement
}

bool intersects(Frustum const& frustum, Sphere const& sphere)
{
	for (int i = 0; i < 6; ++i) {
		const Point& normal = frustum.planes[i].normal;
		const float& distance = frustum.planes[i].distance;
		const float side = Point::dot(sphere.center, normal) + distance;
		if (side < -sphere.radius) {
			return false;
		}
	}
	return true;
}

//
// Line segment
//

bool intersects(LineSegment const& line_segment, AABB const& aabb)
{
	return intersects(aabb, line_segment);
}

bool intersects(LineSegment const& line_segment, AAEBB const& aaebb)
{
	return intersects(aaebb, line_segment);
}

bool intersects(LineSegment const& line_segment, Frustum const& frustum)
{
	return intersects(frustum, line_segment);
}

bool intersects(LineSegment const& line_segment_1, LineSegment const& line_segment_2)
{
	throw std::logic_error("Function not yet implemented.");
	// TODO: Implement
}

bool intersects(LineSegment const& line_segment, OBB const& obb)
{
	Ray ray;
	ray.origin = line_segment.start;
	ray.direction = line_segment.end - line_segment.start;
	float line_length_squared = ray.direction.squaredNorm();
	if (line_length_squared < 0.0000001f) {
		return intersects(obb, line_segment.start);
	}
	ray.direction /= line_length_squared;  // Normalize

	// Begin ray casting

	Point p = obb.center - ray.origin;

	Point X(obb.rotation[0], 0, 0);
	Point Y(0, obb.rotation[1], 0);
	Point Z(0, 0, obb.rotation[2]);

	Point f(Point::dot(X, ray.direction), Point::dot(Y, ray.direction),
	        Point::dot(Z, ray.direction));

	Point e(Point::dot(X, p), Point::dot(Y, p), Point::dot(Z, p));

	float t[6] = {0, 0, 0, 0, 0, 0};
	for (int i = 0; i < 3; ++i) {
		if (0.0 == f[i])  // TODO: Should be approximate equal
		{
			if (-e[i] - obb.half_size[i] > 0 || -e[i] + obb.half_size[i] < 0) {
				return false;
			}
			f[i] = 0.00001f;  // Avoid div by 0!
		}
		t[i * 2 + 0] = (e[i] + obb.half_size[i]) / f[i];  // tmin[x, y, z]
		t[i * 2 + 1] = (e[i] - obb.half_size[i]) / f[i];  // tmax[x, y, z]
	}

	float tmin = std::max(std::max(std::min(t[0], t[1]), std::min(t[2], t[3])),
	                      std::min(t[4], t[5]));
	float tmax = std::min(std::min(std::max(t[0], t[1]), std::max(t[2], t[3])),
	                      std::max(t[4], t[5]));

	// if tmax < 0, ray is intersecting AABB
	// but entire AABB is behing it's origin
	if (tmax < 0) {
		return false;
	}

	// if tmin > tmax, ray doesn't intersect AABB
	if (tmin > tmax) {
		return false;
	}

	// If tmin is < 0, tmax is closer
	float t_result = tmin;

	if (tmin < 0.0f) {
		t_result = tmax;
	}

	// End ray casting
	return t_result >= 0 && t_result * t_result <= line_length_squared;
}

bool intersects(LineSegment const& line_segment, Plane const& plane)
{
	Point ab = line_segment.end - line_segment.start;
	float n_A = Point::dot(plane.normal, line_segment.start);
	float n_AB = Point::dot(plane.normal, ab);
	if (0.0 == n_AB)  // TODO: Almost equal?
	{
		return false;
	}
	float t = (plane.distance - n_A) / n_AB;
	return t >= 0.0 && t <= 1.0;
}

bool intersects(LineSegment const& line_segment, Point const& point)
{
	Point closest_point = closestPoint(line_segment, point);
	float distance_squared = (closest_point - point).squaredNorm();

	// TODO: Almost equal?
	return 0.0 == distance_squared;
}

bool intersects(LineSegment const& line_segment, Ray const& ray)
{
	throw std::logic_error("Function not yet implemented.");
	// TODO: Implement
}

bool intersects(LineSegment const& line_segment, Sphere const& sphere)
{
	Point closest_point = closestPoint(line_segment, sphere.center);
	float distance_squared = (sphere.center - closest_point).squaredNorm();
	return distance_squared <= (sphere.radius * sphere.radius);
}

//
// OBB
//

bool intersects(OBB const& obb, AABB const& aabb) { return intersects(aabb, obb); }

bool intersects(OBB const& obb, AAEBB const& aaebb) { return intersects(aaebb, obb); }

bool intersects(OBB const& obb, Frustum const& frustum)
{
	return intersects(frustum, obb);
}

bool intersects(OBB const& obb, LineSegment const& line_segment)
{
	return intersects(line_segment, obb);
}

bool intersects(OBB const& obb_1, OBB const& obb_2)
{
	std::array<float, 9> obb_1_rot_matrix;
	obb_1.rotation.toRotMatrix(obb_1_rot_matrix);

	std::array<float, 9> obb_2_rot_matrix;
	obb_2.rotation.toRotMatrix(obb_2_rot_matrix);

	Point test[15] = {Point(obb_1_rot_matrix[0], obb_1_rot_matrix[1], obb_1_rot_matrix[2]),
	                  Point(obb_1_rot_matrix[3], obb_1_rot_matrix[4], obb_1_rot_matrix[5]),
	                  Point(obb_1_rot_matrix[6], obb_1_rot_matrix[7], obb_1_rot_matrix[8]),
	                  Point(obb_2_rot_matrix[0], obb_2_rot_matrix[1], obb_2_rot_matrix[2]),
	                  Point(obb_2_rot_matrix[3], obb_2_rot_matrix[4], obb_2_rot_matrix[5]),
	                  Point(obb_2_rot_matrix[6], obb_2_rot_matrix[7], obb_2_rot_matrix[8])};

	for (int i = 0; i < 3; ++i) {  // Fill out rest of axis
		test[6 + i * 3 + 0] = Point::cross(test[i], test[0]);
		test[6 + i * 3 + 1] = Point::cross(test[i], test[1]);
		test[6 + i * 3 + 2] = Point::cross(test[i], test[2]);
	}

	for (int i = 0; i < 15; ++i) {
		if (!overlapOnAxis(obb_1, obb_2, test[i])) {
			return false;  // Seperating axis found
		}
	}

	return true;  // Seperating axis not found
}

bool intersects(OBB const& obb, Plane const& plane)
{
	Point rot[] = {
	    Point(obb.rotation[0], 0, 0),
	    Point(0, obb.rotation[1], 0),
	    Point(0, 0, obb.rotation[2]),
	};
	Point normal = plane.normal;

	// Project the half extents of the AABB onto the plane normal
	float p_len = obb.half_size.x() * std::fabs(Point::dot(normal, rot[0])) +
	              obb.half_size.y() * std::fabs(Point::dot(normal, rot[1])) +
	              obb.half_size.z() * std::fabs(Point::dot(normal, rot[2]));
	// Find the distance from the center of the OBB to the plane
	float distance = Point::dot(plane.normal, obb.center) - plane.distance;
	// Intersection occurs if the distance falls within the projected side
	return std::fabs(distance) <= p_len;
}

bool intersects(OBB const& obb, Point const& point)
{
	// TODO: Implement look earlier. THIS IS WRONG!
	Point dir = point - obb.center;
	std::array<float, 9> obb_rot_matrix;
	obb.rotation.toRotMatrix(obb_rot_matrix);
	for (int i = 0; i < 3; ++i) {
		Point axis(obb_rot_matrix[i * 3], obb_rot_matrix[i * 3 + 1],
		           obb_rot_matrix[i * 3 + 2]);
		float distance = Point::dot(dir, axis);
		if (distance > obb.half_size[i]) {
			return false;
		}
		if (distance < -obb.half_size[i])  // TODO: Should this be else if?
		{
			return false;
		}
	}
	return true;
}

bool intersects(OBB const& obb, Ray const& ray)
{
	Point p = obb.center - ray.origin;

	Point X(obb.rotation[0], 0, 0);
	Point Y(0, obb.rotation[1], 0);
	Point Z(0, 0, obb.rotation[2]);

	Point f(Point::dot(X, ray.direction), Point::dot(Y, ray.direction),
	        Point::dot(Z, ray.direction));

	Point e(Point::dot(X, p), Point::dot(Y, p), Point::dot(Z, p));

	float t[6] = {0, 0, 0, 0, 0, 0};
	for (int i = 0; i < 3; ++i) {
		if (0.0 == f[i])  // TODO: Should be approximate equal?
		{
			if (-e[i] - obb.half_size[i] > 0 || -e[i] + obb.half_size[i] < 0) {
				return false;
			}
			f[i] = 0.00001f;  // Avoid div by 0!
		}

		t[i * 2 + 0] = (e[i] + obb.half_size[i]) / f[i];  // tmin[x, y, z]
		t[i * 2 + 1] = (e[i] - obb.half_size[i]) / f[i];  // tmax[x, y, z]
	}

	float tmin = std::max(std::max(std::min(t[0], t[1]), std::min(t[2], t[3])),
	                      std::min(t[4], t[5]));
	float tmax = std::min(std::min(std::max(t[0], t[1]), std::max(t[2], t[3])),
	                      std::max(t[4], t[5]));

	// if tmax < 0, ray is intersecting AABB
	// but entire AABB is behing it's origin
	if (tmax < 0) {
		return false;
	}
	// if tmin > tmax, ray doesn't intersect AABB
	if (tmin > tmax) {
		return false;
	}
	return true;
}

bool intersects(OBB const& obb, Sphere const& sphere)
{
	Point closest_point = closestPoint(obb, sphere.center);
	float distance_squared = (sphere.center - closest_point).squaredNorm();
	return distance_squared < (sphere.radius * sphere.radius);
}

//
// Plane
//

bool intersects(Plane const& plane, AABB const& aabb) { return intersects(aabb, plane); }

bool intersects(Plane const& plane, AAEBB const& aaebb)
{
	return intersects(aaebb, plane);
}

bool intersects(Plane const& plane, Frustum const& frustum)
{
	return intersects(frustum, plane);
}

bool intersects(Plane const& plane, LineSegment const& line_segment)
{
	return intersects(line_segment, plane);
}

bool intersects(Plane const& plane, OBB const& obb) { return intersects(obb, plane); }

bool intersects(Plane const& plane_1, Plane const& plane_2)
{
	Point d = Point::cross(plane_1.normal, plane_2.normal);

	// TODO: Almost not equal?
	return 0.0 != Point::dot(d, d);
}

bool intersects(Plane const& plane, Point const& point)
{
	return Point::dot(point, plane.normal) - plane.distance;
}

bool intersects(Plane const& plane, Ray const& ray)
{
	float nd = Point::dot(ray.direction, plane.normal);
	float pn = Point::dot(ray.origin, plane.normal);
	if (nd >= 0.0f) {
		return false;
	}
	float t = (plane.distance - pn) / nd;
	return t >= 0.0;
}

bool intersects(Plane const& plane, Sphere const& sphere)
{
	Point closest_point = closestPoint(plane, sphere.center);
	float distance_squared = (sphere.center - closest_point).squaredNorm();
	return distance_squared < (sphere.radius * sphere.radius);
}

//
// Point
//

bool intersects(Point const& point, AABB const& aabb) { return intersects(aabb, point); }

bool intersects(Point const& point, AAEBB const& aaebb)
{
	return intersects(aaebb, point);
}

bool intersects(Point const& point, Frustum const& frustum)
{
	return intersects(frustum, point);
}

bool intersects(Point const& point, LineSegment const& line_segment)
{
	return intersects(line_segment, point);
}

bool intersects(Point const& point, OBB const& obb) { return intersects(obb, point); }

bool intersects(Point const& point, Plane const& plane)
{
	return intersects(plane, point);
}

bool intersects(Point const& point_1, Point const& point_2)
{
	// TODO: Almost equal?
	return point_1 == point_2;
}

bool intersects(Point const& point, Ray const& ray)
{
	if (ray.origin == point) {
		return true;
	}
	Point direction = point - ray.origin;
	direction.normalize();

	// TODO: Almost equal?
	return 1.0 == Point::dot(direction, ray.direction);
}

bool intersects(Point const& point, Sphere const& sphere)
{
	return (point - sphere.center).squaredNorm() < (sphere.radius * sphere.radius);
}

//
// Ray
//

bool intersects(Ray const& ray, AABB const& aabb) { return intersects(aabb, ray); }

bool intersects(Ray const& ray, AAEBB const& aaebb) { return intersects(aaebb, ray); }

bool intersects(Ray const& ray, Frustum const& frustum)
{
	return intersects(frustum, ray);
}

bool intersects(Ray const& ray, LineSegment const& line_segment)
{
	return intersects(line_segment, ray);
}

bool intersects(Ray const& ray, OBB const& obb) { return intersects(obb, ray); }

bool intersects(Ray const& ray, Plane const& plane) { return intersects(plane, ray); }

bool intersects(Ray const& ray, Point const& point) { return intersects(point, ray); }

bool intersects(Ray const& ray_1, Ray const& ray_2)
{
	throw std::logic_error("Function not yet implemented.");
	// TODO: Implement
}

bool intersects(Ray const& ray, Sphere const& sphere)
{
	Point e = sphere.center - ray.origin;
	float rSq = sphere.radius * sphere.radius;
	float eSq = e.squaredNorm();
	float a = Point::dot(e, ray.direction);
	return (rSq - (eSq - a * a)) >= 0.0;
}

//
// Sphere
//

bool intersects(Sphere const& sphere, AABB const& aabb)
{
	return intersects(aabb, sphere);
}

bool intersects(Sphere const& sphere, AAEBB const& aaebb)
{
	return intersects(aaebb, sphere);
}

bool intersects(Sphere const& sphere, Frustum const& frustum)
{
	return intersects(frustum, sphere);
}

bool intersects(Sphere const& sphere, LineSegment const& line_segment)
{
	return intersects(line_segment, sphere);
}

bool intersects(Sphere const& sphere, OBB const& obb) { return intersects(obb, sphere); }

bool intersects(Sphere const& sphere, Plane const& plane)
{
	return intersects(plane, sphere);
}

bool intersects(Sphere const& sphere, Point const& point)
{
	return intersects(point, sphere);
}

bool intersects(Sphere const& sphere, Ray const& ray) { return intersects(ray, sphere); }

bool intersects(Sphere const& sphere_1, Sphere const& sphere_2)
{
	float radius_sum = sphere_1.radius + sphere_2.radius;
	float distance_squared = (sphere_1.center - sphere_2.center).squaredNorm();
	return distance_squared < (radius_sum * radius_sum);
}

//
// Bounding volume
//

bool intersects(BoundingVolume const& bounding_volume_1,
                BoundingVolume const& bounding_volume_2)
{
	for (auto const& bv_2 : bounding_volume_2) {
		if (std::visit([&bounding_volume_1](
		                   auto&& bv) -> bool { return intersects(bounding_volume_1, bv); },
		               bv_2)) {
			return true;
		}
	}
	return false;
}

}  // namespace ufo::geometry