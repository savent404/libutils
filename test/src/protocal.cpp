#include <gtest/gtest.h>
#include <protocal_utils.h>
#include <vector>

extern "C" int match_all(fifo_t *ptr) { return fifo_len(ptr); }
extern "C" int match_deny(fifo_t *ptr) { return 0; }

TEST(protocal, protocal_find_1) {

  FIFO_DEFINE(c, 128, char);
  fifo_t *ptr = FIFO_PTR(c);
  char buf[16], buf_org[16];

  for (int i = 0; i < 16; i++) {
    buf_org[i] = std::rand() & 0xFF;
  }
  fifo_push(ptr, buf_org, 16);

  ASSERT_EQ(protocal_find_frame(ptr, match_all, buf, 16), 16);
  ASSERT_EQ(fifo_len(ptr), 0);
  for (int i = 0; i < 16; i++) {
    ASSERT_EQ(buf[i], buf_org[i]);
  }
}

TEST(protocal, protocal_find_2) {

  FIFO_DEFINE(c, 128, char);
  fifo_t *ptr = FIFO_PTR(c);
  char buf[16], buf_org[16];

  for (int i = 0; i < 16; i++) {
    buf_org[i] = std::rand() & 0xFF;
  }
  fifo_push(ptr, buf_org, 16);

  for (int i = 0; i < 16; i++) {
    char t;
    ASSERT_EQ(protocal_find_frame(ptr, match_deny, buf, 16), 0);
    ASSERT_EQ(fifo_len(ptr), 15 - i);
    if (i < 15) {
        fifo_peek(ptr, 0, &t);
        ASSERT_EQ(t, buf_org[i + 1]);
    }
  }
}