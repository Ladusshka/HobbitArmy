#ifndef __PROGTEST__
#include <cassert>
#include <iomanip>
#include <cstdint>
#include <iostream>
#include <memory>
#include <limits>
#include <optional>
#include <algorithm>
#include <functional>
#include <bitset>
#include <list>
#include <array>
#include <vector>
#include <deque>
#include <unordered_set>
#include <unordered_map>
#include <stack>
#include <queue>
#include <random>
#include <type_traits>
#include <utility>


#include <time.h>

struct Hobbit {
  std::string name;
  int hp, off, def;

  friend bool operator == (const Hobbit&, const Hobbit&) = default;
};

std::ostream& operator << (std::ostream& out, const Hobbit& h) {
  return out
    << "Hobbit{\"" << h.name << "\", "
    << ".hp=" << h.hp << ", "
    << ".off=" << h.off << ", "
    << ".def=" << h.def << "}";
}

template < typename T >
std::ostream& operator << (std::ostream& out, const std::optional<T>& x) {
  if (!x) return out << "EMPTY_OPTIONAL";
  return out << "Optional{" << *x << "}";
}

#endif

struct HobbitNode;
struct InsertResult {
    HobbitNode* node;
    bool inserted;
};
struct EraseResult {
    HobbitNode * node;
    std::optional<Hobbit> erased;
};

struct  State{
    int off=0;
    int def=0;
    int hp=0;

    State(int Off, int Def, int HP):off(Off),def(Def),hp(HP) {}
    State()=default;
};

struct HobbitNode {
    Hobbit hobbit;

    HobbitNode *right;
    HobbitNode *left;
    int depth;
    HobbitNode* minNode;
    HobbitNode* maxNode;
    State pending_update;
    bool hasPending;
    int minimalHPSubtree;

    HobbitNode(const Hobbit & hob)
            : hobbit(hob),

              right(nullptr),
              left(nullptr),
              depth(0),
              minNode(this),
              maxNode(this),
              pending_update(),
              hasPending(false),
              minimalHPSubtree(hob.hp)
    {}


//    Hobbit hobbitPlus(Hobbit & a, Hobbit & b) {
//            Hobbit res;
//            res.name = std::move(a.name);
//            res.hp = a.hp + b.hp;
//            res.off = a.off + b.off;
//            res.def = a.def + b.def;
//
//            return res;
//    }

     void hobbitPlus(const Hobbit & b) {
        this->hobbit.hp += b.hp;
        this->hobbit.off += b.off;
        this->hobbit.def += b.def;
    }

    void add_pending(const State & b) {
        pending_update.hp  += b.hp;
        pending_update.off += b.off;
        pending_update.def += b.def;
        hasPending = true;

        minimalHPSubtree += b.hp;
    }

    void push() {
        if (!hasPending)
            return;

        this->hobbit.hp += pending_update.hp;
        this->hobbit.off += pending_update.off;
        this->hobbit.def += pending_update.def;

        if (left)
            left->add_pending(pending_update);
        if (right)
            right->add_pending(pending_update);

        pending_update = State();
        hasPending = false;
    }

    void recalc_minmax() {
        minNode = this;
        maxNode = this;

        if (left) {
            if (left->minNode->hobbit.name < minNode->hobbit.name)
                minNode = left->minNode;
            if (left->maxNode->hobbit.name > maxNode->hobbit.name)
                maxNode = left->maxNode;
        }
        if (right) {
            if (right->minNode->hobbit.name < minNode->hobbit.name)
                minNode = right->minNode;
            if (right->maxNode->hobbit.name > maxNode->hobbit.name)
                maxNode = right->maxNode;
        }

        minimalHPSubtree = hobbit.hp;
        if (left)
            minimalHPSubtree = std::min(minimalHPSubtree, left->minimalHPSubtree);
        if (right)
            minimalHPSubtree = std::min(minimalHPSubtree, right->minimalHPSubtree);
    }


    const HobbitNode* find_push(HobbitNode* v, const std::string& Name) {
        while (v) {
            v->push();

            if (Name == v->hobbit.name) {
                return v;
            }

            if (Name < v->hobbit.name) {
                v = v->left;
            } else {
                v = v->right;
            }
        }
        return nullptr;
    }

