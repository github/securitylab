unsigned int test000(int x) {
  if (x < 0) {
    x = -x;
  }
  return x;
}

struct MyStruct {
  int myfield;
};

void test001(MyStruct *s) {
  if (s->myfield < 0) {
    s->myfield = -s->myfield;
  }
}

void test002(MyStruct *s, MyStruct *t) {
  if (s->myfield < 0) {
    s->myfield = -t->myfield;
  }
}

unsigned int test003(int x) {
  if (0 > x) {
    x = -x;
  }
  return x;
}

unsigned int test004(int x) {
  if (x > 0) {
    // Do nothing.
  } else {
    x = -x;
  }
  return x;
}

unsigned int test005(int x) {
  if (0 <= x) {
    // Do nothing.
  } else {
    x = -x;
  }
  return x;
}

unsigned int test006(int x) {
  if (!(0 > x)) {
    // Do nothing.
  } else {
    x = -x;
  }
  return x;
}

unsigned int test007(int x) {
  if (!(x > 0)) {
    x = -x;
  }
  return x;
}

unsigned int test008(int x) {
  if (!(0 <= x)) {
    x = -x;
  }
  return x;
}

unsigned int test009(int x) {
  if (!!(x < 0)) {
    x = -x;
  }
  return x;
}

unsigned int test010(int x) {
  bool b = x < 0;
  if (b) {
    x = -x;
  }
  return x;
}

unsigned int test011(int x) {
  bool b = !(0 <= x);
  if (b) {
    x = -x;
  }
  return x;
}

unsigned int test012(int x) {
  bool b = x < 0;
  bool c = !b;
  if (!c) {
    x = -x;
  }
  return x;
}
