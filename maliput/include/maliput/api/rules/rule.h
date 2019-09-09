#pragma once

#include <algorithm>
#include <string>
#include <vector>

#include "drake/common/drake_copyable.h"

#include "maliput/api/regions.h"
#include "maliput/api/type_specific_identifier.h"
#include "maliput/common/maliput_throw.h"

namespace maliput {
namespace api {
namespace rules {

/// Describes a generic rule type.
///
/// A Rule may have multiple states that affect agent behavior while it is
/// driving through the rule's zone. The possible states of a Rule must be
/// semantically coherent. The current state of a Rule is given by a
/// RuleStateProvider. States can be:
///
/// - range based (@see RangeValueRule).
/// - discrete (@see DiscreteValueRule).
class Rule {
 public:
  /// Used to specialize TypeSpecificIdentifier and define a unique TypeId.
  class Type;

  /// Alias for the Rule's unique ID across all Rule types. It is a property of
  /// each Rule's instance, and should be unique across all Rule instances.
  /// To achieve this, backend implementations are encouraged to use
  /// "[rule_type]/[rule_id]" as the string value of a Rule's ID.
  using Id = TypeSpecificIdentifier<class Rule>;

  /// Alias for the Rule's type. Several Rule instances could share the same
  /// TypeId, assuming they really are the same type. Example types include
  /// "right of way rule", "direction usage rule", "vehicle usage rule", etc.
  using TypeId = TypeSpecificIdentifier<class Type>;

  /// Alias of a map holding groups of related rules. The name of each group is
  /// specified by the key, and the semantics vary based on the specific rule
  /// type. The group name must not be an empty string. Each vector of
  /// Rule::Id must not be empty and must contain unique Rule::Ids.
  using RelatedRules = std::map<std::string, std::vector<Id>>;

  /// Defines a base state for a Rule.
  struct State {
    /// Defines common Rule severity levels. Specific rule types can choose to use
    /// these, or define their own custom levels.
    ///@{

    /// Rule must always be obeyed.
    static constexpr int kStrict{0};

    /// Rule should be obeyed on a best-effort basis.
    static constexpr int kBestEffort{1};

    ///@}

    bool operator==(const State& other) const;
    bool operator!=(const State& other) const { return !(*this == other); }

    /// Severity of the Rule::State. A non-negative quantity that specifies the
    /// level of enforcement. The smaller it is, the more strictly the rule is
    /// enforced. Each rule type can define its own set of severity level
    /// semantics. See kStrict and kBestEffort for two commonly used severity
    /// levels.
    int severity{};
    RelatedRules related_rules;
  };

  DRAKE_DEFAULT_COPY_AND_MOVE_AND_ASSIGN(Rule);

  /// Constructs a Rule.
  ///
  /// @param id The Rule ID.
  /// @param type_id The Rule Type ID.
  /// @param zone LaneSRoute to which this rule applies.
  ///
  /// @throws maliput::common::assertion_error When any Rule::Id within
  ///         `related_rules` is duplicated.
  Rule(const Id& id, const TypeId& type_id, const LaneSRoute& zone) : id_(id), type_id_(type_id), zone_(zone) {}

  virtual ~Rule() = default;

  const Id& id() const { return id_; }

  const TypeId& type_id() const { return type_id_; }

  const LaneSRoute& zone() const { return zone_; }

 protected:
  // Validates that `related_rules` requirements are met.
  // @see RelatedRules alias definition for full type requirements.
  // @throws maliput::assertion_error When any of the requirements in
  //         `RelatedRules` are not met.
  void ValidateRelatedRules(const RelatedRules& related_rules) const;

  // Validates that `severity` is a non-negative quantity.
  // @throws maliput::assertion_error When `severity` is negative.
  void ValidateSeverity(int severity) const { MALIPUT_THROW_UNLESS(severity >= 0); }

 private:
  Id id_;
  TypeId type_id_;
  LaneSRoute zone_;
};

}  // namespace rules
}  // namespace api
}  // namespace maliput