#pragma once
#include <base/device/device_export.h>
#include <base/device/geometry/controlpoint.h>
#include <base/device/transforms/glmhelper.h>

#include <vector>

namespace ngs {


// If there is only one control point in the curve,
// the curve will be considered to be constant.
// The value will be the value of the central point
// of the control point.

class DEVICE_EXPORT Curve {
 public:
   Curve(const Curve& other);
   Curve();
   ~Curve();

   // Serialization.
   void save(SimpleSaver& saver);
   void load(SimpleLoader& loader);

   // Operators.
   Curve& operator=(const Curve& other);
   bool operator==(const Curve& right)const;
   bool operator!=(const Curve& other)const;

   // Control Points.
   void set_control_points(const std::vector<glm::vec2>& points); // accepts a flattened list of left handles, center points, and right handles.
   void clear_control_points();
   void remove_control_point(size_t index);
   void insert_control_point(size_t at_index, const ControlPoint& cp);
   void append_control_point(const ControlPoint& cp);
   const std::vector<ControlPoint>& get_control_points() const;

   // Subdivision.
   size_t get_tessellated_size();
   void get_tessellated(std::vector<glm::vec2>& tessellated);

   // Evaluation.
   bool evaluate_at_x(float x, float& y);
   bool evaluate_at_x(size_t segment,float x, float& y);

   // Not too useful.
   // It's difficult to find the right segment for a given y value,
   // as there may be many segments which intersect at y.
   bool evaluate_at_y(size_t segment,float y, float& x);

 private:

   float evaluate_at_t(int dimension, int segment, float t);
   float evaluate_deriv_at_t(int dimension, int segment, float t);

   bool solve_for_time(int dimension, size_t segment,float x, float &time);

   // Subdivision.
   void subdivide_segment(size_t segment, glm::vec2* tessellated);
   void forward_difference(int dimension, float k0, float k1, float k2, float k3, int num_subdivisions, glm::vec2* tessellated);

   // Storage.
   bool path_is_closed;
   std::vector<ControlPoint> _control_points;
   static const int _num_subdivisions=25;

   // Error Tolerance.
   static const float _delta;
};

}


