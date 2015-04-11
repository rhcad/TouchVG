/*
    An Algorithm for Automatically Fitting Digitized Curves by Philip J. Schneider
    from "Graphics Gems", Academic Press, 1990

    This file is derived from https://github.com/erich666/GraphicsGems/blob/master/gems/FitCurves.c
    See also: http://www.graphicsgems.org

    TouchVG modifications Copyright (c) 2014 Zhang Yungui
 */

#include "mgpnt.h"
#include "mgdblpt.h"

//! The bezier segment with four points.
typedef struct {
    point_t pts[4];
    const point_t& operator[](int i) const { return pts[i]; }
    void set(int i, const point_t &pt) { pts[i] = pt; }
    Point2d* copy(Point2d* p) {
        for (int i = 0; i < 4; i++)
            p[i].set((float)pts[i].x, (float)pts[i].y);
        return p;
    }
} BezierCurve;

typedef Point2d (*PtCallback)(void* data, int i);

//! The wrapper class of multiple points.
typedef struct _PtArr {
    const Point2d *d;
    PtCallback  c;
    void        *data;
    
    _PtArr(const Point2d *d) : d(d), c((PtCallback)0) {}
    _PtArr(PtCallback c, void *data) : d((const Point2d *)0), c(c), data(data) {}
    
    point_t operator[](int i) const {
        if (d) return point_t(d[i].x, d[i].y);
        Point2d pt(c(data, i));
        return point_t(pt.x, pt.y);
    }
} PtArr;

// Forward declarations
typedef void        (*FitCubicCallback)(void* data, const Point2d curve[4]);
void                FitCurve(FitCubicCallback fc, void* data, const Point2d *d, int nPts, float error);
void      FitCurve2(FitCubicCallback fc, void* data, PtCallback d, void* data2, int nPts, float error);
static  void        FitCurve_(FitCubicCallback fc, void* data, const PtArr &d, int nPts, float error);
static  void        FitCubic(FitCubicCallback fc, void* data, const PtArr &d, int first, int &last,
                             const point_t& tHat1, const point_t& tHat2, double error);
static  double      *Reparameterize(const PtArr &d, int first, int last, double *u,
                                    const BezierCurve& bezCurve);
static  double      NewtonRaphsonRootFind(const BezierCurve& Q, const point_t& P, double u);
static  point_t     BezierII(int degree, const point_t *V, double t);
static  double      B0(double u), B1(double u), B2(double u), B3(double u);
static  point_t     ComputeLeftTangent(const PtArr &d, int end);
static  point_t     ComputeRightTangent(const PtArr &d, int end);
static  point_t     ComputeCenterTangent(const PtArr &d, int center);
static  double      ComputeMaxError(const PtArr &d, int first, int last,
                                    const BezierCurve& bezCurve, double *u, int &splitPoint);
static  double      *ChordLengthParameterize(const PtArr &d, int first, int &last);
static  BezierCurve GenerateBezier(const PtArr &d, int first, int last,
                                   const double *uPrime, const point_t& tHat1, const point_t& tHat2);

/*
 *  FitCurve :
 *      Fit a cubic Bezier curve to a set of digitized points
 *  d: Array of digitized points
 *  nPts: Number of digitized points
 *  error: tolerance (squared distance between points and fitted curve)
 */
void FitCurve(FitCubicCallback fc, void* data, const Point2d *d, int nPts, float error)
{
    PtArr arr(d);
    FitCurve_(fc, data, arr, nPts, error);
}

void FitCurve2(FitCubicCallback fc, void* data, PtCallback d, void* data2, int nPts, float error)
{
    PtArr arr(d, data2);
    FitCurve_(fc, data, arr, nPts, error);
}

static void FitCurve_(FitCubicCallback fc, void* data, const PtArr &d, int nPts, float error)
{
    point_t     tHat1, tHat2;   // Unit tangent vectors at endpoints
    int         first = 0;
    int         last = nPts - 1;
    int         oldlast;
    const Point2d ptbuf[4] = { Point2d::kInvalid() };
    
    tHat1 = ComputeLeftTangent(d, first);
    while (tHat1.isDegenerate() && first < last)
        tHat1 = ComputeLeftTangent(d, ++first);
    
    tHat2 = ComputeRightTangent(d, last);
    while (tHat2.isDegenerate() && last > first)
        tHat2 = ComputeRightTangent(d, --last);
    
    if (first < last) {
        oldlast = last;
        FitCubic(fc, data, d, first, last, tHat1, tHat2, mgMax(error, 1.1f));
        while (last < oldlast) {
            for (first = last + 1; first < oldlast; first++) {
                tHat1 = ComputeLeftTangent(d, first);
                if (!tHat1.isDegenerate())
                    break;
            }
            last = oldlast;
            if (first < last) {
                (*fc)(data, ptbuf);
                FitCubic(fc, data, d, first, last, tHat1, tHat2, mgMax(error, 1.1f));
            }
        }
    }
}

