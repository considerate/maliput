#pragma once

#include <functional>
#include <ostream>
#include <string>

#include "drake/common/default_scalars.h"
#include "drake/common/drake_assert.h"
#include "drake/common/drake_copyable.h"
#include "drake/common/drake_throw.h"
#include "drake/common/eigen_types.h"
#include "drake/common/extract_double.h"
#include "drake/math/quaternion.h"
#include "drake/math/roll_pitch_yaw.h"
#include "drake/math/rotation_matrix.h"

namespace maliput {
namespace api {

class Lane;

/// A specific endpoint of a specific Lane.
struct LaneEnd {
  /// Labels for the endpoints of a Lane.
  /// kStart is the "s == 0" end, and kFinish is the other end.
  enum Which { kStart, kFinish, };

  /// An arbitrary strict complete ordering, useful for, e.g., std::map.
  struct StrictOrder {
    bool operator()(const LaneEnd& lhs, const LaneEnd& rhs) const {
      auto as_tuple = [](const LaneEnd& le) {
        return std::tie(le.lane, le.end);
      };
      return as_tuple(lhs) < as_tuple(rhs);
    }
  };

  /// Default constructor.
  LaneEnd() = default;

  /// Construct a LaneEnd specifying the @p end of @p lane.
  LaneEnd(const Lane* _lane, Which _end) : lane(_lane), end(_end) {}

  const Lane* lane{};
  Which end{};
};

/// Streams a string representation of @p which_end into @p out. Returns
/// @p out. This method is provided for the purposes of debugging or
/// text-logging. It is not intended for serialization.
std::ostream& operator<<(std::ostream& out, const LaneEnd::Which& which_end);

/// A 3-dimensional rotation.
class Rotation {
 public:
  DRAKE_DEFAULT_COPY_AND_MOVE_AND_ASSIGN(Rotation)

  /// Default constructor, creating an identity Rotation.
  Rotation() : quaternion_(drake::Quaternion<double>::Identity()) {}

  /// Constructs a Rotation from a quaternion @p quaternion (which will be
  /// normalized).
  static Rotation FromQuat(const drake::Quaternion<double>& quaternion) {
    return Rotation(quaternion.normalized());
  }

  /// Constructs a Rotation from @p rpy, a vector of `[roll, pitch, yaw]`,
  /// expressing a roll around X, followed by pitch around Y,
  /// followed by yaw around Z (with all angles in radians).
  static Rotation FromRpy(const drake::Vector3<double>& rpy) {
    return Rotation(drake::math::RollPitchYaw<double>(rpy).ToQuaternion());
  }

  /// Constructs a Rotation expressing a @p roll around X, followed by
  /// @p pitch around Y, followed by @p yaw around Z (with all angles
  /// in radians).
  static Rotation FromRpy(double roll, double pitch, double yaw) {
    return FromRpy(drake::Vector3<double>(roll, pitch, yaw));
  }

  /// Provides a quaternion representation of the rotation.
  const drake::Quaternion<double>& quat() const { return quaternion_; }

  /// Sets value from a Quaternion @p quaternion (which will be normalized).
  void set_quat(const drake::Quaternion<double>& quaternion) {
    quaternion_ = quaternion.normalized();
  }

  /// Provides a 3x3 rotation matrix representation of "this" rotation.
  drake::Matrix3<double> matrix() const {
    return drake::math::RotationMatrix<double>(quaternion_).matrix();
  }

  /// Provides a representation of rotation as a vector of angles
  /// `[roll, pitch, yaw]` (in radians).
  drake::math::RollPitchYaw<double> rpy() const {
    return drake::math::RollPitchYaw<double>(quaternion_);
  }

  // TODO(maddog@tri.global)  Deprecate and/or remove roll()/pitch()/yaw(),
  //                          since they hide the call to rpy(), and since
  //                          most call-sites should probably be using something
  //                          else (e.g., quaternion) anyway.
  /// Returns the roll component of the Rotation (in radians).
  double roll() const { return rpy().roll_angle(); }

  /// Returns the pitch component of the Rotation (in radians).
  double pitch() const { return rpy().pitch_angle(); }

