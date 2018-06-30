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

// ---------------------------------------------------------------------------------
// Splines
// ---------------------------------------------------------------------------------

static double U[50];

double _spline_calc(int _k, int _t, double _u) {
	double firstterm, secondterm;
	
	if (_t == 1) {
		if (U[_k] <= _u && _u < U[_k+1]) {
			return 1.0;
		} else {
			return 0.0;
		}
	}
	
	if (U[_k+_t-1]-U[_k] < 1.0e-10) {
		firstterm = 0.0;
	} else {
		firstterm = ((_u-U[_k])/(U[_k+_t-1]-U[_k]))*_spline_calc(_k,_t-1,_u);
	}
	
	if (U[_k+_t]-U[_k+1] < 1.0e-10) {
		secondterm = 0.0;
	} else {
		secondterm = ((U[_k+_t]-_u)/(U[_k+_t]-U[_k+1]))*_spline_calc(_k+1,_t-1,_u);
	}
	return firstterm + secondterm;
}

List* calculate_spline(List* _points,  int _detail) {
	if (_points == NULL) return NULL;
	if (list_get_count(_points) == 1) return _points;
	if (list_get_count(_points) == 2) return _points;
	
	List* results = list_create();
	
	int t=4;
	int n=list_get_count(_points)-1;
	int m=_detail;
	
	int j,k;
	
	double temp, u;
	

	for (j = 0; j <= n + t; j++) {
		if (j < t) {
			U[j] = 0.0;
		}else {
			if (t <= j && j <= n) {
				U[j] = j-t+1;
			} else {
				U[j] = n-t+2;
			}
		}
	}
	
	for (j = 0; j <= m; j++) {
		u = ((double)j / m)*(n-t+2-.00000001);
		VECTOR* vecCurveTemp = sys_malloc(sizeof(VECTOR));
		vecCurveTemp->x = 0;
		vecCurveTemp->y = 0;
		vecCurveTemp->z = 0;
		
		for (k = 0; k <= n; k++)
		{
			temp = _spline_calc(k, t, u);
			
			VECTOR* vecPointTemp = list_item_at(_points, k);
			
			vecCurveTemp->x += vecPointTemp->x * temp;
			vecCurveTemp->y += vecPointTemp->y * temp;
			vecCurveTemp->z += vecPointTemp->z * temp;
		}
		list_add(results, vecCurveTemp);
	}
	
	list_clear(U);
	list_delete(U);
	return results;
}

VECTOR* math_get_spline(VECTOR* _points, int pointcount, float pos)
{
	if (_points == NULL) return NULL;
	if (pointcount < 2) return NULL;
	
	int t = 4;
	int j, k;
	
	double temp, u;

	for (j = 0; j < pointcount + t; j++)
	{
		if (j < t)
		{
			U[j] = 0.0;
		}
		else
		{
			if (t <= j && j < pointcount)
			{
				U[j] = j - t + 1;
			}
			else
			{
				U[j] = pointcount - t + 1;
			}
		}
	}
	
	u = (pos)*(pointcount - t + 1 - 0.00000001);
	VECTOR vecCurveTemp;
	vecCurveTemp.x = 0; vecCurveTemp.y = 0; vecCurveTemp.z = 0;
	
	for (k = 0; k < pointcount; k++)
	{
		temp = _spline_calc(k, t, u);
		
		vecCurveTemp.x += _points[k].x * temp;
		vecCurveTemp.y += _points[k].y * temp;
		vecCurveTemp.z += _points[k].z * temp;
	}
	
	return vector(vecCurveTemp.x, vecCurveTemp.y, vecCurveTemp.z);
}

int vec_to_bezierBufferSize = 0;
VECTOR *vec_to_bezierBuffer = NULL;

