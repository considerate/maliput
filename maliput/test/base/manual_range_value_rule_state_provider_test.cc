#include "maliput/base/manual_range_value_rule_state_provider.h"

#include <stdexcept>

#include <gtest/gtest.h>

#include "maliput/api/rules/range_value_rule.h"
#include "maliput/api/rules/rule.h"
#include "maliput/common/assertion_error.h"
#include "maliput/test_utilities/mock.h"
#include "maliput/test_utilities/rules_compare.h"
#include "maliput/test_utilities/rules_test_utilities.h"

namespace maliput {
namespace test {
namespace {

using api::rules::MakeRange;
using api::rules::RangeValueRule;
using api::rules::RangeValueRuleStateProvider;
using api::rules::RoadRulebook;
using api::rules::Rule;

class ManualRangeValueRuleStateProviderTest : public ::testing::Test {
 protected:
  const Rule::Id kRuleId{"rvrt/rvr_id"};
  const Rule::Id kUnknownRuleId{"rvrt/unknown_id"};
  const RangeValueRule::Range kRangeA{api::test::CreateRange()};
  const RangeValueRule::Range kInvalidRange{
      MakeRange(Rule::State::kStrict, api::test::CreateEmptyRelatedRules(), "invalid", 456. /* min */, 789. /* max */)};
  const double kDurationUntil{10.};

  void SetUp() override {
    const maliput::api::test::RoadRulebookBuildFlags kRulebookBuildFlags{
        false /* add_right_of_way */, {} /* right_of_way_build_flags */,   false /* add_direction_usage */,
        false /* add_speed_limit */,  false /* add_discrete_value_rule */, true /* add_range_value_rule */};
    road_rulebook_ = api::test::CreateRoadRulebook(kRulebookBuildFlags);
  }

  std::unique_ptr<RoadRulebook> road_rulebook_;
};

TEST_F(ManualRangeValueRuleStateProviderTest, ConstructorConstraints) {
  EXPECT_THROW(ManualRangeValueRuleStateProvider(nullptr), maliput::common::assertion_error);
  EXPECT_NO_THROW(ManualRangeValueRuleStateProvider(road_rulebook_.get()));
}

TEST_F(ManualRangeValueRuleStateProviderTest, SetStateTest) {
  ManualRangeValueRuleStateProvider dut(road_rulebook_.get());

  // Tries to set the state to an unknown Rule::Id in `rulebook_`.
  EXPECT_THROW(dut.SetState(kUnknownRuleId, kRangeA, {}, {}), std::out_of_range);
  // Tries to set an invalid state to the rule.
  EXPECT_THROW(dut.SetState(kRuleId, kInvalidRange, {}, {}), maliput::common::assertion_error);
  // Tries to set an invalid next state to the rule.
  EXPECT_THROW(dut.SetState(kRuleId, kRangeA, {kInvalidRange}, {}), maliput::common::assertion_error);
  // Tries to set a valid next state with a negative duration.
  EXPECT_THROW(dut.SetState(kRuleId, kRangeA, {kRangeA}, {-kDurationUntil}), maliput::common::assertion_error);
  // Tries to set a nullopt next state with duration.
  EXPECT_THROW(dut.SetState(kRuleId, kRangeA, {}, {kDurationUntil}), maliput::common::assertion_error);

  // Sets a valid state, next state and duration until.
  EXPECT_NO_THROW(dut.SetState(kRuleId, kRangeA, {kRangeA}, {kDurationUntil}));
  const drake::optional<RangeValueRuleStateProvider::StateResult> result = dut.GetState(kRuleId);
  EXPECT_TRUE(result.has_value());
  EXPECT_TRUE(MALIPUT_IS_EQUAL(result->range_state, kRangeA));
  EXPECT_TRUE(result->next.has_value());
  EXPECT_TRUE(MALIPUT_IS_EQUAL(result->next->range_state, kRangeA));
  EXPECT_TRUE(result->next->duration_until.has_value());
  EXPECT_EQ(result->next->duration_until.value(), kDurationUntil);
}

}  // namespace
}  // namespace test
}  // namespace maliput