  /// Returns the yaw component of the Rotation (in radians).
  double yaw() const { return rpy().yaw_angle(); }

 private:
  explicit Rotation(const drake::Quaternion<double>& quat) : quaternion_(quat) {}

  drake::Quaternion<double> quaternion_;
};

/// Streams a string representation of @p rotation into @p out. Returns
/// @p out. This method is provided for the purposes of debugging or
/// text-logging. It is not intended for serialization.
std::ostream& operator<<(std::ostream& out, const Rotation& rotation);


/// A position in 3-dimensional geographical Cartesian space, i.e., in the world
/// frame, consisting of three components x, y, and z.
///
/// Instantiated templates for the following kinds of T's are provided:
///
/// - double
/// - drake::AutoDiffXd
/// - drake::symbolic::Expression
///
/// They are already available to link against in the containing library.
template <typename T>
class GeoPositionT {
 public:
  DRAKE_DEFAULT_COPY_AND_MOVE_AND_ASSIGN(GeoPositionT)

  /// Default constructor, initializing all components to zero.
  GeoPositionT() : xyz_(T(0.), T(0.), T(0.)) {}

  /// Fully parameterized constructor.
  GeoPositionT(const T& x, const T& y, const T& z) : xyz_(x, y, z) {}

  /// Constructs a GeoPositionT from a 3-vector @p xyz of the form `[x, y, z]`.
  static GeoPositionT<T> FromXyz(const drake::Vector3<T>& xyz) {
    return GeoPositionT<T>(xyz);
  }

  /// Returns all components as 3-vector `[x, y, z]`.
  const drake::Vector3<T>& xyz() const { return xyz_; }
  /// Sets all components from 3-vector `[x, y, z]`.
  void set_xyz(const drake::Vector3<T>& xyz) { xyz_ = xyz; }

  /// @name Getters and Setters
  //@{
  /// Gets `x` value.
  T x() const { return xyz_.x(); }
  /// Sets `x` value.
  void set_x(const T& x) { xyz_.x() = x; }
  /// Gets `y` value.
  T y() const { return xyz_.y(); }
  /// Sets `y` value.
  void set_y(const T& y) { xyz_.y() = y; }
  /// Gets `z` value.
  T z() const { return xyz_.z(); }
  /// Sets `z` value.
  void set_z(const T& z) { xyz_.z() = z; }
  //@}

  /// Returns L^2 norm of 3-vector.
  T length() const { return xyz_.norm(); }

  /// Constructs a GeoPositionT<double> from other types, producing a clone if
  /// already double.
  GeoPositionT<double> MakeDouble() const {
    return {drake::ExtractDoubleOrThrow(xyz_.x()),
            drake::ExtractDoubleOrThrow(xyz_.y()),
            drake::ExtractDoubleOrThrow(xyz_.z())};
  }

 private:
  explicit GeoPositionT(const drake::Vector3<T>& xyz) : xyz_(xyz) {}

