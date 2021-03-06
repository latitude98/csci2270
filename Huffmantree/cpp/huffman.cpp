/*
 * huffman.cpp
 *
 * Your Name:
 * Your Collaborators:
 *
 * This implementation file contains some functions that are fully
 * built. Others are empty. Some empty ones are optional, others are
 * not.
 *
 * Consider implementing this by iteratively getting manual_driver()
 * to work. The steps are spelled out for you.
 */

#include <iostream>
#include <fstream>
#include <map>
#include <queue>
#include <algorithm>
#include <string>
#include <cctype>
#include "huffman.h"

using namespace std;

void recurse_encodemap(freq_info*& huffman_root, map<char, string> &ret, string &bit);

void manual_driver () {

  /*  1. Create a priority queue of one-node trees using some big text
      file. This gives you a priority queue containing freq_info
      objects. These are meant to be used as nodes in a tree. At the
      moment, they are all disconnected, so you have a forest of tiny
      one-node seedlings. 
  */
  tree_queue queue = read_corpus("pkd.txt");
  // Uncomment the following if you want to debug the queue
  //debug_queue(queue);

  /*  2. Now do something like the following:
      
      while priority queue has more than one item: 
      
        Pop top two items, A and B. They are lowest frequency nodes.
        Create non-leaf node C who points to children A and B. C's
        frequency is the sum of A and B's frequency.  Insert C back
        into the priority queue. We are left with a single node in the
        priority queue. This is the root node of our huffman encoding
        tree. Retain a reference to that root node.
   */
  freq_info* huffman_root = build_huffman_tree(queue);
  // if you want to show debugging info of a huffman tree, uncomment:
  //  int level = 0;
  //  debug_tree(huffman_root, level);
  

  /*  3. The leaf layer of the huffman tree contains symbols. The path
      from root to a leaf is the huffman encoding for that leaf's
      symbol. Every time a left child is followed, that corresponds to
      a zero bit; right children are one bits.

      Traverse the tree and create a bitstring for each leaf. If the
      path from the root to the leaf happens to be Left Left Right
      Left, the bitstring for that leaf is 0010. Store these
      bitstrings in a map. The resulting map use symbols for keys and
      strings that use LEFT_CHAR for 0 and RIGHT_CHAR for 1
      (right). By default these are '.' and '^' respectively:

      key   bitstring
      ---------------
       a     .^.^
       b     ..^.^
       z     ^^.^..^
       #     .^^....^
       e     ^^.

       Notice that 'e' uses three bits while '#' uses 8. Think about
       why this is awesome.
   */
  map<char, string> encoding_map = build_encoding_map(huffman_root);
  // uncomment this if you want to show debugging info of your map.
  // debug_encoding_map(encoding_map);
  
  /*  4. Now that we have a Map of symbols to bitstrings, we can use
      it to encode text data. I'll give little sentences to encode.
      For the output, we're using strings rather than actual bits so
      (a) we can read and debug it more easily, and (b) so we don't
      have to deal with chopping up individual bytes in memory (which
      isn't the point of the exercise).
   */
  string short_string = "And no one sings me lullabyes...";
  string medium_string = "We don't need no education.\nWe don't need no thought control.\nNo dark sarcasm in the classroom.\nTeacher leave those kids alone!\n";
  string long_string = "The lunatic is on the grass.\nThe lunatic is on the grass.\nRemembering games and daisy chains and laughs.\nGot to keep the loonies on the path.\n\nThe lunatic is in the hall.\nThe lunatics are in my hall.\nThe paper holds their folded faces to the floor\nAnd every day the paper boy brings more.\n\nAnd if the dam breaks open many years too soon\nAnd if there is no room upon the hill\nAnd if your head explodes with dark forebodings too\nI'll see you on the dark side of the moon.\n\nThe lunatic is in my head.\nThe lunatic is in my head\nYou raise the blade, you make the change\nYou re-arrange me 'til I'm sane.\nYou lock the door\nAnd throw away the key\nThere's someone in my head but it's not me.\n\nAnd if the cloud bursts, thunder in your ear\nYou shout and no one seems to hear.\nAnd if the band you're in starts playing different tunes\nI'll see you on the dark side of the moon.";

  string short_encoded = encode(short_string, encoding_map);
  string medium_encoded = encode(medium_string, encoding_map);
  string long_encoded = encode(long_string, encoding_map);

  /*  5.  After encoding some strings into your huffman coded 'bit'
      string, you can use the decode function to get back the
      original. The way you do this is to read the input character by
      character, starting from the root of your huffman tree. When you
      see LEFT_CHAR, you move left; a RIGHT_CHAR, move right. When you
      reach a leaf node, you have found the coded character. Record
      that character. Move back to the root and continue reading the
      input string until it is complete. The sequence you read should
      be the original message.
   */

  string redone_short = decode(short_encoded, huffman_root);
  string redone_medium = decode(medium_encoded, huffman_root);
  string redone_long = decode(long_encoded, huffman_root);

  // consider using do_encoding_test (a function defined later on) to
  // see if these things are working properly.
}