    long balance (const HobbitNode * v){
        if(!v){
            return 0;
        }

        int rightDepth = v->right ? v->right->depth : -1;
        int leftDepth = v->left ? v->left->depth : -1;
        return (long )(rightDepth - leftDepth);
    }

     static HobbitNode * rotateLeft(HobbitNode * v){
        if (!v || !v->right)
            return v;
        v->push();
        HobbitNode *  y = v->right;
        y->push();
        v->right = y->left;
        y->left = v;

        int rightDepthV = v->right ? v->right->depth : -1;
        int leftDepthV = v->left ? v->left->depth : -1;
        v->depth = std::max(rightDepthV, leftDepthV) +1;
        int rightDepthY = y->right ? y->right->depth : -1;
        y->depth = std::max(rightDepthY, v->depth) +1;
        v->recalc_minmax();
        y->recalc_minmax();
        return y;
    }

    static HobbitNode * rotateRight(HobbitNode * v){
        if (!v || !v->left)
            return v;
        v->push();
        HobbitNode * y = v->left;
        y->push();
        v->left = y->right;
        y->right = v;

        int rightDepthV = v->right ? v->right->depth : -1;
        int leftDepthV = v->left ? v->left->depth : -1;
        v->depth = std::max(rightDepthV, leftDepthV) +1;
        int rightDepthY = y->right ? y->right->depth : -1;
        int leftDepthY = y->left ? y->left->depth : -1;
        y->depth = std::max(rightDepthY, leftDepthY) +1;
        v->recalc_minmax();
        y->recalc_minmax();
        return y;
    }

     static HobbitNode* BVSMin(HobbitNode* v) {
        if (!v) return nullptr;
        v->push();
        while (v->left) {
            v = v->left;
            v->push();
        }
        return v;
    }




    static InsertResult insert(HobbitNode * v, const std::string & Key, const Hobbit & val) {
        if (!v) {
            HobbitNode* node = new HobbitNode(val);
            return { node, true };
        }

        v->push();

        if (Key < v->hobbit.name) {
            auto sub = insert(v->left, Key, val);
            v->left = sub.node;
;


            if (!sub.inserted)
                return { v, false };
        }
        else if (Key > v->hobbit.name) {
            auto sub = insert(v->right, Key, val);
            v->right = sub.node;



            if (!sub.inserted)
                return { v, false };
        }
        else {

            return { v, false };
        }

        int rightDepth = v->right ? v->right->depth : -1;
        int leftDepth  = v->left  ? v->left->depth  : -1;
        v->depth = std::max(rightDepth, leftDepth) + 1;

        auto bal = v->balance(v);
        if (bal == -2) {
            long leftBalance = v->balance(v->left);
            if (leftBalance <= 0) {            // LL-case
                v = v->rotateRight(v);
            } else {                           // LR-case
                v->left = v->rotateLeft(v->left);
                v = v->rotateRight(v);
            }
        }
        else if (bal == 2) {
            long rightBalance = v->balance(v->right);
            if (rightBalance >= 0) {           // RR-case
                v = v->rotateLeft(v);
            } else {                           // RL-case
                v->right = v->rotateRight(v->right);
                v = v->rotateLeft(v);
            }
        }else {

            v->recalc_minmax();
        }
        return { v, true };
    }



    static HobbitNode* erase(HobbitNode* v,
                             const std::string& Key,
                             bool& found,
                             Hobbit& out)
    {
        if (!v) return nullptr;

        v->push(); // чтобы hobbit в узле был актуальный

        if (Key < v->hobbit.name) {
            v->left = erase(v->left, Key, found, out);
        }
        else if (Key > v->hobbit.name) {
            v->right = erase(v->right, Key, found, out);
        }
        else {
            // нашли нужный узел
            if (!found) {          // записываем только один раз
                out = v->hobbit;
                found = true;
            }

            // 1) нет детей
            if (!v->left && !v->right) {
                delete v;
                return nullptr;
            }
            // 2) только правый
            if (!v->left && v->right) {
                HobbitNode* res = v->right;
                v->right = nullptr;
                delete v;
                return res;
            }
            // 3) только левый
            if (v->left && !v->right) {
                HobbitNode* res = v->left;
                v->left = nullptr;
                delete v;
                return res;
            }

            // 4) два ребёнка
            HobbitNode* w = BVSMin(v->right);   // <-- вот так, без "w->"
            v->hobbit = w->hobbit;              // копируем данные

            // удаляем преемника в правом поддереве
            v->right = erase(v->right, w->hobbit.name, found, out);
        }

        // пересчёт глубины
        int rightDepth = v->right ? v->right->depth : -1;
        int leftDepth  = v->left  ? v->left->depth  : -1;
        v->depth = std::max(rightDepth, leftDepth) + 1;

        // балансировка
        auto bal = v->balance(v);
        if (bal == -2) {
            long leftBalance = v->balance(v->left);
            if (leftBalance <= 0) {            // LL
                v = v->rotateRight(v);
            } else {                           // LR
                v->left = v->rotateLeft(v->left);
                v = v->rotateRight(v);
            }
        } else if (bal == 2) {
            long rightBalance = v->balance(v->right);
            if (rightBalance >= 0) {           // RR
                v = v->rotateLeft(v);
            } else {                           // RL
                v->right = v->rotateRight(v->right);
                v = v->rotateLeft(v);
            }
        }else {
            // поворотов не было, нужно пересчитать самому
            v->recalc_minmax();
        }
        return v;
    }





