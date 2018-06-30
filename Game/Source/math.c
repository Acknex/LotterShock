#include <acknex.h>

var ang_lerp_single(var a1, var a2, var f)
{
	var angle = ang(a1 - a2);
	return ang(a1 - (f * angle));
}

ANGLE* ang_lerp(ANGLE* a, ANGLE* a1, ANGLE* a2, var f)
{
	ANGLE ai;
	ai.pan = ang_lerp_single(a1->pan, a2->pan, f);
	ai.tilt = ang_lerp_single(a1->tilt, a2->tilt, f);
	ai.roll = ang_lerp_single(a1->roll, a2->roll, f);
	
	if(a != NULL)
		vec_set(a, &ai);
	
	return vector(ai.pan, ai.tilt, ai.roll);
}

void vec_rotateXY ( VECTOR *vec, var ang )
{
	var x = fcos(ang,vec->x) - fsin(ang,vec->y);
	vec->y = fsin(ang,vec->x) + fcos(ang,vec->y);
	vec->x = x;
}

void vec_rotateXZ ( VECTOR *vec, var ang )
{
	var x = fcos(ang,vec->x) - fsin(ang,vec->z);
	vec->z = fsin(ang,vec->x) + fcos(ang,vec->z);
	vec->x = x;
}

void vec_rotateYZ ( VECTOR *vec, var ang )
{
	var y = fcos(ang,vec->y) - fsin(ang,vec->z);
	vec->z = fsin(ang,vec->y) + fcos(ang,vec->z);
	vec->y = y;
}

var vec_lengthXY ( VECTOR *vec )
{
	return sqrt ( ( vec->x * vec->x ) + ( vec->y * vec->y ) );
}

var vec_lengthXZ ( VECTOR *vec )
{
	return sqrt ( ( vec->x * vec->x ) + ( vec->z * vec->z ) );
}

var vec_lengthYZ ( VECTOR *vec )
{
	return sqrt ( ( vec->y * vec->y ) + ( vec->z * vec->z ) );
}

void vec_diffXY ( VECTOR *vR, VECTOR *v1, VECTOR *v2 )
{
	vR->x = v1->x - v2->x;
	vR->y = v1->y - v2->y;
}

void vec_diffXZ ( VECTOR *vR, VECTOR *v1, VECTOR *v2 )
{
	vR->x = v1->x - v2->x;
	vR->z = v1->z - v2->z;
}

void vec_diffYZ ( VECTOR *vR, VECTOR *v1, VECTOR *v2 )
{
	vR->y = v1->y - v2->y;
	vR->z = v1->z - v2->z;
}

var vec_distXY ( VECTOR *p1, VECTOR *p2 )
{
	var x = ( p1->x - p2->x );
	var y = ( p1->y - p2->y );
	return sqrt ( ( x * x ) + ( y * y ) );
}

var vec_distXZ ( VECTOR *p1, VECTOR *p2 )
{
	var x = ( p1->x - p2->x );
	var z = ( p1->z - p2->z );
	return sqrt ( ( x * x ) + ( z * z ) );
}

var vec_distYZ ( VECTOR *p1, VECTOR *p2 )
{
	var y = ( p1->y - p2->y );
	var z = ( p1->z - p2->z );
	return sqrt ( ( y * y ) + ( z * z ) );
}

void vec_setXY ( VECTOR *v1, VECTOR *v2 )
{
	v1->x = v2->x;
	v1->y = v2->y;
}

void vec_setXZ ( VECTOR *v1, VECTOR *v2 )
{
	v1->x = v2->x;
	v1->z = v2->z;
}

void vec_setYZ ( VECTOR *v1, VECTOR *v2 )
{
	v1->y = v2->y;
	v1->z = v2->z;
}

void vec_addXY ( VECTOR *v1, VECTOR *v2 )
{
	v1->x += v2->x;
	v1->y += v2->y;
}

void vec_addXZ ( VECTOR *v1, VECTOR *v2 )
{
	v1->x += v2->x;
	v1->z += v2->z;
}

void vec_addYZ ( VECTOR *v1, VECTOR *v2 )
{
	v1->y += v2->y;
	v1->z += v2->z;
}

void vec_subXY ( VECTOR *v1, VECTOR *v2 )
{
	v1->x -= v2->x;
	v1->y -= v2->y;
}

void vec_subXZ ( VECTOR *v1, VECTOR *v2 )
{
	v1->x -= v2->x;
	v1->z -= v2->z;
}

void vec_subYZ ( VECTOR *v1, VECTOR *v2 )
{
	v1->y -= v2->y;
	v1->z -= v2->z;
}

void vec_normalizeXY ( VECTOR *vec, var length )
{
	var factor = length / vec_lengthXY ( vec );
	vec->x *= factor;
	vec->y *= factor;
}

void vec_normalizeXZ ( VECTOR *vec, var length )
{
	var factor = length / vec_lengthXZ ( vec );
	vec->x *= factor;
	vec->z *= factor;
}

void vec_normalizeYZ ( VECTOR *vec, var length )
{
	var factor = length / vec_lengthYZ ( vec );
	vec->y *= factor;
	vec->z *= factor;
}

void vec_scaleXY ( VECTOR *vec, var f )
{
	vec->x *= f;
	vec->y *= f;
}

void vec_scaleXZ ( VECTOR *vec, var f )
{
	vec->x *= f;
	vec->z *= f;
}

void vec_scaleYZ ( VECTOR *vec, var f )
{
	vec->y *= f;
	vec->z *= f;
}

var point_to_line2D_dist ( var px, var py, var l1x, var l1y, var l2x, var l2y )
{
	var varA = l2y - l1y;
	var varB = -( l2x - l1x );
	var varC = ( ( l2x - l1x ) * l1y ) - ( ( l2y - l1y ) * l1x );
	
	return ( ( varA * px ) + ( varB * py ) + varC ) / sqrt ( ( varA * varA ) + ( varB * varB ) );
}

var point_to_line2D_escalar ( var px, var py, var l1x, var l1y, var l2x, var l2y )
{
	return ( ( ( px - l1x ) * ( l2x - l1x ) ) + ( ( py - l1y ) * ( l2y - l1y ) ) ) / ( pow ( l2x - l1x, 2 ) + pow ( l2y - l1y, 2 ) );
}
