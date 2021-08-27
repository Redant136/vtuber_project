#pragma once
#error "incomplete"

#include <assert.h>
#include <stdexcept>
#include <tuple>
#include <vector>
#include <string>

struct JSONData
{
  enum
  {
    DATA,
    VALUE
  } type;
  std::string name;
  union
  {
    std::vector<JSONData> data;
    std::string value;
  };

  JSONData get(std::string &s)
  {
    if (type != DATA)
      throw std::invalid_argument("is value");
    for (unsigned int i = 0; i < data.size(); i++)
    {
      if (s == name)
      {
        return data[i];
      }
    }
    throw std::invalid_argument(s + " is not part of the json");
  }
  JSONData get(unsigned int index)
  {
    if (type != DATA)
      throw std::invalid_argument("is value") return data[index];
  }

  unsigned int toUINT()
  {
    if (type != VALUE)
    {
      throw std::invalid_argument("not value");
    }
    return (unsigned int)std::stol(value);
  }
  int toINT()
  {
    if (type != VALUE)
    {
      throw std::invalid_argument("not value");
    }
    return std::stoi(value);
  }
  long toLONG()
  {
    if (type != VALUE)
    {
      throw std::invalid_argument("not value");
    }
    return std::stol(value);
  }
  unsigned long toULONG()
  {
    if (type != VALUE)
    {
      throw std::invalid_argument("not value");
    }
    return std::stoul(value);
  }
  long long toLONGLONG()
  {
    if (type != VALUE)
    {
      throw std::invalid_argument("not value");
    }
    return std::stoll(value);
  }
  float toFLOAT()
  {
    if (type != VALUE)
    {
      throw std::invalid_argument("not value");
    }
    return std::stof(value);
  }
  double toDOUBLE()
  {
    if (type != VALUE)
    {
      throw std::invalid_argument("not value");
    }
    return std::stod(value);
  }
};

namespace JSONParser
{

  class Importer{
  public:
    Importer(){
    }
    JSONData parse(std::string&s)
    {
      JSONData json;

    }
  private:
    // format: {name, data, remaining}
    std::tuple<std::string, std::string, std::string> splitData(std::string &s)
    {
      if (s[0] != '"')
      {
        throw std::invalid_argument("not name at first index");
      }

      std::string name;
      std::string data;

      name=parseString(s);

      

      unsigned int bracketDepth=0;
      for (unsigned int i = name.size() + 3; i < s.length(); i++)
      {
        if (s[i] == '[' && s[i] == '{')
        {
          bracketDepth++;
        }

        

        if(s[i]==']'&&s[i]=='}'){
          bracketDepth--;
        }
      }
    }

    std::string parseString(std::string &s, unsigned int startIndex = 0)
    {
      if (s[0] != '"')
      {
        throw std::invalid_argument("not quotation at first index");
      }

      std::string name;
      bool escapeNextChar = false;
      for (unsigned int i = startIndex + 1; i < s.length(); i++)
      {
        if (s[i] == '\\' && !escapeNextChar)
        {
          escapeNextChar = true;
        }
        else
        {
          if (escapeNextChar)
          {
            switch (s[i])
            {
            case '"':
              name += '"';
              break;
            case '\\':
              name += '\\';
              break;
            case '/':
              name += '/';
              break;
            case 'b':
              name += '\b';
              break;
            case 'f':
              name += '\f';
              break;
            case 'n':
              name += '\n';
              break;
            case 'r':
              name += '\r';
              break;
            case 't':
              name += '\t';
              break;
            case 'u':
              name += (char)std::stoi(s.substr(i, 4));
              i += 4;
              break;
            default:
              throw std::invalid_argument("wrong escap character");
              break;
            }
            escapeNextChar = false;
          }
          else
          {
            if (s[i] == '"')
            {
              break;
            }
            name += s[i];
          }
        }
      }
      return name;
    }
  };

}