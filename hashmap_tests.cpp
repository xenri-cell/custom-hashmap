#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <random>
#include <map>
#include "hashmap.h"

using namespace std;
using namespace testing;

namespace {

class Random {
 private:
  static mt19937 rng;

 public:
  static void seed(int s) {
    Random::rng.seed(s);
  }

  /**
   * Generate a random integer in the range 0 (inclusive) to `max` (exclusive)
   */
  static int randInt(int max) {
    // Even though mt19937 is standardized, STL distributions aren't!
    // So, unfortunately, even though this is biased, we're forced to
    // do something like this. Technically uint32_fast_t isn't...
    // totally consistent across platforms? But within reason it works.
    return rng() % (max + 1);
  }
};

std::mt19937 Random::rng;

class HashMapCoreTest : public Test  {
  protected:
    HashMap<int, string> hash_map;
    std::map<int, string> truth;

    void SetUp() override  {
      Random::seed(12345);
      hash_map = HashMap<int, string>(10);
      truth.clear();
    }

    void insertRandomElements(int count)  {
      for (int i = 0; i < count; ++i)  {
        int key = Random::randInt(5000);
        string value = "value_" + to_string(i) + "_" + to_string(key);

        if (truth.find(key) == truth.end())  {
          hash_map.insert(key, value);
          truth.insert({key, value});
        }
      }
      EXPECT_EQ(hash_map.size(), truth.size());
      for (const auto& pair : truth)  {
        EXPECT_TRUE(hash_map.contains(pair.first));
        EXPECT_EQ(hash_map.at(pair.first), pair.second);
      }
    }
};

//TEST(FakeTest, PleaseDeleteOnceYouWriteSome) {
  // If no tests exist, GoogleTest fails to compile with a fairly confusing
  // message. We have this empty test that does nothing to allow an empty
  // autograder submission to compile and regularly fail. Once you write your
  // own tests for the project, you can delete this one.
//  EXPECT_THAT(1, Eq(1));
//}

TEST_F(HashMapCoreTest, InitiallyEmpty) {
  EXPECT_TRUE(hash_map.empty());
  EXPECT_EQ(hash_map.size(), 0);
}

TEST_F(HashMapCoreTest, InsertSingleElement) {
  hash_map.insert(10, "ten");
  EXPECT_FALSE(hash_map.empty());
  EXPECT_EQ(hash_map.size(), 1);
  EXPECT_TRUE(hash_map.contains(10));
  EXPECT_FALSE(hash_map.contains(20));
}

TEST_F(HashMapCoreTest, CtorInitialSizeIsZero) {
  HashMap<int, string> default_map;
  EXPECT_EQ(default_map.size(), 0);
  EXPECT_TRUE(default_map.empty());

  HashMap<int, string> sized_map(100);
  EXPECT_EQ(sized_map.size(), 0);
  EXPECT_TRUE(sized_map.empty());
}

TEST_F(HashMapCoreTest, SizeAndEmptyAreConsistent) {
  EXPECT_TRUE(hash_map.empty());
  EXPECT_EQ(hash_map.size(), 0);

  hash_map.insert(1, "A");
  EXPECT_FALSE(hash_map.empty());
  EXPECT_EQ(hash_map.size(), 1);

  hash_map.insert(2, "B");
  EXPECT_EQ(hash_map.size(), 2);
}

TEST_F(HashMapCoreTest, InsertDoesNotOverwriteValue) {
  hash_map.insert(1, "you_up?");
  hash_map.insert(1, "nah"); 
  EXPECT_EQ(hash_map.size(), 1);
  EXPECT_EQ(hash_map.at(1), "you_up?");
}

TEST_F(HashMapCoreTest, ThrowsOutOfRange) {
  hash_map.insert(1, "a");
  EXPECT_THROW(hash_map.at(999), out_of_range);
}

TEST_F(HashMapCoreTest, CollisionInsertAndAt) {

  hash_map.insert(1, "head");
  hash_map.insert(11, "middle");
  hash_map.insert(21, "tail");

  EXPECT_EQ(hash_map.size(), 3);
  EXPECT_TRUE(hash_map.contains(1));
  EXPECT_TRUE(hash_map.contains(11));
  EXPECT_TRUE(hash_map.contains(21));
  
  EXPECT_EQ(hash_map.at(1), "head");
  EXPECT_EQ(hash_map.at(11), "middle");
  EXPECT_EQ(hash_map.at(21), "tail");
}

TEST_F(HashMapCoreTest, EraseMidCollisionNode) {

  hash_map.insert(1, "head");
  hash_map.insert(11, "middle");
  hash_map.insert(21, "tail");
  
  hash_map.erase(11);

  EXPECT_EQ(hash_map.size(), 2);

  EXPECT_TRUE(hash_map.contains(1));
  EXPECT_EQ(hash_map.at(1), "head");
  EXPECT_TRUE(hash_map.contains(21));
  EXPECT_EQ(hash_map.at(21), "tail");

  EXPECT_FALSE(hash_map.contains(11));
  EXPECT_THROW(hash_map.at(11), out_of_range);

  hash_map.erase(1);
  EXPECT_EQ(hash_map.size(), 1);
  EXPECT_TRUE(hash_map.contains(21));
  EXPECT_EQ(hash_map.at(21), "tail");
}

TEST_F(HashMapCoreTest, AssignmentSelfAssignmentProtects) {
  insertRandomElements(10);
  size_t original_size = hash_map.size();
  
  hash_map = hash_map; 
  
  EXPECT_EQ(hash_map.size(), original_size);
  for (const auto& pair : truth) {
    EXPECT_TRUE(hash_map.contains(pair.first));
    EXPECT_EQ(hash_map.at(pair.first), pair.second);
  }
}

TEST_F(HashMapCoreTest, AssignmentOperatorDeepCopyAndCleanup) {
  HashMap<int, string> map_source;
  map_source.insert(100, "source_100");
  map_source.insert(200, "source_200");
  
  hash_map.insert(1, "old");
  hash_map.insert(2, "old");
  
  hash_map = map_source;

  EXPECT_EQ(hash_map.size(), 2);
  EXPECT_EQ(hash_map.at(100), "source_100");
  EXPECT_FALSE(hash_map.contains(1));

  map_source.at(100) = "CHANGED";
  EXPECT_EQ(hash_map.at(100), "source_100");
}

TEST_F(HashMapCoreTest, ClearResetsSizeCorrectly) {
  hash_map.insert(10, "ten");
  hash_map.insert(20, "twenty");
  EXPECT_EQ(hash_map.size(), 2);
  
  hash_map.clear();

  EXPECT_EQ(hash_map.size(), 0); 
  EXPECT_TRUE(hash_map.empty());

  hash_map.insert(500, "new_data");
  EXPECT_EQ(hash_map.size(), 1);
  EXPECT_EQ(hash_map.at(500), "new_data");
  EXPECT_TRUE(hash_map.contains(500));
}

TEST_F(HashMapCoreTest, CopyConstructorIsDeepAndSized) {
  hash_map.insert(10, "The_OG");
  hash_map.insert(20, "Newer_model");
  
  HashMap<int, string> map_copy = hash_map;

  EXPECT_EQ(map_copy.size(), 2);
  EXPECT_FALSE(map_copy.empty());
  EXPECT_EQ(map_copy.at(10), "The_OG");

  hash_map.erase(10);
  EXPECT_TRUE(map_copy.contains(10));
}

TEST_F(HashMapCoreTest, MissingKeyThrowsOnErase) {
  hash_map.insert(1, "a");
  EXPECT_THROW(hash_map.erase(999), out_of_range);
  EXPECT_EQ(hash_map.size(), 1);
}
TEST_F(HashMapCoreTest, EraseOnEmptyMapThrows) {
  EXPECT_THROW(hash_map.erase(1), out_of_range);
  EXPECT_EQ(hash_map.size(), 0); 
}


}  // namespace
