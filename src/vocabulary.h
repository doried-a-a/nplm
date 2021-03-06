#ifndef VOCABULARY_H
#define VOCABULARY_H

#include <vector>
#include <string>
#include <queue>
#include <boost/unordered_map.hpp>
#include "find_string.hpp"

#define NPLM_HAVE_FIND_STRING_PIECE 1

namespace nplm
{

template <typename T>
struct compare_second
{
  bool operator()(const T &lhs, const T &rhs) const { return lhs.second < rhs.second; }
};

class vocabulary {
  std::vector<std::string> m_words;
  typedef boost::unordered_map<std::string, int> WordId;
  WordId m_index;
  int unk;

 public:
  vocabulary()
  {
    unk = insert_word("<unk>");
  }

  vocabulary(const std::vector<std::string> &words)
      :
      m_words(words)
  {
    for (int i=0; i<words.size(); i++)
      m_index[words[i]] = i;
    unk = m_index["<unk>"];
  }

  int lookup_word(const std::string &word) const
  {
    return lookup_word(word, unk);
  }

  // lookup word using custom unknown-word id
  int lookup_word(const std::string &word, int unkid) const
  {
    WordId::const_iterator pos = m_index.find(word);
    return pos == m_index.end() ? unkid : pos->second;
  }

  int lookup_word(std::pair<char const*, char const*> slice) const {
    return lookup_word(slice, unk);
  }

  int lookup_word(std::pair<char const*, char const*> slice, int unkid) const
  {
    WordId::const_iterator pos = find_string(m_index, slice);
    return pos == m_index.end() ? unkid : pos->second;
  }

  int insert_word(const std::string &word)
  {
    int i = size();
    bool inserted = m_index.insert(make_pair(word, i)).second;
    if (inserted)
    {
      m_words.push_back(word);
    }
    return i;
  }

  int size() const { return m_words.size(); }

  // Inserts the most-frequent words from counts until vocab_size words are reached.
  // counts is a collection of pair<string,int>
  template <typename Map>
  int insert_most_frequent(const Map &counts, int vocab_size)
  {
    typedef std::pair<std::string,int> stringint;

    std::priority_queue<stringint,std::vector<stringint>,compare_second<stringint> >
        q(compare_second<stringint>(), std::vector<stringint>(counts.begin(), counts.end()));

    int inserted = 0;
    while (size() < vocab_size && !q.empty())
    {
      insert_word(q.top().first);
      q.pop();
      inserted++;
    }
    return inserted;
  }

  const std::vector<std::string> &words() const { return m_words; }

};

} // namespace nplm

#endif
