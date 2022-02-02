#include <cmath>
#include "intersect.h"
#include "constants.h"
#include "tools.h"

/* | algo           | difficulty | */
/* |----------------+------------| */
/* | BSPherePlane   |          1 | */
/* | BBoxBBox       |          2 | */
/* | BBoxPlane      |          4 | */

// @@ TODO: test if a BSpheres intersects a plane.
//! Returns :
//   +IREJECT outside
//   -IREJECT inside
//    IINTERSECT intersect

int BSpherePlaneIntersect(const BSphere *bs, Plane *pl) {
	/* =================== PUT YOUR CODE HERE ====================== */

	float dist_pl = pl.distance(bs.getPosition()); 	//plano-centro
	float dist_ra = bs.getRadius();					//radio
	int side = 0;
	//INTERSECTAN
	if (dist_pl < dist_ra) {
		return IINTERSECT;
	}
	else {
		//dentro o fuera?? mirar la direccion del vector normal
		side = pl.whichSide(bs.getPosition());
		if (side == -1) {
		 	return -IREJECT;
		}
		else if (side == 1) {
			return +IREJECT:
		}
		return IREJECT;
	}

	/* =================== END YOUR CODE HERE ====================== */
}


// @@ TODO: test if two BBoxes intersect.
//! Returns :
//    IINTERSECT intersect
//    IREJECT don't intersect

int  BBoxBBoxIntersect(const BBox *bba, const BBox *bbb ) {
	/* =================== PUT YOUR CODE HERE ====================== */

	//comprobar eje x
	if ( (bba.m_max.x() >= bbb.m_min.x()) || (bbb.m_max.x() >= bba.m_min.x()) ) {
		//si intersectan, comprobar eje y
		if ( (bba.m_max.y() >= bbb.m_min.y()) || (bbb.m_max.y() >= bba.m_min.y()) ) {
			//si intersectan, comprobar eje z
			if ( (bba.m_max.z() >= bbb.m_min.z()) || (bbb.m_max.z() >= bba.m_min.z()) ) {
				return IINTERSECT;
			}
		}
	}
	return IREJECT;

	/* =================== END YOUR CODE HERE ====================== */
}


// @@ TODO: test if a BBox and a plane intersect.
//! Returns :
//   +IREJECT outside
//   -IREJECT inside
//    IINTERSECT intersect

int  BBoxPlaneIntersect (const BBox *theBBox, Plane *thePlane) {
	/* =================== PUT YOUR CODE HERE ====================== */
	float xcercano, ycercano, zcercano, xlejano, ylejano, zlejano = 0.0f;
	
	//Obtener los puntos maximo y minimo de la diagonal
	//eje x +
	if (thePlane.m_n.x() > 0) {
		xcercano = theBBox.m_min.x();
		xlejano = theBBox.m_max.x();
	}
	//eje x -
	else {
		xcercano = theBBox.m_max.x();
		xlejano = theBBox.m_min.x();
	}
	//eje y +
	if (thePlane.m_n.y() > 0) {
		ycercano = theBBox.m_min.y();
		ylejano = theBBox.m_max.y();
	}
	//eje y -
	else {
		ycercano = theBBox.m_max.y();
		ylejano = theBBox.m_min.y();
	}
	//eje z +
	if (thePlane.m_n.z() > 0) {
		zcercano = theBBox.m_min.z();
		zlejano = theBBox.m_max.z();
	}
	//eje z -
	else {
		zcercano = theBBox.m_max.z();
		zlejano = theBBox.m_min.z();
	}

	//comprobar la interseccion
	if () {
		if () {
			if () {
				return IINTERSECT;
			}
		}
	}

	
		

	/* =================== END YOUR CODE HERE ====================== */
}

// Test if two BSpheres intersect.
//! Returns :
//    IREJECT don't intersect
//    IINTERSECT intersect

int BSphereBSphereIntersect(const BSphere *bsa, const BSphere *bsb ) {

	Vector3 v;
	v = bsa->m_centre - bsb->m_centre;
	float ls = v.dot(v);
	float rs = bsa->m_radius + bsb->m_radius;
	if (ls > (rs * rs)) return IREJECT; // Disjoint
	return IINTERSECT; // Intersect
}


// Test if a BSpheres intersect a BBox.
//! Returns :
//    IREJECT don't intersect
//    IINTERSECT intersect

int BSphereBBoxIntersect(const BSphere *sphere, const BBox *box) {

	float d;
	float aux;
	float r;

	r = sphere->m_radius;
	d = 0;

	aux = sphere->m_centre[0] - box->m_min[0];
	if (aux < 0) {
		if (aux < -r) return IREJECT;
		d += aux*aux;
	} else {
		aux = sphere->m_centre[0] - box->m_max[0];
		if (aux > 0) {
			if (aux > r) return IREJECT;
			d += aux*aux;
		}
	}

	aux = (sphere->m_centre[1] - box->m_min[1]);
	if (aux < 0) {
		if (aux < -r) return IREJECT;
		d += aux*aux;
	} else {
		aux = sphere->m_centre[1] - box->m_max[1];
		if (aux > 0) {
			if (aux > r) return IREJECT;
			d += aux*aux;
		}
	}

	aux = sphere->m_centre[2] - box->m_min[2];
	if (aux < 0) {
		if (aux < -r) return IREJECT;
		d += aux*aux;
	} else {
		aux = sphere->m_centre[2] - box->m_max[2];
		if (aux > 0) {
			if (aux > r) return IREJECT;
			d += aux*aux;
		}
	}
	if (d > r * r) return IREJECT;
	return IINTERSECT;
}

// Test if a Triangle intersects a ray.
//! Returns :
//    IREJECT don't intersect
//    IINTERSECT intersect

int IntersectTriangleRay(const Vector3 & P0,
						 const Vector3 & P1,
						 const Vector3 & P2,
						 const Line *l,
						 Vector3 & uvw) {
	Vector3 e1(P1 - P0);
	Vector3 e2(P2 - P0);
	Vector3 p(crossVectors(l->m_d, e2));
	float a = e1.dot(p);
	if (fabs(a) < Constants::distance_epsilon) return IREJECT;
	float f = 1.0f / a;
	// s = l->o - P0
	Vector3 s(l->m_O - P0);
	float lu = f * s.dot(p);
	if (lu < 0.0 || lu > 1.0) return IREJECT;
	Vector3 q(crossVectors(s, e1));
	float lv = f * q.dot(l->m_d);
	if (lv < 0.0 || lv > 1.0) return IREJECT;
	uvw[0] = lu;
	uvw[1] = lv;
	uvw[2] = f * e2.dot(q);
	return IINTERSECT;
}

/* IREJECT 1 */
/* IINTERSECT 0 */

const char *intersect_string(int intersect) {

	static const char *iint = "IINTERSECT";
	static const char *prej = "IREJECT";
	static const char *mrej = "-IREJECT";
	static const char *error = "IERROR";

	const char *result = error;

	switch (intersect) {
	case IINTERSECT:
		result = iint;
		break;
	case +IREJECT:
		result = prej;
		break;
	case -IREJECT:
		result = mrej;
		break;
	}
	return result;
}