  drake::Vector3<T> xyz_;
};

// Alias for the double scalar type.
using GeoPosition = GeoPositionT<double>;

/// Streams a string representation of @p geo_position into @p out. Returns
/// @p out. This method is provided for the purposes of debugging or
/// text-logging. It is not intended for serialization.
std::ostream& operator<<(std::ostream& out, const GeoPosition& geo_position);

/// GeoPosition overload for the equality operator.
template <typename T>
auto operator==(const GeoPositionT<T>& lhs, const GeoPositionT<T>& rhs) {
  return (lhs.xyz() == rhs.xyz());
}

/// GeoPosition overload for the inequality operator.
template <typename T>
auto operator!=(const GeoPositionT<T>& lhs, const GeoPositionT<T>& rhs) {
  return !(lhs.xyz() == rhs.xyz());
}

/// GeoPosition overload for the plus operator.
template <typename T>
GeoPositionT<T> operator+(const GeoPositionT<T>& lhs,
                          const GeoPositionT<T>& rhs) {
  drake::Vector3<T> result = lhs.xyz();
  result += rhs.xyz();
  return GeoPositionT<T>::FromXyz(result);
}

/// GeoPosition overload for the minus operator.
template <typename T>
GeoPositionT<T> operator-(const GeoPositionT<T>& lhs,
                          const GeoPositionT<T>& rhs) {
  drake::Vector3<T> result = lhs.xyz();
  result -= rhs.xyz();
  return GeoPositionT<T>::FromXyz(result);
}

/// GeoPosition overload for the multiplication by scalar operator.
template <typename T>
GeoPositionT<T> operator*(double lhs,
                          const GeoPositionT<T>& rhs) {
  drake::Vector3<T> result = rhs.xyz();
  result *= lhs;
  return GeoPositionT<T>::FromXyz(result);
}

/// GeoPosition overload for the multiplication by scalar operator.
template <typename T>
GeoPositionT<T> operator*(const GeoPositionT<T>& lhs,
                          double rhs) {
  drake::Vector3<T> result = lhs.xyz();
  result *= rhs;
  return GeoPositionT<T>::FromXyz(result);
}

/// A 3-dimensional position in a `Lane`-frame, consisting of three components:
///
/// * s is longitudinal position, as arc-length along a Lane's reference line.
/// * r is lateral position, perpendicular to the reference line at s.
/// * h is height above the road surface.
///
/// Instantiated templates for the following kinds of T's are provided:
///
/// - double
/// - drake::AutoDiffXd
/// - drake::symbolic::Expression
///
/// They are already available to link against in the containing library.
template <typename T>
class LanePositionT {
 public:
  DRAKE_DEFAULT_COPY_AND_MOVE_AND_ASSIGN(LanePositionT)

  /// Default constructor, initializing all components to zero.
  LanePositionT() : srh_(T(0.), T(0.), T(0.)) {}

  /// Fully parameterized constructor.
  LanePositionT(const T& s, const T& r, const T& h) : srh_(s, r, h) {}

  /// Constructs a LanePosition from a 3-vector @p srh of the form `[s, r, h]`.
  static LanePositionT<T> FromSrh(const drake::Vector3<T>& srh) {
    return LanePositionT<T>(srh);
  }

  /// Returns all components as 3-vector `[s, r, h]`.
  const drake::Vector3<T>& srh() const { return srh_; }
  /// Sets all components from 3-vector `[s, r, h]`.
  void set_srh(const drake::Vector3<T>& srh) { srh_ = srh; }

  /// @name Getters and Setters
  //@{
  /// Gets `s` value.
  T s() const { return srh_.x(); }
  /// Sets `s` value.
  void set_s(const T& s) { srh_.x() = s; }
  /// Gets `r` value.
  T r() const { return srh_.y(); }
  /// Sets `r` value.
  void set_r(const T& r) { srh_.y() = r; }
  /// Gets `h` value.
  T h() const { return srh_.z(); }
  /// Sets `h` value.
  void set_h(const T& h) { srh_.z() = h; }
  //@}

  /// Constructs a LanePositionT<double> from other types, producing a clone if
  /// already double.
  LanePositionT<double> MakeDouble() const {
    return {drake::ExtractDoubleOrThrow(srh_.x()),
            drake::ExtractDoubleOrThrow(srh_.y()),
            drake::ExtractDoubleOrThrow(srh_.z())};
  }

 private:
  explicit LanePositionT(const drake::Vector3<T>& srh) : srh_(srh) {}

  drake::Vector3<T> srh_;
};

// Alias for the double scalar type.
using LanePosition = LanePositionT<double>;

/// Streams a string representation of @p lane_position into @p out. Returns
/// @p out. This method is provided for the purposes of debugging or
/// text-logging. It is not intended for serialization.
std::ostream& operator<<(std::ostream& out, const LanePosition& lane_position);

/// Isometric velocity vector in a `Lane`-frame.
///
/// sigma_v, rho_v, and eta_v are the components of velocity in a
/// (sigma, rho, eta) coordinate system.  (sigma, rho, eta) have the same
/// orientation as the (s, r, h) at any given point in space, however they
/// form an isometric system with a Cartesian distance metric.  Hence,
/// IsoLaneVelocity represents a "real" physical velocity vector (albeit
/// with an orientation relative to the road surface).
struct IsoLaneVelocity {
  /// Default constructor.
  IsoLaneVelocity() = default;