    void range_update(HobbitNode * n, const std::string & f, const std::string & l,const Hobbit & val) {
        if (!n) return;

        if (n->maxNode->hobbit.name < f || l < n->minNode->hobbit.name)
            return;

        if (f <= n->minNode->hobbit.name && n->maxNode->hobbit.name <= l) {
            n->add_pending(State(val.off,val.def, val.hp));
            return;
        }


        n->push();

        if (!(n->hobbit.name < f) && !(l < n->hobbit.name)) {
            n->hobbitPlus(val);
        }

        range_update(n->left,  f, l, val);
        range_update(n->right, f, l, val);


        n->recalc_minHP();
    }
    void recalc_minHP() {
        minimalHPSubtree = hobbit.hp;
        if (left)
            minimalHPSubtree = std::min(minimalHPSubtree, left->minimalHPSubtree);
        if (right)
            minimalHPSubtree = std::min(minimalHPSubtree, right->minimalHPSubtree);
    }


    ~HobbitNode() {
        delete left;
        delete right;
    }


};
struct HobbitArmy {
  static constexpr bool CHECK_NEGATIVE_HP = true;
  using Node = HobbitNode;
  HobbitArmy():root(nullptr){}
    ~HobbitArmy() {
       delete root;
    }
    bool add(const Hobbit& hobbit) {
        if (hobbit.hp <= 0)
            return false;

        if (!root) {
            root = new HobbitNode(hobbit);
            return true;
        }

        auto res = HobbitNode::insert(root, hobbit.name, hobbit);
        root = res.node;
        return res.inserted;
    }

    std::optional<Hobbit> erase(const std::string& hobbit_name) {
        if (!root)
            return std::nullopt;

        bool   found = false;
        Hobbit tmp{};  // сюда запишем удалённого

        root = root->erase(root, hobbit_name, found, tmp);

        if (!found)
            return std::nullopt;
        return tmp;
    }




    std::optional<Hobbit> stats(const std::string& hobbit_name) const {
        if(!root) {
            return std::nullopt;
        }
      auto search = root->find_push(root, hobbit_name);
      if(search) {
          return search->hobbit;
      }

      return std::nullopt;
  }

    bool enchant(
            const std::string& first,
            const std::string& last,
            int hp_diff,
            int off_diff,
            int def_diff
    ) {
        if (first > last) return true;
        if (!root) return true;


        if (hp_diff < 0) {
            if (!can_enchant(root, first, last, hp_diff))
                return false;
        }


        Hobbit add { "", hp_diff, off_diff, def_diff };


        root->range_update(root, first, last, add);
        return true;
    }


    void for_each(auto&& fun) const {
    for_each_impl(root, fun); // FIXME root name
  }

  private:
    static void for_each_impl(Node *node, auto& fun) {
        if (!node) return;

        node->push();

        for_each_impl(node->left,  fun);
        fun(node->hobbit);
        for_each_impl(node->right, fun);
    }
    static bool can_enchant(Node* node,
                            const std::string& first,
                            const std::string& last,
                            int hp_diff)
    {
        if (!node) return true;

        if (node->maxNode->hobbit.name < first || last < node->minNode->hobbit.name)
            return true;

        if (first <= node->minNode->hobbit.name && node->maxNode->hobbit.name <= last) {
            return node->minimalHPSubtree + hp_diff > 0;
        }


        node->push();


        if (!(node->hobbit.name < first) && !(last < node->hobbit.name)) {
            if (node->hobbit.hp + hp_diff <= 0)
                return false;
        }


        if (!can_enchant(node->left, first, last, hp_diff))
            return false;
        if (!can_enchant(node->right, first, last, hp_diff))
            return false;

        return true;
    }
  HobbitNode * root;


};

