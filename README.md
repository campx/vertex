We define several data Structures atop a tree, with a uniform interface
across each for inserting, finding and iterating across the elements.

Array
Tree of depth 1, bredth n
        O
       /|\
      / | \
     O  O  O
Find: Bredth first search
 

List
Tree of depth n, bredth 1

     O<--O<--O<--O
Find: Depth first search, pre-order traversal

BTree
Tree with given branch factor
           O
          /|\
         / | \
        O  O  O
       /   |
      /    |
     O     O
    /|\
   / | \
  O  O  O


Use PathMap and insert_or_assign to propogate changes up the tree
