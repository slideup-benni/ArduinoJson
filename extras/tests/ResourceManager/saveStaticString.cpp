// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2025, Benoit BLANCHON
// MIT License

#include <ArduinoJson/Memory/ResourceManager.hpp>
#include <ArduinoJson/Strings/StringAdapters.hpp>
#include <catch.hpp>

#include "Allocators.hpp"

using namespace ArduinoJson::detail;

TEST_CASE("ResourceManager::saveStaticString() deduplicates strings") {
  SpyingAllocator spy;
  ResourceManager resources(&spy);

  auto str1 = "hello";
  auto str2 = "world";

  auto id1 = resources.saveStaticString(str1);
  auto id2 = resources.saveStaticString(str2);
  REQUIRE(id1 != id2);

  auto id3 = resources.saveStaticString(str1);
  REQUIRE(id1 == id3);

  resources.shrinkToFit();
  REQUIRE(spy.log() ==
          AllocatorLog{
              Allocate(sizeofStaticStringPool()),
              Reallocate(sizeofStaticStringPool(), sizeofStaticStringPool(2)),
          });
  REQUIRE(resources.overflowed() == false);
}

TEST_CASE("ResourceManager::saveStaticString() when allocation fails") {
  SpyingAllocator spy(FailingAllocator::instance());
  ResourceManager resources(&spy);

  auto slotId = resources.saveStaticString("hello");

  REQUIRE(slotId == NULL_SLOT);
  REQUIRE(resources.overflowed() == true);
  REQUIRE(spy.log() == AllocatorLog{
                           AllocateFail(sizeofStaticStringPool()),
                       });
}