/*
 *  FitCubic :
 *      Fit a Bezier curve to a (sub)set of digitized points
 *  d: Array of digitized points
 *  first, last: Indices of first and last pts in region
 *  tHat1, tHat2: Unit tangent vectors at endpoints
 *  error: User-defined error squared
 */
static void FitCubic(FitCubicCallback fc, void* data, const PtArr &d, int first, int &last,
                     const point_t& tHat1, const point_t& tHat2, double error)
{
    BezierCurve bezCurve;       // Control points of fitted Bezier curve
    double      *u;             // Parameter values for point
    double      *uPrime;        // Improved parameter values
    double      maxError;       // Maximum fitting error
    int         splitPoint;     // Point to split point set at
    double      iterationError; // Error below which you try iterating
    int         maxIterations = 5;  // Max times to try iterating
    point_t     tHatCenter;     // Unit tangent vector at splitPoint
    int         i;
    Point2d     ptbuf[4];

    iterationError = error * error;

    // Use heuristic if region only has two points in it
    if (last - first == 1) {
        double dist = d[last].distanceTo(d[first]) / 3.0;

        bezCurve.set(0, d[first]);
        bezCurve.set(3, d[last]);
        bezCurve.set(1, bezCurve[0] + tHat1.scaledVector(dist));
        bezCurve.set(2, bezCurve[3] + tHat2.scaledVector(dist));
        (*fc)(data, bezCurve.copy(ptbuf));
        return;
    }

    // Parameterize points, and attempt to fit curve
    u = ChordLengthParameterize(d, first, last);
    bezCurve = GenerateBezier(d, first, last, u, tHat1, tHat2);

    // Find max deviation of points to fitted curve
    maxError = ComputeMaxError(d, first, last, bezCurve, u, splitPoint);
    if (maxError < error) {
        delete[] u;
        (*fc)(data, bezCurve.copy(ptbuf));
        return;
    }

    // If error not too large, try some reparameterization and iteration
    if (maxError < iterationError) {
        for (i = 0; i < maxIterations; i++) {
            uPrime = Reparameterize(d, first, last, u, bezCurve);
            bezCurve = GenerateBezier(d, first, last, uPrime, tHat1, tHat2);
            maxError = ComputeMaxError(d, first, last, bezCurve, uPrime, splitPoint);
            if (maxError < error) {
                delete[] u;
                delete[] uPrime;
                (*fc)(data, bezCurve.copy(ptbuf));
                return;
            }
            delete[] u;
            u = uPrime;
        }
    }

    // Fitting failed -- split at max error point and fit recursively
    delete[] u;
    tHatCenter = ComputeCenterTangent(d, splitPoint);
    FitCubic(fc, data, d, first, splitPoint, tHat1, tHatCenter, error);
    tHatCenter = point_t(-tHatCenter.x, -tHatCenter.y); // negate
    FitCubic(fc, data, d, splitPoint, last, tHatCenter, tHat2, error);
}


/*
 *  GenerateBezier :
 *  Use least-squares method to find Bezier control points for region.
 *  d: Array of digitized points
 *  first, last: Indices defining region
 *  uPrime: Parameter values for region
 *  tHat1, tHat2: Unit tangents at endpoints
 */