#ifndef __PROGTEST__

////////////////// Dark magic, ignore ////////////////////////

template < typename T >
auto quote(const T& t) { return t; }

std::string quote(const std::string& s) {
  std::string ret = "\"";
  for (char c : s) if (c != '\n') ret += c; else ret += "\\n";
  return ret + "\"";
}

#define STR_(a) #a
#define STR(a) STR_(a)

#define CHECK_(a, b, a_str, b_str) do { \
    auto _a = (a); \
    decltype(a) _b = (b); \
    if (_a != _b) { \
      std::cout << "Line " << __LINE__ << ": Assertion " \
        << a_str << " == " << b_str << " failed!" \
        << " (lhs: " << quote(_a) << ")" << std::endl; \
      fail++; \
    } else ok++; \
  } while (0)

#define CHECK(a, b) CHECK_(a, b, #a, #b)


////////////////// End of dark magic ////////////////////////


void check_army(const HobbitArmy& A, const std::vector<Hobbit>& ref, int& ok, int& fail) {
  size_t i = 0;

  A.for_each([&](const Hobbit& h) {
    CHECK(i < ref.size(), true);
    CHECK(h, ref[i]);
    i++;
  });

  CHECK(i, ref.size());
}

void test1(int& ok, int& fail) {
  HobbitArmy A;
  check_army(A, {}, ok, fail);

  CHECK(A.add({"Frodo", 100, 10, 3}), true);
  CHECK(A.add({"Frodo", 200, 10, 3}), false);
  CHECK(A.erase("Frodo"), std::optional(Hobbit("Frodo", 100, 10, 3)));
  CHECK(A.add({"Frodo", 200, 10, 3}), true);

  CHECK(A.add({"Sam", 80, 10, 4}), true);
  CHECK(A.add({"Pippin", 60, 12, 2}), true);
  CHECK(A.add({"Merry", 60, 15, -3}), true);
  CHECK(A.add({"Smeagol", 0, 100, 100}), false);

  if constexpr(HobbitArmy::CHECK_NEGATIVE_HP)
    CHECK(A.add({"Smeagol", -100, 100, 100}), false);

  CHECK(A.add({"Smeagol", 200, 100, 100}), true);

  CHECK(A.enchant("Frodo", "Frodo", 10, 1, 1), true);
  CHECK(A.enchant("Sam", "Frodo", -1000, 1, 1), true); // empty range
  CHECK(A.enchant("Bilbo", "Bungo", 1000, 0, 0), true); // empty range

  if constexpr(HobbitArmy::CHECK_NEGATIVE_HP)
    CHECK(A.enchant("Frodo", "Sam", -60, 1, 1), false);

  CHECK(A.enchant("Frodo", "Sam", 1, 0, 0), true);
  CHECK(A.enchant("Frodo", "Sam", -60, 1, 1), true);

  CHECK(A.stats("Gandalf"), std::optional<Hobbit>{});
  CHECK(A.stats("Frodo"), std::optional(Hobbit("Frodo", 151, 12, 5)));
  CHECK(A.stats("Merry"), std::optional(Hobbit("Merry", 1, 16, -2)));

  check_army(A, {
    {"Frodo", 151, 12, 5},
    {"Merry", 1, 16, -2},
    {"Pippin", 1, 13, 3},
    {"Sam", 21, 11, 5},
    {"Smeagol", 200, 100, 100},
  }, ok, fail);
}


#include <chrono>
using namespace std::chrono;
int main() {
  int ok = 0, fail = 0;
    auto start = high_resolution_clock::now();
  test1(ok, fail);
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);

    std::cout << "Time taken by function: "
         << duration.count() << " microseconds" << std::endl;
  if (!fail) std::cout << "Passed all " << ok << " tests!" << std::endl;
  else std::cout << "Failed " << fail << " of " << (ok + fail) << " tests." << std::endl;
}

#endif


