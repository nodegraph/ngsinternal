#include <base/objectmodel/componentids.h>
#include <base/utils/stringutil.h>

#include <iostream>

namespace ngs {

const char* get_component_did_name(ComponentDID did) {
  #undef COMPONENT_ENTRY1
  #undef COMPONENT_ENTRY2
  #define COMPONENT_ENTRY1(NAME) case ComponentDID::k##NAME: return #NAME;
  #define COMPONENT_ENTRY2(NAME, VALUE) COMPONENT_ENTRY1(NAME)
  switch (did) {
    COMPONENT_ENTRIES()
  }
  return "unknown component did type";
}

void remove_trailing(std::vector<std::string>& splits, const std::vector<std::string>& trailing) {
  // If the number of trailing elements is greater than the number of splits then we can never match.
  if (trailing.size()>splits.size()) {
    return;
  }
  // Otherwise see if the trailing elements match.
  for (size_t i = 0; i < trailing.size(); ++i) {
    if (splits[splits.size() - trailing.size() + i] != trailing[i]) {
      return;
    }
  }
  // Pop off the matching trailing splits.
  for (size_t i = 0; i < trailing.size(); ++i) {
    splits.pop_back();
  }
}

std::string get_user_friendly_name(const std::string& name) {
  // Split it.
  std::vector<std::string> splits = split_on_capitals(name);

  // Removing some trailing words.:
  remove_trailing(splits, {"Node", "Entity"});
  remove_trailing(splits, {"Entity"});
  remove_trailing(splits, {"Node", "Compute"});
  remove_trailing(splits, {"Compute"});
  remove_trailing(splits, {"Node"});

  // Concatenate some compound words that got split up.
  std::vector<std::string> splits2;
  for (size_t i = 0; i < splits.size(); ++i) {
    if (i == 0) {
      splits2.push_back(splits[i]);
    } else if (splits[i - 1] == "I" && splits[i] == "Frame") {
      splits2.back() = "IFrame";
    } else {
      splits2.push_back(splits[i]);
    }
  }

  // Finally concatenate the splits with a space.
  return concatenate(splits2, " ");
}

std::string get_component_user_did_name(ComponentDID did) {
  std::string name = get_component_did_name(did);
  return get_user_friendly_name(name);
}


}