static BezierCurve GenerateBezier(const PtArr &d, int first, int last,
                                  const double *uPrime, const point_t& tHat1, const point_t& tHat2)
{
    int     i;
    point_t *A, *B;                         // Precomputed rhs for eqn
    const int nPts = last - first + 1;      // Number of pts in sub-curve
    double  C[2][2];                        // Matrix C
    double  X[2];                           // Matrix X
    double  det_C0_C1, det_C0_X, det_X_C1;  // Determinants of matrices
    double  alpha_l, alpha_r;               // Alpha values, left and right
    point_t tmp;                            // Utility variable
    BezierCurve bezCurve;                   // RETURN bezier curve ctl pts

    // Compute the A's
    A = new point_t[nPts * 2];
    B = A + nPts;
    for (i = 0; i < nPts; i++) {
        A[i] = tHat1.scaledVector(B1(uPrime[i]));
        B[i] = tHat2.scaledVector(B2(uPrime[i]));
    }

    // Create the C and X matrices
    C[0][0] = 0.0;
    C[0][1] = 0.0;
    C[1][0] = 0.0;
    C[1][1] = 0.0;
    X[0]    = 0.0;
    X[1]    = 0.0;

    for (i = 0; i < nPts; i++) {
        C[0][0] += A[i].dotProduct(A[i]);
        C[0][1] += A[i].dotProduct(B[i]);
        C[1][0] = C[0][1];
        C[1][1] += B[i].dotProduct(B[i]);

        tmp = d[first + i] - (d[first] * B0(uPrime[i]) + d[first] * B1(uPrime[i]) +
                              d[last] * B2(uPrime[i]) + d[last] * B3(uPrime[i]));
        
        X[0] += A[i].dotProduct(tmp);
        X[1] += B[i].dotProduct(tmp);
    }
    
    delete[] A;

    // Compute the determinants of C and X
    det_C0_C1 = C[0][0] * C[1][1] - C[1][0] * C[0][1];
    det_C0_X  = C[0][0] * X[1]    - C[1][0] * X[0];
    det_X_C1  = X[0]    * C[1][1] - X[1]    * C[0][1];

    // Finally, derive alpha values
    alpha_l = (det_C0_C1 == 0) ? 0.0 : det_X_C1 / det_C0_C1;
    alpha_r = (det_C0_C1 == 0) ? 0.0 : det_C0_X / det_C0_C1;

    // If alpha negative, use the Wu/Barsky heuristic (see text)
    // (if alpha is 0, you get coincident control points that lead to
    // divide by zero in any subsequent NewtonRaphsonRootFind() call.
    double segLength = d[last].distanceTo(d[first]);
    double epsilon = 1.0e-6 * segLength;
    if (alpha_l < epsilon || alpha_r < epsilon) {
        // fall back on standard (probably inaccurate) formula, and subdivide further if needed.
        double dist = segLength / 3.0;
        bezCurve.set(0, d[first]);
        bezCurve.set(3, d[last]);
        bezCurve.set(1, bezCurve[0] + tHat1.scaledVector(dist));
        bezCurve.set(2, bezCurve[3] + tHat2.scaledVector(dist));
        return bezCurve;
    }

    // First and last control points of the Bezier curve are
    // positioned exactly at the first and last data points
    // Control points 1 and 2 are positioned an alpha distance out
    // on the tangent vectors, left and right, respectively
    bezCurve.set(0, d[first]);
    bezCurve.set(3, d[last]);
    bezCurve.set(1, bezCurve[0] + tHat1.scaledVector(alpha_l));
    bezCurve.set(2, bezCurve[3] + tHat2.scaledVector(alpha_r));
    
    return bezCurve;
}


/*
 *  Reparameterize:
 *  Given set of points and their parameterization, try to find
 *   a better parameterization.
 *  d: Array of digitized points
 *  first, last: Indices defining region
 *  u: Current parameter values
 *  bezCurve: Current fitted curve
 */
static double *Reparameterize(const PtArr &d, int first, int last, double *u, const BezierCurve& bezCurve)
{
    double  *uPrime;                // New parameter values

    uPrime = new double[last - first + 1];
    for (int i = first; i <= last; i++) {
        uPrime[i-first] = NewtonRaphsonRootFind(bezCurve, d[i], u[i - first]);
    }
    return uPrime;
}


/*
 *  NewtonRaphsonRootFind :
 *  Use Newton-Raphson iteration to find better root.
 *  Q: Current fitted curve
 *  P: Digitized point
 *  u: Parameter value for "P"
 */