  /// Fully parameterized constructor.
  IsoLaneVelocity(double _sigma_v, double _rho_v, double _eta_v)
      : sigma_v(_sigma_v), rho_v(_rho_v), eta_v(_eta_v) {}

  double sigma_v{};
  double rho_v{};
  double eta_v{};
};


/// A position in the road network, consisting of a pointer to a specific
/// Lane and a `Lane`-frame position in that Lane.
struct RoadPosition {
  /// Default constructor.
  RoadPosition() = default;

  /// Fully parameterized constructor.
  RoadPosition(const Lane* _lane, const LanePosition& _pos)
      : lane(_lane), pos(_pos) {}

  const Lane* lane{};
  LanePosition pos;
};


/// Included in the return result of RoadGeometry::FindRoadPositions().
struct RoadPositionResult {
  /// The candidate RoadPosition.
  RoadPosition road_position;
  /// The position that exactly corresponds to `road_position`.
  GeoPosition nearest_position;
  /// The distance between `nearest_position` and the `geo_position` supplied
  /// to RoadGeometry::FindRoadPositions().
  double distance{};
};


/// Bounds in the lateral dimension (r component) of a `Lane`-frame, consisting
/// of a pair of minimum and maximum r value.  The bounds must straddle r = 0,
/// i.e., the minimum must be <= 0 and the maximum must be >= 0.
class RBounds {
 public:
  DRAKE_DEFAULT_COPY_AND_MOVE_AND_ASSIGN(RBounds)

  /// Default constructor.
  RBounds() = default;

  /// Fully parameterized constructor.
  /// @throws std::runtime_error When @p min is greater than 0.
  /// @throws std::runtime_error When @p max is smaller than 0.
  RBounds(double min, double max) : min_(min), max_(max) {
    DRAKE_THROW_UNLESS(min <= 0.);
    DRAKE_THROW_UNLESS(max >= 0.);
  }

  /// @name Getters and Setters
  //@{
  /// Gets minimum bound.
  double min() const { return min_; }
  /// Sets minimum bound.
  /// @throws std::runtime_error When @p min is greater than 0.
  void set_min(double min) {
    DRAKE_THROW_UNLESS(min <= 0.);
    min_ = min;
  }
  /// Gets maximum bound.
  double max() const { return max_; }
  /// Sets maximum bound.
  /// @throws std::runtime_error When @p max is smaller than 0.
  void set_max(double max) {
    DRAKE_THROW_UNLESS(max >= 0.);
    max_ = max;
  }
  //@}

 private:
  double min_{};
  double max_{};
};


/// Bounds in the elevation dimension (`h` component) of a `Lane`-frame,
/// consisting of a pair of minimum and maximum `h` value.  The bounds
/// must straddle `h = 0`, i.e., the minimum must be `<= 0` and the
/// maximum must be `>= 0`.
class HBounds {
 public:
  DRAKE_DEFAULT_COPY_AND_MOVE_AND_ASSIGN(HBounds)

  /// Default constructor.
  HBounds() = default;

  /// Fully parameterized constructor.
  /// @throws std::runtime_error When @p min is greater than 0.
  /// @throws std::runtime_error When @p max is smaller than 0.
  HBounds(double min, double max) : min_(min), max_(max) {
    DRAKE_THROW_UNLESS(min <= 0.);
    DRAKE_THROW_UNLESS(max >= 0.);
  }

  /// @name Getters and Setters
  //@{
  /// Gets minimum bound.
  double min() const { return min_; }
  /// Sets minimum bound.
  /// @throws std::runtime_error When @p min is greater than 0.
  void set_min(double min) {
    DRAKE_THROW_UNLESS(min <= 0.);
    min_ = min;
  }
  /// Gets maximum bound.
  double max() const { return max_; }
  /// Sets maximum bound.
  /// @throws std::runtime_error When @p max is smaller than 0.
  void set_max(double max) {
    DRAKE_THROW_UNLESS(max >= 0.);
    max_ = max;
  }
  //@}

 private:
  double min_{};
  double max_{};
};

}  // namespace api
}  // namespace maliput
