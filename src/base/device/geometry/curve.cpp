#include <base/device/geometry/curve.h>
#include <base/memoryallocator/taggednew.h>
#include <base/device/geometry/controlpoint.h>

#include <base/utils/simplesaver.h>
#include <base/utils/simpleloader.h>

#include <algorithm>
#include <iostream>

namespace ngs {

const float Curve::_delta = 1.E-6;

Curve::Curve()
    : path_is_closed(false) {
  // by default the curve is flat with a value of zero
  ControlPoint cp0(glm::vec2(-50,0));
  cp0._out_tangent = glm::vec2(100,0);

  ControlPoint cp1(glm::vec2(350,0));
  cp1._in_tangent = glm::vec2(200,0);

  _control_points.push_back(cp0);
  _control_points.push_back(cp1);
}

Curve::Curve(const Curve& other) {
  *this = other;
}

Curve::~Curve() {
}

void Curve::save(SimpleSaver& saver) {
  // Closed state.
  saver.save(path_is_closed);

  // Length of control points.
  size_t length = _control_points.size();
  saver.save(length);

  // Control points.
  for (size_t i=0; i<length; ++i) {
    _control_points[i].save(saver);
  }
}

void Curve::load(SimpleLoader& loader) {
  // Closed state.
  loader.load(path_is_closed);

  // Length of control points.
  size_t length;
  loader.load(length);
  _control_points.resize(length);

  // Control points.
  for (size_t i=0; i<length; ++i) {
    _control_points[i].load(loader);
  }
}

Curve& Curve::operator=(const Curve& other) {
  path_is_closed = other.path_is_closed;
  _control_points = other._control_points;
  return *this;
}

bool Curve::operator==(const Curve& right) const {
  if (path_is_closed != right.path_is_closed) {
    return false;
  }

  if (_control_points.size() != right._control_points.size()) {
    return false;
  }

  for (size_t i=0; i<_control_points.size(); ++i) {
    if (_control_points[i] != right._control_points[i]) {
      return false;
    }
  }
  return true;
}

bool Curve::operator!=(const Curve& other)const {
  return !(*this==other);
}

void Curve::set_control_points(const std::vector<glm::vec2>& points) {
  _control_points.clear();
  size_t next_index = 0;

  // The first control point's left handle doesn't eat up a point.
  // We copy the center point to the left handle.
  if (points.size()>=2) {
    _control_points.push_back(ControlPoint(points[0],points[0],points[1]));
  }
  next_index=2;
  // The non-last control point's eat up 3 points.
  while((next_index+3)<=points.size()) {
    _control_points.push_back(ControlPoint(points[next_index],points[next_index+1],points[next_index+2]));
    next_index+=3;
  }
  // The last control may use a minimum of 2 points.
  if ((next_index+2)<=points.size()) {
    _control_points.push_back(ControlPoint(points[next_index],points[next_index+1],points[next_index+1]));
  }
}

void Curve::clear_control_points() {
  _control_points.clear();
}

void Curve::remove_control_point(size_t index) {
  std::vector<ControlPoint>::iterator iter = _control_points.begin();
  iter += index;
  if (iter != _control_points.end()) {
    _control_points.erase(iter);
  }
}

void Curve::insert_control_point(size_t at_index, const ControlPoint& cp) {
  // Try to find the right spot to insert into.
  std::vector<ControlPoint>::iterator iter = _control_points.begin();
  iter+=at_index;
  if (iter != _control_points.end()) {
    _control_points.insert(iter, cp);
  } else {
    // If the at_index is out of bounds, we append it.
    _control_points.push_back(cp);
  }
}

void Curve::append_control_point(const ControlPoint& cp) {
  _control_points.push_back(cp);
}

const std::vector<ControlPoint>& Curve::get_control_points() const {
  return _control_points;
}

// Note this only finds one root.
// In general there could be more.
bool Curve::solve_for_time(int dimension, size_t segment, float value, float& time) {
  // Find time t, for By(t)=a or Bx(t)=a.
  // B(t) = c0x + c1xt + c2xt2 + c3xt3
  // By(t) = c0y + c1yt + c2yt2 + c3yt3

  // First solve Bx(t)=a.
  // c0x + c1xt + c2xt2 + c3xt3 = a

  // Second use the t value to get y = By(t).

  size_t max_iter = 1000;
  size_t count = 0;
  float t = 0.5; // our first guess. (bias to the left side).
  float diff = value - evaluate_at_t(dimension,segment,t);
  float derivative = evaluate_deriv_at_t(dimension,segment,t);
  float tolerance = 0.0001;
  bool solved = false;
  for (size_t i = 0; i < max_iter; ++i) {
    // If the derivative is close to zero, choose another t to start.
    if (fabs(derivative) < _delta) {
      t = rand() / float(RAND_MAX);
      diff = value - evaluate_at_t(dimension,segment,t);
      derivative = evaluate_deriv_at_t(dimension,segment,t);
    }

    // Adjust the t closer to root.
    t = t - diff / (-derivative);

    // Update the loop variables.
    diff = value - evaluate_at_t(dimension, segment, t);
    derivative = evaluate_deriv_at_t(dimension, segment, t);

    // Break if we're close enough.
    if (fabs(diff) < tolerance) {
      solved = true;
      break;
    }
  }

  time =t;
  if (solved) {
    return true;
  }
  return false;
}

bool Curve::evaluate_at_x(float x, float& y) {
  // If there's no control points, our default value is zero.
  if (_control_points.empty()) {
    y = 0.0f;
    return true;
  }

  // If there's only one control point, we have a constant curve.
  if (_control_points.size()==1) {
    y = _control_points[0]._center.y;
    return true;
  }

  // Find the right segment.
  // We assume that our curves are setup such that for a given x,
  // there is only one intersection with y.
  // This means that the right segment will have terminals which bound x.

  for (size_t i=0; i<_control_points.size()-1; ++i) {
    if ((_control_points[i]._center.x <= x) &&
        (_control_points[i+1]._center.x >=x) ) {
      return evaluate_at_x(i,x,y);
    }
  }
  return false;
}

bool Curve::evaluate_at_x(size_t segment, float x, float& y) {

  // If there's no control points, our default value is zero.
  if (_control_points.empty()) {
    y = 0.0f;
    return true;
  }

  // If there's only one control point, we have a constant curve.
  if (_control_points.size()==1) {
    y = _control_points[0]._center.y;
    return true;
  }

  // Solve for time.
  float time;
  if (!solve_for_time(0,segment,x,time)) {
    return false;
  }
  // Make sure the time is in range [0,1].
  if ((time <-_delta) || (time >1+_delta)) {
    return false;
  }
  // Determine y value from time.
  y = evaluate_at_t(1,segment,time);
  return true;
}

bool Curve::evaluate_at_y(size_t segment, float y, float& x) {
  // If there's only one control point, we have a constant curve.
  if (_control_points.size()==1) {
    x = _control_points[0]._center.x;
    return true;
  }

  // Solve for time.
  float time;
  if (!solve_for_time(1,segment,y,time)) {
    return false;
  }
  // Make sure the time is in range [0,1].
  if ((time <-_delta) || (time >1+_delta)) {
    return false;
  }
  // Determine y value from time.
  x = evaluate_at_t(0,segment,time);
  return true;
}

size_t Curve::get_tessellated_size() {
  // Allocated the tessellated vertices.
  if (!path_is_closed) {
    return (_control_points.size()-1)*_num_subdivisions+1;
  }
  return _control_points.size()*_num_subdivisions+1;
}

void Curve::get_tessellated(std::vector<glm::vec2>& tessellated) {
  // Allocated the tessellated vertices.
  tessellated.resize(get_tessellated_size());

  // Subdivide each segment.
  for(size_t i=0; (i+1)<_control_points.size(); i++)
  {
    subdivide_segment(i, &tessellated[i*_num_subdivisions]);
  }

  // If the curve is close we need to draw the closing segment.
  if(path_is_closed)
  {
      if(_control_points.size()>1)
      {
        subdivide_segment(_control_points.size()-1, &tessellated[(_control_points.size()-1)*_num_subdivisions]);
      }
  }
}

void Curve::subdivide_segment(size_t segment, glm::vec2* tessellated)
{
    size_t a=segment;
    size_t b=a+1;
    // If the path is being closed we loop around.
    if (b == _control_points.size()) {
      b = 0;
    }


    // Note that we are writing to (_num_subdivisions+1) vec2 tuples in tessellated.

    // Subdivide the x dimension.
    {
        float k0=_control_points[a]._center.x;
        float k1=_control_points[a]._out_tangent.x;
        float k2=_control_points[b]._in_tangent.x;
        float k3=_control_points[b]._center.x;
        forward_difference(0,k0,k1,k2,k3,_num_subdivisions,tessellated);
    }
    // Subdivide the y dimension.
    {
        float k0=_control_points[a]._center.y;
        float k1=_control_points[a]._out_tangent.y;
        float k2=_control_points[b]._in_tangent.y;
        float k3=_control_points[b]._center.y;
        forward_difference(1,k0,k1,k2,k3,_num_subdivisions,tessellated);
    }

    // Draw a single bezier curve segment.
//    glBegin(GL_LINE_STRIP);
//    {
//        for(size_t j=0; j<=_num_subdivisions; j++)
//        {
//            glVertex3f(samples[j].x,samples[j].y,_depth);
//        }
//    }
//    glEnd();
}

// This create num_subdivisions+1 samples. The first and last samples are at time t=0.0 and t=1.0 respectively.
// Samples should preallocated to be size of (num_subdivisions+1).
void Curve::forward_difference(int dimension, float k0, float k1, float k2, float k3, int num_subdivisions, glm::vec2* tessellated)
{
    // Time delta for one sub division.
    float t=1.0f/static_cast<float>(num_subdivisions);
    float t2=t*t;
    float t3=t2*t;

    // Parametric variables premultiplied with t.
    float Ch= 3.0f*(-k0+k1)*t;
    float Bh2= 3.0f*(k0-2.0f*k1+k2)*t2;
    float Ah3= (-k0+3.0f*(k1-k2)+k3)*t3;

    // Initial forward differences.
    float F1= Ah3 + Bh2 + Ch;
    float F2= 6*Ah3 + 2*Bh2;
    float F3= 6*Ah3;

    // Loop over the sub divisions.
    float sample=k0;
    for(int i=0; i<=num_subdivisions; i++)
    {
        // Record the bezier sample.
        tessellated[i][dimension]=sample;

        // Update sample and forward differences.
        sample+=F1;
        F1+=F2;
        F2+=F3;
    }
}

float Curve::evaluate_at_t(int dimension, int segment, float t) {
  // Given the following points, defining the bezier segment.
  // P0 = (X0,Y0)
  // P1 = (X1,Y1)
  // P2 = (X2,Y2)
  // P3 = (X3,Y3)
  // The (x,y) values are: t is in [0,1]
  // B(t) = (1-t)^3 * P0 + 3*(1-t)^2 * t * P1 + 3*(1-t) * t^2 * P2 + t^3 * P3

  size_t a=segment;
  size_t b=a+1;

  // If the path is being closed we loop around.
  if (b == _control_points.size()) {
    b = 0;
  }

  float p0=_control_points[a]._center[dimension];
  float p1=_control_points[a]._out_tangent[dimension];
  float p2=_control_points[b]._in_tangent[dimension];
  float p3=_control_points[b]._center[dimension];

  float s = 1.0f-t;
  float s2 = s*s;
  float s3 = s*s2;

  float t2 = t*t;
  float t3 = t*t2;

  return (s3)     * p0 +
         (3*s2*t) * p1 +
         (3*s*t2) * p2 +
         (t3)     * p3;
}

float Curve::evaluate_deriv_at_t(int dimension, int segment, float t) {
  // Given the following points, defining the bezier segment.
  // P0 = (X0,Y0)
  // P1 = (X1,Y1)
  // P2 = (X2,Y2)
  // P3 = (X3,Y3)
  // The (x,y) values are: t is in [0,1]
  // B(t) = (1-t)^3 * P0 + 3*(1-t)^2 * t * P1 + 3*(1-t) * t^2 * P2 + t^3 * P3
  // The derivates are:
  // B'(t) = 3(1-t)^2(P1-P0) + 6(1-t)t(P2-P1) + 3t^2(P3 - P2)

  size_t a=segment;
  size_t b=a+1;

  // If the path is being closed we loop around.
  if (b == _control_points.size()) {
    b = 0;
  }

  float p0=_control_points[a]._center[dimension];
  float p1=_control_points[a]._out_tangent[dimension];
  float p2=_control_points[b]._in_tangent[dimension];
  float p3=_control_points[b]._center[dimension];

  float s = 1.0f-t;
  float s2 = s*s;

  float t2 = t*t;

  return 3.0f*s2  *(p1-p0) +
         6.0f*s*t *(p2-p1) +
         3.0f*t2  *(p3-p2);
}

}