static double NewtonRaphsonRootFind(const BezierCurve& Q, const point_t& P, double u)
{
    double      numerator, denominator;
    point_t     Q1[3], Q2[2];       // Q' and Q''
    point_t     Q_u, Q1_u, Q2_u;    // u evaluated at Q, Q', & Q''
    double      uPrime;             // Improved u
    int         i;

    // Compute Q(u)
    Q_u = BezierII(3, Q.pts, u);

    // Generate control vertices for Q'
    for (i = 0; i <= 2; i++) {
        Q1[i].x = (Q[i+1].x - Q[i].x) * 3.0;
        Q1[i].y = (Q[i+1].y - Q[i].y) * 3.0;
    }

    // Generate control vertices for Q''
    for (i = 0; i <= 1; i++) {
        Q2[i].x = (Q1[i+1].x - Q1[i].x) * 2.0;
        Q2[i].y = (Q1[i+1].y - Q1[i].y) * 2.0;
    }

    // Compute Q'(u) and Q''(u)
    Q1_u = BezierII(2, Q1, u);
    Q2_u = BezierII(1, Q2, u);

    // Compute f(u)/f'(u)
    numerator = (Q_u.x - P.x) * (Q1_u.x) + (Q_u.y - P.y) * (Q1_u.y);
    denominator = ((Q1_u.x) * (Q1_u.x) + (Q1_u.y) * (Q1_u.y) +
                   (Q_u.x - P.x) * (Q2_u.x) + (Q_u.y - P.y) * (Q2_u.y));
    
    if (denominator == 0.0)
        return u;

    // u = u - f(u)/f'(u)
    uPrime = u - (numerator/denominator);
    return uPrime;
}


/*
 *  Bezier :
 *      Evaluate a Bezier curve at a particular parameter value
 *  degree: The degree of the bezier curve
 *  V: Array of control points
 *  t: Parametric value to find point for
 */
static point_t BezierII(int degree, const point_t *V, double t)
{
    int         i, j;
    point_t     Vtemp[5];       // Local copy of control points

    // Copy array
    for (i = 0; i <= degree; i++) {
        Vtemp[i] = V[i];
    }

    // Triangle computation
    for (i = 1; i <= degree; i++) {
        for (j = 0; j <= degree-i; j++) {
            Vtemp[j] = (1.0 - t) * Vtemp[j] + t * Vtemp[j+1];
        }
    }

    return Vtemp[0];            // Point on curve at parameter t
}


/*
 *  B0, B1, B2, B3 :
 *  Bezier multipliers
 */
static double B0(double u) {
    double tmp = 1.0 - u;
    return (tmp * tmp * tmp);
}

static double B1(double u) {
    double tmp = 1.0 - u;
    return (3 * u * (tmp * tmp));
}

static double B2(double u) {
    double tmp = 1.0 - u;
    return (3 * u * u * tmp);
}

static double B3(double u) {
    return (u * u * u);
}


/*
 *  ComputeLeftTangent, ComputeRightTangent, ComputeCenterTangent :
 *  Approximate unit tangents at endpoints and "center" of digitized curve
 *  d: Digitized points
 *  end: Index to "left" end of region
 */
static point_t ComputeLeftTangent(const PtArr &d, int end) {
    return (d[end+1] - d[end]).normalized();
}

// end: Index to "right" end of region
static point_t ComputeRightTangent(const PtArr &d, int end) {
    return (d[end-1] - d[end]).normalized();
}

// center: Index to point inside region
static point_t ComputeCenterTangent(const PtArr &d, int center) {
    return (d[center-1] - d[center+1]).normalized();
}

/*
 *  ChordLengthParameterize :
 *  Assign parameter values to digitized points
 *  using relative distances between points.
 */
static double *ChordLengthParameterize(const PtArr &d, int first, int &last)
{
    int     i;
    double  *u;         // Parameterization

    u = new double[last-first+1];

    u[0] = 0.0;
    for (i = first+1; i <= last; i++) {
        if (d[i].isDegenerate()) {
            last = i - 1;
            break;
        }
        u[i-first] = u[i-first-1] + d[i].distanceTo(d[i-1]);
    }

    for (i = first + 1; i <= last; i++) {
        u[i-first] = u[i-first] / u[last-first];
    }

    return u;
}


/*
 *  ComputeMaxError :
 *  Find the maximum squared distance of digitized points
 *  to fitted curve.
 *  d:  Array of digitized points
 *  first, last: Indices defining region
 *  bezCurve: Fitted Bezier curve
 *  u: Parameterization of points
 *  splitPoint: Point of maximum error
 */
static double ComputeMaxError(const PtArr &d, int first, int last,
                             const BezierCurve& Q, double *u, int &splitPoint)
{
    int     i;
    double  maxDist = 0.0;  // Maximum error
    double  dist;           // Current error
    point_t P;              // Point on curve
    point_t v;              // Vector from point to curve

    splitPoint = (last - first + 1) / 2;
    for (i = first + 1; i < last; i++) {
        P = BezierII(3, Q.pts, u[i-first]);
        v = P - d[i];
        dist = v.lengthSquare();
        if (dist >= maxDist) {
            maxDist = dist;
            splitPoint = i;
        }
    }
    return maxDist;
}