VECTOR *vec_to_bezier(VECTOR *pos, VECTOR *points, int count, float p)
{
	// Check if we need more space
	if(count > vec_to_bezierBufferSize)
	{
		// Free previous
		if(vec_to_bezierBuffer != NULL)
			sys_free(vec_to_bezierBuffer);
		// Reset buffer ptr
		vec_to_bezierBuffer = NULL;
	}
	// Check if we need a new bezier buffer
	if(vec_to_bezierBuffer == NULL)
	{
		// Allocate the bezier buffer
		vec_to_bezierBufferSize = count + 10;
		vec_to_bezierBuffer = sys_malloc(sizeof(VECTOR) * vec_to_bezierBufferSize);
	}
	// Copy points to bezier buffer
	memcpy(vec_to_bezierBuffer, points, sizeof(VECTOR) * count);
	
	// Go through the complete buffer until we have only 1 item left.
	while(count > 1)
	{
		int i;
		for(i = 0; i < count - 1; i++)
		{
			// Interpolate 2 items to 1
			vec_lerp(&vec_to_bezierBuffer[i], &vec_to_bezierBuffer[i], &vec_to_bezierBuffer[i + 1], p);
		}
		// Reduce number of items
		count --;
	}
	// Get temporary vector if needed
	if(pos == NULL) pos = vector(0, 0, 0);
	
	// Copy to result
	vec_set(pos, &vec_to_bezierBuffer[0]);
	return pos;
}



VECTOR *vec_to_catmull(VECTOR *pos, VECTOR *v0, VECTOR *v1, VECTOR *v2, VECTOR *v3, float s)
{
	var s1 = s;
	pos->x = 0.5 * (2 * v1->x + (v2->x - v0->x) *s1 + (2 *v0->x - 5 * v1->x + 4 * v2->x - v3->x) * s1 * s1 + (v3->x -3 * v2->x + 3 * v1->x - v0->x) * s1 * s1 * s1);
	pos->y = 0.5 * (2 * v1->y + (v2->y - v0->y) *s1 + (2 *v0->y - 5 * v1->y + 4 * v2->y - v3->y) * s1 * s1 + (v3->y -3 * v2->y + 3 * v1->y - v0->y) * s1 * s1 * s1);
	pos->z = 0.5 * (2 * v1->z + (v2->z - v0->z) *s1 + (2 *v0->z - 5 * v1->z + 4 * v2->z - v3->z) * s1 * s1 + (v3->z -3 * v2->z + 3 * v1->z - v0->z) * s1 * s1 * s1);
	return pos;
}


int float_cmp(float _f1, float _f2) {
	float f1 = floor(_f1 * 100) / 100;
	float f2 = floor(_f2 * 100) / 100;
	if (f1 > f2) return 1;
	if (f2 > f1) return -1;
	return 0;
}

void mat_eye ( float *_matrix, VECTOR *_vecPos, VECTOR *_vecDir )
{
	VECTOR _vecEye, _vecX, _vecY, _vecZ;
	_vecEye.x = _vecPos->x;
	_vecEye.y = _vecPos->z;
	_vecEye.z = _vecPos->y;
	_vecZ.x = _vecDir->x;
	_vecZ.y = _vecDir->z;
	_vecZ.z = _vecDir->y;
	vec_normalize ( &_vecZ, 1 );
	if ( abs(_vecZ.y) < 0.99 )
		vec_cross ( &_vecX, vector(0,1,0), &_vecZ );
	else
		vec_set ( &_vecX, vector(1,0,0) );
	vec_normalize ( &_vecX, 1 );
	vec_cross ( &_vecY, &_vecX, &_vecZ );
	vec_normalize ( &_vecY, 1 );
	
	_matrix[0] = _vecX.x;
	_matrix[1] = _vecY.x;
	_matrix[2] = _vecZ.x;
	_matrix[3] = 0;
	_matrix[4] = _vecX.y;
	_matrix[5] = _vecY.y;
	_matrix[6] = _vecZ.y;
	_matrix[7] = 0;
	_matrix[8] = _vecX.z;
	_matrix[9] = _vecY.z;
	_matrix[10] = _vecZ.z;
	_matrix[11] = 0;
	_matrix[12] = -vec_dot ( &_vecX, &_vecEye );
	_matrix[13] = -vec_dot ( &_vecY, &_vecEye );
	_matrix[14] = -vec_dot ( &_vecZ, &_vecEye );
	_matrix[15] = 1;
}