void debug_queue(tree_queue &queue) {
	cout << "Debugging queue..." << endl;
	unsigned int i;
	unsigned int j = queue.size();
	for ( i = 0; i < j;i++)
	{
		freq_info* first = queue.top();
		cout << first->symbol << endl;
		queue.pop();
	}
	
}

void debug_tree(freq_info*& node, int& level) {
  // implement (optional)
  cout << "Debugging tree..." << endl;
}

void debug_encoding_map(map<char, string>& huffman_map) {
  // implement (optional)
  cout << "Debugging encoding map..." << endl;
}

freq_info* init_freq_info_supernode(freq_info* left, freq_info* right) {
  freq_info* ret = new freq_info;
  ret->freq = left->freq + right->freq;
  ret->left = left;
  ret->right = right;
  ret->is_leaf = false;
  return ret;
}

freq_info* build_huffman_tree(tree_queue &queue) {
  freq_info* root;
  while ( queue.size() > 1)
  {
	  freq_info* one = queue.top();
	  queue.pop();
	  freq_info* two = queue.top();
	  queue.pop();
	  root = init_freq_info_supernode(one, two);
	  queue.push(root);
  }
  return root;
}

map<char, string> build_encoding_map(freq_info*& huffman_root) {
  map<char, string> ret;
  string bit = "";
  recurse_encodemap(huffman_root, ret, bit);
  
  return ret;
}

void recurse_encodemap(freq_info*& huffman_root, map<char, string> &ret, string &bit)
{
		
  if (huffman_root == NULL)
  {
	  return;
  }
  else 
  {
	  //if node is a leaf...
	  if (huffman_root->is_leaf == true)
	  {
		  ret[huffman_root->symbol] = bit;
	  }
	  //we must traverse into children
	  else
	  {	
		  //go right, add right char to bitstring
		  //recursion acts just like in-order traversal of binary tree
		  if ( huffman_root->right != NULL)
		  {
			bit.push_back(RIGHT_CHAR);
			recurse_encodemap(huffman_root->right, ret, bit);
			//go back to parent and delete 1 char
			bit.erase(bit.end()-1);
			}
			// go left, add left char to bitstring
		if ( huffman_root->left != NULL)
			{
		 
			bit.push_back(LEFT_CHAR);
			recurse_encodemap(huffman_root->left, ret, bit);
			//go back to parent, delete last char
			bit.erase(bit.end()-1);
			}
	  }
  }	
  return;
}


string encode(string& original, map<char, string>& encoding_map) {
  string result = "";
  //go through chars of original string and append bitstring from map 
  //of each char to another string
  for (unsigned int i = 0; i < original.size(); i++)
  {
	  result.append (encoding_map[original[i]]);
  }
	return result;
}

string decode(string& encoded, freq_info*& huffman_root) {
  string result = "";
  freq_info* temp = huffman_root;
	while ( encoded.size() > 0)
	{
		if (temp->is_leaf != true && *encoded.begin() == LEFT_CHAR)
		{
		  encoded.erase(encoded.begin());
		  temp = temp->left;
	    }
		else if ( temp->is_leaf != true && *encoded.begin() == RIGHT_CHAR)
		{
		 encoded.erase(encoded.begin());
		 temp = temp->right;
		}
		else
		{
			result.push_back(temp->symbol);
			temp = huffman_root;
		}
	}
	result.push_back(temp->symbol);
  return result;
}
