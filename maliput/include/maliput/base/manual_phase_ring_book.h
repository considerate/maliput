#pragma once

#include <memory>
#include <optional>
#include <vector>

#include "maliput/api/rules/phase_ring.h"
#include "maliput/api/rules/phase_ring_book.h"
#include "maliput/api/rules/right_of_way_rule.h"
#include "maliput/api/rules/rule.h"
#include "maliput/common/maliput_copyable.h"

namespace maliput {

/// A simple concrete implementation of the api::rules::PhaseRingBook abstract
/// interface that enables manual addition and removal of api::rules::PhaseRing
/// instances.
class ManualPhaseRingBook : public api::rules::PhaseRingBook {
 public:
  MALIPUT_NO_COPY_NO_MOVE_NO_ASSIGN(ManualPhaseRingBook)

  ManualPhaseRingBook();

  ~ManualPhaseRingBook() override;

  /// Adds @p ring to this ManualPhaseRingBook.
  ///
  /// @throws std::exception if an api::rules::PhaseRing with the same ID
  /// already exists, or if @p ring contains a rule that already exists in a
  /// previously added api::rules::PhaseRing.
  void AddPhaseRing(const api::rules::PhaseRing& ring);

  /// Removes an api::rules::PhaseRing with an ID of @p ring_id from this
  /// ManualPhaseRingBook.
  ///
  /// @throws std::exception if the specified api::rules::PhaseRing does not
  /// exist.
  void RemovePhaseRing(const api::rules::PhaseRing::Id& ring_id);

 private:
  std::vector<api::rules::PhaseRing::Id> DoGetPhaseRings() const override;

  std::optional<api::rules::PhaseRing> DoGetPhaseRing(const api::rules::PhaseRing::Id& ring_id) const override;

  std::optional<api::rules::PhaseRing> DoFindPhaseRing(const api::rules::RightOfWayRule::Id& rule_id) const override;

  std::optional<api::rules::PhaseRing> DoFindPhaseRing(const api::rules::Rule::Id& rule_id) const override;

  class Impl;
  std::unique_ptr<Impl> impl_;
};

}  // namespace maliput
