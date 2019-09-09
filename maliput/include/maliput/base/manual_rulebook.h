#pragma once

#include <memory>
#include <vector>

#include "drake/common/drake_copyable.h"
#include "maliput/api/regions.h"
#include "maliput/api/rules/direction_usage_rule.h"
#include "maliput/api/rules/discrete_value_rule.h"
#include "maliput/api/rules/range_value_rule.h"
#include "maliput/api/rules/right_of_way_rule.h"
#include "maliput/api/rules/road_rulebook.h"
#include "maliput/api/rules/speed_limit_rule.h"

namespace maliput {

/// ManualRulebook is a simple concrete implementation of the
/// api::rules::RoadRulebook abstract interface.
class ManualRulebook : public api::rules::RoadRulebook {
 public:
  DRAKE_NO_COPY_NO_MOVE_NO_ASSIGN(ManualRulebook);

  /// Constructs an empty ManualRulebook (i.e., containing no rules).
  ManualRulebook();

  ~ManualRulebook() override;

  /// Removes all rules.
  void RemoveAll();

  /// Adds a new RightOfWayRule.
  ///
  /// @throws maliput::common::assertion_error if a rule with the same ID
  /// already exists in the ManualRulebook.
  void AddRule(const api::rules::RightOfWayRule& rule);

  /// Removes the RightOfWayRule with the specified `id`.
  ///
  /// @throws maliput::common::assertion_error if no such rule exists.
  void RemoveRule(const api::rules::RightOfWayRule::Id& id);

  /// Adds a new SpeedLimitRule.
  ///
  /// @throws maliput::common::assertion_error if a rule with the same ID
  /// already exists in the ManualRulebook.
  void AddRule(const api::rules::SpeedLimitRule& rule);

  /// Removes the SpeedLimitRule with the specified `id`.
  ///
  /// @throws maliput::common::assertion_error if no such rule exists.
  void RemoveRule(const api::rules::SpeedLimitRule::Id& id);

  /// Adds a new DirectionUsageRule.
  ///
  /// @throws maliput::common::assertion_error if a rule with the same ID
  /// already exists in the ManualRulebook.
  void AddRule(const api::rules::DirectionUsageRule& rule);

  /// Removes the DirectionUsageRule with the specified `id`.
  ///
  /// @throws maliput::common::assertion_error if no such rule exists.
  void RemoveRule(const api::rules::DirectionUsageRule::Id& id);

  /// Adds a new DiscreteValueRule.
  ///
  /// @throws maliput::common::assertion_error if a rule with the same ID
  ///         already exists in the ManualRulebook.
  void AddRule(const api::rules::DiscreteValueRule& rule);

  /// Adds a new RangeValueRule.
  ///
  /// @throws maliput::common::assertion_error if a rule with the same ID
  ///         already exists in the ManualRulebook.
  void AddRule(const api::rules::RangeValueRule& rule);

  /// Removes the Rule with the specified `id`.
  ///
  /// @throws maliput::common::assertion_error if no such rule exists.
  void RemoveRule(const api::rules::Rule::Id& id);

 private:
  api::rules::RoadRulebook::QueryResults DoFindRules(const std::vector<api::LaneSRange>& ranges,
                                                     double tolerance) const override;
  api::rules::RoadRulebook::QueryResults DoRules() const override;
  api::rules::RightOfWayRule DoGetRule(const api::rules::RightOfWayRule::Id& id) const override;
  api::rules::SpeedLimitRule DoGetRule(const api::rules::SpeedLimitRule::Id& id) const override;
  api::rules::DirectionUsageRule DoGetRule(const api::rules::DirectionUsageRule::Id& id) const override;
  api::rules::DiscreteValueRule DoGetDiscreteValueRule(const api::rules::Rule::Id& id) const override;
  api::rules::RangeValueRule DoGetRangeValueRule(const api::rules::Rule::Id& id) const override;

  class Impl;
  std::unique_ptr<Impl> impl_;
};

}  // namespace maliput