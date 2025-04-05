#ifndef GEOMETRY_HPP
#define GEOMETRY_HPP

#include "raylib.h"
#include "raymath.h"

// Function to check if a point is on a line segment
bool PointOnLineSegment(Vector2 point, Vector2 start, Vector2 end, float tolerance = 0.01f) {
	Vector2 lineVec = Vector2Subtract(end, start);
	Vector2 pointVec = Vector2Subtract(point, start);
	float lineLength = Vector2Length(lineVec);
	float pointLength = Vector2Length(pointVec);

	// Check if the point is within the line segment bounds
	if (pointLength > lineLength + tolerance) {
		return false;
	}

	// Normalize the vectors
	lineVec = Vector2Normalize(lineVec);
	pointVec = Vector2Normalize(pointVec);

	// Check if the point is on the line
	float dotProduct = Vector2DotProduct(lineVec, pointVec);
	return fabs(dotProduct - 1.0f) < tolerance;
}

// Function to check if a point is on a line segment
bool PointOnLineSegments(Vector2 point, std::vector<Vector2> path, float tolerance = 0.001f) {
	for (size_t i = 0; i < path.size() - 1; ++i) {
		if (PointOnLineSegment(point, path[i], path[i + 1], tolerance)) {
			return true;
		}
	}
	return false;
}

// Function to get the cell position under the mouse position
Vector2 GetCellPosUnderMouse(float cell_size) {
	Vector2 mouse_pos = GetMousePosition();
	Vector2 cell_pos = {
		floorf(mouse_pos.x / cell_size),
		floorf(mouse_pos.y / cell_size)
	};
	return cell_pos;
}

#endif // GEOMETRY_HPP